#!/usr/bin/python3
# -*- coding: utf-8 -*-
import logging
import argparse
import errno
from os import getcwd, path, makedirs

MAX_LOAD_SIZE = 100 * 1024 * 1024
CHUNK_TYPE_DONT_CARE = 0
CHUNK_TYPE_CRC_CHECK = 1
FORMAT = "%(levelname)s: %(message)s"
logging.basicConfig(level=logging.INFO, format=FORMAT)


def parse_Args():
    parser = argparse.ArgumentParser(description="Create CVITEK device image")

    parser.add_argument(
        "file_path",
        metavar="file_path",
        type=str,
        help="the file you want to pack with cvitek image header",
    )
    parser.add_argument(
        "--output_dir",
        metavar="output_folder_path",
        type=str,
        help="the folder path to save output, defuale will be ./rawimages",
        default=path.join(getcwd(), "rawimages"),
    )
    parser.add_argument(
        "-v", "--verbose", help="increase output verbosity", action="store_true"
    )
    args = parser.parse_args()
    if args.verbose:
        logging.debug("Enable more verbose output")
        logging.getLogger().setLevel(level=logging.DEBUG)

    return args


class ImagerRemover(object):
    @staticmethod
    def removeHeader(img, out):
        """
        Header format total 64 bytes
        4 Bytes: Magic
        4 Bytes: Version
        4 Bytes: Chunk header size
        4 Bytes: Total chunks
        4 Bytes: File size
        32 Bytes: Extra Flags
        12 Bytes: Reserved
        """
        with open(img, "rb") as fd:
            magic = fd.read(4)
            if magic != b"CIMG":
                logging.error("%s is not cvitek image!!" % img)
                raise TypeError

            with open(out, "wb") as fo:
                fd.seek(64)  # Skip Header
                # Skip chunk header
                while fd.read(64):
                    fo.write(fd.read(MAX_LOAD_SIZE))


def main():
    args = parse_Args()
    output_path = path.join(args.output_dir, path.basename(args.file_path))
    logging.debug("Input %s, Output %s\n" % (args.file_path, output_path))
    logging.debug("Creating folder for output\n")
    try:
        makedirs(args.output_dir)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise
    ImagerRemover.removeHeader(args.file_path, output_path)
    logging.info("Write %s Done" % output_path)


if __name__ == "__main__":
    main()
