#ifndef CVI_USB_H
#define CVI_USB_H

#include <sys/types.h>
#include "dwc2_stdtypes.h"
#define BCD_USB_SS                      0x0300  // 3.00 version USB
#define ID_VENDOR                       0x3346  // Cvitek
#define ID_PRODUCT                      0x1000  // TTY
#define BCD_DEVICE_SS                   0x0001  // 0.1
#define BCD_USB_HS_ONLY                 0x0201  // 2.01  /*Only HS with BOS descriptor*/
#define BCD_USB_HS                      0x0210  // 2.10
#define BCD_DEVICE_HS                   0x0200  // 2.00

#define USB_MANUFACTURER_STRING         "CVITEK"
#define USB_PRODUCT_STRING              "USB Com Port"
#define USB_SERIAL_NUMBER_STRING        "123456789ABC" // should 12 chars long

#define DISABLE_TIMEOUT			99999

#define USB_PHY_VBUS_VALID		BIT(0)
#define USB_PHY_ID_OVERRIDE_ENABLE	BIT(6)
#define USB_PHY_ID_VALUE		BIT(7)

#define BULK_EP_IN 0x81
#define BULK_EP_OUT 0x01
#define BULK_EP_NOTIFY 0x82
#define HEADER_SIZE 8

#define SS_PERIPH_DISABLED_SET (1 << 21)
#define OTG_NRDY               (1 << 11)
#define HOST_BUS_DROP          (1 << 9)
#define DEV_BUS_REQ            (1)

#define CV_USB_DL 0x1

TYPEDEF enum {
CVI_USB_TX_DATA_TO_RAM = 0,
CVI_USB_TX_FLAG,
CVI_USB_BREAK,
CVI_USB_KEEP_DL,
} CVI_USB_TOKEN;

typedef void func(void);

#define ALIGN_CACHE_SIZE(x)	ROUND(x, CONFIG_SYS_CACHELINE_SIZE)

#define USB_BUF_BASE      CV_IO_BUF_BASE

#define PD_SIZE           4096
#define BUF_SIZE          512
#define EP0_SIZE          128
#define CB_SIZE           128
#define RSP_SIZE          16
#define ACM_SIZE          128
#define STP_SIZE          8
#define HANDLER_SIZE	  1024

#define BLK_BUF_ADDR  USB_BUF_BASE				// 512
#define CMD_BUF_ADDR  (BLK_BUF_ADDR + ALIGN_CACHE_SIZE(BUF_SIZE))  // 512
#define CB0_BUF_ADDR  (CMD_BUF_ADDR + ALIGN_CACHE_SIZE(BUF_SIZE))  // 128
#define CB1_BUF_ADDR  (CB0_BUF_ADDR + ALIGN_CACHE_SIZE(CB_SIZE))   // 128
#define CB2_BUF_ADDR  (CB1_BUF_ADDR + ALIGN_CACHE_SIZE(CB_SIZE))   // 128
#define EP0_BUF_ADDR  (CB2_BUF_ADDR + ALIGN_CACHE_SIZE(CB_SIZE))   // 64
#define RSP_BUF_ADDR  (EP0_BUF_ADDR + ALIGN_CACHE_SIZE(EP0_SIZE))  // 32
#define ACM_BUF_ADDR  (RSP_BUF_ADDR + ALIGN_CACHE_SIZE(RSP_SIZE))  // 128
#define STP_BUF_ADDR  (ACM_BUF_ADDR + ALIGN_CACHE_SIZE(ACM_SIZE))  // 32
#define HANDLER_ADDR  (STP_BUF_ADDR + ALIGN_CACHE_SIZE(STP_SIZE))  // 1024

extern uint16_t cv_usb_vid;

int AcmApp(void *buf, uint32_t offset, uint32_t size);

#endif
