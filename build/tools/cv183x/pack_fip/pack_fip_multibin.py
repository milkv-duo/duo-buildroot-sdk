#!/usr/bin/env python3

import sys
import logging
import os
import re
import os.path
import collections
import uuid
import argparse
from struct import pack, unpack

try:
    import coloredlogs
except ImportError:
    coloredlogs = None

PYTHON_MIN_VERSION = (3, 5, 2)  # Ubuntu 16.04 LTS contains Python v3.5.2 by default
if sys.version_info < PYTHON_MIN_VERSION:
    print("Python >= %r is required" % (PYTHON_MIN_VERSION,))
    sys.exit(-1)


TOC_HEADER_NAME = 0xAA640001
FIP_ALIGN_SIZE = 2 * 1024
ENTRY_SIZE = 0x28

IV_ZERO = b"\0" * 16


class FIP_HEADER_FLAG:
    BitRange = collections.namedtuple("BitRange", "shift, bits")

    REE_SCS = BitRange(0, 2)
    REE_ENCRYPTION = BitRange(2, 2)

    @classmethod
    def test(cls, value, flag):
        v = value >> flag.shift
        v &= (1 << flag.bits) - 1
        return v

    @classmethod
    def value(cls, flag):
        v = (1 << flag.bits) - 1
        v <<= flag.shift
        return v


class FIP_UUID:
    # from arm-trusted-firmware/include/tools_share/firmware_image_package.h
    uuid_c_define = """
/* ToC Entry UUIDs */
#define UUID_LICENSE_FILE \
    {0x25360c62, 0x5151, 0x48ad, 0xb5, 0x91, {0x2d, 0x35, 0x67, 0x26, 0x85, 0xa5} }
#define UUID_TRUSTED_UPDATE_FIRMWARE_SCP_BL2U \
    {0x03279265, 0x742f, 0x44e6, 0x8d, 0xff, {0x57, 0x9a, 0xc1, 0xff, 0x06, 0x10} }
#define UUID_TRUSTED_UPDATE_FIRMWARE_BL2U \
    {0x37ebb360, 0xe5c1, 0x41ea, 0x9d, 0xf3, {0x19, 0xed, 0xa1, 0x1f, 0x68, 0x01} }
#define UUID_TRUSTED_UPDATE_FIRMWARE_NS_BL2U \
    {0x111d514f, 0xe52b, 0x494e, 0xb4, 0xc5, {0x83, 0xc2, 0xf7, 0x15, 0x84, 0x0a} }
#define UUID_TRUSTED_FWU_CERT \
    {0xb28a4071, 0xd618, 0x4c87, 0x8b, 0x2e, {0xc6, 0xdc, 0xcd, 0x50, 0xf0, 0x96} }
#define UUID_TRUSTED_BOOT_FIRMWARE_BL2 \
    {0x0becf95f, 0x224d, 0x4d3e, 0xa5, 0x44, {0xc3, 0x9d, 0x81, 0xc7, 0x3f, 0x0a} }
#define UUID_BLD \
    {0x3dfd6697, 0xbe89, 0x49e8, 0xae, 0x5d, {0x78, 0xa1, 0x40, 0x60, 0x82, 0x13} }
#define UUID_EL3_RUNTIME_FIRMWARE_BL31 \
    {0x6d08d447, 0xfe4c, 0x4698, 0x9b, 0x95, {0x29, 0x50, 0xcb, 0xbd, 0x5a, 0x00} }
#define UUID_SECURE_PAYLOAD_BL32 \
    {0x89e1d005, 0xdc53, 0x4713, 0x8d, 0x2b, {0x50, 0x0a, 0x4b, 0x7a, 0x3e, 0x38} }
#define UUID_NON_TRUSTED_FIRMWARE_BL33 \
    {0xa7eed0d6, 0xeafc, 0x4bd5, 0x97, 0x82, {0x99, 0x34, 0xf2, 0x34, 0xb6, 0xe4} }
/* Key certificates */
#define UUID_ROT_KEY_CERT \
    {0x721d2d86, 0x60f8, 0x11e4, 0x92, 0x0b, {0x8b, 0xe7, 0x62, 0x16, 0x0f, 0x24} }
#define UUID_BLD1_KEY_CERT \
    {0x90e87e82, 0x60f8, 0x11e4, 0xa1, 0xb4, {0x77, 0x7a, 0x21, 0xb4, 0xf9, 0x4c} }
#define UUID_BLD2_KEY_CERT \
    {0xa1214202, 0x60f8, 0x11e4, 0x8d, 0x9b, {0xf3, 0x3c, 0x0e, 0x15, 0xa0, 0x14} }
#define UUID_SOC_FW_KEY_CERT \
    {0xccbeb88a, 0x60f9, 0x11e4, 0x9a, 0xd0, {0xeb, 0x48, 0x22, 0xd8, 0xdc, 0xf8} }
#define UUID_TRUSTED_OS_FW_KEY_CERT \
    {0x03d67794, 0x60fb, 0x11e4, 0x85, 0xdd, {0xb7, 0x10, 0x5b, 0x8c, 0xee, 0x04} }
#define UUID_BL33_KEY_CERT \
    {0x2a83d58a, 0x60fb, 0x11e4, 0x8a, 0xaf, {0xdf, 0x30, 0xbb, 0xc4, 0x98, 0x59} }
/* Content certificates */
#define UUID_TRUSTED_BOOT_FW_CERT \
    {0xea69e2d6, 0x635d, 0x11e4, 0x8d, 0x8c, {0x9f, 0xba, 0xbe, 0x99, 0x56, 0xa5} }
#define UUID_BLD_CONTENT_CERT \
    {0x046fbe44, 0x635e, 0x11e4, 0xb2, 0x8b, {0x73, 0xd8, 0xea, 0xae, 0x96, 0x56} }
#define UUID_SOC_FW_CONTENT_CERT \
    {0x200cb2e2, 0x635e, 0x11e4, 0x9c, 0xe8, {0xab, 0xcc, 0xf9, 0x2b, 0xb6, 0x66} }
#define UUID_TRUSTED_OS_FW_CONTENT_CERT \
    {0x11449fa4, 0x635e, 0x11e4, 0x87, 0x28, {0x3f, 0x05, 0x72, 0x2a, 0xf3, 0x3d} }
#define UUID_BL33_CONTENT_CERT \
    {0xf3c1c48e, 0x635d, 0x11e4, 0xa7, 0xa9, {0x87, 0xee, 0x40, 0xb2, 0x3f, 0xa7} }
/* CV keys */
#define UUID_CV_TRUSTED_KEY_CERT \
    {0x64fbfc49, 0x4b8c, 0x4ad3, 0xb9, 0x92, {0x93, 0x55, 0x89, 0xee, 0xf0, 0x12} }
#define UUID_CV_NON_TRUSTED_KEY_CERT \
    {0xcb48bf0d, 0x7012, 0x4201, 0xbc, 0x35, {0x8a, 0x51, 0xc4, 0x90, 0x90, 0x94} }

/* DDR init*/
#define UUID_CV_DDRINIT_KEY_CERT \
    {0xa61c53c9, 0x886c, 0x484f, 0x96, 0x5d, {0xd2, 0xda, 0xd7, 0xc3, 0xeb, 0x13} }
#define UUID_CV_DDRINIT_CONTENT_CERT \
    {0x9dfaabd2, 0x7f1b, 0x47e6, 0xa8, 0xa6, {0x6a, 0xc3, 0x10, 0xcc, 0xac, 0x91} }
#define UUID_CV_DDRINIT \
    {0x5888a5cd, 0x38fc, 0x4f66, 0xae, 0x3d, {0x2e, 0x18, 0x6d, 0x69, 0x41, 0xfb} }

/* Fast boot */
#define UUID_CV_FASTBOOT_KEY_CERT \
    {0x285df54e, 0x7b50, 0x4309, 0x9b, 0x52, {0x4b, 0xc4, 0x92, 0x82, 0x60, 0xdd} }
#define UUID_CV_FASTBOOT_CONTENT_CERT \
    {0x61f7595b, 0x8d77, 0x4e13, 0x91, 0x2a, {0x63, 0x6e, 0x58, 0xda, 0x5b, 0x69} }
#define UUID_CV_FASTBOOT \
    {0x43766198, 0xc363, 0x48db, 0xa9, 0x97, {0xf1, 0x0e, 0x93, 0x80, 0x4f, 0xea} }
"""
    NAMES = collections.OrderedDict(
        [
            ("LICENSE_FILE", "UUID_LICENSE_FILE"),
            ("BL2", "UUID_TRUSTED_BOOT_FIRMWARE_BL2"),
            ("BLD", "UUID_BLD"),
            ("BL31", "UUID_EL3_RUNTIME_FIRMWARE_BL31"),
            ("BL32", "UUID_SECURE_PAYLOAD_BL32"),
            ("BL33", "UUID_NON_TRUSTED_FIRMWARE_BL33"),
            ("BLD1_KEY_CERT", "UUID_BLD1_KEY_CERT"),
            ("BLD2_KEY_CERT", "UUID_BLD2_KEY_CERT"),
            ("SOC_FW_KEY_CERT", "UUID_SOC_FW_KEY_CERT"),
            ("TRUSTED_OS_FW_KEY_CERT", "UUID_TRUSTED_OS_FW_KEY_CERT"),
            ("BL33_KEY_CERT", "UUID_BL33_KEY_CERT"),
            ("TRUSTED_BOOT_FW_CERT", "UUID_TRUSTED_BOOT_FW_CERT"),
            ("BLD_CONTENT_CERT", "UUID_BLD_CONTENT_CERT"),
            ("SOC_FW_CONTENT_CERT", "UUID_SOC_FW_CONTENT_CERT"),
            ("TRUSTED_OS_FW_CONTENT_CERT", "UUID_TRUSTED_OS_FW_CONTENT_CERT"),
            ("BL33_CONTENT_CERT", "UUID_BL33_CONTENT_CERT"),
            ("CV_TRUSTED_KEY_CERT", "UUID_CV_TRUSTED_KEY_CERT"),
            ("CV_NON_TRUSTED_KEY_CERT", "UUID_CV_NON_TRUSTED_KEY_CERT"),
            ("DDRINIT", "UUID_CV_DDRINIT"),
            ("FASTBOOT", "UUID_CV_FASTBOOT"),
        ]
    )

    @classmethod
    def cls_init(cls):
        txt = cls.uuid_c_define
        txt = txt.replace("\r\n", "\n")
        txt = txt.replace("\\\n", "\n")
        rx = r"""
            \#define\s+
            (?P<name>\S+)\s+
            {
                \s*(?P<u0>0x\S+)\s*,\s*
                \s*(?P<u1>0x\S+)\s*,\s*
                \s*(?P<u2>0x\S+)\s*,\s*
                \s*(?P<u3>0x\S+)\s*,\s*
                \s*(?P<u4>0x\S+)\s*,\s*
                {
                    \s*(?P<u5>0x\S+)\s*,\s*
                    \s*(?P<u6>0x\S+)\s*,\s*
                    \s*(?P<u7>0x\S+)\s*,\s*
                    \s*(?P<u8>0x\S+)\s*,\s*
                    \s*(?P<u9>0x\S+)\s*,\s*
                    \s*(?P<u10>0x\S+)\s*
                }\s*,?\s*
            }
        """
        for m in re.finditer(rx, txt, flags=re.X):
            name = m.group("name")
            u = m.group(*["u%d" % i for i in range(11)])
            u = [int(i, 0) for i in u]
            u = pack("<IHHBBBBBBBB", *u)
            u = uuid.UUID(bytes=u)
            setattr(cls, name, u)


class Entry:
    __slots__ = ["name", "loc", "uuid", "address", "flag", "content"]

    def __init__(self):
        self.loc = 0
        self.uuid = uuid.UUID(int=0)
        self.address = 0
        self.flag = 0
        self.content = b""

    @classmethod
    def make(cls, uuid_name, content):
        entry = cls()
        entry.name = uuid_name
        entry.uuid = getattr(FIP_UUID, uuid_name)
        entry.content = content
        return entry

    @classmethod
    def from_fip(cls, name, loc, fip_bin):
        data = fip_bin[loc : loc + ENTRY_SIZE]
        uuid_bytes, address, size, flag = unpack("<16sQQQ", data)
        content = fip_bin[address : address + size]

        entry = cls()
        entry.name = name
        entry.loc = loc
        entry.uuid = uuid.UUID(bytes=uuid_bytes)
        entry.address = address
        entry.flag = flag
        entry.content = content
        return entry

    def to_bytes(self):
        return pack("<16sQQQ", self.uuid.bytes, self.address, self.size, self.flag)

    @property
    def size(self):
        return len(self.content)

    @property
    def end(self):
        return self.address + self.size

    def __str__(self):
        return "<%-31s loc=0x%03x U=%s a=0x%05x,0x%05x,0x%05x f=0x%x>" % (
            self.name,
            self.loc,
            self.uuid.hex[:8],
            self.address,
            self.end,
            self.size,
            self.flag,
        )


class FIP:
    ENTRY_NAMES = [
        "LICENSE_FILE",
        "BL2",
        "BLD",
        "BL31",
        "BL32",
        "BL33",
        "BLD1_KEY_CERT",
        "BLD2_KEY_CERT",
        "CV_TRUSTED_KEY_CERT",
        "SOC_FW_KEY_CERT",
        "TRUSTED_OS_FW_KEY_CERT",
        "CV_NON_TRUSTED_KEY_CERT",
        "BL33_KEY_CERT",
        "TRUSTED_BOOT_FW_CERT",
        "BLD_CONTENT_CERT",
        "SOC_FW_CONTENT_CERT",
        "TRUSTED_OS_FW_CONTENT_CERT",
        "BL33_CONTENT_CERT",
    ]

    TOC_Header = collections.namedtuple(
        "TOC_Header", "name, serial, flag_res, flag_plat, flag_res2"
    )

    def __init__(self, path):
        logging.info("FIP_BIN: %s", path)
        self.path = path

    def load(self):
        with open(self.path, "rb") as fp:
            self.binary = fp.read()
        logging.info("%s is %d bytes", self.path, len(self.binary))

        self.header = self.TOC_Header(*unpack("<IIIHH", self.binary[0x00:0x10]))
        if self.header.name != TOC_HEADER_NAME:
            raise ValueError(
                "FIP header is 0x%08x but should be 0x%08x"
                % (self.header[0], TOC_HEADER_NAME)
            )

        logging.info("TOC header: flag_plat=0x%04x", self.header.flag_plat)
        logging.info(
            "  REE_SCS: %r",
            FIP_HEADER_FLAG.test(self.header.flag_plat, FIP_HEADER_FLAG.REE_SCS),
        )
        logging.info(
            "  REE_ENCRYPTION: %r",
            FIP_HEADER_FLAG.test(self.header.flag_plat, FIP_HEADER_FLAG.REE_ENCRYPTION),
        )

        ents = []
        for name, uuid_name in FIP_UUID.NAMES.items():
            try:
                ents.append((name, self.find_entry(uuid_name)))
            except ValueError as err:
                logging.info("%s", err)

        ents.sort(key=lambda x: x[1].address)
        for n, (k, v) in enumerate(ents):
            logging.debug("%s", v)

        last = ents[-1][1].end
        rest = self.binary[last:]
        loc = rest.find(b"APLB")
        if loc < 0:
            self.blp_binary = b""
        else:
            self.blp_binary = rest[loc:]
        logging.debug("BLD/DDRC: %d bytes @0x%x", len(self.blp_binary), (last + loc))

        self.ents = collections.OrderedDict(ents)

    def make_fip(self, output_path=None):
        logging.info("New TOC header: flag_plat=0x%04x", self.header.flag_plat)
        header_bin = pack("<IIIHH", *self.header)
        fip_bin = header_bin

        offset = (len(self.ents) + 1) * ENTRY_SIZE + 0x10

        for name in self.ENTRY_NAMES:
            entry = self.ents[name]
            entry.address = offset
            fip_bin += entry.to_bytes()
            offset += entry.size

        null_entry = Entry()
        null_entry.address = offset
        fip_bin += null_entry.to_bytes()

        for name in self.ENTRY_NAMES:
            entry = self.ents[name]
            fip_bin += entry.content

        if (len(fip_bin) % FIP_ALIGN_SIZE) > 0:
            fip_bin += b"\x00" * (FIP_ALIGN_SIZE - len(fip_bin) % FIP_ALIGN_SIZE)

        fip_bin += self.blp_binary

        if output_path:
            path = output_path
        else:
            path = os.path.splitext(self.path)
            path = path[0] + "_merged" + path[1]
        logging.info("Save new FIP image to %s", path)
        with open(path, "wb") as fp:
            fp.write(fip_bin)

    def dump_uuids(self):
        for k, v in vars(FIP_UUID).items():
            if k.startswith("UUID_"):
                print("%-38s" % k, v.hex)

    def find_entry(self, uuid_name):
        uuid = getattr(FIP_UUID, uuid_name)
        loc = self.binary.find(uuid.bytes)
        if loc < 0:
            raise ValueError("%s is not found" % uuid_name)
        return Entry.from_fip(uuid_name, loc, self.binary)


class FIP_Multi(FIP):
    ENTRY_NAMES_1ST = [
        "BL2",
        "BLD",
        "BLD1_KEY_CERT",
        "BLD2_KEY_CERT",
        "CV_TRUSTED_KEY_CERT",
        "SOC_FW_KEY_CERT",
        "TRUSTED_OS_FW_KEY_CERT",
        "CV_NON_TRUSTED_KEY_CERT",
        "BL33_KEY_CERT",
        "TRUSTED_BOOT_FW_CERT",
        "BLD_CONTENT_CERT",
        "SOC_FW_CONTENT_CERT",
        "TRUSTED_OS_FW_CONTENT_CERT",
        "BL33_CONTENT_CERT",
    ]

    ENTRY_NAMES_2ND = [
        "LICENSE_FILE",
        "DDRINIT",
        "FASTBOOT",
        "CV_TRUSTED_KEY_CERT",
        "SOC_FW_KEY_CERT",
        "TRUSTED_OS_FW_KEY_CERT",
        "CV_NON_TRUSTED_KEY_CERT",
        "BL33_KEY_CERT",
        "SOC_FW_CONTENT_CERT",
        "TRUSTED_OS_FW_CONTENT_CERT",
        "BL33_CONTENT_CERT",
        "BL31",
        "BL32",
        "BL33",
    ]

    def _gen_fip(self, entry_names, with_blp_ddrc=True):
        logging.info("New TOC header: flag_plat=0x%04x", self.header.flag_plat)
        header_bin = pack("<IIIHH", *self.header)

        fip_bin = header_bin[:]

        offset = (len(entry_names) + 1) * ENTRY_SIZE + 0x10
        for name in entry_names:
            entry = self.ents[name]
            entry.address = offset
            fip_bin += entry.to_bytes()
            offset += entry.size

        null_entry = Entry()
        null_entry.address = offset
        fip_bin += null_entry.to_bytes()

        for name in entry_names:
            entry = self.ents[name]
            fip_bin += entry.content

        if with_blp_ddrc:
            logging.info("Append BLP/DDRC")
            if (len(fip_bin) % FIP_ALIGN_SIZE) > 0:
                fip_bin += b"\x00" * (FIP_ALIGN_SIZE - len(fip_bin) % FIP_ALIGN_SIZE)
            fip_bin += self.blp_binary

        return fip_bin

    def gen_multibin(self, output_path):
        logging.info("Generate 1st FIP images")
        fip1st_bin = self._gen_fip(self.ENTRY_NAMES_1ST, True)
        logging.info("Generate 2nd FIP images")
        fip2nd_bin = self._gen_fip(self.ENTRY_NAMES_2ND, False)

        path = os.path.splitext(output_path)
        path_1st = path[0] + "_1st" + path[1]
        path_2nd = path[0] + "_2nd" + path[1]

        logging.info("Save 1st FIP image to %s", path_1st)
        with open(path_1st, "wb") as fp:
            fp.write(fip1st_bin)

        logging.info("Save 2nd FIP image to %s", path_2nd)
        with open(path_2nd, "wb") as fp:
            fp.write(fip2nd_bin)


def init_logging(log_file=None, file_level="DEBUG", stdout_level="WARNING"):
    root_logger = logging.getLogger()
    root_logger.setLevel(logging.NOTSET)

    fmt = "%(asctime)s %(levelname)8s:%(name)s:%(message)s"

    if log_file is not None:
        file_handler = logging.FileHandler(log_file, encoding="utf-8")
        file_handler.setFormatter(logging.Formatter(fmt))
        file_handler.setLevel(file_level)
        root_logger.addHandler(file_handler)

    if coloredlogs:
        os.environ["COLOREDLOGS_DATE_FORMAT"] = "%H:%M:%S"

        field_styles = {
            "asctime": {"color": "green"},
            "hostname": {"color": "magenta"},
            "levelname": {"color": "black", "bold": True},
            "name": {"color": "blue"},
            "programname": {"color": "cyan"},
        }

        level_styles = coloredlogs.DEFAULT_LEVEL_STYLES
        level_styles["debug"]["color"] = "cyan"

        coloredlogs.install(
            level=stdout_level,
            fmt=fmt,
            field_styles=field_styles,
            level_styles=level_styles,
            milliseconds=True,
        )


def parse_fip(fip_path):
    logging.debug("parse_fip: %s", fip_path)
    fip = FIP(fip_path)
    fip.load()


def unpack_fip(fip_path):
    logging.debug("unpack_fip: %s", fip_path)
    fip = FIP(fip_path)
    fip.load()

    def save(name, content):
        fn = os.path.splitext(fip_path)
        fn = "%s_%s%s" % (fn[0], name, fn[1])
        logging.info("Save %s", fn)
        with open(fn, "wb") as fp:
            fp.write(content)

    for k, v in fip.ents.items():
        save(k, v.content)

    save("BLP", fip.blp_binary)


def merge_fip(fip_path, inputs, output_path):
    logging.debug("merge_fip: %s", fip_path)
    fip = FIP(fip_path)
    fip.load()

    bl33_bin = inputs.get("BL33_BIN")
    if bl33_bin:
        logging.debug("merge bl33")
        fip.ents["BL33"].content = bl33_bin

    bld_bin = inputs.get("BLP_BIN")
    if bld_bin:
        pass
    bldp_bin = inputs.get("BLDP_BIN")
    if bldp_bin:
        pass

    fip.make_fip(output_path)


def gen_multibin(fip_path, inputs, output_path):
    logging.debug("gen_multibin: %s", fip_path)
    fip = FIP_Multi(fip_path)
    fip.load()

    fastboot_bin = inputs.get("fastboot")
    if fastboot_bin:
        logging.debug("Merge FASTBOOT")
        fip.ents["FASTBOOT"] = Entry.make("UUID_CV_FASTBOOT", fastboot_bin)

    bl33_bin = inputs.get("bl33")
    if bl33_bin:
        logging.debug("Merge BL33")
        fip.ents["BL33"].content = bl33_bin

    fip.gen_multibin(output_path)


def main():
    parser = argparse.ArgumentParser(description="FIP packer")

    parser.add_argument("--blp", type=str)
    parser.add_argument("--ddrc", type=str)
    parser.add_argument("--ddrinit", type=str)
    parser.add_argument("--fastboot", type=str)
    parser.add_argument("--bl33", "--add-bl33", type=str, help="Merge BL33 into FIP")

    parser.add_argument("--multibin", action="store_true")

    parser.add_argument("FIP_BIN", type=str, nargs=1, help="Input FIP binary")
    parser.add_argument("--output", type=str, help="Output filename")

    parser.add_argument(
        "--version", action="store_true", help="Output version information and exit"
    )
    parser.add_argument(
        "--verbose",
        help="Increase output verbosity",
        action="store_const",
        const=logging.DEBUG,
        default=logging.DEBUG,
    )

    parser.add_argument("--unpack", action="store_true", help="Unpack FIP.bin")
    parser.add_argument("--parse", action="store_true", help="Parse FIP.bin")

    args = parser.parse_args()

    init_logging(stdout_level=args.verbose)

    logging.debug("args=%r", args)

    FIP_UUID.cls_init()

    if not args.output:
        if args.multibin:
            fn = list(os.path.splitext(args.FIP_BIN[0]))
            fn[0] = fn[0].rstrip("_single")
            fn = "%s%s%s" % (fn[0], "", fn[1])
            args.output = fn
        else:
            fn = os.path.splitext(args.FIP_BIN[0])
            fn = "%s_%s%s" % (fn[0], "merged", fn[1])
            args.output = fn

    if args.parse:
        parse_fip(args.FIP_BIN[0])

    if args.unpack:
        unpack_fip(args.FIP_BIN[0])

    inputs = collections.OrderedDict()
    for name in ["bl33", "blp", "ddrc", "ddrinit", "fastboot"]:
        fn = getattr(args, name)
        if not fn:
            continue
        logging.info("Open %s", fn)
        with open(fn, "rb") as fp:
            inputs[name] = fp.read()

    if args.multibin:
        gen_multibin(args.FIP_BIN[0], inputs, args.output)
    else:
        if len(inputs):
            merge_fip(args.FIP_BIN[0], inputs, args.output)


if __name__ == "__main__":
    main()
