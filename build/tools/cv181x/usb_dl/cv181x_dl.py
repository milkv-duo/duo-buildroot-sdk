#!/usr/bin/python3
import argparse
import logging
import os

FORMAT = "%(levelname)s: %(message)s"
logging.basicConfig(level=logging.INFO, format=FORMAT)
parser = argparse.ArgumentParser(description="Create CVITEK device image")


def parse_Args():
    cur_dir = os.path.abspath(os.getcwd())
    parser.add_argument(
        "--image_dir",
        metavar="path",
        type=str,
        default=cur_dir,
        help="the folder path to dir inclued fip,rootfs kernel and xml",
    )
    parser.add_argument(
        "-v", "--verbose", help="increase output verbosity", action="store_true"
    )
    parser.add_argument("--mac",
                        metavar="mac address",
                        type=str,
                        help="set mac address")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--serial", action="store_true", default=False)
    group.add_argument("--libusb", action="store_true", default=False)

    args = parser.parse_args()

    return args


def main():
    args = parse_Args()
    image_dir = args.image_dir
    mac = args.mac

    if (not args.serial and not args.libusb) or args.serial:
        driver = "serial"
    else:
        driver = "libusb"
    logging.info("Using %s" % driver)

    logging.info("CV181X USB download start\n")

    cmd = (
        "python rom_usb_dl/cv181x_rom_usb_download.py --image_dir "
        + image_dir
    )
    os.system(cmd)

    cmd = (
        "python rom_usb_dl/cv181x_uboot_usb_download.py --image_dir "
        + image_dir
        + " --"
        + driver
    )
    if (mac):
        cmd = cmd + " --mac " + mac
    os.system(cmd)

    logging.info("CV181X USB download end\n")


if __name__ == "__main__":
    main()
