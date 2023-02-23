#!/usr/bin/python3
from cv_usb_util.cv_usb import cv_usb
from cv_usb_util.cv_usb_pyserial import cv_usb_pyserial
import argparse
import logging
import os
import sys
import time
import cv_usb_util.cv_usb_pkt as pkt
from array import array
FORMAT = '%(levelname)s: %(message)s'
logging.basicConfig(level=logging.INFO, format=FORMAT)
parser = argparse.ArgumentParser(description='Create CVITEK device image')

header_size = 64
max_chunk_size = 100 * 1024 * 1024


def parse_Args():
    parser.add_argument(
        "--cmd",
        metavar="connect | efusew | efuser | read_sn",
        type=str,
        help="")

    parser.add_argument(
        "--area",
        metavar="USER | DEVICE_ID | HASH0_PUBLIC | LOADER_EK | DEVICE_EK | \
                 LOCK_WRITE_HASH0_PUBLIC | LOCK_WRITE_LOADER_EK | LOCK_WRITE_DEVICE_EK",
        type=str,
        help="")

    parser.add_argument(
        "--data",
        metavar="USER_40B | DEVICE_ID_8B | HASH0_PUBLIC_32B | LOADER_EK_16B | DEVICE_EK_16B",
        type=str,
        help="")

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


def reboot_device(usb, wait_for_reboot):
    usb.restart()
    logging.info("Connecting to u-boot:")
    usb.query([pkt.uboot_vidpid, pkt.uboot_cvi_vidpid])
    time.sleep(0.02)

    usb.send_req_data(pkt.CVI_USB_REBOOT, 0x04003000, 0, None)

    if wait_for_reboot:
        usb.restart()
        usb.query([pkt.rom_vidpid, pkt.rom_cvi_vidpid])
        usb.send_req_data(pkt.CV_USB_BREAK, 0x04003000, 0, None)

    logging.info("reboot device done")


def convert(byte):
    if sys.version_info[0] < 3:
        return ord(byte)
    else:
        return byte


def connect_to_uboot():
    if not os.path.isfile("fip.bin"):
        logging.error("Failed to find fip.bin")
        sys.exit(pkt.FIP_NOT_FOUND)

    if not os.path.isfile("cv_dl_magic.bin"):
        logging.error("Failed to find cv_dl_magic.bin")
        sys.exit(pkt.MGC_NOT_FOUND)
    else:
        cv_dl_magic = open(resource_path('cv_dl_magic.bin'), 'rb')
        cv_dl_magic_size = os.path.getsize(resource_path('cv_dl_magic.bin'))

    cv_dl_magic = open(resource_path('cv_dl_magic.bin'), 'rb')
    cv_dl_magic_size = os.path.getsize(resource_path('cv_dl_magic.bin'))
    logging.info("CV1822 USB download start...")

    cv_usb_serial = cv_usb_pyserial()

    logging.info("Connecting to ROM 1st stage...")
    cv_usb_serial.serial_query([pkt.rom_vidpid])
    logging.info("done")

    logging.info("Send cv_dl_magic.bin...")
    cv_usb_serial.usb_send_chunk(cv_dl_magic,
                                 cv_dl_magic_size,
                                 pkt.DUMMY_ADDR,
                                 0,
                                 Type="magic")
    logging.info("done")

    logging.info("Send 64KB fip.bin...")
    fip_bin = open("fip.bin", 'rb')
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
    cv_usb_serial.serial_query([pkt.rom_vidpid])
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
    cv_usb_serial.usb_send_2nd_fip("fip.bin", pkt.DDR_FIP_ADDR, 0)
    logging.info("done")

    cv_usb_serial.usb_send_req_data(pkt.CV_USB_BREAK, pkt.DUMMY_ADDR, 0, None)
    logging.info("break")


def main():
    args = parse_Args()

    if args.cmd == "connect":
        connect_to_uboot()
    else:
        driver = "pyserial"
        logging.info("Using %s" % driver)
        usb = cv_usb(driver)
        usb.restart()
        logging.info("Connecting to u-boot...")
        usb.query([pkt.uboot_vidpid, pkt.uboot_cvi_vidpid])
        time.sleep(0.02)

    if args.cmd == "read_sn":
        cmd = array('B', [ord(c) for c in ""])
        sn = array('B', [ord(c) for c in ""])
        logging.info("DEVICE SN: ")
        sn = usb.send_req_data_rsp(pkt.CV_USB_READ_SN, 0, len(cmd) + 8, cmd)
        for i in range(8, 16, 1):
            sys.stdout.write("%x" % convert(sn[i]))
        sys.stdout.write("\n")
        sys.exit(pkt.SUCCESS)

    if args.cmd == "efusew":
        if args.area is None:
            logging.error("please specify --area")
            sys.exit(pkt.PARAM_ERROR)

        cmd = array('B', [ord(c) for c in ""])
        for c in [ord(ch) for ch in args.cmd + ' ']:
            cmd.append(c)
        for c in [ord(ch) for ch in args.area + ' ']:
            cmd.append(c)
        if args.data is not None:
            for c in [ord(ch) for ch in args.data]:
                cmd.append(c)
        logging.info("Program efuse: area %s; data %s ... " % (args.area,
                                                               args.data))
        result = usb.send_req_data_rsp(pkt.CV_USB_EFUSEW, 0, len(cmd) + 8, cmd)

        if convert(result[8]) == 1:
            sys.stdout.write("RESULT: PASS\n")
            sys.exit(pkt.SUCCESS)
        else:
            sys.stdout.write("RESULT: FAIL\n")
            sys.exit(pkt.FAIL)

    if args.cmd == "efuser":
        if args.area is None:
            logging.error("please specify --area")
            sys.exit(pkt.PARAM_ERROR)

        datalen = 0
        cmd = array('B', [ord(c) for c in ""])
        for c in [ord(ch) for ch in args.cmd + ' ']:
            cmd.append(c)
        for c in [ord(ch) for ch in args.area + ' ']:
            cmd.append(c)
        logging.info("Read efuse: area %s... " % (args.area))
        result = usb.send_req_data_rsp(pkt.CV_USB_EFUSER, 0, len(cmd) + 8, cmd)

        if args.area == "USER":
            datalen = 40
        elif args.area == "DEVICE_ID":
            datalen = 8
        elif args.area == "HASH0_PUBLIC":
            datalen = 32
        elif args.area == "LOADER_EK":
            datalen = 16
        elif args.area == "DEVICE_EK":
            datalen = 16
        else:
            datalen = 1

        for i in range(8, 8 + datalen, 1):
            sys.stdout.write("%02x" % convert(result[i]))
        sys.stdout.write("\n")
        sys.exit(pkt.SUCCESS)


if __name__ == '__main__':
    main()
