#include <linux/module.h>
#include <linux/device.h>
#include <linux/clk-provider.h>
#include <linux/clk.h>
#include <linux/pm_runtime.h>
#include <linux/platform_device.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>

#include <linux/of.h>
#include <linux/of_dma.h>
#include <linux/acpi.h>
#include <linux/acpi_dma.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

#include "../dmaengine.h"
#include "cvitek-dma.h"

#define DRV_NAME "dw_dmac"

/* #define CONFIG_SYSDMA_JIRA_BUG_BM1880_17 */

/*
 *	The dmaengine doc says that, defer most work to a tasklet is inefficient,
 *	and there may be schedule latency, and slow down the trans rate.
 *	so we should avoid this ?
 */

/* #define DO_NOT_USE_TASK */

static void dw_dma_off(struct dw_dma *dw);
static void dw_dma_on(struct dw_dma *dw);
static bool dw_dma_filter(struct dma_chan *chan, void *param);
static int dw_dma_cyclic_start(struct dma_chan *chan);

#define to_dw_desc(h)	list_entry(h, struct dw_desc, desc_node)

#define DWC_DEFAULT_CTL(_chan) ({				\
			struct dw_dma_chan *_dwc = to_dw_dma_chan(_chan);	\
			struct dma_slave_config *_sconfig = &_dwc->dma_sconfig; \
			bool _is_slave = is_slave_direction(_dwc->direction);	\
			u8 _smsize = _is_slave ? _sconfig->src_maxburst :	\
				DW_DMA_MSIZE_32;			\
			u8 _dmsize = _is_slave ? _sconfig->dst_maxburst :	\
				DW_DMA_MSIZE_32;			\
			u8 _dms = (_dwc->direction == DMA_MEM_TO_DEV) ?		\
				_dwc->dws.p_master : _dwc->dws.m_master;	\
			u8 _sms = (_dwc->direction == DMA_DEV_TO_MEM) ?		\
				_dwc->dws.p_master : _dwc->dws.m_master;	\
										\
			(DWC_CTL_DST_MSIZE(_dmsize)				\
			 | DWC_CTL_SRC_MSIZE(_smsize)				\
			 | DWC_CTL_DMS(_dms)					\
			 | DWC_CTL_SMS(_sms));					\
		})


static inline struct dw_dma_chan_regs __iomem *
__dwc_regs(struct dw_dma_chan *dwc)
{
	return dwc->ch_regs;
}

static inline struct dw_dma_regs __iomem *
__dw_regs(struct dw_dma *dw)
{
	return dw->regs;
}

static inline struct dw_desc *txd_to_dw_desc(struct dma_async_tx_descriptor *txd)
{
	return container_of(txd, struct dw_desc, txd);
}

static inline struct dw_dma_chan *to_dw_dma_chan(struct dma_chan *chan)
{
	return container_of(chan, struct dw_dma_chan, chan);
}

static inline struct dw_dma *to_dw_dma(struct dma_device *ddev)
{
	return container_of(ddev, struct dw_dma, dma);
}

static struct device *chan2dev(struct dma_chan *chan)
{
	return &chan->dev->device;
}

/*
 * Fix sconfig's burst size according to dw_dmac. We need to convert them as:
 * 1 -> 0, 4 -> 1, 8 -> 2, 16 -> 3.
 *
 * NOTE: burst size 2 is not supported by controller.
 *
 * This can be done by finding least significant bit set: n & (n - 1)
 */
static inline void convert_burst(u32 *maxburst)
{
	if (*maxburst > 1)
		*maxburst = fls(*maxburst) - 2;
	else
		*maxburst = 0;
}

static inline void dwc_dump_chan_regs(struct dw_dma_chan *dwc)
{
	dev_err(chan2dev(&dwc->chan),
		"  SAR: 0x%llx DAR: 0x%llx LLP: 0x%llx CTL: 0x%llx BLOCK_TS: 0x%llx\n",
		channel_readq(dwc, SAR),
		channel_readq(dwc, DAR),
		channel_readq(dwc, LLP),
		channel_readq(dwc, CTL),
		channel_readq(dwc, BLOCK_TS));
}

static inline void dwc_dump_lli(struct dw_dma_chan *dwc, struct dw_desc *desc)
{
	dev_err(chan2dev(&dwc->chan), "  desc: s0x%llx d0x%llx l0x%llx c0x%llx b0x%llx\n",
		lli_read(desc, sar),
		lli_read(desc, dar),
		lli_read(desc, llp),
		lli_read(desc, ctl),
		lli_read(desc, block_ts));
}

#if 0
#define ENABLE_ALL_REG_STATUS	(0x3 << 0 | 0xFFF << 3 | 0x3F << 16 | 0x1F << 27)
static void dwc_interrupts_set(struct dw_dma_chan *dwc, bool val)
{
	u64 int_val;

	if (val) {
		/* channel int signal enable*/
		int_val = DWC_CH_INTSIG_DMA_TRA_DONE;
		channel_writeq(dwc, INTSIGNAL_ENABLEREG, int_val);

		/*  channel int status enable in reg, all enable*/
		channel_writeq(dwc, INTSTATUS_ENABLEREG, ENABLE_ALL_REG_STATUS);
	} else {
		/* channel int signal disable*/
		int_val = channel_readq(dwc, INTSIGNAL_ENABLEREG);
		int_val &= ~DWC_CH_INTSIG_DMA_TRA_DONE;
		channel_writeq(dwc, INTSIGNAL_ENABLEREG, int_val);
		/*  channel int status disable in reg, all disable*/
		int_val = channel_readq(dwc, INTSTATUS);
		channel_writeq(dwc, INTCLEARREG, int_val);
		int_val &= ~ENABLE_ALL_REG_STATUS;
		channel_writeq(dwc, INTSTATUS_ENABLEREG, int_val);
	}
}
#endif

static void dwc_prepare_clk(struct dw_dma *dw)
{
	int err;

	dw->clk_count++;
	if (dw->clk_count == 1)	{
		err = clk_enable(dw->clk);
		if (err)
			dev_err(dw->dev, "CVITEK DMA enable clk_sdma_axi failed\n");
	}
}

static void dwc_unprepare_clk(struct dw_dma *dw)
{

	dw->clk_count--;

	if (dw->clk_count == 0)
		clk_disable(dw->clk);

	if (dw->clk_count < 0)
		dev_err(dw->dev, "CVITEK sysDMA clk count is invalid\n");
}

static void dwc_initialize(struct dw_dma_chan *dwc)
{
	u64 cfg = 0, int_status_reg;
	bool has_device_fc;
	bool hs_polarity = dwc->dws.hs_polarity;
	struct dma_slave_config	*sconfig = &dwc->dma_sconfig;

	has_device_fc = sconfig->device_fc;
	if (test_bit(DW_DMA_IS_INITIALIZED, &dwc->flags))
		return;

	cfg = DWC_CFG_DST_PER((u64)dwc->dws.dst_id)
	      | DWC_CFG_SRC_PER((u64)dwc->dws.src_id)
	      | DWC_CFG_SRC_OSR_LMT((u64)DW_DMA_MAX_NR_REQUESTS - 1)
	      | DWC_CFG_DST_OSR_LMT((u64)DW_DMA_MAX_NR_REQUESTS - 1)
	      | DWC_CFG_CH_PRIOR((u64)dwc->priority)
	      | DWC_CFG_DST_MULTBLK_TYPE(LINK_LIST)
	      | DWC_CFG_SRC_MULTBLK_TYPE(LINK_LIST);

	/* choose Flowcontrol and handshaking type */
	switch (dwc->direction) {
	case DMA_MEM_TO_MEM:
		cfg |= DWC_CFG_TT_FC((u64)DW_DMA_FC_D_M2M);
		break;
	case DMA_MEM_TO_DEV:
		cfg |= has_device_fc ? DWC_CFG_TT_FC((u64)DW_DMA_FC_DP_M2P)
		       : DWC_CFG_TT_FC((u64)DW_DMA_FC_D_M2P);
		cfg |= DWC_CFG_HS_SEL_DST_HW;
		break;
	case DMA_DEV_TO_MEM:
		cfg |= has_device_fc ? DWC_CFG_TT_FC((u64)DW_DMA_FC_SP_P2M)
		       : DWC_CFG_TT_FC((u64)DW_DMA_FC_D_P2M);
		cfg |= DWC_CFG_HS_SEL_SRC_HW;
		break;
	default:
		break;
	}

	/* Set polarity of handshake interface */
	cfg |= hs_polarity ? DWC_CFG_SRC_HWHS_POL_H | DWC_CFG_DST_HWHS_POL_H : 0;

	channel_writeq(dwc, CFG, cfg);
	/* Enable interrupts */
	if (test_bit(DW_DMA_IS_CYCLIC, &dwc->flags))
		int_status_reg =  DWC_CH_INTSTA_BLOCK_TFR_DONE;
	else {
		int_status_reg =  DWC_CH_INTSTA_DMA_TFR_DONE;
#if 0
		| DWC_CH_INTSTA_BLOCK_TFR_DONE
		| DWC_CH_INTSTA_SRC_TFR_COMP_EN
		| DWC_CH_INTSTA_DST_TFR_COMP_EN
		| DWC_CH_INTSTA_SRC_DEC_ERR_EN
		| DWC_CH_INTSTA_DST_DEC_ERR_EN
		| DWC_CH_INTSTA_LLI_RD_DEV_ERR_EN
		| DWC_CH_INTSTA_LLI_WD_DEV_ERR_EN
		| DWC_CH_INTSTA_LLI_RD_SLV_ERR_EN
		| DWC_CH_INTSTA_LLI_WD_SLV_ERR_EN
		| DWC_CH_INTSTA_CH_ABORTED_EN;
#endif
	}

	channel_writeq(dwc, INTSTATUS_ENABLEREG, int_status_reg);

	set_bit(DW_DMA_IS_INITIALIZED, &dwc->flags);
	dwc->status &= ~DWC_CH_INTSIG_DMA_TRA_DONE;
}

/* Called with dwc->lock held and bh disabled */
#define DWC_DMA_MAX_RETRY	3000
static void dwc_dostart(struct dw_dma_chan *dwc, struct dw_desc *first)
{
	u64 ch_en_reg;
	struct dw_dma *dw = to_dw_dma(dwc->chan.device);
	u32 retry_count = 0;

	/* ASSERT:  channel is idle */
	while (dma_readq(dw, CH_EN) & dwc->mask) {

		if (retry_count >= DWC_DMA_MAX_RETRY) {
			dev_err(chan2dev(&dwc->chan),
				"%s: BUG: Attempted to start non-idle channel\n",
				__func__);
			//dwc_dump_chan_regs(dwc);

			/* The tasklet will hopefully advance the queue... */
			return;
		}
		retry_count++;
	}

	dwc_initialize(dwc);

	channel_writeq(dwc, LLP, first->txd.phys);

	ch_en_reg = dwc->mask << DW_DMAC_CH_EN_WE_OFFSET;
	dma_writeq(dw, CH_EN, dwc->mask | ch_en_reg);
}

static struct dw_desc *dwc_first_active(struct dw_dma_chan *dwc)
{
	return to_dw_desc(dwc->active_list.next);
}

static void dwc_dostart_first_queued(struct dw_dma_chan *dwc)
{
	struct dw_desc *desc;

	if (list_empty(&dwc->queue))
		return;

	list_move(dwc->queue.next, &dwc->active_list);
	desc = dwc_first_active(dwc);
	dwc_dostart(dwc, desc);
}

static dma_cookie_t dwc_tx_submit(struct dma_async_tx_descriptor *tx)
{
	unsigned long flags;
	dma_cookie_t cookie;
	struct dw_desc *desc = txd_to_dw_desc(tx);
	struct dw_dma_chan *dwc = to_dw_dma_chan(tx->chan);

	spin_lock_irqsave(&dwc->lock, flags);
	cookie = dma_cookie_assign(tx);
	if (test_bit(DW_DMA_IS_CYCLIC, &dwc->flags)) {
		spin_unlock_irqrestore(&dwc->lock, flags);
		return cookie;
	}
	/*
	 * REVISIT: We should attempt to chain as many descriptors as
	 * possible, perhaps even appending to those already submitted
	 * for DMA. But this is hard to do in a race-free manner.
	 */

	list_add_tail(&desc->desc_node, &dwc->queue);
	spin_unlock_irqrestore(&dwc->lock, flags);

	return cookie;
}

static struct dw_desc *dwc_desc_get(struct dw_dma_chan *dwc)
{
	dma_addr_t phys;
	struct dw_desc *desc;
	struct dw_dma *dw = to_dw_dma(dwc->chan.device);

	desc = dma_pool_zalloc(dw->desc_pool, GFP_ATOMIC, &phys);
	if (!desc)
		return NULL;

	dwc->descs_allocated++;
	INIT_LIST_HEAD(&desc->tx_list);
	dma_async_tx_descriptor_init(&desc->txd, &dwc->chan);
	desc->txd.tx_submit = dwc_tx_submit;
	desc->txd.flags = DMA_CTRL_ACK;
	desc->txd.phys = phys;

	return desc;
}

static void dwc_desc_put(struct dw_dma_chan *dwc, struct dw_desc *desc)
{
	struct dw_desc *child, *_next;
	struct dw_dma *dw = to_dw_dma(dwc->chan.device);

	if (unlikely(!desc))
		return;

	list_for_each_entry_safe(child, _next, &desc->tx_list, desc_node) {
		list_del(&child->desc_node);
		dma_pool_free(dw->desc_pool, child, child->txd.phys);
		dwc->descs_allocated--;
	}

	dma_pool_free(dw->desc_pool, desc, desc->txd.phys);
	dwc->descs_allocated--;
}

#ifdef CONFIG_SYSDMA_JIRA_BUG_BM1880_17
static dma_addr_t fix_dma_bug_copy_get(struct dw_dma_chan *dwc,
				       struct dma_chan *chan, struct scatterlist *sgl,
				       int sg_index, int sg_len, int size)
{
	int i;
	dma_addr_t bug_addr;
	unsigned int *bug_buf = NULL;
	unsigned char *tmp_buf, *record;
	unsigned int fix_size;
	char dma_pool_name[32];
	struct dbg_fix_sg *db_sg = dwc->bug_info;

	if (!db_sg) {
		db_sg = kzalloc(sizeof(*db_sg), GFP_ATOMIC);
		if (!db_sg) {
			dev_err(chan->device->dev,
				"BUG: Alloc db_sg failed,No memory !\n");
			return 0;
		}
		dwc->bug_info = db_sg;
	}
	db_sg->count = sg_len;
	if (!db_sg->fix_buf) {
		db_sg->fix_buf = kcalloc(sg_len, sizeof(struct dbg_fix_buf), GFP_ATOMIC);
		if (!db_sg->fix_buf) {
			dev_err(chan->device->dev,
				"BUG: Alloc fix_buf failed,No memory !\n");
			return 0;
		}
	}
	if (!dwc->bug_fix_dma_pool) {
		snprintf(dma_pool_name, sizeof(dma_pool_name), "%s_%x",
			 "dma_chan_buf", dwc->mask);
		dwc->bug_fix_dma_pool = dma_pool_create(dma_pool_name, chan->device->dev,
							PAGE_SIZE, BIT(2), 0);
		if (!dwc->bug_fix_dma_pool) {
			dev_err(chan->device->dev, "unable to allocate dma pool\n");
			return 0;
		}
	}

	record = tmp_buf = kzalloc(size, GFP_ATOMIC);
	if (!tmp_buf) {
		dev_err(chan->device->dev,
			"BUG: Alloc tmp_buf failed,No memory !\n");
		return 0;
	}

	fix_size = (size << 2);
	if (unlikely(fix_size > PAGE_SIZE)) {
		bug_buf = dma_alloc_coherent(chan->device->dev, fix_size,
					     &bug_addr, GFP_ATOMIC);
		if (!bug_buf) {
			dev_err(chan->device->dev,
				"BUG: Alloc bug_buf failed,No DMA memory !\n");
			return 0;
		}
	} else {
		bug_buf = dma_pool_zalloc(dwc->bug_fix_dma_pool, GFP_ATOMIC, &bug_addr);
		if (!bug_buf) {
			dev_err(chan->device->dev,
				"BUG: Alloc bug_buf failed,No DMA Pool memory !\n");
			return 0;
		}
	}

	db_sg->fix_buf[sg_index].id = sg_index;
	db_sg->fix_buf[sg_index].cpu_addr = bug_buf;
	db_sg->fix_buf[sg_index].dma_hanle = bug_addr;
	db_sg->fix_buf[sg_index].size = fix_size;
	db_sg->fix_buf[sg_index].state = 1;

	//sg_copy_to_buffer(sgl, nents, tmp_buf, size);
	memcpy((void *)tmp_buf, sg_virt(sgl), size);

	for (i = 0; i < size; i++) {
		memcpy((void *)bug_buf, (void *)tmp_buf, 1);
		bug_buf++;
		tmp_buf++;
	}

	kfree(record);

	return bug_addr;
}

static void fix_dma_bug_copy_put(struct dw_dma_chan *dwc, struct dma_chan *chan)
{
	int i;
	struct dbg_fix_sg *db_sg;
	struct dbg_fix_buf *db_buf;

	if (!dwc->bug_info)
		return;
	if (!dwc->bug_info->fix_buf)
		return;

	db_sg = dwc->bug_info;
	db_buf = db_sg->fix_buf;

	for (i = 0; i < db_sg->count; i++) {
		if (db_buf->size > PAGE_SIZE) {
			dma_free_coherent(chan->device->dev, db_buf->size,
					  db_buf->cpu_addr, db_buf->dma_hanle);
		} else {
			dma_pool_free(dwc->bug_fix_dma_pool, db_buf->cpu_addr, db_buf->dma_hanle);
		}
		db_buf->state = 0;
		db_buf++;
	}

	/* dump info */
	db_buf = db_sg->fix_buf;
	for (i = 0; i < db_sg->count; i++) {
		if (db_buf->state == 1)
			dev_warn(chan->device->dev,
				 "db_buf id: %d,cpu_addr %p, dma_hanle %pad, size 0x%x state %d\n",
				 db_buf->id, db_buf->cpu_addr, &(db_buf->dma_hanle), db_buf->size, db_buf->state);
	}

	kfree(db_sg->fix_buf);
	db_sg->fix_buf = NULL;
}
#endif

static void dwc_descriptor_complete(struct dw_dma_chan *dwc, struct dw_desc *desc,
				    bool callback_required)
{
	unsigned long flags;
	struct dw_desc *child;
	struct dmaengine_desc_callback cb;
	struct dma_async_tx_descriptor *txd = &desc->txd;
	struct dw_dma *dw = to_dw_dma(dwc->chan.device);

	spin_lock_irqsave(&dwc->lock, flags);
	dma_cookie_complete(txd);
	dwc->status &= ~DWC_CH_INTSIG_DMA_TRA_DONE;
	if (callback_required)
		dmaengine_desc_get_callback(txd, &cb);
	else
		memset(&cb, 0, sizeof(cb));

	/* async_tx_ack */
	list_for_each_entry(child, &desc->tx_list, desc_node)
		async_tx_ack(&child->txd);
	async_tx_ack(&desc->txd);
	dwc_desc_put(dwc, desc);

#ifdef CONFIG_SYSDMA_JIRA_BUG_BM1880_17
	fix_dma_bug_copy_put(dwc, &dwc->chan);
#endif

	dmaengine_desc_callback_invoke(&cb, NULL);
	dwc_unprepare_clk(dw);

	spin_unlock_irqrestore(&dwc->lock, flags);
}

static void dwc_complete_all(struct dw_dma *dw, struct dw_dma_chan *dwc)
{
	unsigned long flags;
	struct dw_desc *desc, *_desc;

	LIST_HEAD(list);

	spin_lock_irqsave(&dwc->lock, flags);

	/*
	 * Submit queued descriptors ASAP, i.e. before we go through
	 * the completed ones.
	 */
	list_splice_init(&dwc->active_list, &list);
	dwc_dostart_first_queued(dwc);

	spin_unlock_irqrestore(&dwc->lock, flags);

	list_for_each_entry_safe(desc, _desc, &list, desc_node)
		dwc_descriptor_complete(dwc, desc, true);
}

/* Returns how many bytes were already received from source */
static inline u32 dwc_get_sent(struct dw_dma_chan *dwc)
{
	u64 bts = channel_readq(dwc, BLOCK_TS);
	u64 ctl = channel_readq(dwc, CTL);

	return ((bts & DWC_BLOCK_TS_MASK) + 1) * (1 << (ctl >> 8 & 7));
}

static inline void dwc_chan_disable(struct dw_dma *dw, struct dw_dma_chan *dwc)
{
	u64 dma_ch_en;

	dma_ch_en = dma_readq(dw, CH_EN);
	dma_ch_en |= (dwc->mask << DW_DMAC_CH_EN_WE_OFFSET);
	dma_ch_en |= (dwc->mask << DW_DMAC_CH_ABORT_WE_OFFSET);
	dma_ch_en |= (dwc->mask << DW_DMAC_CH_ABORT_OFFSET);
	dma_ch_en &= ~dwc->mask;
	dma_writeq(dw, CH_EN, dma_ch_en);
	while (dma_readq(dw, CH_EN) & dwc->mask)
		cpu_relax();
}

static int dwc_resume(struct dma_chan *chan)
{
	unsigned long flags;
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);
	struct dw_dma *dw = to_dw_dma(dwc->chan.device);

	spin_lock_irqsave(&dwc->lock, flags);

	if (test_bit(DW_DMA_IS_PAUSED, &dwc->flags)) {
		dma_clear_bit(dw, CH_EN,
			      (1 << (__ffs(dwc->mask) + DW_DMAC_CH_PAUSE_OFFSET)));

		clear_bit(DW_DMA_IS_PAUSED, &dwc->flags);
	}
	spin_unlock_irqrestore(&dwc->lock, flags);

	return 0;
}

static int dwc_pause(struct dma_chan *chan)
{
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);
	struct dw_dma *dw = to_dw_dma(dwc->chan.device);

	unsigned long flags;
	unsigned int count = 20; /* timeout iterations */
	u64 dma_ch_en;

	spin_lock_irqsave(&dwc->lock, flags);

	dma_set_bit(dw, CH_EN,
		    (1 << (__ffs(dwc->mask) + DW_DMAC_CH_PAUSE_OFFSET))
		    | (1 << (__ffs(dwc->mask) + DW_DMAC_CH_PAUSE_EN_OFFSET)));

	while (!(dma_readq(dw, CH_EN)
		 & (1 << (__ffs(dwc->mask) + DW_DMAC_CH_PAUSE_OFFSET)))
	       && count--)
		udelay(2);

	dma_ch_en = dma_readq(dw, CH_EN);
	dma_ch_en |= (dwc->mask << DW_DMAC_CH_EN_WE_OFFSET);
	dma_ch_en &= ~dwc->mask;
	dma_writeq(dw, CH_EN, dma_ch_en);

	set_bit(DW_DMA_IS_PAUSED, &dwc->flags);

	spin_unlock_irqrestore(&dwc->lock, flags);

	return 0;
}

static void dwc_scan_descriptors(struct dw_dma *dw, struct dw_dma_chan *dwc)
{
	dma_addr_t llp;
	unsigned long flags;
	u64 status_xfer;
	struct dw_desc *desc, *_desc;
	struct dw_desc *child;

	spin_lock_irqsave(&dwc->lock, flags);
	llp = channel_readq(dwc, LLP);
	status_xfer = dma_readq(dw, CH_EN);

	if (~(status_xfer & 0xff) & dwc->mask) {
		spin_unlock_irqrestore(&dwc->lock, flags);
		dwc_complete_all(dw, dwc);
		return;
	}

	if (list_empty(&dwc->active_list)) {
		spin_unlock_irqrestore(&dwc->lock, flags);
		return;
	}

	list_for_each_entry_safe(desc, _desc, &dwc->active_list, desc_node) {
		/* Initial residue value */
		desc->residue = desc->total_len;
		/* Check first descriptors addr */
		if (desc->txd.phys == DWC_LLP_LOC(llp)) {
			spin_unlock_irqrestore(&dwc->lock, flags);
			return;
		}

		/* Check first descriptors llp */
		if (lli_read(desc, llp) == llp) {
			/* This one is currently in progress */
			desc->residue -= dwc_get_sent(dwc);
			spin_unlock_irqrestore(&dwc->lock, flags);
			return;
		}

		desc->residue -= desc->len;
		list_for_each_entry(child, &desc->tx_list, desc_node) {
			if (lli_read(child, llp) == llp) {
				/* Currently in progress */
				desc->residue -= dwc_get_sent(dwc);
				spin_unlock_irqrestore(&dwc->lock, flags);
				return;
			}
			desc->residue -= child->len;
		}

		/*
		 * No descriptors so far seem to be in progress, i.e.
		 * this one must be done.
		 */
		spin_unlock_irqrestore(&dwc->lock, flags);
		dwc_descriptor_complete(dwc, desc, true);
		spin_lock_irqsave(&dwc->lock, flags);
	}

	dev_err(chan2dev(&dwc->chan),
		"BUG: All descriptors done, but channel not idle!\n");

	/* Try to continue after resetting the channel... */
	dwc_chan_disable(dw, dwc);

	dwc_dostart_first_queued(dwc);
	spin_unlock_irqrestore(&dwc->lock, flags);
}

/*----------------------------------------------------------------------*/
static void dwc_issue_pending(struct dma_chan *chan)
{
	unsigned long flags;
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);

	if (test_bit(DW_DMA_IS_CYCLIC, &dwc->flags)) {
		dw_dma_cyclic_start(chan);
		return;
	}

	spin_lock_irqsave(&dwc->lock, flags);
	if (list_empty(&dwc->active_list))
		dwc_dostart_first_queued(dwc);
	spin_unlock_irqrestore(&dwc->lock, flags);

}

static int dwc_stop_cyclic_all(struct dw_dma_chan *dwc)
{
	int i;
	unsigned long flags;
	struct dw_desc *desc;
	struct dma_async_tx_descriptor *txd;
	struct dw_dma *dw = to_dw_dma(dwc->chan.device);

	desc = dwc->cdesc->desc[0];
	txd = &desc->txd;

	spin_lock_irqsave(&dwc->lock, flags);
	dma_cookie_complete(txd);
	dwc->status &= ~DWC_CH_INTSIG_DMA_TRA_DONE;

	clear_bit(DW_DMA_IS_CYCLIC, &dwc->flags);

	for (i = 0; i < dwc->cdesc->periods; i++)
		dwc_desc_put(dwc, dwc->cdesc->desc[i]);

	kfree(dwc->cdesc->desc);
	kfree(dwc->cdesc);

	dwc_unprepare_clk(dw);
	spin_unlock_irqrestore(&dwc->lock, flags);

	return 0;
}

static int dwc_terminate_all(struct dma_chan *chan)
{
	unsigned long flags;
	struct dw_desc *desc, *_desc;
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);
	struct dw_dma *dw = to_dw_dma(chan->device);

	LIST_HEAD(list);

	spin_lock_irqsave(&dwc->lock, flags);
	dwc_chan_disable(dw, dwc);
	spin_unlock_irqrestore(&dwc->lock, flags);

	dwc_resume(chan);

	if (test_bit(DW_DMA_IS_CYCLIC, &dwc->flags))
		return dwc_stop_cyclic_all(dwc);

	/* active_list entries will end up before queued entries */
	spin_lock_irqsave(&dwc->lock, flags);
	list_splice_init(&dwc->queue, &list);
	list_splice_init(&dwc->active_list, &list);
	spin_unlock_irqrestore(&dwc->lock, flags);

	/* Flush all pending and queued descriptors */
	list_for_each_entry_safe(desc, _desc, &list, desc_node)
		dwc_descriptor_complete(dwc, desc, false);

	return 0;
}

static int dwc_config(struct dma_chan *chan, struct dma_slave_config *sconfig)
{
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);

	/* Check if chan will be configured for slave transfers */
	if (!is_slave_direction(sconfig->direction))
		return -EINVAL;

	memcpy(&dwc->dma_sconfig, sconfig, sizeof(*sconfig));
	dwc->direction = sconfig->direction;

	convert_burst(&dwc->dma_sconfig.src_maxburst);
	convert_burst(&dwc->dma_sconfig.dst_maxburst);

	return 0;
}

static struct dw_desc *dwc_find_desc(struct dw_dma_chan *dwc, dma_cookie_t c)
{
	struct dw_desc *desc;

	list_for_each_entry(desc, &dwc->active_list, desc_node)
		if (desc->txd.cookie == c)
			return desc;

	return NULL;
}

static u32 dwc_get_residue(struct dw_dma_chan *dwc, dma_cookie_t cookie)
{
	u32 residue;
	unsigned long flags;
	struct dw_desc *desc;

	spin_lock_irqsave(&dwc->lock, flags);

	desc = dwc_find_desc(dwc, cookie);
	if (desc)
		if (desc == dwc_first_active(dwc))
			residue = desc->residue;
		else
			residue = desc->total_len;
	else
		residue = 0;

	spin_unlock_irqrestore(&dwc->lock, flags);

	return residue;
}

static enum dma_status dwc_tx_status(struct dma_chan *chan, dma_cookie_t cookie,
				     struct dma_tx_state *txstate)
{
	enum dma_status ret;
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);

	ret = dma_cookie_status(chan, cookie, txstate);
	if (ret == DMA_COMPLETE)
		return ret;

	if (test_bit(DW_DMA_IS_CYCLIC, &dwc->flags)) {
		if (cookie == dwc->cdesc->desc[0]->txd.cookie) {
			dma_set_residue(txstate, dwc->cdesc->desc[0]->residue);
			return ret;
		}
		dma_set_residue(txstate, dwc->cdesc->desc[0]->total_len);
		return ret;
	}

	dwc_scan_descriptors(to_dw_dma(chan->device), dwc);

	ret = dma_cookie_status(chan, cookie, txstate);
	if (ret == DMA_COMPLETE)
		return ret;

	dma_set_residue(txstate, dwc_get_residue(dwc, cookie));

	if (test_bit(DW_DMA_IS_PAUSED, &dwc->flags) && ret == DMA_IN_PROGRESS)
		return DMA_PAUSED;

	return ret;
}

static struct dma_async_tx_descriptor *dwc_prep_dma_memcpy(struct dma_chan *chan,
		dma_addr_t dest, dma_addr_t src,
		size_t len, unsigned long flags)
{
	u8 m_master;
	u64 ctl = 0;
	unsigned int trans_width;
	unsigned int data_width;
	size_t offset = 0;
	size_t xfer_count = 0;
	struct dw_dma *dw = to_dw_dma(chan->device);
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);
	struct dw_desc *desc;
	struct dw_desc *first;
	struct dw_desc *prev;
	unsigned long spin_flags;
	u32 trans_block;

	m_master = dwc->dws.m_master;
	data_width = dw->data_width[m_master];

	trans_block = dwc->block_size;

#ifdef DBG_DMA
	pr_err("%s trans_block %d, len 0x%x\n", __func__, trans_block, (int)len);
#endif

	if (unlikely(!len))
		return NULL;

	spin_lock_irqsave(&dwc->lock, spin_flags);
	dwc_prepare_clk(dw);
	spin_unlock_irqrestore(&dwc->lock, spin_flags);


	dwc->direction = DMA_MEM_TO_MEM;

	/*
	 * for mem2mem type we defaultly set the src/dst width bits
	 * to max value 32(axi bus width) consider of performence,
	 * you can change it by dts.
	 */

	trans_width = __ffs(data_width | src | dest | len);
	ctl = DWC_DEFAULT_CTL(chan)
	      | DWC_CTL_DST_WIDTH(trans_width)
	      | DWC_CTL_SRC_WIDTH(trans_width)
	      | DWC_CTL_DST_INC
	      | DWC_CTL_SRC_INC
	      | DWC_CTL_DST_STA_EN
	      | DWC_CTL_SRC_STA_EN;

	/* axi bus max width is 32bits == 4 bytes
	 * dma max block ts is 32, for one dma transfer, the max data size
	 * can be transferred is 32 * 4 bytes
	 */
	prev = first = NULL;
	for (offset = 0; offset < len; offset += xfer_count << trans_width) {
		xfer_count = min_t(size_t, (len - offset) >> trans_width,
				   trans_block >> trans_width);
		desc = dwc_desc_get(dwc);
		if (!desc)
			goto err_desc_get;
		lli_write(desc, sar, src + offset);
		lli_write(desc, dar, dest + offset);
		lli_write(desc, ctl, ctl | DWC_CTL_SHADOWREG_OR_LLI_VALID);
		lli_write(desc, block_ts, xfer_count - 1);
		desc->len = xfer_count << trans_width;
		if (!first) {
			first = desc;
		} else {
			lli_write(prev, llp, desc->txd.phys);
			list_add_tail(&desc->desc_node, &first->tx_list);
		}
		prev = desc;
	}
	/* set the final block in lli,
	 *dma end this transfer wen the current block completes
	 */
	lli_set(prev, ctl, DWC_CTL_SHADOWREG_OR_LLI_LAST);

	if (flags & DMA_PREP_INTERRUPT) {
		/* Trigger interrupt after last block */
		lli_set(prev, ctl, DWC_CTL_IOC_BLT_EN);
	}

	prev->lli.llp = 0;
	first->txd.flags = flags;
	first->total_len = len;

	return &first->txd;

err_desc_get:
	dwc_desc_put(dwc, first);
	return NULL;
}

static struct dma_async_tx_descriptor *dwc_prep_dma_slave_sg(struct dma_chan *chan,
		struct scatterlist *sgl, unsigned int sg_len, enum dma_transfer_direction direction,
		unsigned long flags, void *context)
{
	u8 m_master;
	u32 src_max_burst;
	u32 dst_max_burst;
	u32 trans_block;
	u64 ctl = 0;
	unsigned int reg_width;
	unsigned int mem_width;
	unsigned int data_width;
	unsigned int i;
	size_t total_len = 0;
	dma_addr_t reg;
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);
	struct dw_dma *dw = to_dw_dma(chan->device);
	struct dma_slave_config *sconfig = &dwc->dma_sconfig;
	struct dw_desc *prev;
	struct dw_desc *first;
	struct scatterlist *sg;
	unsigned long spin_flags;
	u64 tmp;

	spin_lock_irqsave(&dwc->lock, spin_flags);
	dwc_prepare_clk(dw);
	spin_unlock_irqrestore(&dwc->lock, spin_flags);

	m_master = dwc->dws.m_master;
	src_max_burst = sconfig->src_maxburst;
	dst_max_burst = sconfig->dst_maxburst;
	trans_block = dwc->block_size;
	data_width = dw->data_width[m_master];

	if (unlikely(!is_slave_direction(direction) || !sg_len))
		return NULL;

	dwc->direction = direction;

	prev = first = NULL;
	switch (direction) {
	case DMA_MEM_TO_DEV:
		reg_width = __ffs(sconfig->dst_addr_width);
		reg = sconfig->dst_addr;
		ctl = (DWC_DEFAULT_CTL(chan)
		       | DWC_CTL_DST_WIDTH(reg_width)
		       | DWC_CTL_DST_FIX
		       | DWC_CTL_SRC_INC
		       | DWC_CTL_DST_STA_EN
		       | DWC_CTL_SRC_STA_EN);

		for_each_sg(sgl, sg, sg_len, i) {
			struct dw_desc *desc;
			u64 mem, len, dlen, tmp_len;

			mem = sg_dma_address(sg);
			len = sg_dma_len(sg);
			/* bug fix axi fix 32bits m2d,
			 * reg_width <0: 8bits,1:16bits,2:32bits>
			 */
#ifdef CONFIG_SYSDMA_JIRA_BUG_BM1880_17
			if (reg_width != 0x3)
				mem = (u64)fix_dma_bug_copy_get(dwc, chan, sg, i, sg_len, len);
			tmp_len = len * (4 >> reg_width);
#endif
			tmp_len = len;

#ifdef CONFIG_SYSDMA_JIRA_BUG_BM1880_17
			mem_width = 2;
#else
			mem_width = __ffs(data_width | mem | len);
#endif

slave_sg_todev_fill_desc:
			desc = dwc_desc_get(dwc);
			if (!desc)
				goto err_desc_get;
			lli_write(desc, sar, mem);
			lli_write(desc, dar, reg);
			lli_write(desc, ctl, ctl |
				  (DWC_CTL_SRC_WIDTH(mem_width) | DWC_CTL_SHADOWREG_OR_LLI_VALID));
			if (tmp_len >> mem_width > trans_block) {
				dlen = trans_block << mem_width;
				mem += dlen;
				tmp_len -= dlen;
			} else {
				dlen = tmp_len;
				tmp_len = 0;
			}
			/* single trans when dst_max_burst == 0*/
			lli_write(desc, block_ts, !dst_max_burst ? 0 : ((dlen >> mem_width) - 1));
			//lli_write(desc, block_ts, dlen);
			desc->len = dlen;
			div_u64(desc->len, (4 >> reg_width));

			if (!first) {
				first = desc;
			} else {
				lli_write(prev, llp, desc->txd.phys);
				list_add_tail(&desc->desc_node, &first->tx_list);
			}
			prev = desc;
			tmp = dlen;
			div_u64(tmp, (4 >> reg_width));
			total_len += tmp;

			if (tmp_len)
				goto slave_sg_todev_fill_desc;
		}
		break;
	case DMA_DEV_TO_MEM:
		reg_width = __ffs(sconfig->src_addr_width);
		reg = sconfig->src_addr;
		ctl = (DWC_DEFAULT_CTL(chan)
		       | DWC_CTL_SRC_WIDTH(reg_width)
		       | DWC_CTL_DST_INC
		       | DWC_CTL_SRC_FIX
		       | DWC_CTL_DST_STA_EN
		       | DWC_CTL_SRC_STA_EN);

		for_each_sg(sgl, sg, sg_len, i) {
			struct dw_desc	*desc;
			u64 len = 0, dlen = 0, mem = 0;

			mem = sg_dma_address(sg);
			len = sg_dma_len(sg);
#ifdef CONFIG_SYSDMA_JIRA_BUG_BM1880_17
			mem_width = 2;
#else
			mem_width = __ffs(data_width | mem | len);
#endif

slave_sg_fromdev_fill_desc:
			desc = dwc_desc_get(dwc);
			if (!desc)
				goto err_desc_get;

			lli_write(desc, sar, reg);
			lli_write(desc, dar, mem);
			ctl |= (DWC_CTL_DST_WIDTH(mem_width) | DWC_CTL_SHADOWREG_OR_LLI_VALID);
			lli_write(desc, ctl, ctl);
			if ((len >> reg_width) > trans_block) {
				dlen = (trans_block << reg_width);
#ifdef CONFIG_SYSDMA_JIRA_BUG_BM1880_17
				mem += dlen * (4 >> reg_width);
#else
				mem += dlen;
#endif
				len -= dlen;
			} else {
				dlen = len;
				len = 0;
			}

			lli_write(desc, block_ts, !src_max_burst ? 0 : (dlen >> reg_width) - 1);
			desc->len = dlen;

			if (!first) {
				first = desc;
			} else {
				lli_write(prev, llp, desc->txd.phys);
				list_add_tail(&desc->desc_node, &first->tx_list);
			}
			prev = desc;
			total_len += dlen;

			if (len)
				goto slave_sg_fromdev_fill_desc;
		}
		break;
	default:
		return NULL;
	}

	/* set the final block in lli,
	 * dma end this transfer wen the current block completes
	 */
	lli_set(prev, ctl, DWC_CTL_SHADOWREG_OR_LLI_LAST);

	if (flags & DMA_PREP_INTERRUPT)
		/* Trigger interrupt after last block */
		lli_set(prev, ctl, DWC_CTL_IOC_BLT_EN);

	prev->lli.llp = 0;
	first->total_len = total_len;

	return &first->txd;

err_desc_get:
	dev_err(chan2dev(chan),
		"not enough descriptors available. Direction %d\n", direction);
	dwc_desc_put(dwc, first);
	return NULL;
}

/* --------------------- Cyclic DMA API extensions -------------------- */

/**
 * dw_dma_cyclic_start - start the cyclic DMA transfer
 * @chan: the DMA channel to start
 *
 * Must be called with soft interrupts disabled. Returns zero on success or
 * -errno on failure.
 */
static int dw_dma_cyclic_start(struct dma_chan *chan)
{
	unsigned long flags;
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);

	if (!test_bit(DW_DMA_IS_CYCLIC, &dwc->flags)) {
		dev_err(chan2dev(&dwc->chan), "missing prep for cyclic DMA\n");
		return -ENODEV;
	}

	spin_lock_irqsave(&dwc->lock, flags);

	/* Enable interrupts to perform cyclic transfer */
	//dwc_interrupts_set(dwc, true);

	dwc_dostart(dwc, dwc->cdesc->desc[0]);

	spin_unlock_irqrestore(&dwc->lock, flags);

	return 0;
}

/**
 * dw_dma_cyclic_stop - stop the cyclic DMA transfer
 * @chan: the DMA channel to stop
 *
 * Must be called with soft interrupts disabled.
 */
void dw_dma_cyclic_stop(struct dma_chan *chan)
{
	unsigned long flags;
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);
	struct dw_dma *dw = to_dw_dma(dwc->chan.device);

	spin_lock_irqsave(&dwc->lock, flags);

	dwc_chan_disable(dw, dwc);

	spin_unlock_irqrestore(&dwc->lock, flags);
}

/**
 * dw_dma_cyclic_prep - prepare the cyclic DMA transfer
 * @chan: the DMA channel to prepare
 * @buf_addr: physical DMA address where the buffer starts
 * @buf_len: total number of bytes for the entire buffer
 * @period_len: number of bytes for each period
 * @direction: transfer direction, to or from device
 *
 * Must be called before trying to start the transfer. Returns a valid struct
 * dw_cyclic_desc if successful or an ERR_PTR(-errno) if not successful.
 */
static struct dma_async_tx_descriptor *dwc_prep_dma_cyclic(struct dma_chan *chan,
		dma_addr_t buf_addr, size_t buf_len, size_t period_len,
		enum dma_transfer_direction direction,
		unsigned long dma_flags)
{
	unsigned int i;
	unsigned int reg_width;
	unsigned int periods;
	unsigned long flags;
	unsigned long was_cyclic;
	struct dw_cyclic_desc *cdesc;
	struct dw_cyclic_desc *retval = NULL;
	struct dw_desc *desc;
	struct dw_desc *last = NULL;
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);
	struct dma_slave_config *sconfig = &dwc->dma_sconfig;
	unsigned int tmp_len;
	size_t target_period_len;
	u64 ctl_val = 0;
	struct dw_dma *dw = to_dw_dma(chan->device);

	spin_lock_irqsave(&dwc->lock, flags);
	dwc_prepare_clk(dw);

	if (!list_empty(&dwc->queue) || !list_empty(&dwc->active_list)) {
		spin_unlock_irqrestore(&dwc->lock, flags);
		dev_dbg(chan2dev(&dwc->chan),
			"queue and/or active list are not empty\n");
		return ERR_PTR(-EBUSY);
	}

	was_cyclic = test_and_set_bit(DW_DMA_IS_CYCLIC, &dwc->flags);
	spin_unlock_irqrestore(&dwc->lock, flags);
	if (was_cyclic) {
		dev_dbg(chan2dev(&dwc->chan),
			"channel already prepared for cyclic DMA\n");
		return ERR_PTR(-EBUSY);
	}

	retval = ERR_PTR(-EINVAL);

	if (unlikely(!is_slave_direction(direction)))
		goto out_err;

	dwc->direction = direction;

	if (direction == DMA_MEM_TO_DEV)
		reg_width = __ffs(sconfig->dst_addr_width);
	else
		reg_width = __ffs(sconfig->src_addr_width);

	/* Change period to 128 bytes due to the maximum LLI data length*/
	/*of Bitmain sysDMA (32 bytes * 4 bytes bus width)*/
	tmp_len = (1 << (__ffs(buf_len) - 1));
	target_period_len = period_len;
	if (period_len > BM_DMA_PERIOD_LEN)
		period_len = BM_DMA_PERIOD_LEN;

	if (tmp_len < period_len)
		period_len = tmp_len;

	periods = buf_len / period_len;

	/* Check for too big/unaligned periods and unaligned DMA buffer. */
	if (period_len > (dwc->block_size << reg_width))
		goto out_err;
	if (unlikely(period_len & ((1 << reg_width) - 1)))
		goto out_err;
	if (unlikely(buf_addr & ((1 << reg_width) - 1)))
		goto out_err;

	retval = ERR_PTR(-ENOMEM);

	cdesc = kzalloc(sizeof(struct dw_cyclic_desc), GFP_ATOMIC);
	if (!cdesc)
		goto out_err;
	cdesc->desc = kcalloc(periods, sizeof(struct dw_desc *), GFP_ATOMIC);
	if (!cdesc->desc)
		goto out_err_alloc;

	for (i = 0; i < periods; i++) {
		desc = dwc_desc_get(dwc);
		if (!desc)
			goto out_err_desc_get;

		switch (direction) {
		case DMA_MEM_TO_DEV:
			lli_write(desc, dar, sconfig->dst_addr);
			lli_write(desc, sar, buf_addr + period_len * i);
			ctl_val = (DWC_DEFAULT_CTL(chan)
#if defined(CONFIG_ARCH_CVITEK)
				   | DWC_CTL_ARLEN_EN
				   | DWC_CTL_ARLEN(7UL)
				   | DWC_CTL_AWLEN_EN
				   | DWC_CTL_AWLEN(0UL)
#endif
				   | DWC_CTL_SHADOWREG_OR_LLI_VALID
				   | DWC_CTL_DST_WIDTH(reg_width)
				   | DWC_CTL_SRC_WIDTH(reg_width)
				   | DWC_CTL_DST_FIX
				   | DWC_CTL_SRC_INC);
			lli_write(desc, ctl, ctl_val);

			if ((period_len * (i + 1)) % target_period_len == 0)
				lli_set(desc, ctl, DWC_CTL_IOC_BLT_EN);
			break;
		case DMA_DEV_TO_MEM:
			lli_write(desc, dar, buf_addr + period_len * i);
			lli_write(desc, sar, sconfig->src_addr);
			ctl_val = (DWC_DEFAULT_CTL(chan)
#if defined(CONFIG_ARCH_CVITEK)
				   | DWC_CTL_ARLEN_EN
				   | DWC_CTL_ARLEN(0UL)
				   | DWC_CTL_AWLEN_EN
				   | DWC_CTL_AWLEN(7UL)
#endif
				   | DWC_CTL_SHADOWREG_OR_LLI_VALID
				   | DWC_CTL_SRC_WIDTH(reg_width)
				   | DWC_CTL_DST_WIDTH(reg_width)
				   | DWC_CTL_DST_INC
				   | DWC_CTL_SRC_FIX);

			lli_write(desc, ctl, ctl_val);

			if ((period_len * (i + 1)) % target_period_len == 0)
				lli_set(desc, ctl, DWC_CTL_IOC_BLT_EN);
			break;
		default:
			break;
		}

		lli_write(desc, block_ts, (period_len >> reg_width) - 1);
		cdesc->desc[i] = desc;

		if (last) {
			lli_write(last, llp, desc->txd.phys);
			last->total_len = target_period_len;
		}

		last = desc;
	}

	/* Let's make a cyclic list */
	lli_write(last, llp, cdesc->desc[0]->txd.phys);

	/* Trigger interrupt for last block */
	lli_set(last, ctl, DWC_CTL_IOC_BLT_EN);

	if (dw->log_on)
		dev_info(chan2dev(&dwc->chan),
			 "cyclic prepared buf 0x%llx len %zu period %zu periods %d\n",
			 buf_addr, buf_len, period_len, periods);

	cdesc->desc[0]->total_len = buf_len;
	cdesc->desc[0]->hw_pos = 0;
	cdesc->desc[0]->residue = buf_len;
	cdesc->periods = periods;
	cdesc->period_len = target_period_len;
	cdesc->last_sent = 0;
	dwc->cdesc = cdesc;
	dwc->hw_pos = 0;
	dwc->interrupt_count = 0;

	return &cdesc->desc[0]->txd;

out_err_desc_get:
	while (i--)
		dwc_desc_put(dwc, cdesc->desc[i]);
out_err_alloc:
	kfree(cdesc->desc);
out_err:
	kfree(cdesc);
	clear_bit(DW_DMA_IS_CYCLIC, &dwc->flags);

	return (struct dma_async_tx_descriptor *)retval;
}


static void dwc_handle_cyclic(struct dw_dma *dw, struct dw_dma_chan *dwc)
{
	dma_addr_t llp;
	int i;
	u32 sent_total = 0;
	struct dw_desc *desc, *first;
	struct dmaengine_desc_callback cb;
	u32 new_hw_pos = 0;

	llp = channel_readq(dwc, LLP);
	first = dwc->cdesc->desc[0];
	sent_total += dwc->cdesc->last_sent;

	if (first->txd.phys != DWC_LLP_LOC(llp)) {
		for (i = 1; i < dwc->cdesc->periods; i++) {
			desc = dwc->cdesc->desc[i];
			new_hw_pos += dwc_get_sent(dwc);
			if (desc->txd.phys == DWC_LLP_LOC(llp))
				break;
		}
	} else
		new_hw_pos = 0; /* back to ring buffer head */

	if (new_hw_pos >= first->hw_pos)
		sent_total += new_hw_pos - first->hw_pos;
	else
		sent_total += first->total_len + new_hw_pos - first->hw_pos;

	first->hw_pos = new_hw_pos;
	first->residue = first->total_len - (new_hw_pos - new_hw_pos % dwc->cdesc->period_len);
	dwc->hw_pos = new_hw_pos;
	dwc->interrupt_count++;

	if (sent_total > dwc->cdesc->period_len) {
		dmaengine_desc_get_callback(&first->txd, &cb);
		dmaengine_desc_callback_invoke(&cb, NULL);
		dwc->cdesc->last_sent = new_hw_pos % dwc->cdesc->period_len;
	} else {
		dwc->cdesc->last_sent = sent_total;
	}

	if (dw->log_on)
		dev_info(chan2dev(&dwc->chan),
			 "SAR:0x%llx DAR:0x%llx residue:%d sent_total:%d ch_status:0x%llx int_status:0x%llx\n",
			 channel_readq(dwc, SAR), channel_readq(dwc, DAR), first->residue, sent_total,
			 channel_readq(dwc, STATUS), channel_readq(dwc, INTSTATUS));

	/* TODO error resuem */
}

/**
 * dw_dma_cyclic_free - free a prepared cyclic DMA transfer
 * @chan: the DMA channel to free
 */
void dw_dma_cyclic_free(struct dma_chan *chan)
{
	unsigned int i;
	unsigned long flags;
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);
	struct dw_dma *dw = to_dw_dma(dwc->chan.device);
	struct dw_cyclic_desc *cdesc = dwc->cdesc;

	dev_dbg(chan2dev(&dwc->chan), "%s\n", __func__);

	if (!cdesc)
		return;

	spin_lock_irqsave(&dwc->lock, flags);

	dwc_chan_disable(dw, dwc);

	//dwc_interrupts_set(dwc, false);

	spin_unlock_irqrestore(&dwc->lock, flags);

	for (i = 0; i < cdesc->periods; i++)
		dwc_desc_put(dwc, cdesc->desc[i]);

	kfree(cdesc->desc);
	kfree(cdesc);

	dwc->cdesc = NULL;

	clear_bit(DW_DMA_IS_CYCLIC, &dwc->flags);
}

static void dwc_free_chan_resources(struct dma_chan *chan)
{
	unsigned long flags;
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);
	struct dw_dma *dw = to_dw_dma(chan->device);

	LIST_HEAD(list);

	/* ASSERT:  channel is idle */
	WARN_ON(!list_empty(&dwc->active_list));
	WARN_ON(!list_empty(&dwc->queue));
	WARN_ON(dma_readq(to_dw_dma(chan->device), CH_EN) & dwc->mask);

	spin_lock_irqsave(&dwc->lock, flags);

	/* Clear custom channel configuration */
	memset(&dwc->dws, 0, sizeof(struct dw_dma_slave));

	clear_bit(DW_DMA_IS_INITIALIZED, &dwc->flags);

	/* Disable interrupts */
	channel_writeq(dwc, INTSTATUS_ENABLEREG, 0);
	//dwc_interrupts_set(dwc, false);

	/* Disable controller in case it was a last user */
	dw->in_use &= ~dwc->mask;
	dwc->status &= ~DWC_CH_INTSIG_DMA_TRA_DONE;
	if (!dw->in_use)
		dw_dma_off(dw);

	spin_unlock_irqrestore(&dwc->lock, flags);
}

static int dwc_alloc_chan_resources(struct dma_chan *chan)
{
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);
	struct dw_dma *dw = to_dw_dma(chan->device);
	unsigned long flags;

	spin_lock_irqsave(&dwc->lock, flags);
	/* ASSERT:  channel is idle */
	if (dma_readq(dw, CH_EN) & dwc->mask)
		return -EIO;

	dma_cookie_init(chan);

	/*
	 * NOTE: some controllers may have additional features that we
	 * need to initialize here, like "scatter-gather" (which
	 * doesn't mean what you think it means), and status writeback.
	 */

	/*
	 * We need controller-specific data to set up slave transfers.
	 */
	if (chan->private && !dw_dma_filter(chan, chan->private)) {
		dev_warn(chan2dev(chan), "Wrong controller-specific data\n");
		return -EINVAL;
	}

	/* Enable controller here if needed */

	if (!dw->in_use)
		dw_dma_on(dw);

	dw->in_use |= dwc->mask;
	spin_unlock_irqrestore(&dwc->lock, flags);

	return 0;
}

#ifndef DO_NOT_USE_TASK
static void dw_dma_tasklet(unsigned long data)
{
	unsigned int i;
	struct dw_dma *dw = (struct dw_dma *)data;
	struct dw_dma_chan *dwc;
	u64 ch_en;

	ch_en = dma_readq(dw, CH_EN);

	for (i = 0; i < dw->dma.chancnt; i++) {

		dwc = &dw->chan[i];
		if (test_bit(DW_DMA_IS_CYCLIC, &dwc->flags)) {
			if ((ch_en >> i) & 0x1)
				dwc_handle_cyclic(dw, dwc);
		} else{
		    if(dwc->status == DWC_CH_INTSIG_DMA_TRA_DONE)
				dwc_scan_descriptors(dw, dwc);
        }
		//dwc_interrupts_set(dwc, true);
	}
}
#endif


static void dw_dma_off(struct dw_dma *dw)
{
	unsigned int i;

	dma_writeq(dw, CFG, 0); /* disable dmac and interrupt */

	/* Clear all interrupts on all channels. */
#if 0
	for (i = 0; i < dw->dma.chancnt; i++)
		dwc_interrupts_set(&dw->chan[i], false);
#endif

	//while (dma_readq(dw, CFG) & DW_CFG_DMA_EN)
	//	cpu_relax();

	for (i = 0; i < dw->dma.chancnt; i++)
		clear_bit(DW_DMA_IS_INITIALIZED, &dw->chan[i].flags);
}

static void dw_dma_on(struct dw_dma *dw)
{
	dma_writeq(dw, CFG, DW_CFG_DMA_EN | DW_CFG_DMA_INT_EN);
}

#ifdef DO_NOT_USE_TASK
static void instead_of_tasklet(struct dw_dma *dw)
{
	unsigned int i;
	struct dw_dma_chan *dwc;
	u64 ch_en;

	ch_en = dma_readq(dw, CH_EN);

	for (i = 0; i < dw->dma.chancnt; i++) {
		dwc = &dw->chan[i];
		if (test_bit(DW_DMA_IS_CYCLIC, &dwc->flags)) {
			if ((ch_en >> i) & 0x1)
				dwc_handle_cyclic(dw, dwc);
		} else
			dwc_scan_descriptors(dw, dwc);
		//dwc_interrupts_set(dwc, true);
	}
}
#endif

static irqreturn_t dw_dma_interrupt(int irq, void *dev_id)
{
	int i;
	u64 status,dwc_status;
	struct dw_dma *dw = dev_id;
	struct dw_dma_chan *dwc;

	/* Check if we have any interrupt from the DMAC which is not in use */
	if (!dw->in_use)
		return IRQ_NONE;

	status = dma_readq(dw, INTSTATUS);
	/* Check if we have any interrupt from the DMAC */
	if (!status)
		return IRQ_NONE;


	dma_writeq(dw, COMM_INTCLEAR, 0x10f); /* clear all common interrupts */
	for (i = 0; i < dw->dma.chancnt; i++) {
		dwc = &dw->chan[i];
		dwc_status = channel_readq(dwc, INTSTATUS);
		if(dwc_status == DWC_CH_INTSIG_DMA_TRA_DONE)
			dwc->status = DWC_CH_INTSIG_DMA_TRA_DONE;
		channel_writeq(dwc, INTCLEARREG, dwc_status);
		//dwc_interrupts_set(dwc, false);
	}

	status = dma_readq(dw, INTSTATUS);
	if (status) {
		dev_dbg(dw->dma.dev,
			"BUG: Unexpected interrupts pending: 0x%llx\n",
			status);
	}

#ifdef DO_NOT_USE_TASK
	instead_of_tasklet(dw);
#else
	tasklet_schedule(&dw->tasklet);
#endif
	return IRQ_HANDLED;
}

static int dw_dma_channel_init(struct platform_device *pdev,
			       struct dw_dma *dw)
{
	int i;

	dw->chan = devm_kcalloc(dw->dev, dw->nr_channels,
				sizeof(*dw->chan), GFP_KERNEL);
	if (!dw->chan)
		return -ENOMEM;

	/* Create a pool of consistent memory blocks for hardware descriptors */
	dw->desc_pool = dmam_pool_create("dw_dmac_desc_pool", dw->dev,
					 sizeof(struct dw_desc), 64, 0);
	if (!dw->desc_pool) {
		dev_err(dw->dev, "No memory for descriptors dma pool\n");
		return -ENOMEM;
	}

	INIT_LIST_HEAD(&dw->dma.channels);
	for (i = 0; i < dw->nr_channels; i++) {
		struct dw_dma_chan *dwc = &dw->chan[i];

		dwc->chan.device = &dw->dma;
		dma_cookie_init(&dwc->chan);
		if (dw->chan_allocation_order == CHAN_ALLOCATION_ASCENDING)
			list_add_tail(&dwc->chan.device_node, &dw->dma.channels);
		else
			list_add(&dwc->chan.device_node, &dw->dma.channels);

		/* 7 is highest priority & 0 is lowest. */
		if (dw->chan_priority == CHAN_PRIORITY_ASCENDING)
			dwc->priority = dw->nr_channels - i - 1;
		else
			dwc->priority = i;

		dwc->ch_regs = &__dw_regs(dw)->CHAN[i];
		spin_lock_init(&dwc->lock);
		dwc->mask = 1 << i;

		channel_writeq(dwc, INTCLEARREG, 0xffffffff);

		INIT_LIST_HEAD(&dwc->active_list);
		INIT_LIST_HEAD(&dwc->queue);

		dwc->direction = DMA_TRANS_NONE;
		dwc->block_size = dw->block_size;
		dwc->axi_tr_width = dw->block_size;
	}

	dma_clear_bit(dw, CH_EN, DW_DMA_CHAN_MASK);

	/* channel en bit set */
	dma_set_bit(dw, CH_EN, DW_DMA_CHAN_MASK << DW_DMAC_CH_EN_WE_OFFSET);
	dma_set_bit(dw, CH_EN, DW_DMA_CHAN_MASK << DW_DMAC_CH_PAUSE_EN_OFFSET);

	/* Set capabilities */
	dma_cap_set(DMA_SLAVE, dw->dma.cap_mask);
	dma_cap_set(DMA_PRIVATE, dw->dma.cap_mask);
	dma_cap_set(DMA_MEMCPY, dw->dma.cap_mask);
	dma_cap_set(DMA_CYCLIC, dw->dma.cap_mask);

	dw->dma.dev = dw->dev;
	dw->dma.device_alloc_chan_resources = dwc_alloc_chan_resources;
	dw->dma.device_free_chan_resources = dwc_free_chan_resources;

	dw->dma.device_prep_dma_memcpy = dwc_prep_dma_memcpy;
	dw->dma.device_prep_slave_sg = dwc_prep_dma_slave_sg;
	dw->dma.device_prep_dma_cyclic = dwc_prep_dma_cyclic;

	dw->dma.device_config = dwc_config;
	dw->dma.device_pause = dwc_pause;
	dw->dma.device_resume = dwc_resume;
	dw->dma.device_terminate_all = dwc_terminate_all;

	dw->dma.device_tx_status = dwc_tx_status;
	dw->dma.device_issue_pending = dwc_issue_pending;

	/* DMA capabilities */
	dw->dma.src_addr_widths = DW_DMA_BUSWIDTHS;
	dw->dma.dst_addr_widths = DW_DMA_BUSWIDTHS;
	dw->dma.directions = BIT(DMA_DEV_TO_MEM) | BIT(DMA_MEM_TO_DEV) |
			     BIT(DMA_MEM_TO_MEM);
	dw->dma.residue_granularity = DMA_RESIDUE_GRANULARITY_BURST;

	return 0;
}

static int dw_dma_parse_dt(struct platform_device *pdev,
			   struct dw_dma *dw)
{
	u32 tmp, arr[DW_DMA_MAX_NR_MASTERS];
	struct device_node *np = pdev->dev.of_node;

	if (!np) {
		dev_err(&pdev->dev, "Missing DT data\n");
		return -EINVAL;
	}

	/* set default config, can be override by dt */
	dw->nr_masters = 2;
	dw->is_private = true;
	dw->is_memcpy = true;
	dw->chan_allocation_order = CHAN_ALLOCATION_ASCENDING;
	dw->chan_priority = CHAN_PRIORITY_ASCENDING;

	of_property_read_u8(np, "dma-masters", &dw->nr_masters);
	if (dw->nr_masters < 1 || dw->nr_masters > DW_DMA_MAX_NR_MASTERS)
		return -EINVAL;

	of_property_read_u8(np, "dma-channels", &dw->nr_channels);
	if (dw->nr_channels > DW_DMA_MAX_NR_CHANNELS)
		return -EINVAL;

	if (of_property_read_bool(np, "is_private"))
		dw->is_private = true;

	of_property_read_u8(np, "chan_allocation_order",
			    &dw->chan_allocation_order);
	of_property_read_u8(np, "chan_priority", &dw->chan_priority);
	of_property_read_u32(np, "block_size", &dw->block_size);
	of_property_read_u32(np, "block-ts", &dw->block_ts);
	of_property_read_u32(np, "axi_tr_width", &dw->axi_tr_width);
	if (!of_property_read_u32_array(np, "data-width", arr, dw->nr_masters)) {
		for (tmp = 0; tmp < dw->nr_masters; tmp++)
			dw->data_width[tmp] = arr[tmp];
	}

	return 0;
}

static int dw_dma_resource_init(struct platform_device *pdev,
				struct dw_dma *dw)
{
	int err;
	struct resource *mem;

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	dw->regs = devm_ioremap_resource(&pdev->dev, mem);
	if (IS_ERR(dw->regs)) {
		err = PTR_ERR(dw->regs);
		goto err_resource;
	}

	dma_writeq(dw, RESET, 1); /* reset sysDMA */

	dw->irq = platform_get_irq(pdev, 0);
	if (dw->irq < 0)
		return -EINVAL;
	err = devm_request_irq(dw->dev, dw->irq, dw_dma_interrupt,
			       IRQF_SHARED, "dw_dmac", dw);
	if (err)
		return err;
	spin_lock_init(&dw->lock);


	err = dma_coerce_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));
	if (err)
		goto err_resource;

#ifndef DO_NOT_USE_TASK
	tasklet_init(&dw->tasklet, dw_dma_tasklet, (unsigned long)dw);
#endif

	return 0;
err_resource:
	return err;
}

static bool dw_dma_filter(struct dma_chan *chan, void *param)
{
	struct dw_dma_chan *dwc = to_dw_dma_chan(chan);
	struct dw_dma_slave *dws = param;

	if (dws->dma_dev != chan->device->dev)
		return false;

	/* We have to copy data since dws can be temporary storage */
	memcpy(&dwc->dws, dws, sizeof(struct dw_dma_slave));

	return true;
}

static struct dma_chan *dw_dma_of_xlate(struct of_phandle_args *dma_spec,
					struct of_dma *ofdma)
{
	struct dw_dma *dw = ofdma->of_dma_data;
	struct dw_dma_slave slave = {
		.dma_dev = dw->dma.dev,
	};
	dma_cap_mask_t cap;

	if (dma_spec->args_count != 3)
		return NULL;

	slave.src_id = dma_spec->args[0];
	slave.dst_id = dma_spec->args[0];
	slave.m_master = dma_spec->args[1];
	slave.p_master = dma_spec->args[2];

	if (WARN_ON(slave.src_id >= DW_DMA_MAX_NR_REQUESTS ||
		    slave.dst_id >= DW_DMA_MAX_NR_REQUESTS ||
		    slave.m_master >= dw->nr_masters ||
		    slave.p_master >= dw->nr_masters)) {
		return NULL;
	}
	dma_cap_zero(cap);
	dma_cap_set(DMA_SLAVE, cap);

	/* TODO: there should be a simpler way to do this */
	return dma_request_channel(cap, dw_dma_filter, &slave);
}

static ssize_t __show_status(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	return 0;
}
static DEVICE_ATTR(signal, 0644, __show_status, NULL);

static struct attribute *dbg_attrs[] = {
	&dev_attr_signal.attr,
	NULL
};

static struct attribute_group dma_group_attrs = {
	.name = "info",
	.attrs = dbg_attrs
};

static int dma_proc_show(struct seq_file *m, void *v)
{
	struct dw_dma *dw = m->private;

	if (!dma_readq(dw, CH_EN))
		dwc_prepare_clk(dw);

	seq_printf(m, "CFG: 0x%llx, CH_EN=0x%llx, INT_STATUS=0x%llx, COMM_INTSTATUS=0x%llx\n",
		   dma_readq(dw, CFG), dma_readq(dw, CH_EN), dma_readq(dw, INTSTATUS),  dma_readq(dw, COMM_INTSTATUS));

	if (!dma_readq(dw, CH_EN))
		dwc_unprepare_clk(dw);

	return 0;
}

static int seq_dma_open(struct inode *inode, struct file *file)
{
	return single_open(file, dma_proc_show, PDE_DATA(inode));
}

static int ch_proc_show(struct seq_file *m, void *v)
{
	struct dw_dma *dw = m->private;
	int i;

	if (!dma_readq(dw, CH_EN))
		dwc_prepare_clk(dw);

	for (i = 0; i < dw->nr_channels; i++) {
		struct dw_dma_chan *dwc = &dw->chan[i];

		seq_printf(m, "Ch%d - SAR:0x%llx DAR:0x%llx ch_status:0x%llx int_status:0x%llx hw_pos:%d intc:%d\n", i,
			   channel_readq(dwc, SAR), channel_readq(dwc, DAR),
			   channel_readq(dwc, STATUS), channel_readq(dwc, INTSTATUS),
			   dwc->hw_pos, dwc->interrupt_count);
	}

	if (!dma_readq(dw, CH_EN))
		dwc_unprepare_clk(dw);

	return 0;
}

static int seq_ch_open(struct inode *inode, struct file *file)
{
	return single_open(file, ch_proc_show, PDE_DATA(inode));
}

static int log_proc_show(struct seq_file *m, void *v)
{
	struct dw_dma *dw = m->private;

	seq_printf(m, "sysDMA log is %s\n", dw->log_on ? "on" : "off");

	return 0;
}

static int seq_log_open(struct inode *inode, struct file *file)
{
	return single_open(file, log_proc_show, PDE_DATA(inode));
}

static ssize_t log_level_write(struct file *file, const char __user *data, size_t len, loff_t *off)
{
	struct dw_dma *dw = PDE_DATA(file_inode(file));
	char buf[3];

	if (data && !copy_from_user(buf, data, len)) {
		buf[len - 1] = '\0';
		if (buf[0] == '1')
			dw->log_on = 1;
		else if (buf[0] == '0')
			dw->log_on = 0;
		else
			dev_dbg(dw->dev, "Incorrect input for /proc/sysDMA/log_level\n");
	}

	return len;
}

static const struct proc_ops dma_proc_ops = {
	.proc_open = seq_dma_open,
	.proc_read = seq_read,
	.proc_release = single_release,
};

static const struct proc_ops ch_proc_ops = {
	.proc_open = seq_ch_open,
	.proc_read = seq_read,
	.proc_release = single_release,
};

static const struct proc_ops log_proc_ops = {
	.proc_open = seq_log_open,
	.proc_read = seq_read,
	.proc_write = log_level_write,
	.proc_release = single_release,
};

static int dw_dma_probe(struct platform_device *pdev)
{
	int err;
	struct dw_dma *dw;
	struct device *dev = &pdev->dev;
	struct proc_dir_entry *proc_dma_folder;
	struct proc_dir_entry *proc_dma;
	struct proc_dir_entry *proc_ch;
	struct proc_dir_entry *proc_log;

	dw = devm_kzalloc(dev, sizeof(*dw), GFP_KERNEL);
	if (!dw)
		return -ENOMEM;

	dw->dev = dev;
	err = dw_dma_resource_init(pdev, dw);
	if (err) {
		dev_err(dev,
			"CVITEK DMA resource init error %d\n", err);
		goto err_resource;
	}

	err = dw_dma_parse_dt(pdev, dw);
	if (err) {
		dev_err(dev,
			"CVITEK DMA parse devicetree error %d\n", err);
		goto err_dt;
	}

	pm_runtime_enable(dev);
	pm_runtime_get_sync(dev);

	dw_dma_off(dw);

	err = dw_dma_channel_init(pdev, dw);
	if (err) {
		dev_err(dev,
			"CVITEK DMA channel init error %d\n", err);
		goto err_chan;
	}

	err = dma_async_device_register(&dw->dma);
	if (err)
		goto err_dma_register;

	pm_runtime_put_sync_suspend(dev);


	dw->clk = devm_clk_get(dw->dev, "clk_sdma_axi");
	if (IS_ERR(dw->clk)) {
		dev_err(dev, "CVITEK DMA get clk_dma_axi failed\n");
		return PTR_ERR(dw->clk);
	}

	err = clk_prepare(dw->clk);
	dw->clk_count = 0;

	if (err)
		return err;

	platform_set_drvdata(pdev, dw);

	if (pdev->dev.of_node) {
		err = of_dma_controller_register(pdev->dev.of_node,
						 dw_dma_of_xlate, dw);
		if (err) {
			dev_err(&pdev->dev,
				"could not register of_dma_controller\n");
			goto err_of_dma;
		}
	}

	if (sysfs_create_group(&pdev->dev.kobj, &dma_group_attrs))
		dev_err(&pdev->dev, "Could not register attrs for dma\n");

	dev_info(dev, "CVITEK DMA Controller, %d channels, probe done!\n",
		 dw->nr_channels);

	proc_dma_folder = proc_mkdir("sysDMA", NULL);
	if (!proc_dma_folder)
		dev_err(&pdev->dev, "Error creating sysDMA proc folder entry\n");

	proc_dma = proc_create_data("dma_status", 0664, proc_dma_folder, &dma_proc_ops, dw);
	if (!proc_dma)
		dev_err(&pdev->dev, "Create sysDMA status proc failed!\n");

	proc_ch = proc_create_data("ch_status", 0664, proc_dma_folder, &ch_proc_ops, dw);
	if (!proc_ch)
		dev_err(&pdev->dev, "Create sysDMA channel proc failed!\n");

	proc_log = proc_create_data("log_level", 0664, proc_dma_folder, &log_proc_ops, dw);
	if (!proc_log)
		dev_err(&pdev->dev, "Create sysDMA log level proc failed!\n");

	dw->log_on = false;
	return 0;

err_resource:
err_dt:
	return err;
err_chan:
err_dma_register:
err_of_dma:
	pm_runtime_disable(&pdev->dev);
	return err;
}

static int __dw_dma_remove(struct dw_dma *dw)
{
	struct dw_dma_chan *dwc, *_dwc;

	pm_runtime_get_sync(dw->dev);

	dw_dma_off(dw);
	dma_async_device_unregister(&dw->dma);

	free_irq(dw->irq, dw);
	tasklet_kill(&dw->tasklet);

	list_for_each_entry_safe(dwc, _dwc, &dw->dma.channels,
				 chan.device_node) {
		list_del(&dwc->chan.device_node);
		dma_clear_bit(dw, CH_EN, dwc->mask);
	}

	pm_runtime_put_sync_suspend(dw->dev);
	return 0;
}

static int dw_dma_remove(struct platform_device *pdev)
{
	struct dw_dma *dw = platform_get_drvdata(pdev);

	if (pdev->dev.of_node)
		of_dma_controller_free(pdev->dev.of_node);

	__dw_dma_remove(dw);
	pm_runtime_disable(&pdev->dev);
	clk_unprepare(dw->clk);

	return 0;
}

static void dw_dma_shutdown(struct platform_device *pdev)
{
	struct dw_dma *dw = platform_get_drvdata(pdev);
	/*
	 * We have to call dw_dma_disable() to stop any ongoing transfer. On
	 * some platforms we can't do that since DMA device is powered off.
	 * Moreover we have no possibility to check if the platform is affected
	 * or not. That's why we call pm_runtime_get_sync() / pm_runtime_put()
	 * unconditionally. On the other hand we can't use
	 * pm_runtime_suspended() because runtime PM framework is not fully
	 * used by the driver.
	 */
	pm_runtime_get_sync(dw->dev);
	dw_dma_off(dw);
	pm_runtime_put_sync_suspend(dw->dev);

}

static const struct of_device_id dw_dma_of_id_table[] = {
	{ .compatible = "snps,dmac-bm" },
	{}
};
MODULE_DEVICE_TABLE(of, dw_dma_of_id_table);

#ifdef CONFIG_PM_SLEEP
static int dw_dma_suspend_late(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct dw_dma *dw = platform_get_drvdata(pdev);

	dw_dma_off(dw);

	return 0;
}

static int dw_dma_resume_early(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct dw_dma *dw = platform_get_drvdata(pdev);

	clk_prepare_enable(dw->clk);
	dma_writeq(dw, CH_EN, 0xFF00);
	dw_dma_on(dw);

	return 0;
}
#else
#define dw_dma_suspend_late	NULL
#define dw_dma_resume_early	NULL
#endif /* CONFIG_PM_SLEEP */

static const struct dev_pm_ops dw_dev_pm_ops = {
	SET_NOIRQ_SYSTEM_SLEEP_PM_OPS(dw_dma_suspend_late, dw_dma_resume_early)
};

static struct platform_driver dw_dma_driver = {
	.probe = dw_dma_probe,
	.remove = dw_dma_remove,
	.shutdown = dw_dma_shutdown,
	.driver = {
		.name = DRV_NAME,
		.pm = &dw_dev_pm_ops,
		.of_match_table = of_match_ptr(dw_dma_of_id_table),
	},
};

static int __init dw_dma_init(void)
{
	return platform_driver_register(&dw_dma_driver);
}
subsys_initcall(dw_dma_init);

static void __exit dw_dma_exit(void)
{
	platform_driver_unregister(&dw_dma_driver);
}
module_exit(dw_dma_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("CVITEK DMA Controller platform driver");
MODULE_ALIAS("platform:" DRV_NAME);
