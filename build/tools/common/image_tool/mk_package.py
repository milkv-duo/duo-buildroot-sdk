#!/usr/bin/python3
# -*- coding: utf-8 -*-
from zipfile import ZipFile, ZIP_DEFLATED
from XmlParser import XmlParser
from hashlib import md5
from os import path
from tempfile import NamedTemporaryFile
import logging
import argparse
from raw2cimg import ImagerBuilder

FORMAT = "%(levelname)s: %(message)s"
logging.basicConfig(level=logging.INFO, format=FORMAT)


def argparser():
    parser = argparse.ArgumentParser(description="Pack CVI upgrade package")
    parser.add_argument("xml", help="path to partition xml")
    parser.add_argument("input", metavar="image_folder", help="path to images folder")
    parser.add_argument(
        "-v", "--verbose", help="increase output verbosity", action="store_true"
    )
    parser.add_argument(
        "-o",
        "--output",
        help="path to output file, default is upgrade.zip",
        default="upgrade.zip",
    )
    parser.add_argument(
        "-f",
        "--file",
        nargs=2,
        metavar=("FOLDER IN ZIP", "FILE"),
        help="extra files you want to add to the upgrade.zip, "
        "all the files will add to utils folder.",
        action="append",
    )
    args = parser.parse_args()
    if args.verbose:
        logging.debug("Enable more verbose output")
        logging.getLogger().setLevel(level=logging.DEBUG)

    return args


def getMD5Sum(file_path: str) -> str:
    m = md5()
    # Partially caculate md5sum for speeding up
    with open(file_path, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            m.update(chunk)

    return m.hexdigest()


def main():
    args = argparser()
    parser = XmlParser(args.xml)
    parts = parser.parse(install=args.input)
    storage = parser.getStorage()
    logging.debug(args)

    imgBuilder = ImagerBuilder(storage, args.input)
    # create a ZipFile object
    with ZipFile(args.output, "w", ZIP_DEFLATED) as zipObj:
        # create metadata for record md5sum
        metadata = NamedTemporaryFile(prefix="meta")

        # Since emmc will not define fip in partition.xml add them
        # manually.
        if storage == "emmc":
            fip_path = path.join(args.input, "fip.bin")
            if path.isfile(fip_path):
                zipObj.write(fip_path, "fip.bin")

        # Add partition file to zip
        for p in parts:
            # Skip file size is equal to zero(Not exists)
            if p["file_size"] == 0:
                continue
            # Try pack header first to avoid user copy image without header
            if p["file_name"] != "fip.bin":
                imgBuilder.packHeader(p)

            # Add file to zipfile
            zipObj.write(p["file_path"], path.basename(p["file_path"]))

            # get MD5sum
            m = getMD5Sum(p["file_path"])
            logging.debug("%s  %s" % (path.basename(p["file_path"]), m))
            with open(metadata.name, "a") as meta:
                meta.write("%s  %s\n" % (m, path.basename(p["file_path"])))

        # Add extra files to zip
        if args.file:
            for folder, f in args.file:
                logging.debug(f)
                m = getMD5Sum(f)
                in_zip_path = path.join(folder, path.basename(f))
                logging.debug("%s  %s\n" % (m, in_zip_path))
                with open(metadata.name, "a") as meta:
                    meta.write("%s  %s\n" % (m, in_zip_path))
                zipObj.write(f, in_zip_path)

        # Add metadata.txt and partition.xml
        zipObj.write(metadata.name, path.join("META", "metadata.txt"))
        zipObj.write(args.xml, path.basename(args.xml))
        # Show zipinfo message
        if args.verbose:
            for info in zipObj.infolist():
                logging.debug(info)
    logging.info("Packing %s done!" % args.output)
    return


if __name__ == "__main__":
    main()
