#!/usr/local/bin/python

# ========================================================================
# Define
# ========================================================================
rom_vidpid = 'VID:PID=30B1:1000'
rom_cvi_vidpid = 'VID:PID=3346:1000'
prg_vidpid = 'VID:PID=30B1:1000'
uboot_vidpid = 'VID:PID=30B1:1001'
uboot_cvi_vidpid = 'VID:PID=3346:1001'
verify_vidpid = 'VID:PID='
kernel_acm_vidpid = 'VID:PID=30B1:1003'
kernel_libusb_vidpid = 'VID:PID=30B1:1003'

SUCCESS = 0
FAIL = 1
SERIAL_TIMEOUT = 2
FIP_NOT_FOUND = 3
MGC_NOT_FOUND = 4
PARAM_ERROR = 5

HEADER_SIZE = 8

CV_USB_NONE = 0
CV_USB_INFO = 1
CV_USB_VERBOSE = 2
CV_USB_JUMP = 3
CV_USB_BREAK = 4
CV_USB_KEEP_DL = 5
CV_USB_PRG_CMD = 6
CV_USB_RESET_ARM = 7
CV_USB_TEST_THERMAL_SENSOR = 8
CV_USB_TEST_EMMC = 9
CV_USB_EFUSEW = 11
CV_USB_READ_SN = 12
CV_USB_EFUSE_PATCH = 13
CV_USB_EFUSER = 14
CV_USB_NONE_FAST = 20
CV_USB_JUMP_ACK = 21
CVI_USB_REBOOT = 22

# ========================================================================
# New protocol of prg.bin
# ========================================================================
USB_MSG_S2D_SIZE = 16
USB_MSG_D2S_SIZE = 16
CV_USB_RUNTIME = 0x80
CV_USB_S2D = 0x81
CV_USB_D2S = 0x82
CVI_USB_PROGRAME = 0x83
# Cannot be too larger in Windows!
USB_BULK_MAX_SIZE = 0x80000  # 0x4000000

MSG_TOKEN_OFFSET = 0

RSP_CRC16_HI_OFFSET = 2
RSP_CRC16_LO_OFFSET = 3
RSP_TOKEN_OFFSET = 6
