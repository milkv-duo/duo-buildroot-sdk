/**********************************************************************
 *
 * usb_tty.c
 * ACM class application.
 *
 ***********************************************************************/
#include <stdlib.h>
#include <malloc.h>
//#include <memalign.h>
#include <crc.h>
#include <watchdog.h>
#include "include/debug.h"
#include "include/cvi_usb.h"
#include <mmio.h>
#include <asm/io.h>
#include "include/platform_def.h"
#include "include/usb_tty.h"
#include "include/system_common.h"
#include "include/cvi_ch9.h"
#include "include/cvi_drv_if.h"
#include "include/cvi_udc_otg_regs.h"
#include "include/cvi_udc_otg_priv.h"
#include "include/cvi_udc.h"
#include "include/cvi_errno.h"
#include "include/cvi_private.h"
#include "cvi_reboot.h"
#include "cvi_efuse.h"
#include <common.h>
#include <command.h>
#include <linux/delay.h>
#include "cvi_update.h"
#include "include/dps.h"

#ifdef BUILD_ATF
extern uint16_t cvi_usb_vid;
#else
uint16_t cvi_usb_vid = 0x3346;
#endif

uint16_t cvi_usb_pid;

static void bulkOutCmplMain(struct usb_ep *ep, struct usb_request *req);
static void sramOutReqS2D(uint64_t addr, uint32_t size);

#define GLOBAL_MEM_START_ADDR 0x80000000
#define USB_PHY_DETECTION

/* cvi USB driver object */
static struct cvi_drv_obj drv_obj = {
	.plat = {
		.regs_otg = USB_BASE, // address where USB core is mapped
		.usb_gusbcfg    = 0x40081408,
		.rx_fifo_sz     = 512,
		.np_tx_fifo_sz  = 512,
		.tx_fifo_sz     = 512,
	},
};

/* variable declare */
static uint8_t *bulkBuf, *cmdBuf, *ep0Buff;
static struct usb_ep *epIn, *epOut, *epInNotify;
static struct usb_request *bulkInReq, *bulkOutReq, *ep0Req, *IntInReq;
static uint8_t configValue;
static uint8_t configBreak;
static uint8_t acm_configValue;
static uint8_t current_speed = CH9_USB_SPEED_UNKNOWN;
static uint8_t mem_alloc_cnt;
static uint32_t transfer_size;
static uint8_t flagEnterDL;
struct f_acm *acm;

static uint8_t *bulkBufAlloc;
static uint8_t *cmdBufAlloc;
static uint8_t *cb0_buf;
static uint8_t *cb1_buf;
static uint8_t *cb2_buf;
static uint8_t *ep0BuffAlloc;
static uint8_t *rsp_buf;
static uint8_t *acm_buf;
static uint8_t *setup_buf;
static uint8_t *handler;

/* string will be filled then in initializing section */
static char vendorDesc[sizeof(USB_MANUFACTURER_STRING) * 2 + 2];
static char productDesc[sizeof(USB_PRODUCT_STRING) * 2 + 2];
static char serialDesc[sizeof(USB_SERIAL_NUMBER_STRING) * 2 + 2];

typedef void func(void);

static void init_param(void)
{
	bulkBuf = NULL;
	cmdBuf = NULL;
	ep0Buff = NULL;
	epIn = NULL;
	epOut = NULL;
	epInNotify = NULL;
	bulkInReq = NULL;
	bulkOutReq = NULL;
	ep0Req = NULL;
	IntInReq = NULL;
	configValue = 0;
	configBreak = 0;
	acm_configValue = 0;
	current_speed = CH9_USB_SPEED_UNKNOWN;
	mem_alloc_cnt = 0;
	transfer_size = 0;
	acm = NULL;
}

struct usb_msg_header {
	uint8_t token;
	uint8_t len_hi;
	uint8_t len_low;
	uint8_t addr4;
	uint8_t addr3;
	uint8_t addr2;
	uint8_t addr1;
	uint8_t addr0;
} __packed;

struct usb_msg {
	struct usb_msg_header header;
} __packed;

struct usb_msg_s2d {
	struct usb_msg_header header;
	size_t size;
} __packed;

struct usb_msg_d2s {
	struct usb_msg_header header;
	size_t size;
} __packed;

struct usb_rsp {
	uint8_t no_use0;
	uint8_t no_use1;
	uint8_t crc16_hi;
	uint8_t crc16_low;
	uint8_t no_use3;
	uint8_t no_use4;
	uint8_t token;
	uint8_t ack_index;
	uint8_t reserved[RSP_SIZE - 8];
} __packed;

struct sram_info {
	uint64_t sram_dest;
	uint32_t total_len;
	uint8_t reserved[4];
} packed;

static struct sram_info sram_info;

static char *_allow_cmds[] = { "setenv", "saveenv", "efusew", "efuser" };

#if USB_RW_EFUSE // Mark_to_do
enum CVI_EFUSE_LOCK_WRITE_E {
	CVI_EFUSE_LOCK_WRITE_HASH0_PUBLIC = CVI_EFUSE_OTHERS + 1,
	CVI_EFUSE_LOCK_WRITE_LOADER_EK,
	CVI_EFUSE_LOCK_WRITE_DEVICE_EK,
	CVI_EFUSE_LOCK_WRITE_LAST
};

static char *_allow_areas[] = { "USER",	     "DEVICE_ID", "HASH0_PUBLIC",
				"LOADER_EK", "DEVICE_EK", "AREA_LAST" };
static char *_allow_wl_areas[] = { "LOCK_HASH0_PUBLIC",
				   "LOCK_LOADER_EK",
				   "LOCK_DEVICE_EK",
				   "LOCK_LAST",
				   "SECUREBOOT",
				   "OTHERS",
				   "LOCK_WRITE_HASH0_PUBLIC",
				   "LOCK_WRITE_LOADER_EK",
				   "LOCK_WRITE_DEVICE_EK" };
#endif // USB_RW_EFUSE

void print_buf_addr(void)
{
	INFO("bulkBufAlloc: %p\n", bulkBufAlloc);
	INFO("cmdBufAlloc: %p\n", cmdBufAlloc);
	INFO("ep0BuffAlloc: %p\n", ep0BuffAlloc);
	INFO("setup_buf: %p\n", setup_buf);
	INFO("handler: %p\n", handler);
	INFO("cb0_buf: %p\n", cb0_buf);
	INFO("cb1_buf: %p\n", cb1_buf);
	INFO("cb2_buf: %p\n", cb2_buf);
	INFO("rsp_buf: %p\n", rsp_buf);
	INFO("acm_buf: %p\n", acm_buf);
}

void __attribute__((optimize("O0"))) set_config_break(uint8_t value)
{
	configBreak = value;
}

uint8_t __attribute__((optimize("O0"))) get_config_break(void)
{
	return configBreak;
}

void __attribute__((optimize("O0"))) set_acm_config(uint8_t value)
{
	acm_configValue = value;
}

uint8_t __attribute__((optimize("O0"))) get_acm_config(void)
{
	return acm_configValue;
}

static int acm_mem_init(void)
{
	bulkBufAlloc = memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN_CACHE_SIZE(BUF_SIZE));
	memset(bulkBufAlloc, 0, ALIGN_CACHE_SIZE(BUF_SIZE));
	cmdBufAlloc = memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN_CACHE_SIZE(BUF_SIZE));
	memset(cmdBufAlloc, 0, ALIGN_CACHE_SIZE(BUF_SIZE));
	ep0BuffAlloc = memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN_CACHE_SIZE(EP0_SIZE));
	memset(ep0BuffAlloc, 0, ALIGN_CACHE_SIZE(EP0_SIZE));
	setup_buf = memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN_CACHE_SIZE(STP_SIZE));
	memset(setup_buf, 0, ALIGN_CACHE_SIZE(STP_SIZE));
	handler = memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN_CACHE_SIZE(HANDLER_SIZE));
	memset(handler, 0, ALIGN_CACHE_SIZE(HANDLER_SIZE));
	cb0_buf = malloc(CB_SIZE);
	memset(cb0_buf, 0, CB_SIZE);
	cb1_buf = malloc(CB_SIZE);
	memset(cb1_buf, 0, CB_SIZE);
	cb2_buf = malloc(CB_SIZE);
	memset(cb2_buf, 0, CB_SIZE);;
	rsp_buf = malloc(RSP_SIZE);
	memset(rsp_buf, 0, RSP_SIZE);
	acm_buf = malloc(ACM_SIZE);
	memset(acm_buf, 0, ACM_SIZE);

	set_config_break(0);
	set_acm_config(0);
	current_speed = CH9_USB_SPEED_UNKNOWN;
	mem_alloc_cnt = 0;
	transfer_size = 0;
	flagEnterDL = 0;
	return 0;
}

static void acm_mem_release(void)
{
	free(bulkBufAlloc);
	free(cmdBufAlloc);
	free(ep0BuffAlloc);
	free(setup_buf);
	free(handler);
	free(cb0_buf);
	free(cb1_buf);
	free(cb2_buf);
	free(rsp_buf);
	free(acm_buf);
}

/* interrupt handler */
void AcmIsr(void)
{
	cviusb_gadget_handle_interrupts(0);
}

static int getDescAcm(CH9_UsbSpeed speed, uint8_t *acmDesc)
{
	int i = 0;
	void *desc;
	int sum = 0;
	void *(*tab)[];

	switch (speed) {
	case CH9_USB_SPEED_FULL:
		tab = &descriptorsFs;
		break;
	case CH9_USB_SPEED_HIGH:
		tab = &descriptorsHs;
		break;

	default:
		return -1;
	}

	desc = (*tab)[i];

	while (desc) {
		int length = *(uint8_t *)desc;

		VERBOSE("acm get length %d\n", length);
		memcpy(&acmDesc[sum], desc, length);
		sum += length;
		desc = (*tab)[++i];
	}
	/* VERBOSE("acm get sum:%d\n", sum); */
	return sum;
}

static void clearReq(struct usb_request *req)
{
	memset(req, 0, sizeof(*req));
}

static void reset(struct usb_gadget *gadget)
{
	/* struct CUSBD_Dev *dev; */

	INFO("Application: %s\n", __func__);
}

static void disconnect(struct usb_gadget *gadget)
{
	set_acm_config(0);
	mem_alloc_cnt = 1;
	configValue = 0;
	NOTICE("Application: %s\n", __func__);
}

static void resume(struct usb_gadget *gadget)
{
	VERBOSE("Application: %s\n", __func__);
}

static void reqComplete(struct usb_ep *ep, struct usb_request *req)
{
	VERBOSE("Request on endpoint completed\n");
	if (req->status == -EIO) {
		NOTICE("IO Abort !!!!!\n");
	}
}

static void suspend(struct usb_gadget *gadget)
{
	VERBOSE("Application: %s %c\n", __func, ' ');
}

static void *requestMemAlloc(struct usb_gadget *gadget, uint32_t requireSize)
{
	void *ptr;
	/* VERBOSE("requestMemAlloc: size %d\n", requireSize); */
	if (mem_alloc_cnt == 0)
		ptr = cb0_buf;
	else if (mem_alloc_cnt == 1)
		ptr = cb1_buf;
	else
		ptr = cb2_buf;
	VERBOSE("%s: ptr %p, size %d, mem_alloc_cnt %d\n", __func__, ptr,
		requireSize, mem_alloc_cnt);
	mem_alloc_cnt++;
	return ptr;
}

static void requestMemFree(struct usb_gadget *gadget, void *usbRequest)
{
}

static void resetOutReq(void)
{
	VERBOSE("epOut->ops->queue\n");
	bulkOutReq->length = transfer_size;
	bulkOutReq->buf = cmdBuf;
	bulkOutReq->dma = (uintptr_t)cmdBuf;
	bulkOutReq->complete = bulkOutCmplMain;
	cvi_cache_flush(bulkOutReq->dma, bulkOutReq->length);
	epOut->ops->queue(epOut, bulkOutReq);
}

static void bulkResetOutReq(struct usb_ep *ep, struct usb_request *req)
{
	/* INFO("bulkReset%sReq complete\n", (ep->address == BULK_EP_IN)?"In":"Out"); */
	resetOutReq();
}

static void sendInReq(uint32_t length, uint8_t token, cvi_reqComplete complete,
		      uint8_t *pRsp, uint8_t rspLen)
{
	uint16_t crc;
	static uint8_t ack_idx;
	struct usb_rsp *rsp = (struct usb_rsp *)rsp_buf;

	memset(rsp_buf, 0, RSP_SIZE);
	if (pRsp && rspLen > 0 && rspLen <= (RSP_SIZE - 8))
		memcpy(rsp_buf + 8, pRsp, rspLen);
	crc = crc16_ccitt(0, cmdBuf, length);
	VERBOSE("CRC: %x\n", crc);
	rsp->crc16_hi = (crc >> 8) & 0xFF;
	rsp->crc16_low = crc & 0xFF;
	rsp->ack_index = ack_idx;
	rsp->token = token;
	ack_idx++;
	clearReq(bulkInReq);
	if (rspLen > 8)
		bulkInReq->length = RSP_SIZE;
	else
		bulkInReq->length = 16;
	bulkInReq->buf = rsp_buf;
	bulkInReq->dma = (uintptr_t)rsp_buf;
	bulkInReq->complete = complete;
	VERBOSE("epIn->ops->queue\n");

	cvi_cache_flush(bulkInReq->dma, bulkInReq->length);
	epIn->ops->queue(epIn, bulkInReq);
}

static void bulkCmplEmpty(struct usb_ep *ep, struct usb_request *req)
{
	VERBOSE("%s\n", __func__);
}

static void resetOutReqS2D(uint64_t addr, size_t size, cvi_reqComplete complete)
{
	/* INFO("epOut->ops->queue S2D, addr:0x%lx, size:0x%lx\n", addr, size); */
	bulkOutReq->length = size;
	bulkOutReq->buf = (uint8_t *)addr;
	bulkOutReq->dma = (uintptr_t)addr;
	bulkOutReq->complete = complete;

	cvi_cache_flush(bulkOutReq->dma, bulkOutReq->length);
	epOut->ops->queue(epOut, bulkOutReq);
}

static void sramCompl(struct usb_ep *ep, struct usb_request *req)
{
	uint32_t left = sram_info.total_len -= req->length;
	uint64_t target = sram_info.sram_dest + req->length;
	/* INFO("sram copy data to 0x%lx, len = 0x%x\n", sram_info.sram_dest, req->length); */
	memcpy((void *)sram_info.sram_dest, (void *)req->buf, req->length);

	if (left == 0U)
		resetOutReq();
	else
		sramOutReqS2D(target, left);
}

static void sramOutReqS2D(uint64_t addr, uint32_t size)
{
	sram_info.total_len = size;
	sram_info.sram_dest = addr;

	bulkOutReq->length = (sram_info.total_len > BUF_SIZE) ?
				     BUF_SIZE :
					   sram_info.total_len;
	bulkOutReq->buf = bulkBuf;
	bulkOutReq->dma = (uintptr_t)bulkBuf;
	bulkOutReq->complete = sramCompl;

	cvi_cache_flush(bulkOutReq->dma, bulkOutReq->length);
	epOut->ops->queue(epOut, bulkOutReq);
}

static void sendInReqD2S(uint64_t addr, size_t size, cvi_reqComplete complete)
{
	/* INFO("epIn->ops->queue D2S\n"); */
	clearReq(bulkInReq);
	bulkInReq->length = size;
	bulkInReq->buf = (uint8_t *)addr;
	bulkInReq->dma = (uintptr_t)addr;
	bulkInReq->complete = complete;

	cvi_cache_flush(bulkInReq->dma, bulkInReq->length);
	epIn->ops->queue(epIn, bulkInReq);
}

#if USB_RW_EFUSE // Mark_to_do
static int ahex2int(char a, char b)
{
	a = (a <= '9') ? a - '0' : (a & 0x7) + 9;
	b = (b <= '9') ? b - '0' : (b & 0x7) + 9;

	return (a << 4) + b;
}

static void efusew_cmd(uint32_t length, uint8_t *ack_result, uint8_t ack_len)
{
	uint8_t read_buf[128];
	uint32_t i;

	if (strncmp((void *)((uintptr_t)cmdBuf + (uintptr_t)HEADER_SIZE),
		    "efusew ", strlen("efusew ")) == 0) {
		char cmd[128] = { '\0' };
		uint8_t area_idx = 0;
		uint8_t area_size = 0;
		uint8_t data_offset = 0;
		uint8_t efuse_locked = 0;
#ifdef CONFIG_HW_WATCHDOG
		hw_watchdog_disable();
#endif
		strlcpy(cmd,
			(void *)((uintptr_t)cmdBuf + (uintptr_t)HEADER_SIZE),
			min((uint32_t)(length - HEADER_SIZE),
			    (uint32_t)sizeof(cmd)));
		NOTICE("run command: %s\n", cmd);
		run_command(cmd, 0);

		memset(ack_result, ACK_FAIL, ack_len);

		// Check param for lock efuse area
		for (area_idx = 0; area_idx < ARRAY_SIZE(_allow_wl_areas);
		     area_idx++) {
			if (strncmp((void *)((uintptr_t)cmdBuf +
					     (uintptr_t)HEADER_SIZE +
					     strlen("efusew ")),
				    _allow_wl_areas[area_idx],
				    strlen(_allow_wl_areas[area_idx])) == 0) {
				area_idx += CVI_EFUSE_AREA_LAST + 1;

				NOTICE("area_idx = %d\n", area_idx);
				if (area_idx >=
				    CVI_EFUSE_LOCK_WRITE_HASH0_PUBLIC) {
					efuse_locked = CVI_EFUSE_IsWriteLocked(
						area_idx);
					NOTICE("CVI_EFUSE_IsWriteLocked %d\n",
					       efuse_locked);
				} else {
					efuse_locked =
						CVI_EFUSE_IsLocked(area_idx);
					NOTICE("CVI_EFUSE_IsLocked %d\n",
					       efuse_locked);
				}

				if (efuse_locked > 0)
					ack_result[0] = ACK_PASS;
				break;
			}
		}

		// Read efuse data from corresponding area
		for (area_idx = 0; area_idx < ARRAY_SIZE(_allow_areas);
		     area_idx++) {
			if (strncmp((void *)((uintptr_t)cmdBuf +
					     (uintptr_t)HEADER_SIZE +
					     strlen("efusew ")),
				    _allow_areas[area_idx],
				    strlen(_allow_areas[area_idx])) == 0) {
				NOTICE("area_idx = %d\n", area_idx);
				switch (area_idx) {
				case CVI_EFUSE_AREA_USER:
					area_size = 40;
					break;
				case CVI_EFUSE_AREA_DEVICE_ID:
					area_size = 8;
					break;
				case CVI_EFUSE_AREA_HASH0_PUBLIC:
					area_size = 32;
					break;
				case CVI_EFUSE_AREA_LOADER_EK:
					area_size = 16;
					break;
				case CVI_EFUSE_AREA_DEVICE_EK:
					area_size = 16;
					break;
				default:
					NOTICE("efuse area %d\n", area_idx);
					break;
				}

				memset(read_buf, 0, sizeof(read_buf));
				memset(ack_result, ACK_FAIL, ack_len);
				ack_result[0] = ACK_PASS;

				CVI_EFUSE_Read(area_idx, read_buf, area_size);
				for (i = 0; i < area_size; i++)
					INFO("efuse_data[%d] = %x\n", i,
					     read_buf[i]);
				for (i = 0; i < length; i++)
					INFO("cmdBuf[%d] = %x\n", i, cmdBuf[i]);

				// Verify results
				data_offset = HEADER_SIZE + strlen("efusew ") +
					      strlen(_allow_areas[area_idx]) +
					      1;
				INFO("data_offset %x\n", data_offset);
				for (i = 0; i < (length - data_offset) / 2;
				     i++) {
					uint8_t data;

					data = ahex2int(
						cmdBuf[data_offset + i * 2],
						cmdBuf[data_offset + i * 2 + 1]);
					INFO("data = %x; read_buf = %x\n", data,
					     read_buf[i]);
					if (data != read_buf[i]) {
						NOTICE("ERROR err_byte_idx = %x\n",
						       i);
						NOTICE("ERROR write_byte = %x\n",
						       data);
						NOTICE("ERROR efuse_byte = %x\n",
						       read_buf[i]);
						ack_result[0] = ACK_FAIL;
						ack_result[1] = i;
						ack_result[2] = data;
						ack_result[3] = read_buf[i];
						break;
					}
				}
				break;
			}
		}
	}
}

static void efuser_cmd(uint32_t length, uint8_t *read_buf, uint8_t buf_len)
{
	uint8_t i;

	if (strncmp((void *)((uintptr_t)cmdBuf + (uintptr_t)HEADER_SIZE),
		    "efuser ", strlen("efuser ")) == 0) {
		char cmd[128] = { '\0' };
		uint8_t area_idx = 0;
		uint8_t area_size = 0;
#ifdef CONFIG_HW_WATCHDOG
		hw_watchdog_disable();
#endif
		strlcpy(cmd,
			(void *)((uintptr_t)cmdBuf + (uintptr_t)HEADER_SIZE),
			min((uint32_t)(length - HEADER_SIZE),
			    (uint32_t)sizeof(cmd)));
		NOTICE("run command: %s\n", cmd);
		run_command(cmd, 0);

		// Read efuse data from corresponding area
		for (area_idx = 0; area_idx < ARRAY_SIZE(_allow_areas);
		     area_idx++) {
			if (strncmp((void *)((uintptr_t)cmdBuf +
					     (uintptr_t)HEADER_SIZE +
					     strlen("efuser ")),
				    _allow_areas[area_idx],
				    strlen(_allow_areas[area_idx])) == 0) {
				NOTICE("area_idx = %d\n", area_idx);
				switch (area_idx) {
				case CVI_EFUSE_AREA_USER:
					area_size = 40;
					break;
				case CVI_EFUSE_AREA_DEVICE_ID:
					area_size = 8;
					break;
				case CVI_EFUSE_AREA_HASH0_PUBLIC:
					area_size = 32;
					break;
				case CVI_EFUSE_AREA_LOADER_EK:
					area_size = 16;
					break;
				case CVI_EFUSE_AREA_DEVICE_EK:
					area_size = 16;
					break;
				default:
					NOTICE("efuse area %d\n", area_idx);
					break;
				}

				memset(read_buf, 0, buf_len);
				CVI_EFUSE_Read(area_idx, read_buf, area_size);
				for (i = 0; i < area_size; i++)
					INFO("efuse_data[%d] = %x\n", i,
					     read_buf[i]);
				break;
			}
		}

		for (area_idx = 0; area_idx < ARRAY_SIZE(_allow_wl_areas);
		     area_idx++) {
			if (strncmp((void *)((uintptr_t)cmdBuf +
					     (uintptr_t)HEADER_SIZE +
					     strlen("efuser ")),
				    _allow_wl_areas[area_idx],
				    strlen(_allow_wl_areas[area_idx])) == 0) {
				area_idx += CVI_EFUSE_AREA_LAST + 1;
				NOTICE("area_idx = %d\n", area_idx);

				memset(read_buf, 0, buf_len);
				if (area_idx >=
				    CVI_EFUSE_LOCK_WRITE_HASH0_PUBLIC) {
					read_buf[0] = CVI_EFUSE_IsWriteLocked(
						area_idx);
					NOTICE("CVI_EFUSE_IsWriteLocked %d\n",
					       read_buf[0]);
				} else {
					read_buf[0] =
						CVI_EFUSE_IsLocked(area_idx);
					NOTICE("CVI_EFUSE_IsLocked %d\n",
					       read_buf[0]);
				}
				break;
			}
		}
	}
}
#endif // USB_RW_EFUSE

static void bulkOutCmplMain(struct usb_ep *ep, struct usb_request *req)
{
	struct usb_gadget *gadget = drv_obj.gadget;
	uint64_t dest_addr = 0x0;
	uint32_t i = 0;
	uint16_t crc = 0;
	struct usb_msg *msg = (struct usb_msg *)req->buf;
	struct usb_msg_s2d *msg_s2d = (struct usb_msg_s2d *)req->buf;
	struct usb_msg_d2s *msg_d2s = (struct usb_msg_d2s *)req->buf;
	uint32_t length =
		((uint32_t)msg->header.len_hi << 8) | msg->header.len_low;
	func *jump_fun;
#if USB_RW_EFUSE // Mark_to_do
	uint8_t read_buf[128];
	uint8_t ack_result[16];
	uint32_t sn_hi, sn_lo;
#endif // USB_RW_EFUSE
#ifdef CONFIG_NAND_SUPPORT
	char cmd[255] = { '\0' };
	static char prevExtra[EXTRA_FLAG_SIZE + 1] = { '\0' };
#endif

	if (req->status == -ESHUTDOWN)
		return;

	dest_addr = ((uint64_t)(msg->header.addr4) << 32) |
		    ((uint64_t)(msg->header.addr3) << 24) |
		    ((uint64_t)(msg->header.addr2) << 16) |
		    ((uint64_t)(msg->header.addr1) << 8) |
		    ((uint64_t)(msg->header.addr0));

	if (length == 0 && dest_addr == 0) {
		VERBOSE("buffer zero\n");
		resetOutReq();
		return;
	}
	/* dest_addr += GLOBAL_MEM_START_ADDR; */
	switch (msg->header.token) {
	case CVI_USB_INFO:
		/* INFO("CVI_USB_INFO\n"); */
		sendInReq(length, CVI_USB_INFO, bulkResetOutReq, NULL, 0);
		return;
	case CVI_USB_S2D:
		/* INFO("CVI_USB_S2D, addr = 0x%lx, len = 0x%lx\n",dest_addr, msg_s2d->size); */
		sendInReq(length, CVI_USB_S2D, bulkCmplEmpty, NULL, 0);

		if (dest_addr >= GLOBAL_MEM_START_ADDR)
		{
			resetOutReqS2D(dest_addr, msg_s2d->size, bulkResetOutReq);

#ifdef CONFIG_NAND_SUPPORT
			// Erase partition first
			if (!strncmp((char *)((uintptr_t)HEADER_ADDR), "CIMG", 4)) {
				strlcpy(prevExtra,
					(char *)((uintptr_t)HEADER_ADDR + 20),
					EXTRA_FLAG_SIZE);
				snprintf(cmd, 255, "nand erase.part -y %s", prevExtra);
				pr_debug("%s\n", cmd);
				run_command(cmd, 0);
			}
#endif
		}
		else
			sramOutReqS2D(dest_addr, msg_s2d->size);
		return;
	case CVI_USB_D2S:
		{
			/* INFO("CVI_USB_D2S\n"); */

			if (dest_addr)
				sendInReqD2S(dest_addr, msg_d2s->size, bulkResetOutReq);
			else
			{
				unsigned char sendbuf[8];

				uint64_t image_addr = CVIMMAP_ION_ADDR;
				for (int i=0; i<sizeof(sendbuf); i++)
				{
					sendbuf[i] = (image_addr & 0xff);
					image_addr >>= 8;
				}

				sendInReqD2S((uint64_t)sendbuf, sizeof(sendbuf), bulkResetOutReq);
			}

			return;
		}
	case CVI_USB_NONE:
		// INFO("CVI_USB_NONE, addr = 0x%llx, len = 0x%x\n", dest_addr, length);
		memcpy((void *)dest_addr, cmdBuf + HEADER_SIZE,
		       length - HEADER_SIZE);
#ifdef CONFIG_HW_WATCHDOG
		WATCHDOG_RESET();
#endif
		sendInReq(length, CVI_USB_NONE, bulkResetOutReq, NULL, 0);
		return;
	case CVI_USB_JUMP:
		jump_fun = (func *)dest_addr;
		NOTICE("CVI_USB_JUMP to %llx\n", dest_addr);
		if (gadget && gadget->ops && gadget->ops->pullup)
			gadget->ops->pullup(gadget, 0);
		NOTICE("stop USB port\n");
		jump_fun();
		NOTICE("CVI_USB_JUMP back\n");
		resetOutReq();
		break;
	case CVI_USB_PROGRAM:
		/* INFO("CVI_USB_PROGRAM\n"); */
		_prgImage((void *)UPDATE_ADDR, 0x40, NULL);
		sendInReq(length, CVI_USB_PROGRAM, bulkResetOutReq, NULL, 0);
		NOTICE("CVI_USB_PROGRAM done\n");
		return;
	case CVI_USB_RESET_ARM:
		NOTICE("CVI_USB_RESET_ARM\n");
		break;
	case CVI_USB_BREAK:
		INFO("CVI_USB_BREAK\n");
		set_config_break(1);
		break;
	case CVI_USB_KEEP_DL:
		NOTICE("CVI_USB_KEEP_DL\n");
		crc = crc16_ccitt(0, cmdBuf, length);
		if (crc == 0xB353) {
			flagEnterDL = 1;
			NOTICE("flagEnterDL %d\n", flagEnterDL);
		} else {
			flagEnterDL = 0;
			NOTICE("MAGIC NUM NOT MATCH\n");
			NOTICE("flagEnterDL %d\n", flagEnterDL);
		}
		break;
	case CVI_USB_PRG_CMD:
		NOTICE("CVI_USB_PRG_CMD\n");
		for (i = 0; i < ARRAY_SIZE(_allow_cmds); i++) {
			if (strncmp((void *)((uintptr_t)cmdBuf +
						  (uintptr_t)HEADER_SIZE),
					 _allow_cmds[i],
					 strlen(_allow_cmds[i])) == 0) {
				char cmd[255] = { '\0' };

				strncpy(cmd,
					(void *)((uintptr_t)cmdBuf +
						 (uintptr_t)HEADER_SIZE),
					min(length - HEADER_SIZE,
					    (uint32_t)254));
				NOTICE("run command: %s\n", cmd);
				run_command(cmd, 0);
				break;
			}
		}
		sendInReq(length, CVI_USB_PRG_CMD, bulkResetOutReq, NULL, 0);
		break;

#if USB_RW_EFUSE // Mark_to_do
	case CVI_USB_EFUSEW:
		NOTICE("CVI_USB_EFUSEW_CMD\n");
		efusew_cmd(length, ack_result, sizeof(ack_result));
		sendInReq(length, CVI_USB_EFUSEW, bulkResetOutReq, ack_result,
			  sizeof(ack_result));
		break;

	case CVI_USB_EFUSER:
		NOTICE("CVI_USB_EFUSER\n");
		efuser_cmd(length, read_buf, sizeof(read_buf));
		sendInReq(length, CVI_USB_EFUSER, bulkResetOutReq, read_buf,
			  40);
		break;

	case CVI_USB_READ_SN:
		NOTICE("CVI_USB_READ_SN\n");
#ifdef CONFIG_HW_WATCHDOG
		hw_watchdog_disable();
#endif
		sn_hi = readl(GP_REG4);
		sn_lo = readl(GP_REG5);
		ack_result[0] = (sn_hi >> 24) & 0xFF;
		ack_result[1] = (sn_hi >> 16) & 0xFF;
		ack_result[2] = (sn_hi >> 8) & 0xFF;
		ack_result[3] = (sn_hi >> 0) & 0xFF;
		ack_result[4] = (sn_lo >> 24) & 0xFF;
		ack_result[5] = (sn_lo >> 16) & 0xFF;
		ack_result[6] = (sn_lo >> 8) & 0xFF;
		ack_result[7] = (sn_lo >> 0) & 0xFF;
		NOTICE("0x%x%x\n", sn_hi, sn_lo);
		sendInReq(length, CVI_USB_READ_SN, bulkResetOutReq, ack_result,
			  sizeof(ack_result));
		break;
#endif // USB_RW_EFUSE
	case CVI_USB_REBOOT:
		NOTICE("CVI_USB_REBOOT\n");
		// software_root_reset(); // Mark_to_do
		mmio_setbits_32(TOP_BASE + 0x8, 0x4); // wdt reset enable
		mmio_write_32(WATCHDOG_BASE + 0x4,
			      0x20); // Set timeout range reigster
		mmio_write_32(WATCHDOG_BASE + 0xc,
			      0x76); // Counter restart register
		mmio_write_32(WATCHDOG_BASE,
			      0x13); // Response mode and enable WDT
		break;

	default:
		VERBOSE("token not defined:[%d]\n", msg->header.token);
		resetOutReq();
		break;
	}
}

/* ACM control ... data handling is delegated to tty library code.
 * The main task of this function is to activate and deactivate
 * that code based on device state; track parameters like line
 * speed, handshake state, and so on; and issue notifications.
 */

static void acm_complete_set_line_coding(struct usb_ep *ep,
					 struct usb_request *req)
{
	struct usb_cdc_line_coding *value = req->buf;

	acm->port_line_coding = *value;
	VERBOSE("acm data transfer complete\n");
}

static void print_ep0_buf(uint32_t length)
{
	int i;

	for (i = 0; i < length; i++)
		VERBOSE("%02X ", ep0Buff[i]);
	VERBOSE(" %c\n", ' ');
}

static int bind(struct usb_gadget *gadget)
{
	NOTICE("%s()\n", __func__);
	if (drv_obj.gadget) {
		NOTICE("gadget already %s\n", __func__);
		return 0;
	}
	drv_obj.gadget = gadget;

	return 0;
}

static void unbind(struct usb_gadget *gadget)
{
	NOTICE("%s()\n", __func__);
	drv_obj.gadget = NULL;
}

static int setup(struct usb_gadget *gadget, const CH9_UsbSetup *ctrl)
{
	/* get device reference */
	int length = 0;
	uint16_t status_value[2];
	struct CH9_UsbDeviceDescriptor *devDesc;
	CH9_UsbEndpointDescriptor *endpointEpInDesc, *endpointEpOutDesc,
		*endpointEpInDesc2;
	CH9_UsbSetup TmpCtrl;

	*(status_value + 0) = 0;
	*(status_value + 1) = 0;

	TmpCtrl.bRequest = ctrl->bRequest;
	TmpCtrl.bmRequestType = ctrl->bmRequestType;
	TmpCtrl.wIndex = le16ToCpu(ctrl->wIndex);
	TmpCtrl.wLength = le16ToCpu(ctrl->wLength);
	TmpCtrl.wValue = le16ToCpu(ctrl->wValue);

	VERBOSE("Speed: %d\n", gadget->speed);
	VERBOSE("bRequest: %02X\n", TmpCtrl.bRequest);
	VERBOSE("bRequestType: %02X\n", TmpCtrl.bmRequestType);
	VERBOSE("wIndex: %04X\n", TmpCtrl.wIndex);
	VERBOSE("wValue: %04X\n", TmpCtrl.wValue);
	VERBOSE("wLength: %04X\n", TmpCtrl.wLength);

	ep0Req->buf = ep0Buff;
	ep0Req->dma = (uintptr_t)ep0Buff;
	ep0Req->complete = reqComplete;

	switch (gadget->speed) {
	case CH9_USB_SPEED_FULL:
		endpointEpInDesc = &acm_fs_in_desc;
		endpointEpOutDesc = &acm_fs_out_desc;
		endpointEpInDesc2 = &acm_fs_notify_desc;
		devDesc = &devHsDesc;
		break;

	case CH9_USB_SPEED_HIGH:
		endpointEpInDesc = &acm_hs_in_desc;
		endpointEpOutDesc = &acm_hs_out_desc;
		endpointEpInDesc2 = &acm_fs_notify_desc;
		devDesc = &devHsDesc;
		break;

	default:
		VERBOSE("Unknown speed: %d\n", gadget->speed);
		return 1;
	}

	switch (ctrl->bmRequestType & CH9_USB_REQ_TYPE_MASK) {
	case CH9_USB_REQ_TYPE_STANDARD:

		switch (TmpCtrl.bRequest) {
		case CH9_USB_REQ_GET_DESCRIPTOR:
			VERBOSE("GET DESCRIPTOR %c\n", ' ');
			if ((TmpCtrl.bmRequestType & CH9_REQ_RECIPIENT_MASK) ==
			    CH9_USB_REQ_RECIPIENT_INTERFACE) {
				switch (TmpCtrl.wValue >> 8) {
				default:
					return -1;
				}
			} else if ((TmpCtrl.bmRequestType &
				    CH9_REQ_RECIPIENT_MASK) ==
				   CH9_USB_REQ_RECIPIENT_DEVICE) {
				switch (TmpCtrl.wValue >> 8) {
				case CH9_USB_DT_DEVICE:
					length = CH9_USB_DS_DEVICE;
					if (cvi_usb_vid != 0) {
						NOTICE("Patch VID %x\n",
						       cvi_usb_vid);
						devDesc->idVendor =
							cpuToLe16(cvi_usb_vid);
					}
					if (cvi_usb_pid != 0) {
						NOTICE("Patch PID %x\n",
						       cvi_usb_pid);
						devDesc->idProduct =
							cpuToLe16(cvi_usb_pid);
					}
					memmove(ep0Buff, devDesc, 18);
					VERBOSE("DevDesc[0] = %d\n",
						devDesc->bLength);
					print_ep0_buf(length);
					break;

				case CH9_USB_DT_CONFIGURATION: {
					uint8_t *ptr =
						&ep0Buff[CH9_USB_DS_CONFIGURATION];
					uint16_t acmDescLen =
						(uint16_t)getDescAcm(
							gadget->speed, ptr);

					length = le16ToCpu(
						acmDescLen +
						CH9_USB_DS_CONFIGURATION);
					ConfDesc.wTotalLength =
						cpuToLe16(length);
					memmove(ep0Buff, &ConfDesc,
						CH9_USB_DS_CONFIGURATION);
					print_ep0_buf(length);
					break;
				}

				case CH9_USB_DT_STRING: {
					uint8_t descIndex =
						(uint8_t)(TmpCtrl.wValue &
							  0xFF);
					char *strDesc;

					VERBOSE("StringDesc %c\n", ' ');
					switch (descIndex) {
					case 0:
						strDesc = (char *)&languageDesc;
						length = strDesc[0];
						VERBOSE("language %c\n", ' ');
						break;

					case 1:
						strDesc = (char *)&vendorDesc;
						length = strDesc[0];
						VERBOSE("vendor %c\n", ' ');
						break;

					case 2:
						strDesc = (char *)&productDesc;
						length = strDesc[0];
						VERBOSE("product %c\n", ' ');
						break;

					case 3:
						strDesc = (char *)&serialDesc;
						length = strDesc[0];
						VERBOSE("serial %c\n", ' ');
						break;

					default:
						return -1;
					}
					memmove(ep0Buff, strDesc, length);
					break;
				}

				case CH9_USB_DT_BOS: {
					int offset = 0;

					length =
						le16ToCpu(bosDesc.wTotalLength);

					memmove(ep0Buff, &bosDesc,
						CH9_USB_DS_BOS);
					offset += CH9_USB_DS_BOS;
					memmove(&ep0Buff[offset],
						&capabilityExtDesc,
						CH9_USB_DS_DEVICE_CAPABILITY_20);
				}
					print_ep0_buf(length);
					VERBOSE("BosDesc %c\n", ' ');
					break;

				case CH9_USB_DT_DEVICE_QUALIFIER:
					length = CH9_USB_DS_DEVICE_QUALIFIER;
					memmove(ep0Buff, &qualifierDesc,
						length);
					break;

				case CH9_USB_DT_OTHER_SPEED_CONFIGURATION: {
					uint8_t *ptr =
						&ep0Buff[CH9_USB_DS_CONFIGURATION];
					uint16_t acmDescLen =
						(uint16_t)getDescAcm(
							gadget->speed, ptr);

					length = le16ToCpu(
						acmDescLen +
						CH9_USB_DS_CONFIGURATION);
					ConfDesc.wTotalLength =
						cpuToLe16(length);
					memmove(ep0Buff, &ConfDesc,
						CH9_USB_DS_CONFIGURATION);
					print_ep0_buf(length);
					break;
				}

				default:
					return -1;

				} /* switch */
			} /* if */
			break;

		case CH9_USB_REQ_SET_CONFIGURATION: {
			struct usb_ep *ep;
			struct list_head *list;

			VERBOSE("SET CONFIGURATION(%d)\n",
				le16ToCpu(TmpCtrl.wValue));
			if (TmpCtrl.wValue > 1)
				return -1; /* no such configuration */
			/* unconfigure device */
			if (TmpCtrl.wValue == 0) {
				configValue = 0;
				for (list = gadget->ep_list.next;
				     list != &gadget->ep_list;
				     list = list->next) {
					ep = (struct usb_ep *)list;
					if (ep->name)
						ep->ops->disable(ep);
				}
				return 0;
			}

			/* device already configured */
			if (configValue == 1 && TmpCtrl.wValue == 1)
				return 0;

			/* configure device */
			configValue = (uint8_t)TmpCtrl.wValue;

			for (list = gadget->ep_list.next;
			     list != &gadget->ep_list; list = list->next) {
				ep = (struct usb_ep *)list;
				if (ep->name &&
				    (!strcmp(ep->name, "ep1in-bulk"))) {
					ep->ops->enable(ep, endpointEpInDesc);
					VERBOSE("enable EP IN\n");
					break;
				}
			}
			for (list = gadget->ep_list.next;
			     list != &gadget->ep_list; list = list->next) {
				ep = (struct usb_ep *)list;
				if ((ep->name &&
				     !strcmp(ep->name, "ep2out-bulk"))) {
					ep->ops->enable(ep, endpointEpOutDesc);
					VERBOSE("enable EP OUT\n");
					break;
				}
			}
			for (list = gadget->ep_list.next;
			     list != &gadget->ep_list; list = list->next) {
				ep = (struct usb_ep *)list;
				if ((ep->name &&
				     !strcmp(ep->name, "ep3in-int"))) {
					ep->ops->enable(ep, endpointEpInDesc2);
					break;
					VERBOSE("enable EP Notify\n");
				}
			}

			/*Code control  Self powered feature of USB*/
			if (ConfDesc.bmAttributes &
			    CH9_USB_CONFIG_SELF_POWERED) {
				if (gadget->ops->set_selfpowered) {
					gadget->ops->set_selfpowered(gadget, 1);
				}
			} else {
				if (gadget->ops->set_selfpowered) {
					gadget->ops->set_selfpowered(gadget, 0);
				}
			}
		}
			return 0;

		case CH9_USB_REQ_GET_CONFIGURATION:
			length = 1;
			memmove(ep0Buff, &configValue, length);
			/* VERBOSE("CH9_USB_REQ_GET_CONFIGURATION %c\n", ' '); */
			break;

		default:
			return -1;
		}
		break;

	case CH9_USB_REQ_TYPE_CLASS: {
		/* SET_LINE_CODING ... just read and save what the host sends */
		switch (TmpCtrl.bRequest) {
		case USB_CDC_REQ_SET_LINE_CODING:
			length = TmpCtrl.wLength;
			ep0Req->complete = acm_complete_set_line_coding;
			VERBOSE("USB_CDC_REQ_SET_LINE_CODING %d\n", length);
			set_acm_config(1);
			break;
		case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
			acm->port_handshake_bits = TmpCtrl.wValue;
			set_acm_config(1);
			VERBOSE("USB_CDC_REQ_SET_CONTROL_LINE_STATE %c\n", ' ');
			break;
		case USB_CDC_REQ_GET_LINE_CODING:
			length = TmpCtrl.wLength;
			memmove(ep0Buff, &acm->port_line_coding, length);
			/* ep0Req->complete = acm_complete_get_line_coding; */
			VERBOSE("USB_CDC_REQ_GET_LINE_CODING %d\n", length);
			set_acm_config(1);
			break;
		}
		break;
	}
	}

	if (length > 0) {
		ep0Req->length =
			TmpCtrl.wLength < length ? TmpCtrl.wLength : length;

		cvi_cache_flush(ep0Req->dma, ep0Req->length);
		gadget->ep0->ops->queue(gadget->ep0, ep0Req);
	}
	return 0;
}

static void get_unicode_string(char *target, const char *src)
{
	size_t src_len = strlen(src) * 2;
	int i;

	*target++ = src_len + 2;
	*target++ = CH9_USB_DT_STRING;

	if (src_len > 100)
		src_len = 100;
	for (i = 0; i < src_len; i += 2) {
		*target++ = *src++;
		*target++ = 0;
	}
}

static struct usb_gadget_driver g_driver = {
	.function = "TTY",
	.speed = CH9_USB_SPEED_HIGH,
	.bind = bind,
	.unbind = unbind,
	.setup = setup,
	.reset = reset,
	.disconnect = disconnect,
	.suspend = suspend,
	.resume = resume,
	.req_mem_alloc = requestMemAlloc,
	.req_mem_free = requestMemFree,
};

int acm_app_init(void)
{
	struct usb_ep *ep0 = drv_obj.gadget->ep0;

	/*  set unicode strings */
	get_unicode_string(vendorDesc, USB_MANUFACTURER_STRING);
	get_unicode_string(productDesc, USB_PRODUCT_STRING);
	get_unicode_string(serialDesc, USB_SERIAL_NUMBER_STRING);

	/*  align buffers to modulo8 address */
	ep0Buff = ep0BuffAlloc;
	bulkBuf = bulkBufAlloc;
	cmdBuf = cmdBufAlloc;

	memset(ep0BuffAlloc, 0x00, EP0_SIZE);
	memset(bulkBufAlloc, 0x00, BUF_SIZE);
	memset(cmdBufAlloc, 0x00, BUF_SIZE);

	/* allocate request for ep0 */
	ep0Req = ep0->ops->alloc_request(ep0);

	/* Change descriptor for maxSpeed == HS only Device*/
	/* For USB2.0 we have to modified wTotalLength of BOS descriptor*/
	if (drv_obj.gadget->max_speed < CH9_USB_SPEED_SUPER) {
		bosDesc.wTotalLength = cpuToLe16(
			CH9_USB_DS_BOS + CH9_USB_DS_DEVICE_CAPABILITY_20);
		bosDesc.bNumDeviceCaps = 1;
		devHsDesc.bcdUSB = cpuToLe16(BCD_USB_HS_ONLY);
	}

	/* acm init */
	acm = (struct f_acm *)acm_buf;
	acm->port_line_coding.dwDTERate = 921600;
	acm->port_line_coding.bCharFormat = USB_CDC_1_STOP_BITS;
	acm->port_line_coding.bParityType = USB_CDC_NO_PARITY;
	acm->port_line_coding.bDataBits = 8;
	acm->port_handshake_bits = 0;
	/* VERBOSE("acm size %X\n", sizeof(struct f_acm)); */
	return 0;
}

#ifdef BUILD_ATF
uint8_t usb_phy_det_connection(void)
{
	uint8_t phy_det_connected = 0;
	uint32_t cvi_usb_phy_config = 0;

	cvi_usb_phy_config = plat_cvi_gpio_read(BIT_MASK_GPIO_USB_PHY_DET_OFF);
	if (cvi_usb_phy_config == 0) {
		phy_det_connected = 1;
		NOTICE("by pass USB phy detection\n");
	} else {
		mmio_clrbits_32(GPIO_BASE + 0x4, 1 << 4);
		phy_det_connected =
			(mmio_read_32(GPIO_BASE + 0x50) & (1 << 4)) >> 4;
		INFO("phy_det_connected %d\n", phy_det_connected);
	}

	return phy_det_connected;
}
#endif

#ifndef BUILD_ATF
uint32_t plat_cvi_gpio_read(uint32_t mask)
{
	NOTICE("Overwrite fip_src to FIP_SRC_USB\n");
	return FIP_SRC_USB;
}
#endif

#if defined(USB_PHY_DETECTION)
uint8_t usb_vbus_det(void)
{
	uint8_t vbus;
	uint32_t save_pinmux;

	save_pinmux = mmio_read_32(PINMUX_USB_VBUS_DET);
	mmio_write_32(PINMUX_USB_VBUS_DET, 0);
	vbus = (!!(mmio_read_32(REG_TOP_CONF_INFO) & BIT_TOP_CONF_INFO_VBUS));
	mmio_write_32(PINMUX_USB_VBUS_DET, save_pinmux);

	return vbus;
}
#endif

void acm_patch_id(unsigned short vid, unsigned short pid)
{
	cvi_usb_vid = vid;
	cvi_usb_pid = pid;
}

/* ACM entry */
int acm_app(void)
{
	struct usb_gadget *gadget;
	uint32_t res = 0; /* keeps result of operation on driver */
	struct list_head *list; // used in for_each loop
	struct usb_ep *ep;
	int fip_src = FIP_SRC_MEMMAP;
	uint32_t ts = 0;
#ifdef BUILD_ATF
	uint8_t phy_det_connected = 0;

	phy_det_connected = usb_phy_det_connection();

	if (phy_det_connected == 0) {
		NOTICE("USB cable is not connected\n");
		return res;
	}
#endif

#if defined(USB_PHY_DETECTION)
	{
		uint32_t cnt = 50;

		INFO("waiting for connection ...\n");
		/* debounce */
		while (cnt--) {
			if (!usb_vbus_det())
				cnt = 50;
			mdelay(1);
		}
		INFO("detect vbus ...\n");
	}
#endif
	init_param();
	acm_mem_init();
	print_buf_addr();
	fip_src = plat_cvi_gpio_read(0);
	NOTICE("fip_src %d\n", fip_src);
	if (fip_src == FIP_SRC_USB)
		flagEnterDL = 1;
	else
		flagEnterDL = 0;
#ifdef USB_IRQ_MODE
	request_irq(USB_DEV_INTR0, AcmIsr, 0, NULL, NULL);
	request_irq(USB_DEV_INTR1, AcmIsr, 0, NULL, NULL);
#endif
	drv_obj.plat.handler = (void *)handler;
	drv_obj.plat.size = HANDLER_SIZE;
	drv_obj.plat.ctrl_req = (void *)setup_buf;
	res = cvi_udc_probe(&drv_obj.plat);
	if (res != 0) {
		goto error;
	}

	/* bind the gadget object here. */
	if (cviusb_gadget_register_driver(&g_driver) < 0) {
		NOTICE("Gadget Register Fail\n");
		goto error;
	}
	gadget = drv_obj.gadget;
	if (!gadget) {
		NOTICE("Gadget object not existed!\n");
		goto error;
	}

	acm_app_init();

	VERBOSE("Initializing OK! %d\n", __LINE__);

	ts = get_timer(0);
	VERBOSE("ts: %u\n", get_timer(ts));

	gadget->ops->pullup(gadget, 1);
unconfigured:
	while (!get_acm_config()) {
#ifndef USB_IRQ_MODE
		AcmIsr();
#endif
		if (get_timer(ts) > 1000 && flagEnterDL == 0) {
			NOTICE("Enumeration failed\n");
			acm_mem_release();
			return 0;
		}
	}
	NOTICE("USB enumeration done\n");

	mem_alloc_cnt = 1;
	/* find IN endpoint */
	for (list = gadget->ep_list.next; list != &gadget->ep_list;
	     list = list->next) {
		ep = (struct usb_ep *)list;
		if (ep->desc && (ep->desc->bEndpointAddress == BULK_EP_IN)) {
			bulkInReq = ep->ops->alloc_request(ep);
			epIn = ep;
		} else if (ep->desc &&
			   (ep->desc->bEndpointAddress == BULK_EP_OUT)) {
			bulkOutReq = ep->ops->alloc_request(ep);
			epOut = ep;
		} else if (ep->desc &&
			   (ep->desc->bEndpointAddress == BULK_EP_NOTIFY)) {
			IntInReq = ep->ops->alloc_request(ep);
			epInNotify = ep;
		}
	}

	current_speed = gadget->speed;
	switch (current_speed) {
	case CH9_USB_SPEED_FULL:
		transfer_size = 64;
		break;
	case CH9_USB_SPEED_HIGH:
		transfer_size = 512;
		break;
	case CH9_USB_SPEED_SUPER:
		transfer_size = 1024;
		break;
	default:
		VERBOSE("Test error\n");
		acm_mem_release();
		return -1;
	}

	VERBOSE("OUT DATA TRANSFER size :%d\n", transfer_size);
	clearReq(bulkOutReq);
	bulkOutReq->buf = cmdBuf;
	bulkOutReq->dma = (uintptr_t)cmdBuf;
	bulkOutReq->complete = bulkOutCmplMain;
	bulkOutReq->length = transfer_size;
	cvi_cache_flush(bulkOutReq->dma, bulkOutReq->length);

	VERBOSE("IN DATA TRANSFER\n");
	clearReq(bulkInReq);
	bulkInReq->buf = bulkBuf;
	bulkInReq->dma = (uintptr_t)bulkBuf;
	bulkInReq->complete = bulkResetOutReq;
	bulkInReq->length = transfer_size;
	cvi_cache_flush(bulkInReq->dma, bulkInReq->length);

	epOut->ops->queue(epOut, bulkOutReq);
	NOTICE("connection speed: %d\n", gadget->speed);
	ts = get_timer(0);
	VERBOSE("ts: %u\n", get_timer(ts));

	while (1) {
#ifndef USB_IRQ_MODE
		AcmIsr();
#endif
		if (!get_acm_config())
			goto unconfigured;
		if (get_config_break())
			break;
		if (flagEnterDL == 0) {
			if (get_timer(ts) > 1000) {
				NOTICE("wait data timeout\n");
				break;
			}
		}
	}
	NOTICE("Leave transfer loop\n");
	gadget->ops->pullup(gadget, 0);
	cviusb_gadget_unregister_driver(&g_driver);
	acm_mem_release();
	NOTICE("USB stop\n");
	return 0;

error:
	ERROR("Error %u\n", res);
	return res;
}
