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
FORMAT = '%(levelname)s: %(message)s'
logging.basicConfig(level=logging.INFO, format=FORMAT)
parser = argparse.ArgumentParser(description='Create CVITEK device image')

header_size = 64
max_chunk_size = 16 * 1024 * 1024
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
        "--zipfile",
        metavar="path to upgrade.zip",
        type=str,
        help="the path of upgrade.zip")
    parser.add_argument(
        '--location',
        metavar="",
        type=str)
    parser.add_argument(
        '--pid',
        metavar='1001',
        default='1001',
        type=str)
    parser.add_argument(
        "-v",
        "--verbose",
        help="increase output verbosity",
        action="store_true")
    parser.add_argument("--mac",
                        metavar="mac address",
                        type=str,
                        help="set mac address")
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--serial', action='store_true', default=False)
    group.add_argument('--libusb', action='store_true', default=False)

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
    chunk_header.fromfile(fd, int(header_size / 4))
    chunk_header[2] = offset
    # Modify header according to the partition.xml
    fd.seek(last_pos)
    fd.write(chunk_header)
    fd.seek(last_pos)


def set_macaddress(usb, libusb, mac, timeout):
    global uboot_vidpid
    global uboot_cvi_vidpid

    if not libusb:
        usb.restart()
        usb.query([uboot_vidpid, uboot_cvi_vidpid], timeout)
        time.sleep(0.2)

    # Send setenv
    cmd = array('B', [ord(c) for c in "setenv ethaddr "])
    for c in [ord(ch) for ch in mac]:
        cmd.append(c)
    usb.send_req_data(pkt.CV_USB_PRG_CMD, 0, len(cmd) + 8, cmd, 1)

    # Send savenv
    cmd = array('B', [ord(c) for c in "saveenv"])
    usb.send_req_data(pkt.CV_USB_PRG_CMD, 0, len(cmd) + 8, cmd, 1)
    # Break command
    # usb.send_req_data(pkt.CV_USB_UBREAK, 0x04003000, 0, None)


def set_filesize(usb, libusb, filesize, timeout):
    global uboot_vidpid
    global uboot_cvi_vidpid

    if not libusb:
        usb.restart()
        usb.query([uboot_vidpid, uboot_cvi_vidpid], timeout)
        time.sleep(0.2)

    # Send setenv
    cmd = array('B', [ord(c) for c in "setenv filesize "])
    for c in [ord(ch) for ch in hex(filesize)]:
        cmd.append(c)

    usb.send_req_data(pkt.CV_USB_PRG_CMD, 0, len(cmd) + 8, cmd, 1)


def reboot_device(usb, libusb, timeout):
    global uboot_vidpid
    global uboot_cvi_vidpid

    usb.send_req_data(pkt.CV_USB_UBREAK, 0x04003000, 0, None)
    usb.query([uboot_vidpid, uboot_cvi_vidpid], timeout)

    usb.send_req_data(pkt.CVI_USB_REBOOT, 0x04003000, 0, None)
    logging.info("reboot device done")


def main():
    global uboot_vidpid
    global uboot_cvi_vidpid
    args = parse_Args()
    image_dir = args.image_dir
    location = args.location
    pid = args.pid
    # location != None is usb_mult_dl
    xml = glob(os.path.join(image_dir, "partition*.xml"))
    if len(xml) != 1:
        logging.error("Cannot get correct partition xml in %s", image_dir)
        usage()
        return

    xmlParser = XmlParser(xml[0])
    parts = xmlParser.parse(image_dir)

    if (not args.serial and not args.libusb) or args.serial:
        driver = "pyserial"
        timeout = 90
    else:
        driver = "libusb"
        timeout = 30
    logging.info("Using %s" % driver)
    usb = cv_usb(driver)
    files = []
    prog_parts = []
    if xmlParser.getStorage() == "emmc":
        fip_path = os.path.join(image_dir, "fip.bin")
        files.append(fip_path)
        prog_parts.append({})  # Add empty dict for fip

    uboot_vidpid = "VID:PID=30B1:" + pid
    uboot_cvi_vidpid = "VID:PID=3346:" + pid
    usb.query([uboot_vidpid, uboot_cvi_vidpid], timeout)

    # get image addr
    recvbuf = usb.recive_data(0, 8)
    hex_arr = [hex(x)[2:].rjust(2, '0') for x in recvbuf]
    hex_arr.reverse()
    hex_join = "0x" + "".join(hex_arr)
    pkt.IMG_ADDR = int(hex_join, 16)

    set_filesize(usb, driver == "libusb", os.path.getsize(os.path.join(image_dir, "fip.bin")), timeout)

    for p in parts:
        if p['file_size'] != 0:
            files.append(p['file_path'])
            prog_parts.append(p)

    for i, f in enumerate(files):
        if(location is not None):
            # For UI Tool
            print("SEND FILE %s " % files[i] + "LOCATION=" + location + "\r\n")
        else:
            logging.info("SEND FILE %s " % files[i] + "\r\n")

        if i == 0:
            usb.send_file(f, pkt.IMG_ADDR, 0)
            usb.send_req_data(pkt.CV_USB_UBREAK, 0x04003000, 0, None)
            usb.query([uboot_vidpid, uboot_cvi_vidpid], timeout)
            time.sleep(0.02)
        else:
            if (driver == "pyserial"):
                usb.restart()
                logging.info("Connecting to u-boot... ")
                usb.query([uboot_vidpid, uboot_cvi_vidpid], timeout)
                time.sleep(0.02)
            fd = open(f, 'r+b')
            usb.send_chunk(fd, header_size, pkt.IMG_ADDR, 0)
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
                    usb.query([uboot_vidpid, uboot_cvi_vidpid], timeout)
                    time.sleep(0.02)
                send_size = min(remain_file_size, max_chunk_size + chunk_header_sz)
                # location != None is usb_mult_dl
                # if(location is None):
                #    changeOffset(fd, offset)
                usb.send_chunk(
                    fd,
                    send_size,
                    pkt.IMG_ADDR,
                    0,
                )
                logging.info("CVI_USB_PROGRAM")
                usb.send_req_data(pkt.CVI_USB_PROGRAM, 0x04003000, 0, None, 1)
                remain_file_size -= send_size
                offset += max_chunk_size
                if file_size == fd.tell():
                    break
            fd.close()

    if args.mac:
        set_macaddress(usb, driver == "libusb", args.mac, timeout)

    reboot_device(usb, driver, timeout)
    logging.info("USB download complete")


if __name__ == '__main__':
    main()
