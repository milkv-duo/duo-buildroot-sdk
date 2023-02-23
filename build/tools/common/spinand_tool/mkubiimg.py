#!/usr/bin/python
# -*- coding: utf-8 -*-
import argparse
import logging
from os import path, getcwd
from XmlParser import XmlParser
from tempfile import mkdtemp
import sys
import subprocess

FORMAT = "%(levelname)s: %(message)s"
logging.basicConfig(level=logging.INFO, format=FORMAT)


def resource_path(relative_path):
    """ Get absolute path to resource, works for dev and for PyInstaller   """
    try:
        # PyInstaller creates a temp folder and stores path in _MEIPASS
        base_path = sys._MEIPASS
    except Exception:
        base_path = path.dirname(path.realpath(__file__))
    return path.join(base_path, relative_path)


def parse_Args():
    parser = argparse.ArgumentParser(description="Create UBI image")
    parser.add_argument("xml", help="path to partition xml")
    parser.add_argument("label", help="label of the partition")
    parser.add_argument(
        "input_path", metavar="input", type=str, help="input file or folder for packing"
    )
    parser.add_argument(
        "output_path",
        metavar="output_file_path",
        type=str,
        help="the folder path to install dir inclued fip,rootfs and kernel",
    )

    parser.add_argument(
        "-v", "--verbose", help="increase output verbosity", action="store_true"
    )
    parser.add_argument(
        "-p",
        "--pagesize",
        help="page size of nand, default is 2Kib",
        type=str,
        default="2K",
    )
    parser.add_argument(
        "-b",
        "--blocksize",
        help="block size of nand, default is 128Kib",
        type=str,
        default="128K",
    )

    parser.add_argument("--ubinize", help="path to ubinize", type=str)
    parser.add_argument("--mkfs", help="path to mkfs.ubifs", type=str)
    parser.add_argument(
        "--ubionly",
        help="create ubi image only",
        action="store_true",
    )
    parser.add_argument(
        "--maxsize",
        help="Set max size for the partition"
        "(For the partition without size assigned) ",
        type=int,
    )

    parser.add_argument(
        "--reserved",
        help="Set reserved blocks percentage for the partition,"
        "For example: --reserved 10 means 10%% reserved blocks."
        "(For the partition without size assigned) ",
        type=int,
    )
    args = parser.parse_args()
    if args.verbose:
        logging.debug("Enable more verbose output")
        logging.getLogger().setLevel(level=logging.DEBUG)

    if args.ubinize is None:
        args.ubinize = resource_path("ubinize")

    if args.mkfs is None:
        args.mkfs = resource_path("mkfs.ubifs")
    return args


def log_subprocess_output(pipe):
    for line in iter(pipe.readline, b""):  # b'\n'-separated lines
        logging.debug("got line from subprocess: %r", line)


def create_ubicfg(img_path, part_size, label, output, ubionly=False):
    with open(output, "w") as f:
        f.write("[ubifs]\n")
        f.write("mode=ubi\n")
        f.write("image=%s\n" % img_path)
        f.write("vol_id=0\n")
        if not ubionly:
            f.write("vol_size=%d\n" % part_size)
        f.write("vol_type=dynamic\n")
        f.write("vol_name=%s\n" % label)
        f.write("vol_flags=autoresize\n")

        # Debug message
        logging.debug("[ubifs]")
        logging.debug("[mode]=ubi")
        logging.debug("image=%s" % img_path)
        logging.debug("vol_id=0")
        logging.debug("vol_size=%d" % part_size)
        logging.debug("vol_type=dynamic")
        logging.debug("vol_name=%s" % label)
        logging.debug("vol_flags=autoresize")


def create_ubifs(
    input_dir, output_dir, internal_part_size, pagesize, pebsize, mkfs, verbose=False
):

    lebsize = pebsize - 2 * pagesize
    part_lebcnt = internal_part_size / lebsize
    ubifs_args = "-r %s -m %d -e %d -c %d -F" % (
        input_dir,
        pagesize,
        lebsize,
        part_lebcnt,
    )
    logging.debug("ubifs args %s", ubifs_args)
    tmp_ubifs = path.join(output_dir, "tmp.ubifs")
    mkfs_cmd = "%s %s -o %s" % (mkfs, ubifs_args, tmp_ubifs)
    logging.debug("mkfs_cmd:%s", mkfs_cmd)
    try:
        process = subprocess.Popen(
            mkfs_cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            cwd=getcwd(),
            shell=True,
        )
    except Exception:
        return -1
    if verbose:
        with process.stdout:
            log_subprocess_output(process.stdout)
    ret = process.wait()
    return [ret, tmp_ubifs]


def create_ubi(img_path, cfg_path, output, pagesize, pebsize, ubinize, verbose=False):
    ubi_args = "-p %d -m %d" % (pebsize, pagesize)
    logging.debug("ubi args %s", ubi_args)
    ubinize_cmd = "%s -o %s %s %s" % (ubinize, output, ubi_args, cfg_path)
    logging.debug("ubinize_cmd:%s", ubinize_cmd)
    try:
        process = subprocess.Popen(
            ubinize_cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            cwd=getcwd(),
            shell=True,
        )
    except Exception:
        return -1
    if verbose:
        with process.stdout:
            log_subprocess_output(process.stdout)
    ret = process.wait()
    return ret


def main():
    args = parse_Args()
    parser = XmlParser(args.xml)
    parts = parser.parse()
    for p in parts:
        # Since xml parser will parse with abspath and the user input path can
        # be relative path, use file name to check.
        if args.label == p["label"]:
            part = p

            break
    try:
        part_size = part["part_size"]
        label = part["label"]
    except Exception:
        logging.error("label is not found in partition.xml, please check!")
        return -1
    logging.debug("get partition as below:")
    logging.debug(p)
    pagesize = XmlParser.parse_size(args.pagesize)
    pebsize = XmlParser.parse_size(args.blocksize)
    lebsize = pebsize - 2 * pagesize

    if part_size == sys.maxsize:
        if args.maxsize:
            part_size = args.maxsize
        else:
            logging.error("please use --maxsize size to assign size")
            return 1
    # 1.
    #    2 PEBs are used to store the volume table;
    #    1 PEB is reserved for wear-leveling purposes;
    #    1 PEB is reserved for the atomic LEB change operation;
    # 2.
    #    some amount of PEBs are reserved for bad PEB handling;
    #    this is applicable for NAND flash but not for NOR flash
    #    the amount of reserved PEBs is configurable and is equal to
    #    20 blocks per 1024 blocks by default.
    # 3.
    #    UBI stores the EC and VID headers at the beginning of each PEB;
    #    the number of bytes used for these purposes depends on
    #    the flash type and is explained below.
    # 4.
    #    According to above design, set at lease 5 blocks reserved
    #    Set extra reserved for bad blocks, at least 1 block or 1% of partition
    #    size for r/w partition
    # 5.
    #    SP: PEB
    #    SL: LEB
    #    P: Total number of PEBs on the MTD device
    #    O: The overhead related to storing EC and VID headers in bytes, i.e. O = SP - SL
    #    B: Number of bad block handling
    #    UBI Overhead = (B + 4) * SP + O * (P - B - 4)
    SP = pebsize
    SL = lebsize
    P = int(part_size / pebsize)
    Ox = SP - SL
    try:
        B = max(1, int((args.reserved / 100) * P), int(0.01 * P))
    except Exception:
        B = max(int(0.01 * P), 1)
    ubi_overhead = (B + 4) * SP + Ox * (P - B - 4)
    lebcnt = int((part_size - ubi_overhead) / pebsize)

    internal_part_size = lebcnt * lebsize
    tmpdir = mkdtemp()
    if not args.ubionly:
        logging.info("Creating ubifs")
        ret, fs_path = create_ubifs(
            args.input_path,
            tmpdir,
            internal_part_size,
            pagesize,
            pebsize,
            args.mkfs,
            args.verbose,
        )
        if ret:
            logging.error("create ubifs error, please enable verbose!")
            return -1
    else:
        fs_path = args.input_path
    tmp_cfg_path = path.join(tmpdir, "tmp.cfg")
    create_ubicfg(
        fs_path,
        internal_part_size,
        label,
        tmp_cfg_path,
        (args.ubionly or part_size == sys.maxsize),
    )

    ret = create_ubi(
        fs_path,
        tmp_cfg_path,
        args.output_path,
        pagesize,
        pebsize,
        args.ubinize,
        args.verbose,
    )
    if ret:
        logging.error("create ubi image error, please enable verbose!")
        return -1
    return 0


if __name__ == "__main__":
    main()
