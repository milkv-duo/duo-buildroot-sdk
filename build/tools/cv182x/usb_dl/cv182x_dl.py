#!/usr/bin/python3
import argparse
import logging
import os

FORMAT = '%(levelname)s: %(message)s'
logging.basicConfig(level=logging.INFO, format=FORMAT)
parser = argparse.ArgumentParser(description='Create CVITEK device image')


def parse_Args():
    cur_dir = os.path.abspath(os.getcwd())
    parser.add_argument(
        '--image_dir',
        metavar='path',
        type=str,
        default=cur_dir,
        help='the folder path to dir inclued fip,rootfs kernel and xml')
    parser.add_argument("-v",
                        "--verbose",
                        help="increase output verbosity",
                        action="store_true")
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--serial', action='store_true', default=False)
    group.add_argument('--libusb', action='store_true', default=False)

    args = parser.parse_args()

    return args


def main():
    args = parse_Args()
    image_dir = args.image_dir

    if (not args.serial and not args.libusb) or args.serial:
        driver = "serial"
    else:
        driver = "libusb"
    logging.info("Using %s" % driver)

    logging.info("cv182x USB download start\n")
    cmd = "python rom_usb_dl/cv182x_rom_usb_download.py --image_dir " + image_dir
    result = os.system(cmd)

    if result:
        exit(-1)

    cmd = "python rom_usb_dl/cv182x_uboot_usb_download.py --image_dir " + image_dir + " --" + driver
    os.system(cmd)
    logging.info("cv182x USB download end\n")


if __name__ == '__main__':
    main()
