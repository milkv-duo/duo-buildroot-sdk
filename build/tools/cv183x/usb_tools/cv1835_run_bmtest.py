#!/usr/local/bin/python

from cv_usb_util.cv_usb_pyserial import cv_usb_pyserial
#from cv_usb_util.cv_usb_libusb import cv_usb_libusb
import os
import sys
import time
import cv_usb_util.cv_usb_pkt as pkt
from array import array

if __name__ == '__main__':
    print("CV1835 run bmtest...")
    filename = sys.argv[1]
    total_time = time.time()

    cv_usb_serial = cv_usb_pyserial()
    if len(sys.argv) > 1:
        cv_usb_serial.parse_arg()

    print("Connecting to uboot...")
    cv_usb_serial.serial_query([pkt.uboot_vidpid, pkt.prg_vidpid])
    time.sleep(0.02)

    # raw_input("break")
    cnt = 0
    while cnt < 1:
        cnt = cnt + 1

        print("Sending %s        " % filename)
        cv_usb_serial.usb_send_file(filename, 0x108100000, 0)
        # print ("Done %d" % cnt)

    cv_usb_serial.usb_send_req_data(pkt.CV_USB_BREAK, 0x04003000, 0, None)
    print("CV_USB_BREAK is sent")
