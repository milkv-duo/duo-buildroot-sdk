#ifndef __CVI_UPDATE_H__
#define __CVI_UPDATE_H__

#define EXTRA_FLAG_SIZE 32
#define COMPARE_STRING_LEN 
#define SD_UPDATE_MAGIC 0x4D474E32
#define ETH_UPDATE_MAGIC 0x4D474E35
#define USB_DRIVE_UPGRADE_MAGIC 0x55425355
#define FIP_UPDATE_MAGIC 0x55464950
#define UPDATE_DONE_MAGIC 0x50524F47
#define OTA_MAGIC 0x5245434F
//#define ALWAYS_USB_DRVIVE_UPGRATE

#define SECTOR_SIZE 0x200
#define HEADER_MAGIC "CIMG"
#define MAX_LOADSIZE (16 * 1024 * 1024)

enum chunk_type_e { dont_care = 0, check_crc };
enum storage_type_e { sd_dl = 0, usb_dl };

// UART update defines
#define UART_UPDATE_MAGIC 0x4D474E33
#define UART_DL_BAUDRATE 1500000

#undef pr_debug
#ifdef DEBUG
#define pr_debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define pr_debug(fmt, ...)
#endif

int _prgImage(char *file, uint32_t chunk_header_size, char *file_name);

#endif /* __CVI_UPDATE_H__ */
