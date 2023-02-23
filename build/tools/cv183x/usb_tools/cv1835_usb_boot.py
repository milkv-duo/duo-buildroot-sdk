#!/usr/local/bin/python

from cv_usb_util.cv_usb_pyserial import cv_usb_pyserial
#from cv_usb_util.cv_usb_libusb import cv_usb_libusb
import os
import sys
import time
import cv_usb_util.cv_usb_pkt as pkt
from array import array

if __name__ == '__main__':
    enter_linux = 0
    if len(sys.argv) != 1:
        filename = sys.argv[1]
        enter_linux = 1
    else:
        filename = ""
        enter_linux = 0
    print("CV1835 USB boot %s, start" % filename)
    total_time = time.time()

    cv_usb_serial = cv_usb_pyserial()
    if len(sys.argv) > 1:
        cv_usb_serial.parse_arg()

    print("Connecting to ROM code...")
    cv_usb_serial.serial_query([pkt.rom_vidpid, pkt.prg_vidpid])
    time.sleep(0.02)
    cv_usb_serial.usb_send_file('cv_dl_magic.bin', 0x4003000, 0)
    print("Done")

    # raw_input("break")
    cnt = 0
    while cnt < 1:
        cnt = cnt + 1
        print("send fip.bin        ")
        cv_usb_serial.usb_send_file('fip.bin', 0x0C040000, 0)
        # print ("Done %d" % cnt)

    # Set SRAM flag
    flag = array('B')
    flag.append(ord('1'))
    flag.append(ord('N'))
    flag.append(ord('G'))
    flag.append(ord('M'))
    cv_usb_serial.usb_send_req_data(pkt.CV_USB_NONE, 0x0E00FC00, 12, flag)
    print("Magic number is sent")

    if enter_linux == 1:
        # Set SRAM flag
        flag = array('B')
        flag.append(ord('1'))
        flag.append(ord('N'))
        flag.append(ord('G'))
        flag.append(ord('M'))
        cv_usb_serial.usb_send_req_data(pkt.CV_USB_NONE, 0x0E00FC08, 12, flag)
        print("Magic number is sent")

    cv_usb_serial.usb_send_req_data(pkt.CV_USB_BREAK, 0x04003000, 0, None)
    print("CV_USB_BREAK is sent")
    print("CV1835 USB boot, done")

    if enter_linux == 1:
        print("Connecting to u-boot...")
        cv_usb_serial.serial_query([pkt.uboot_vidpid, pkt.prg_vidpid])
        time.sleep(0.02)
        # raw_input("break")
        cnt = 0
        while cnt < 1:
            cnt = cnt + 1
            print("send ramboot_mini.itb        ")
            cv_usb_serial.usb_send_file('ramboot_mini.itb', 0x100090000, 0)
            # print ("Done %d" % cnt)

        cv_usb_serial.usb_send_req_data(pkt.CV_USB_BREAK, 0x04003000, 0, None)
        print("CV_USB_BREAK is sent")
        print("CV1835 USB boot, done")
