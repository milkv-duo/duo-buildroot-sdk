#!/usr/bin/python3
# -*- coding: utf-8 -*-
import logging
import argparse
import sys
from os import path, stat
from array import array
from XmlParser import XmlParser


FORMAT = "%(levelname)s: %(message)s"
logging.basicConfig(level=logging.INFO, format=FORMAT)
LBA_SIZE = 512
MAX_WRITE_SIZE = 50 * 1024 * 1024
DEFAULT_BLOCK_SIZE = 128 * 1024
FIP_BACKUP_BLOCK_POS = 10
SV_BLOCK_NUM = 4
BLOCK_SIZE_FOR_4K_NAND = 262144

global chip_list
chip_list = ["cv183x", "cv182x", "cv181x"]


def parse_Args():
    parser = argparse.ArgumentParser(description="Create CVITEK device image for burning")
    parser.add_argument("chip", help="the current chip for using")
    parser.add_argument("xml", help="path to partition xml")
    parser.add_argument("images_path", help="path to images")
    parser.add_argument(
        "output",
        metavar="output",
        type=str,
        help="the output folder for saving the data.bin and boot.bin",
    )
    parser.add_argument(
        "-m",
        "--max_write_size",
        type=int,
        help="max write buffer size when generating file. "
        "Increasing the size when speedup the procedue "
        "but it will use more system memory. "
        "Default is 50MB.",
        default=MAX_WRITE_SIZE,
    )
    parser.add_argument(
        "-b",
        "--block_size",
        type=int,
        help="block size only for nand, defaule is 128K",
        default=DEFAULT_BLOCK_SIZE,
    )
    parser.add_argument(
        "-v", "--verbose", help="increase output verbosity", action="store_true"
    )
    args = parser.parse_args()
    if args.verbose:
        logging.debug("Enable more verbose output")
        logging.getLogger().setLevel(level=logging.DEBUG)

    return args


def handle_fip_for_4k_page(images_path, blocksize: int):

    logging.info("handle fip for 4K page....")
    fill_array = array("B", [0xFF for _ in range(2048)])
    fip_path = path.join(images_path, "fip.bin")
    new_fip_path = path.join(images_path, "fip_4k.bin")
    if path.exists(fip_path):
        with open(fip_path, "rb") as fip:
            with open(new_fip_path, "wb") as new_fip:
                off = 0
                while True:
                    data = fip.read(2048)
                    if data == b"":
                        break

                    new_fip.seek(off, 0)
                    new_fip.write(data)
                    fill_array.tofile(new_fip)
                    new_fip.flush()
                    off = off + 4096

        new_fip_size = stat(new_fip_path).st_size
        print("new fip size is %d " % new_fip_size)
        append_array = array("B", [0xFF for _ in range(blocksize * 5 - new_fip_size)])
        with open(new_fip_path, "ab") as f:
            append_array.tofile(f)


def raw_image_check(f, filename):

    if filename == "fip.bin":
        return
        # CIMG
    head = f.read(4)
    if head == b"CIMG":
        logging.info("%s is not raw image, please use raw image" % filename)
        sys.exit(-1)


def genDataBin(
    out,
    parts,
    images_path,
    storage_type,
    max_write_size=50 * 1024 * 1024,
    block_size=128 * 1024,
    chip="cv182x",
):

    if chip not in chip_list:
        logging.info("do not support %s" % chip)
        logging.info("only support ")
        logging.info(chip)
        sys.exit(-1)

    sv_array = array("B")
    if storage_type == "spinand" and chip != "cv181x":
        sv_path = path.join(images_path, "sv.bin")
        if path.exists(sv_path):
            logging.info("sv.bin is exist!")
            sv_size = stat(sv_path).st_size
            with open(sv_path, "rb") as f:
                sv_array.fromfile(f, sv_size)
            out.seek(0, 0)
            sv_array.tofile(out)
        else:
            logging.info("there is no sv.bin, please add it")

    for i, p in enumerate(parts):
        file_array = array("B")
        skip_size = 0
        # we change offset of fip for nand
        if p["file_name"] == "fip.bin" and storage_type == "spinand" and chip != "cv181x":
            p["offset"] = block_size * SV_BLOCK_NUM
            skip_size = block_size * SV_BLOCK_NUM
            # handle fip for 4K page
            if block_size == BLOCK_SIZE_FOR_4K_NAND:
                handle_fip_for_4k_page(images_path, block_size)
                # use fip_4k.bin to pack
                p["file_name"] = "fip_4k.bin"

        if p["file_name"] == "":
            continue

        logging.debug("file name is %s" % p["file_name"])
        file_path = path.join(images_path, p["file_name"])

        if not path.exists(file_path):
            continue

        logging.debug("%s is exits" % file_path)
        file_size = stat(file_path).st_size

        logging.debug("%s size is %d" % (p["file_name"], file_size))
        logging.debug("Packing %s" % p["label"])
        if path.exists(file_path):
            with open(file_path, "rb") as f:
                raw_image_check(f, p["file_name"])
                f.seek(0)
                file_array.fromfile(f, file_size)
            out.seek(p["offset"], 0)
            logging.info("Writing %s to pos %d" % (p["label"], p["offset"]))
            file_array.tofile(out)
            # for fip.bin of spi nand, we do a backup at 9th block
            if i == 0 and storage_type == "spinand":
                out.seek(block_size * FIP_BACKUP_BLOCK_POS, 0)
                file_array.tofile(out)
                logging.info(
                    "do a backup for fip.bin at %d"
                    % (block_size * FIP_BACKUP_BLOCK_POS)
                )

        # Only append 0xff when the partition is not the last partition.
        if i != len(parts) - 1 and p["file_name"] != "fip.bin":
            append_size = p["part_size"] - file_size - skip_size
            # This part may seems stupid, but it works when image is too large
            # to keep content in memory.
            for j in range(0, append_size, max_write_size):
                append_byte = array(
                    "B", [0xFF for _ in range(min(max_write_size, append_size - j))]
                )
                append_byte.tofile(out)
    logging.info("generating Data.bin done!")


def genBootBin(out, images_path):
    file_path = path.join(images_path, "fip.bin")
    try:
        file_array = array("B")
        fip_size = stat(file_path).st_size
        with open(file_path, "rb") as f:
            file_array.fromfile(f, fip_size)
            for _ in range(0x800 * LBA_SIZE - stat(file_path).st_size):
                file_array.append(0xFF)
        file_array.tofile(out)
        # write twice to backup fip
        file_array.tofile(out)
        logging.info("generating Boot.bin done!")
        out.close()
    except FileNotFoundError as e:
        logging.error("fip.bin is not exist")
        raise e


def main():
    args = parse_Args()
    xmlParser = XmlParser(args.xml)
    parts = xmlParser.parse(args.images_path)
    out_path = path.join(args.output, "Data.bin")

    storage_type = xmlParser.getStorage()
    logging.info("storage type is %s " % storage_type)

    with open(out_path, "wb") as out:
        genDataBin(
            out,
            parts,
            args.images_path,
            storage_type,
            args.max_write_size,
            args.block_size,
            args.chip,
        )

    # append fip to 1M for emmc
    if storage_type == "emmc":
        fip_out_path = path.join(args.output, "Boot.bin")
        try:
            fd = open(fip_out_path, "wb")
        except Exception:
            logging.error("Create %s failed!", out_path)
            raise
        genBootBin(fd, args.images_path)


if __name__ == "__main__":
    main()
