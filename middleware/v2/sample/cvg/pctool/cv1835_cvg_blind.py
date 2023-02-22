#!/usr/local/bin/python

from cv_usb_util.cv_usb_libusb import cv_usb_libusb
import os
import sys
import time
import cv_usb_util.cv_usb_pkt as pkt
from array import array
import random
import threading

HEADER_SIZE = 8
OFFSET_LEN = 0
OFFSET_CRC = 4
MAX_SIZE = 2*1024*1024
test_cnt = 5000
offset = 8

def gen_tx_data(data, len):
    for i in range(0, len):
        data.append(random.randint(0,255))

def tx_thread(cv_usb_libusb):
    #total_time = 0
    start_len = HEADER_SIZE + 4
    request_len = start_len

    for i in range(0, test_cnt):
        data_len = request_len - HEADER_SIZE
        data = array('B')
        header = array('B')
        gen_tx_data(data, data_len)
        checksum = cv_usb_libusb.crc16_ccitt(data)
        header.append(data_len & 0xFF)
        header.append(data_len>>8 & 0xFF)
        header.append(data_len>>16 & 0xFF)
        header.append(data_len>>24 & 0xFF)
        header.append(checksum & 0xFF)
        header.append(checksum>>8 & 0xFF)
        header.append(0)
        header.append(0)
        data = header + data
        actual_len = cv_usb_libusb.libusb_write(data, 0, 0)
        if actual_len != request_len:
            print ("host send too slow, maybe try again?")
            # TODO here
            break;
        request_len = request_len + 8
        if request_len > MAX_SIZE:
            request_len = start_len
        #print (data)
        del data[:]
        del header[:]
    print("TX blind test complete")

if __name__ == '__main__':
    print ("Cvitek USB Gadget blind transfer test")
    #total_time = 0
    start_len = HEADER_SIZE + 4
    request_len = start_len

    cv_usb_libusb = cv_usb_libusb()
    if len(sys.argv) > 1:
        cv_usb_libusb.parse_arg()

    device = cv_usb_libusb.libusb_query([pkt.kernel_libusb_vidpid], 10)
    print ("Done")
    t_thread = threading.Thread(target = tx_thread, args = (cv_usb_libusb,))
    t_thread.start()

    for i in range(0, test_cnt):
        data = cv_usb_libusb.libusb_read(request_len)
        if len(data) != request_len:
            print ("host rcvs timeout, may be try again?")
            # TODO here
            break
        #print (data)
        header = data[0:8]
        data_len = header[0] + (header[1]<<8) + (header[2]<<16) + (header[3]<<24)
        if (data_len != (request_len-HEADER_SIZE)):
            print ("size mismatch %d %d" %(data_len, request_len-HEADER_SIZE))
            break

        crc1 = header[4] + (header[5]<<8)
        crc2 = cv_usb_libusb.crc16_ccitt(data[8:request_len])
        if (crc1 != crc2):
            print ("crc mismatch %x %x" %(crc1, crc2))
            break

        request_len = request_len + 8
        if request_len > MAX_SIZE:
            request_len = start_len
        del data[:]
        del header[:]
    print("RX blind test complete")
    t_thread.join()
    del cv_usb_libusb
    #print ("--- send %d B in %s Seconds %d MB per second ---\n" %(total_size, total_time, round((total_size/total_time)/1024/1024,2)))
