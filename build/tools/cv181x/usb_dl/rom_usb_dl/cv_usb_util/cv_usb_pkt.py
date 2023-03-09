#!/usr/local/bin/python
from array import array
import os
import re
import serial.tools.list_ports
import sys
import time

# ========================================================================
# Define
# ========================================================================
rom_vidpid = "VID:PID=3346:1000"
rom_cvi_vidpid = "VID:PID=3346:1000"
prg_vidpid = "VID:PID=30B1:1000"
uboot_vidpid = "VID:PID=30B1:1001"
uboot_cvi_vidpid = "VID:PID=3346:1001"
verify_vidpid = "VID:PID="
kernel_acm_vidpid = "VID:PID=30B1:1003"
kernel_libusb_vidpid = "VID:PID=30B1:1003"

SUCCESS = 0
FAIL = 1
TIMEOUT = -1

HEADER_SIZE = 8

# ROM USB command
CVI_USB_TX_DATA_TO_RAM = 0
CVI_USB_TX_FLAG = 1

# uboot USB command
CV_USB_NONE = 0
CV_USB_INFO = 1

# Common command
CV_USB_BREAK = 2
CV_USB_KEEP_DL = 3
CV_USB_UBREAK = 4
CV_USB_PRG_CMD = 6
CVI_USB_REBOOT = 22
CVI_USB_PROGRAM = 0x83

# Cannot be too larger in Windows!
USB_BULK_MAX_SIZE = 0x80000  # 0x4000000

MSG_TOKEN_OFFSET = 0

RSP_CRC16_HI_OFFSET = 2
RSP_CRC16_LO_OFFSET = 3
RSP_TOKEN_OFFSET = 6

DUMMY_ADDR = 0xFF
DDR_FIP_ADDR = 0x80800000
IMG_ADDR = 0x83940000

FIP_TX_OFFSET = 0
FIP_TX_SIZE = 0
