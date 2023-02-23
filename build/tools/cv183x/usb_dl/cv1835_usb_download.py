#!/usr/bin/python3
from cv_usb_util.cv_usb import cv_usb
import argparse
import logging
import os
import sys
import time
import cv_usb_util.cv_usb_pkt as pkt
from glob import glob
from array import array
from XmlParser import XmlParser
from zipfile import ZipFile
from tempfile import mkdtemp
from singleton import SingleInstance
FORMAT = '%(levelname)s: %(message)s'
logging.basicConfig(level=logging.INFO, format=FORMAT)
parser = argparse.ArgumentParser(description='Create CVITEK device image')

header_size = 64
max_chunk_size = 100 * 1024 * 1024
uboot_vidpid = ""
uboot_cvi_vidpid = ""


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
    parser.add_argument("--zipfile",
                        metavar="path to upgrade.zip",
                        type=str,
                        help="the path of upgrade.zip")
    parser.add_argument("-v",
                        "--verbose",
                        help="increase output verbosity",
                        action="store_true")
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--serial', action='store_true', default=False)
    group.add_argument('--libusb', action='store_true', default=False)

    parser.add_argument("--mac",
                        metavar="mac address",
                        type=str,
                        help="set mac address")

    args = parser.parse_args()
    if args.zipfile:
        args.image_dir = mkdtemp()
        with ZipFile(args.zipfile, 'r') as zipObj:
            zipObj.extractall(args.image_dir)
    if args.verbose:
        logging.debug("Enable more verbose output")
        logging.getLogger().setLevel(level=logging.DEBUG)

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


def changeOffset(fd, offset):
    last_pos = fd.tell()
    """
    Since we cannot send cmd to device due to security issue. We can
    only modify the header information for changing the offset we
    want to program.
    """
    # Get Chunk header
    chunk_header = array('I')
    chunk_header.fromfile(fd, 64)
    chunk_header[2] = offset
    # Modify header according to the partition.xml
    fd.seek(last_pos)
    fd.write(chunk_header)
    fd.seek(last_pos)


def set_macaddress(usb, libusb, mac):
    global uboot_vidpid
    global uboot_cvi_vidpid

    if not libusb:
        usb.restart()
        logging.info("set macaddress... ")
        usb.query([uboot_vidpid, uboot_cvi_vidpid])
        time.sleep(0.2)

    # Send setenv
    cmd = array('B', [ord(c) for c in "setenv ethaddr "])
    for c in [ord(ch) for ch in mac]:
        cmd.append(c)
    usb.send_req_data(pkt.CV_USB_PRG_CMD, 0, len(cmd) + 8, cmd)

    # Send savenv
    cmd = array('B', [ord(c) for c in "saveenv"])
    usb.send_req_data(pkt.CV_USB_PRG_CMD, 0, len(cmd) + 8, cmd)
    # Break command
    # usb.send_req_data(pkt.CV_USB_BREAK, 0x04003000, 0, None)


def reboot_device(usb, libusb):
    global uboot_vidpid
    global uboot_cvi_vidpid

    # if not libusb:
    #     usb.restart()
    #     usb.query([uboot_vidpid,uboot_cvi_vidpid])
    #     time.sleep(0.2)

    usb.send_req_data(pkt.CVI_USB_REBOOT, 0x04003000, 0, None)

    logging.info("reboot device done")


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
    xml = glob(os.path.join(image_dir, "partition*.xml"))
    if len(xml) != 1:
        logging.error("Cannot get correct partition xml in %s", image_dir)
        usage()
        return

    xmlParser = XmlParser(xml[0])
    parts = xmlParser.parse(image_dir)
    fip_path = os.path.join(image_dir, "fip.bin")
    if not os.path.isfile(fip_path):
        logging.error("Cannot find fip.bin in %s", image_dir)
    cv_dl_magic = open(resource_path('cv_dl_magic.bin'), 'rb')
    cv_dl_magic_size = os.path.getsize(resource_path('cv_dl_magic.bin'))
    logging.info("CV1835 USB download start... ")

    if (not args.serial and not args.libusb) or args.serial:
        driver = "pyserial"
    else:
        driver = "libusb"
    logging.info("Using %s" % driver)
    usb = cv_usb(driver)
    logging.info("Connecting to ROM code... ")
    usb.query([pkt.rom_vidpid, pkt.rom_cvi_vidpid])
    usb.send_chunk(cv_dl_magic, cv_dl_magic_size, 0x4003000, 0, Type="magic")
    logging.info("done")

    logging.info("Send fip.bin...")
    usb.send_file(fip_path, 0x0C040000, 0)
    logging.info("done")

    # Set SRAM flag
    logging.info("Send magic number for USB boot... ")
    flag = array('B', [ord(c) for c in "1NGM"])
    usb.send_req_data(pkt.CV_USB_NONE, 0x0E00FC00, 12, flag)
    logging.info("done")

    logging.info("Send magic number for USB download... ")
    flag = array('B')
    flag = array('B', [ord(c) for c in "3NGM"])
    usb.send_req_data(pkt.CV_USB_NONE, 0x0E00FC08, 12, flag)
    logging.info("done")

    flag = array('B')
    flag = array('B', [int(c) for c in pid])
    usb.send_req_data(pkt.CV_USB_NONE, 0x0E00FFFC, 12, flag)
    logging.info("set uboot pid")

    usb.send_req_data(pkt.CV_USB_BREAK, 0x04003000, 0, None)
    files = []
    prog_parts = []
    if xmlParser.getStorage() == "emmc":
        files.append(fip_path)
        prog_parts.append({})  # Add empty dict for fip

    uboot_vidpid = "VID:PID=30B1:" + pid
    uboot_cvi_vidpid = "VID:PID=3346:" + pid
    usb.query([uboot_vidpid, uboot_cvi_vidpid])

    for p in parts:
        if p['file_size'] != 0:
            files.append(p['file_path'])
            prog_parts.append(p)

    for i, f in enumerate(files):
        if(location is not None):
            # For UI Tool
            print("SEND FILE %s " % files[i] + "LOCATION=" + location + "\r\n")
        else:
            logging.info("Send %s   " % files[i])

        if i == 0:
            usb.send_file(f, 0x100090000, 0)
            usb.send_req_data(pkt.CV_USB_BREAK, 0x04003000, 0, None)
            usb.query([uboot_vidpid, uboot_cvi_vidpid])
            time.sleep(0.02)
        else:
            if (driver == "pyserial"):
                usb.restart()
                logging.info("Connecting to u-boot... ")
                usb.query([uboot_vidpid, uboot_cvi_vidpid])
            time.sleep(1)
            fd = open(f, 'r+b')
            usb.send_chunk(fd, header_size, 0x100080000, 0)
            fd.seek(0)
            header = array('I')
            header.fromfile(fd, int(header_size / 4))
            chunk_header_sz = header[2]
            cnt = header[3]
            file_size = header[4]
            remain_file_size = file_size
            offset = prog_parts[i]["offset"]
            for j in range(cnt):
                if (j > 0 and driver == "pyserial"):
                    usb.restart()
                    logging.info("Connecting to u-boot... ")
                    usb.query([pkt.uboot_vidpid, pkt.uboot_cvi_vidpid])
                    time.sleep(0.02)
                send_size = min(remain_file_size, max_chunk_size + chunk_header_sz)
                # location != None is usb_mult_dl
                if(location is None):
                    changeOffset(fd, offset)
                usb.send_chunk(
                    fd,
                    send_size,
                    0x100090000,
                    0,
                )
                logging.info("CVI_USB_PROGRAME")
                usb.send_req_data(pkt.CVI_USB_PROGRAME, 0x04003000, 0, None)
                remain_file_size -= send_size
                offset += max_chunk_size
                if file_size == fd.tell():
                    break
            fd.close()
    logging.info("Image download finished, waiting for reboot!")

    if args.mac:
        set_macaddress(usb, driver == "libusb", args.mac)
    reboot_device(usb, driver == "libusb")


if __name__ == '__main__':
    main()
