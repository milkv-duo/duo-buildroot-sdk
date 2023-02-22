#!/usr/local/bin/python

from cv_usb_util.cv_usb_libusb import cv_usb_libusb
import os
import sys
import time
import cv_usb_util.cv_usb_pkt as pkt
from array import array
import filecmp
import random

test_cnt = 500
file_path = '/mnt/data/'
#patterns = ['ramboot_mini.itb']
#patterns = ['bulk01']
patterns = ['bulk01', 'bulk02', 'bulk03', 'bulk04', 'bulk05', 'bulk06', 'bulk07', 'bulk08']
check_files = ['Rbulk00']

if __name__ == '__main__':
    print ("Cvitek USB gadget file transfer test")
    total_time = 0
    total_read_time = 0
    total_write_time = 0
    total_size = 0

    cv_usb_libusb = cv_usb_libusb()
    if len(sys.argv) > 1:
        cv_usb_libusb.parse_arg()

    device = cv_usb_libusb.libusb_query([pkt.kernel_libusb_vidpid], 10)
    print ("Done")

    for i in range(0, test_cnt):
        filename = patterns[random.randint(0, len(patterns)-1)]
        filename1 = check_files[i%len(check_files)]
        file_size = os.path.getsize(filename)
        total_size = total_size + file_size*2 # send and receive

        target_path = file_path + filename
        print ("Sending %s (%dB)..." % (filename, file_size))
        cv_usb_libusb.usb_send_req_kernel(pkt.CV_USB_S2D, file_size, target_path, 0)
        dest_addr = 0
        cv_usb_libusb.usb_send_file(filename, dest_addr, 0)
        total_time = total_time + cv_usb_libusb.ioTime
        total_write_time = total_write_time + cv_usb_libusb.ioTime

        cv_usb_libusb.usb_send_req_kernel(pkt.CV_USB_D2S, file_size, target_path, 0)
        dest_addr = 0
        cv_usb_libusb.usb_read_file(filename1, file_size)
        total_time = total_time + cv_usb_libusb.ioTime
        total_read_time = total_read_time + cv_usb_libusb.ioTime

        if (filecmp.cmp(filename, filename1) is True):
            print ("[%d] bulk test pass" %i)
        else:
            print ("bulk-out test fail")
            sys.exit(-1)

    del cv_usb_libusb
    print ("--- send %d B in %s Seconds %d MB per second ---\n" %(total_size, total_time, round((total_size/total_time)/1024/1024,2)))
    print ("--- Read %d MB/s Write %d MB/s ---\n" %(round((total_size/2/total_read_time)/1024/1024,2), round((total_size/2/total_write_time)/1024/1024,2)))
