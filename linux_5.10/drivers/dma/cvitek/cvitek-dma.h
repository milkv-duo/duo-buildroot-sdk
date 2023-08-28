#ifndef __CV_DMA_H__
#define __CV_DMA_H__

#if defined(CONFIG_ARM)
/* Bitfields in CTL */
#define DWC_CTL_SMS(n)		((n & 0x1)<<0)	/* src master select */
#define DWC_CTL_DMS(n)		((n & 0x1)<<2)	/* dst master select */
#define DWC_CTL_SRC_INC		(0<<4)	/* Source Address Increment update*/
#define DWC_CTL_SRC_FIX		(1<<4)	/* Source Address Increment not*/
#define DWC_CTL_DST_INC		(0<<6)	/* Destination Address Increment update*/
#define DWC_CTL_DST_FIX		(1<<6)	/* Destination Address Increment not*/
#define DWC_CTL_SRC_WIDTH(n)	((n & 0x7)<<8)	/* Source Transfer Width */
#define DWC_CTL_DST_WIDTH(n)	((n & 0x7)<<11)	/* Destination Transfer Width */
#define DWC_CTL_SRC_MSIZE(n)	((n & 0xf)<<14)	/* SRC Burst Transaction Length, data items */
#define DWC_CTL_DST_MSIZE(n)	((n & 0xf)<<18)	/* DST Burst Transaction Length, data items */
#define DWC_CTL_AR_CACHE(n)	((n & 0xf)<<22)
#define DWC_CTL_AW_CACHE(n)	((n & 0xf)<<26)
#define DWC_CTL_N_LAST_W_EN	(1<<30)	/* last write posted write enable/disable*/
#define DWC_CTL_N_LAST_W_DIS	(0<<30)	/* last write posted wrtie enable/disable*/
#define DWC_CTL_ARLEN_DIS	(0ULL<<38) /* Source Burst Length Disable */
#define DWC_CTL_ARLEN_EN	(1ULL<<38) /* Source Burst Length Enable */
#define DWC_CTL_ARLEN(n)	((n & 0xffULL)<<39)
#define DWC_CTL_AWLEN_DIS	(0ULL<<47) /* DST Burst Length Enable */
#define DWC_CTL_AWLEN_EN	(1ULL<<47)
#define DWC_CTL_AWLEN(n)	((n & 0xffULL)<<48)
#define DWC_CTL_SRC_STA_DIS	(0ULL<<56)
#define DWC_CTL_SRC_STA_EN	(1ULL<<56)
#define DWC_CTL_DST_STA_DIS	(0ULL<<57)
#define DWC_CTL_DST_STA_EN	(1ULL<<57)
#define DWC_CTL_IOC_BLT_DIS	(0ULL<<58)	/* Interrupt On completion of Block Transfer */
#define DWC_CTL_IOC_BLT_EN	(1ULL<<58)
#define DWC_CTL_SHADOWREG_OR_LLI_LAST	(1ULL<<62)	/* Last Shadow Register/Linked List Item */
#define DWC_CTL_SHADOWREG_OR_LLI_VALID	(1ULL<<63)	/* Shadow Register content/Linked List Item valid */

#define DWC_BLOCK_TS_MASK	0x3FFFFF

#define DWC_CFG_SRC_MULTBLK_TYPE(x)	((x & 0x7) << 0)
#define DWC_CFG_DST_MULTBLK_TYPE(x)	((x & 0x7) << 2)
#define DWC_CFG_TT_FC(x)		((x & 0x7ULL) << 32)
#define DWC_CFG_HS_SEL_SRC_HW	(0ULL<<35)
#define DWC_CFG_HS_SEL_SRC_SW	(1ULL<<35)
#define DWC_CFG_HS_SEL_DST_HW	(0ULL<<36)
#define DWC_CFG_HS_SEL_DST_SW	(1ULL<<36)
#define DWC_CFG_SRC_HWHS_POL_H	(0ULL << 37)
#define DWC_CFG_SRC_HWHS_POL_L	(1ULL << 37)
#define DWC_CFG_DST_HWHS_POL_H	(0ULL << 38)
#define DWC_CFG_DST_HWHS_POL_L	(1ULL << 38)
#define DWC_CFG_SRC_PER(x)	((x & 0xffULL) << 39)
#define DWC_CFG_DST_PER(x)	((x & 0xffULL) << 44)

#define DWC_CFG_CH_PRIOR_MASK	(0x7ULL << 49)	/* priority mask */
#define DWC_CFG_CH_PRIOR(x)	(((x) & 0x7ULL) << 49)	/* priority */
#define DWC_CFG_SRC_OSR_LMT(x)	(((x) & 0xfULL) << 55) /* max request x + 1 <= 16 */
#define DWC_CFG_DST_OSR_LMT(x)	(((x) & 0xfULL) << 59)

#define DWC_CFG_GET_TT_FC(x)		((x >> 32ULL) & 0x7)

#else

/* Bitfields in CTL */
#define DWC_CTL_SMS(n)		((n & 0x1)<<0)	/* src master select */
#define DWC_CTL_DMS(n)		((n & 0x1)<<2)	/* dst master select */
#define DWC_CTL_SRC_INC		(0<<4)	/* Source Address Increment update*/
#define DWC_CTL_SRC_FIX		(1<<4)	/* Source Address Increment not*/
#define DWC_CTL_DST_INC		(0<<6)	/* Destination Address Increment update*/
#define DWC_CTL_DST_FIX		(1<<6)	/* Destination Address Increment not*/
#define DWC_CTL_SRC_WIDTH(n)	((n & 0x7)<<8)	/* Source Transfer Width */
#define DWC_CTL_DST_WIDTH(n)	((n & 0x7)<<11)	/* Destination Transfer Width */
#define DWC_CTL_SRC_MSIZE(n)	((n & 0xf)<<14)	/* SRC Burst Transaction Length, data items */
#define DWC_CTL_DST_MSIZE(n)	((n & 0xf)<<18)	/* DST Burst Transaction Length, data items */
#define DWC_CTL_AR_CACHE(n)	((n & 0xf)<<22)
#define DWC_CTL_AW_CACHE(n)	((n & 0xf)<<26)
#define DWC_CTL_N_LAST_W_EN	(1<<30)	/* last write posted write enable/disable*/
#define DWC_CTL_N_LAST_W_DIS	(0<<30)	/* last write posted wrtie enable/disable*/
#define DWC_CTL_ARLEN_DIS	(0UL<<38) /* Source Burst Length Disable */
#define DWC_CTL_ARLEN_EN	(1UL<<38) /* Source Burst Length Enable */
#define DWC_CTL_ARLEN(n)	((n & 0xff)<<39)
#define DWC_CTL_AWLEN_DIS	(0UL<<47) /* DST Burst Length Enable */
#define DWC_CTL_AWLEN_EN	(1UL<<47)
#define DWC_CTL_AWLEN(n)	((n & 0xff)<<48)
#define DWC_CTL_SRC_STA_DIS	(0UL<<56)
#define DWC_CTL_SRC_STA_EN	(1UL<<56)
#define DWC_CTL_DST_STA_DIS	(0UL<<57)
#define DWC_CTL_DST_STA_EN	(1UL<<57)
#define DWC_CTL_IOC_BLT_DIS	(0UL<<58)	/* Interrupt On completion of Block Transfer */
#define DWC_CTL_IOC_BLT_EN	(1UL<<58)
#define DWC_CTL_SHADOWREG_OR_LLI_LAST	(1UL<<62)	/* Last Shadow Register/Linked List Item */
#define DWC_CTL_SHADOWREG_OR_LLI_VALID	(1UL<<63)	/* Shadow Register content/Linked List Item valid */

#define DWC_BLOCK_TS_MASK	0x3FFFFF

#define DWC_CFG_SRC_MULTBLK_TYPE(x)	((x & 0x7) << 0)
#define DWC_CFG_DST_MULTBLK_TYPE(x)	((x & 0x7) << 2)
#define DWC_CFG_TT_FC(x)		((x & 0x7) << 32)
#define DWC_CFG_HS_SEL_SRC_HW	(0UL<<35)
#define DWC_CFG_HS_SEL_SRC_SW	(1UL<<35)
#define DWC_CFG_HS_SEL_DST_HW	(0UL<<36)
#define DWC_CFG_HS_SEL_DST_SW	(1UL<<36)
#define DWC_CFG_SRC_HWHS_POL_H	(0UL << 37)
#define DWC_CFG_SRC_HWHS_POL_L	(1UL << 37)
#define DWC_CFG_DST_HWHS_POL_H	(0UL << 38)
#define DWC_CFG_DST_HWHS_POL_L	(1UL << 38)
#define DWC_CFG_SRC_PER(x)	((x & 0xff) << 39)
#define DWC_CFG_DST_PER(x)	((x & 0xff) << 44)

#define DWC_CFG_CH_PRIOR_MASK	(0x7UL << 49)	/* priority mask */
#define DWC_CFG_CH_PRIOR(x)	((x & 0x7) << 49)	/* priority */
#define DWC_CFG_SRC_OSR_LMT(x)	(((x) & 0xf) << 55) /* max request x + 1 <= 16 */
#define DWC_CFG_DST_OSR_LMT(x)	(((x) & 0xf) << 59)

#define DWC_CFG_GET_TT_FC(x)		((x >> 32) & 0x7)

#endif

#define DWC_CH_INTSTA_DMA_TFR_DONE	(1<<1)
#define DWC_CH_INTSTA_BLOCK_TFR_DONE	(1<<0)
#define DWC_CH_INTSTA_SRC_TFR_COMP_EN	(1<<3)
#define DWC_CH_INTSTA_DST_TFR_COMP_EN	(1<<4)
#define DWC_CH_INTSTA_SRC_DEC_ERR_EN	(1<<5)
#define DWC_CH_INTSTA_DST_DEC_ERR_EN	(1<<6)
#define DWC_CH_INTSTA_SRC_SLV_ERR_EN	(1<<7)
#define DWC_CH_INTSTA_SRC_DST_ERR_EN	(1<<8)
#define DWC_CH_INTSTA_LLI_RD_DEV_ERR_EN	(1<<9)
#define DWC_CH_INTSTA_LLI_WD_DEV_ERR_EN	(1<<10)
#define DWC_CH_INTSTA_LLI_RD_SLV_ERR_EN	(1<<11)
#define DWC_CH_INTSTA_LLI_WD_SLV_ERR_EN	(1<<12)
#define DWC_CH_INTSTA_SHDW_LLI_INVALID_ERR_EN	(1<<13)
#define DWC_CH_INTSTA_SLVIF_MULTBLK_TYPE_ERR_EN	(1<<14)
#define DWC_CH_INTSTA_SLVIF_DEC_ERR_EN	(1<<16)
#define DWC_CH_INTSTA_CH_ABORTED_EN	(1<<31)
#define DWC_CH_SUSPENDED		(1 << 29)

#define DWC_CH_INTSIG_BLK_TRA_DONE	(1 << 0)
#define DWC_CH_INTSIG_DMA_TRA_DONE	(1 << 1)
#define DWC_CH_INTSIG_SRC_COMP_DONE	(1 << 3)
#define DWC_CH_INTSIG_DST_COMP_DONE	(1 << 4)

/* Bitfields in LLP */
#define DWC_LLP_LMS(x)		((x) & 1)	/* list master select */
#define DWC_LLP_LOC(x)		(((x) & ~0x3f))	/* next lli */

#define DW_DMAC_CH_PAUSE_EN_OFFSET	24
#define DW_DMAC_CH_PAUSE_OFFSET		16
#define DW_DMAC_CH_EN_WE_OFFSET		8
#define DW_DMAC_CH_ABORT_WE_OFFSET	40
#define DW_DMAC_CH_ABORT_OFFSET	32


#define DW_CFG_DMA_EN		(1 << 0)
#define DW_CFG_DMA_INT_EN	(1 << 1)

#define DW_DMA_MAX_NR_MASTERS	2
#define DW_DMA_MAX_NR_CHANNELS	8
#define DW_DMA_MAX_NR_REQUESTS	16
#define DW_DMA_CHAN_MASK	((1 << DW_DMA_MAX_NR_CHANNELS) - 1)
#define DW_DWC_MAX_BLOCK_TS	32

/* The set of bus widths supported by the DMA controller */
#define DW_DMA_BUSWIDTHS			  \
			(BIT(DMA_SLAVE_BUSWIDTH_UNDEFINED)	| \
			BIT(DMA_SLAVE_BUSWIDTH_1_BYTE)		| \
			BIT(DMA_SLAVE_BUSWIDTH_2_BYTES)		| \
			BIT(DMA_SLAVE_BUSWIDTH_4_BYTES))

#define BM_DMA_PERIOD_LEN 64 //The maxmium LLI data length of BM sysDMA

#ifdef CONFIG_DW_DMAC_BIG_ENDIAN_IO
typedef __be32 __dw32;
typedef __be64 __dw64;
#else
typedef __le32 __dw32;
typedef __le64 __dw64;
#endif

enum dwc_multblk_type {
	CONTIGUOUS,
	RELOAD,
	SHADOW_REGISTER,
	LINK_LIST
};

/* flow controller */
enum dw_dma_fc {
	DW_DMA_FC_D_M2M, /* FlowControl is DMAC, mem to mem */
	DW_DMA_FC_D_M2P, /* FlowControl is DMAC, mem to perip */
	DW_DMA_FC_D_P2M,
	DW_DMA_FC_D_P2P,
	DW_DMA_FC_SP_P2M, /* FlowControl is Source periph, periph to mem */
	DW_DMA_FC_SP_P2P,
	DW_DMA_FC_DP_M2P, /* FlowControl is Dst periph, periph to mem */
	DW_DMA_FC_DP_P2P,
};

enum dwc_tr_width {
	BITS_WIDTH_UNDEFINED = 0,
	BITS_WIDTH_8_BITS,
	BITS_WIDTH_16_BITS,
	BITS_WIDTH_32_BITS,
	BITS_WIDTH_64_BITS,
	BITS_WIDTH_128_BITS,
	BITS_WIDTH_256_BITS,
	BITS_WIDTH_512_BITS,
};

/* chan intstatus*/
enum dw_dmac_flags {
	DW_DMA_IS_CYCLIC = 0,
	DW_DMA_IS_SOFT_LLP = 1,
	DW_DMA_IS_PAUSED = 2,
	DW_DMA_IS_INITIALIZED = 3,
};

/* bursts size */
enum dw_dma_msize {
	DW_DMA_MSIZE_1,
	DW_DMA_MSIZE_4,
	DW_DMA_MSIZE_8,
	DW_DMA_MSIZE_16,
	DW_DMA_MSIZE_32,
	DW_DMA_MSIZE_64,
	DW_DMA_MSIZE_128,
	DW_DMA_MSIZE_256,
};

/*
 * Redefine this macro to handle differences between 32- and 64-bit
 * addressing, big vs. little endian, etc.
 */
#define DW_REG(name)	u64 name

/* Hardware register definitions. */
struct dw_dma_chan_regs {
	DW_REG(SAR);		/* Source Address Register */
	DW_REG(DAR);		/* Destination Address Register */
	DW_REG(BLOCK_TS);		/* Block transfer size */
	DW_REG(CTL);	/* Contorl dma transer */
	DW_REG(CFG);	/* Configure dma transer */
	DW_REG(LLP);		/* Linked List Pointer */
	DW_REG(STATUS);		/* Status of dma transer */
	DW_REG(SWHSSRCREG);		/* SW handshake source register */
	DW_REG(SWHSDSTREG);	/* SW handshake Destination register */
	DW_REG(BLK_TFR_RESUMEREQREG);	/* Block transfer resume request */
	DW_REG(AXI_IDREG);	/* AXI ID register */
	DW_REG(AXI_QOSREG);	/* AXI QoS register */
	DW_REG(SSTAT);
	DW_REG(DSTAT);
	DW_REG(SSTATAR);
	DW_REG(DSTATAR);
	DW_REG(INTSTATUS_ENABLEREG);
	DW_REG(INTSTATUS);
	DW_REG(INTSIGNAL_ENABLEREG);
	DW_REG(INTCLEARREG);
	u64 __reserved1[12];
};

struct dw_dma_regs {
	/* Common Registers */
	DW_REG(ID);
	DW_REG(COMPVER);
	DW_REG(CFG);
	DW_REG(CH_EN);
	u64 __reserved0[2];

	DW_REG(INTSTATUS);
	DW_REG(COMM_INTCLEAR);
	DW_REG(COMM_INTSTATUS_EN);
	DW_REG(COMM_INTSIGNAL_EN);
	DW_REG(COMM_INTSTATUS);
	DW_REG(RESET);

	u64 __reserved1[20];
	/* channel regs */
	struct dw_dma_chan_regs	CHAN[DW_DMA_MAX_NR_CHANNELS];
};

/**
 * struct dw_dma_slave - Controller-specific information about a slave
 *
 * @dma_dev:	required DMA master device
 * @src_id:	src request line
 * @dst_id:	dst request line
 * @m_master:	memory master for transfers on allocated channel
 * @p_master:	peripheral master for transfers on allocated channel
 * @hs_polarity:set active low polarity of handshake interface
 */
struct dw_dma_slave {
	struct device *dma_dev;
	u8 src_id;
	u8 dst_id;
	u8 m_master;
	u8 p_master;
	bool hs_polarity;
};

struct dbg_fix_buf {
	int id;
	void *cpu_addr;
	dma_addr_t dma_hanle;
	unsigned int size;
	unsigned int state;
};
struct dbg_fix_sg {
	int count;
	struct dbg_fix_buf *fix_buf;
};

struct dw_cyclic_desc {
	struct dw_desc **desc;
	unsigned long periods;
	size_t period_len;
	unsigned int last_sent;
	void (*period_callback)(void *param);
	void *period_callback_param;
};

struct dw_dma_chan {
	struct dma_chan chan;
	void __iomem *ch_regs;
	u64 mask;
	u64 priority;
	enum dma_transfer_direction direction;

	spinlock_t lock;

	/* these other elements are all protected by lock */
	unsigned long flags;
	unsigned long status;
	struct list_head active_list;
	struct list_head queue;
	struct dw_cyclic_desc *cdesc;

	unsigned int descs_allocated;

	/* hardware configuration */
	unsigned int axi_tr_width;
	unsigned int block_size;
	unsigned int block_ts;

	/* custom slave configuration */
	struct dw_dma_slave dws;

	/* configuration passed via .device_config */
	struct dma_slave_config dma_sconfig;

	/* fix bug sysdma */
	struct dbg_fix_sg *bug_info;
	struct dma_pool *bug_fix_dma_pool;
	u32 hw_pos;
	u32 interrupt_count;
};

/* LLI == Linked List Item; a.k.a. DMA block descriptor */
struct dw_lli {
	/* values that are not changed by hardware */
	__dw64 sar;
	__dw64 dar;
	__dw64 block_ts;
	__dw64 llp;	/* chain to next lli */
	__dw64 ctl;

	/* sstat and dstat can snapshot peripheral register state.
	 * silicon config may discard either or both...
	 */
	__dw32 sstat;
	__dw32 dstat;
	__dw64 llp_status;
	__dw64 reserved;
};

struct dw_desc {
	/* FIRST values the hardware uses */
	struct dw_lli lli;

#ifdef CONFIG_DW_DMAC_BIG_ENDIAN_IO
#define lli_set(d, reg, v)		((d)->lli.reg |= cpu_to_be64(v))
#define lli_clear(d, reg, v)		((d)->lli.reg &= ~cpu_to_be64(v))
#define lli_read(d, reg)		be64_to_cpu((d)->lli.reg)
#define lli_write(d, reg, v)		((d)->lli.reg = cpu_to_be64(v))
#else
#define lli_set(d, reg, v)		((d)->lli.reg |= cpu_to_le64(v))
#define lli_clear(d, reg, v)		((d)->lli.reg &= ~cpu_to_le64(v))
#define lli_read(d, reg)		le64_to_cpu((d)->lli.reg)
#define lli_write(d, reg, v)		((d)->lli.reg = cpu_to_le64(v))
#endif

	/* THEN values for driver housekeeping */
	struct list_head desc_node;
	struct list_head tx_list;
	struct dma_async_tx_descriptor txd;
	size_t len;
	size_t total_len;
	u32 residue;
	u32 hw_pos;
};

#ifdef CONFIG_PM_SLEEP
struct dw_dma_pm_chan_regs {
	DW_REG(SAR);		/* Source Address Register */
	DW_REG(DAR);		/* Destination Address Register */
	DW_REG(BLOCK_TS);		/* Block transfer size */
	DW_REG(CTL);	/* Contorl dma transer */
	DW_REG(CFG);	/* Configure dma transer */
	DW_REG(LLP);		/* Linked List Pointer */
	DW_REG(INTSTATUS_ENABLEREG);
	DW_REG(INTSIGNAL_ENABLEREG);
};
struct dw_dma_reg_context {
	u64 cfg;
	u64 chen;
	struct dw_dma_pm_chan_regs	chan[DW_DMA_MAX_NR_CHANNELS];
};
#endif

struct dw_dma {
	struct device *dev;
	void __iomem *regs;
	struct dma_pool *desc_pool;
	struct dma_device dma;
	struct clk *clk;
	struct tasklet_struct tasklet;

	/* dma configs */
	unsigned char nr_channels;
	bool is_private;
	bool is_memcpy;
#define CHAN_ALLOCATION_ASCENDING	0	/* zero to seven */
#define CHAN_ALLOCATION_DESCENDING	1	/* seven to zero */
	unsigned char chan_allocation_order;
#define CHAN_PRIORITY_ASCENDING		0	/* chan0 highest */
#define CHAN_PRIORITY_DESCENDING	1	/* chan7 highest */
	unsigned char chan_priority;
	unsigned int axi_tr_width;
	unsigned int block_size;
	unsigned int block_ts;
	unsigned char nr_masters;
	unsigned char data_width[DW_DMA_MAX_NR_MASTERS];

	/* channels */
	struct dw_dma_chan *chan;
	u8 in_use;
	u8 clk_count;
	u8 log_on;

	/* interrupt*/
	int irq;
	spinlock_t lock;
#ifdef CONFIG_PM_SLEEP
	struct dw_dma_reg_context *reg_ctx;
#endif
};

#ifdef CONFIG_DW_DMAC_BIG_ENDIAN_IO
#define dma_readl_native	ioread32be
#define dma_writel_native	iowrite32be
#else
#define dma_readl_native	readl
#define dma_writel_native	writel
#ifdef readq
#define dma_readq_native	readq
#else
#define dma_readq_native(__reg)	(*(volatile u64 __force *)(__reg))
#endif
#ifdef writeq
#define dma_writeq_native	writeq
#else
#define dma_writeq_native(__value,__reg)	\
	(*(volatile u64 __force *)(__reg) = (__value))
#endif
#endif

#define dma_readq(dw, name) \
		dma_readq_native(&(__dw_regs(dw)->name))

#define dma_writeq(dw, name, val) \
		dma_writeq_native((val), &(__dw_regs(dw)->name))

#define channel_readq(dwc, name) \
		dma_readq_native(&(__dwc_regs(dwc)->name))

#define channel_writeq(dwc, name, val) \
		dma_writeq_native((val), &(__dwc_regs(dwc)->name))

#define dma_set_bit(dw, name, mask) \
		dma_writeq_native(((mask) | dma_readq_native(&(__dw_regs(dw)->name))) \
			, &(__dw_regs(dw)->name))

#define dma_clear_bit(dw, name, mask) \
		dma_writeq_native((~mask & dma_readq_native(&(__dw_regs(dw)->name))) \
			, &(__dw_regs(dw)->name))

#define channel_set_bit(dwc, name, mask) \
		dma_writeq_native((mask | dma_readq_native(&(__dwc_regs(dwc)->name))), \
			&(__dwc_regs(dwc)->name))

#define channel_clear_bit(dwc, name, mask) \
	dma_writeq_native((~mask & dma_readq_native(&(__dwc_regs(dwc)->name))), \
		&(__dwc_regs(dwc)->name))
#endif
