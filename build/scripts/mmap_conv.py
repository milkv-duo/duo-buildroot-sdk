#!/usr/bin/env python3

import logging
import argparse
import importlib.util
from os.path import basename, splitext, abspath
from collections import OrderedDict
import random


MEMMAP_PREFIX = "CVIMMAP_"


def sort_mmap(mlist):
    base = mlist.get("CVIMMAP_DRAM_BASE")
    if not base:
        base = 0

    mm = mlist.items()

    mm = sorted(mm, key=lambda x: x[1])
    mm = sorted(mm, key=lambda x: x[0])

    return OrderedDict(mm)


def parse_mmap(mmap_module):
    try:
        mmap = mmap_module.MemoryMap
    except AttributeError:
        logging.error("Memory map file must have 'class MemoryMap'")
        raise

    no_prefix = getattr(mmap_module.MemoryMap, "_no_prefix", [])

    mlist = OrderedDict()

    for attr in mmap.__dict__:
        if attr.startswith("_"):
            continue

        value = getattr(mmap, attr)
        if attr not in no_prefix:
            attr = MEMMAP_PREFIX + attr
        mlist[attr] = int(value)

    return sort_mmap(mlist)


def int_to_si(n):
    off = ""

    for i in [0x80000000, 0x100000000]:
        if n >= i:
            off = "offset "
            n -= i
            break

    if n < 1024 * 1024:
        s = "{0}KiB".format(n / 1024)
    else:
        s = "{0}MiB".format(n / (1024 * 1024))

    return off + s


def mmap_to_ld(mlist):
    mlist = ["{0:s} = {1:#x};".format(a, v) for a, v in mlist.items()]

    conf = "\n".join(mlist)
    return conf


def mmap_to_conf(mlist):
    mlist = ["{0:s}={1:#x}".format(a, v) for a, v in mlist.items()]

    conf = "\n".join(mlist)
    return conf


def mmap_to_h(mlist):
    mlist = ["#define {0:s} {1:#x}  /* {2} */".format(a, v, int_to_si(v)) for a, v in mlist.items()]

    r = random.randint(0x80000000, 0xFFFFFFFF)

    conf = (
        "#ifndef __BOARD_MMAP__{0:08x}__\n"
        "#define __BOARD_MMAP__{0:08x}__\n\n"
        "{1}\n\n"
        "#endif /* __BOARD_MMAP__{0:08x}__ */\n".format(r, "\n".join(mlist))
    )
    return conf


def main():
    logging.basicConfig(
        format="%(levelname)8s:%(module)s: %(message)s", level=logging.NOTSET
    )

    parser = argparse.ArgumentParser(description="Generate mmap.h")
    parser.add_argument("--type", choices=["h", "conf", "ld"], required=True)
    parser.add_argument("MAP_FILE", type=str, nargs=1)
    parser.add_argument("OUTPUT", type=str, nargs=1)
    args = parser.parse_args()

    map_file_path = args.MAP_FILE[0]
    logging.info("map_file_path is at %s", map_file_path)
    map_name = splitext(basename(map_file_path))[0]

    # Load map_file as python module
    spec = importlib.util.spec_from_file_location(map_name, map_file_path)
    mmap_module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mmap_module)

    mlist = parse_mmap(mmap_module)

    if args.type == "h":
        out = mmap_to_h(mlist)
    elif args.type == "conf":
        out = mmap_to_conf(mlist)
    elif args.type == "ld":
        out = mmap_to_ld(mlist)

    out_path = abspath(args.OUTPUT[0])
    logging.info("Generate to %s", out_path)
    with open(out_path, "w") as fp:
        fp.write(out)


if __name__ == "__main__":
    main()
