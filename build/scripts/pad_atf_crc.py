#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK

import logging
import argparse
import binascii
from struct import pack

import build_helper

try:
    import argcomplete
except ImportError:
    argcomplete = None


build_helper.check_python_min_version()


def parse_args():
    parser = argparse.ArgumentParser(description="Pad ATF CRC")
    parser.add_argument("-v", "--verbose", default="INFO")
    parser.add_argument("--logfile", type=str)
    parser.add_argument("--pad-crc", type=str)

    if argcomplete:
        argcomplete.autocomplete(parser)

    return parser.parse_args()


def pad_crc(path):
    logging.info("Pad CRC16 to %s", path)
    with open(path, "rb") as fp:
        din = fp.read()
    c = binascii.crc_hqx(din, 0)
    logging.info("CRC16=0x%04x", c)

    dout = din + pack("<H", c) + b"\xFE\xCA"
    with open(path, "wb") as fp:
        fp.write(dout)


def main():
    args = parse_args()

    build_helper.init_logging(args.logfile, stdout_level=args.verbose)
    build_helper.dump_debug_info()

    if args.pad_crc:
        pad_crc(args.pad_crc)

    logging.info("END")


if __name__ == "__main__":
    main()
