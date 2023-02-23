#!/usr/bin/python3
from cv_usb_util.cv_usb_pyserial import cv_usb_pyserial
import argparse
import logging
import os
import sys
import cv_usb_util.cv_usb_pkt as pkt
from singleton import SingleInstance
from array import array
import time

parser = argparse.ArgumentParser(description='usb download scripts for cv1822')


def parse_Args():
    cur_dir = os.path.abspath(os.getcwd())
    parser.add_argument(
        '--image_dir',
        metavar='path',
        type=str,
        default=cur_dir,
        help='the folder path to dir inclued fip,rootfs kernel and xml')
    parser.add_argument(
        '--location',
        metavar='',
        type=str)
    parser.add_argument(
        '--pid',
        metavar='1001',
        default='1001',
        type=str)
    parser.add_argument("-v",
                        "--verbose",
                        help="increase output verbosity",
                        action="store_true")
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--serial', action='store_true', default=False)
    group.add_argument('--libusb', action='store_true', default=False)

    args = parser.parse_args()

    return args


def usage():
    parser.print_usage()


def resource_path(relative_path):
    """ Get absolute path to resource, works for dev and for PyInstaller   """
    try:
        # PyInstaller creates a temp folder and stores path in _MEIPASS
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.dirname(os.path.realpath(__file__))
        return os.path.join(base_path, relative_path)


def main():
    args = parse_Args()
    image_dir = args.image_dir
    location = args.location
    pid = args.pid
    # location != None is usb_mult_dl
    if(location is None):
        try:
            me = SingleInstance()
        except Exception:
            print("%d" % me)
            exit(-1)
    fip_path = os.path.join(image_dir, "fip.bin")
    logging.info("fip_path: %s" % fip_path)

    cv_dl_magic = open(resource_path('cv_dl_magic.bin'), 'rb')
    cv_dl_magic_size = os.path.getsize(resource_path('cv_dl_magic.bin'))
    if(location is None):
        logging.info("CV1822 USB download start...")
    else:
        # For UI Tool
        print("CV1822 USB download start " + "LOCATION=" + location + "\r\n")

    cv_usb_serial = cv_usb_pyserial()

    logging.info("Connecting to ROM 1st stage...")
    if(location is None):
        cv_usb_serial.serial_query([pkt.rom_vidpid])
    else:
        cv_usb_serial.serial_query([pkt.rom_cvi_vidpid], 0, 0, location)
    logging.info("done")

    logging.info("Send cv_dl_magic.bin...")
    cv_usb_serial.usb_send_chunk(cv_dl_magic,
                                 cv_dl_magic_size,
                                 pkt.DUMMY_ADDR,
                                 0,
                                 Type="magic")
    logging.info("done")

    logging.info("Send 64KB fip.bin...")
    fip_bin = open(fip_path, 'rb')
    cv_usb_serial.usb_send_chunk(fip_bin,
                                 64 * 1024,
                                 pkt.TPU_SRAM_FIP_ADDR,
                                 0,
                                 Type="file")
    logging.info("done")

    flag = array('B')
    flag = array('B', [ord(c) for c in "3NGM"])
    cv_usb_serial.usb_send_req_data(pkt.CVI_USB_TX_DATA_TO_SRAM, 0x0E000004,
                                    12, flag)
    logging.info("set flag")

    cv_usb_serial.usb_send_req_data(pkt.CV_USB_BREAK, pkt.DUMMY_ADDR, 0, None)
    logging.info("break")

    del cv_usb_serial
    cv_usb_serial = cv_usb_pyserial()

    # Add delay to avoid open the previous disconnected com port.
    time.sleep(0.1)
    logging.info("Connecting to ROM 2nd stage...")
    if(location is None):
        cv_usb_serial.serial_query([pkt.rom_vidpid])
    else:
        cv_usb_serial.serial_query([pkt.rom_cvi_vidpid], 0, 0, location)
    logging.info("done")

    cv_dl_magic = open(resource_path('cv_dl_magic.bin'), 'rb')
    cv_dl_magic_size = os.path.getsize(resource_path('cv_dl_magic.bin'))
    logging.info("Send cv_dl_magic.bin...")
    cv_usb_serial.usb_send_chunk(cv_dl_magic,
                                 cv_dl_magic_size,
                                 pkt.DUMMY_ADDR,
                                 0,
                                 Type="magic")
    logging.info("done")

    logging.info("Send fip.bin...")
    cv_usb_serial.usb_send_2nd_fip(fip_path, pkt.DDR_FIP_ADDR, 0)
    logging.info("done")

    flag = array('B')
    flag = array('B', [int(c) for c in pid])
    cv_usb_serial.usb_send_req_data(pkt.CVI_USB_TX_DATA_TO_SRAM, 0x0C021FFC,
                                    12, flag)
    logging.info("set uboot pid")

    cv_usb_serial.usb_send_req_data(pkt.CV_USB_BREAK, pkt.DUMMY_ADDR, 0, None)
    logging.info("break")


if __name__ == '__main__':
    main()
