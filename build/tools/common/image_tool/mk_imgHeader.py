#!/usr/bin/python3
# -*- coding: utf-8 -*-
import argparse
import logging
import os
from XmlParser import XmlParser

FORMAT = "%(levelname)s: %(message)s"
logging.basicConfig(level=logging.INFO, format=FORMAT)


def main():
    parser = argparse.ArgumentParser(description="Create imgs.h for u-boot")
    parser.add_argument("xml", help="path to partition xml")
    parser.add_argument("output", help="output folder")
    args = parser.parse_args()

    parser = XmlParser(args.xml)
    parts = parser.parse()
    storage = parser.getStorage()
    with open(os.path.join(args.output, "imgs.h"), "w") as of:
        of.write("char imgs[][255] = {")
        if storage == "emmc":
            of.write('"fip.bin",\n')
        for p in parts:
            if p["file_name"] != "":
                of.write('"%s",\n' % p["file_name"])
        of.write("};")


if __name__ == "__main__":
    main()
