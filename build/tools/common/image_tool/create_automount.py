#!/usr/bin/python3

import logging
import argparse
from os import path, chmod
from XmlParser import XmlParser

FORMAT = "%(levelname)s: %(message)s"
logging.basicConfig(level=logging.INFO, format=FORMAT)


def parse_Args():
    parser = argparse.ArgumentParser(description="Create CVITEK device image")
    parser.add_argument("xml", help="path to partition xml")
    parser.add_argument(
        "output",
        metavar="output",
        type=str,
        help="the output folder for saving the fstab",
    )
    args = parser.parse_args()
    return args


def genCase(case, out, parts, storage):
    out.write("%s)\n" % case)
    ubi_cnt = 0
    for i, p in enumerate(parts):
        if p["label"] in ("BOOT", "MISC", "ROOTFS", "fip"):
            continue
        if not p["mountpoint"]:
            continue
        if case == "start":
            # out.write('DL_FLAG=`devmem 0x0e000030`\n')
            out.write(
                "ENV_DLFLAG=`fw_printenv dl_flag 2>/dev/null |awk -F= '{print $2}'`\n"
            )
            out.write('printf "Mounting %s partition\\n"\n' % p["label"])
            if storage == "emmc":
                source = "/dev/mmcblk0p" + str((i + 1))
                if i == len(parts) - 1:
                    out.write(
                        "if [ $DL_FLAG == '0x50524F47' ] || [ -z $ENV_DLFLAG ] || [ $ENV_DLFLAG == 'prog' ]; then\n"
                    )
                    out.write(
                        'printf "OK\\nFix\\n" | parted ---pretend-input-tty /dev/mmcblk0 print\n'
                    )
                    out.write("parted -s /dev/mmcblk0 resizepart %d 100%% \n" % (i + 1))
                    out.write("fi\n")
                out.write("e2fsck.static -y %s\n" % source)
                out.write(
                    "mount -t %s -o sync %s %s\n" % (p["type"], source, p["mountpoint"])
                )
                out.write("if [ $? != 0  ]; then\n")
                out.write(
                    "echo 'Mount %s failed, Try formatting and remounting'\n"
                    % p["label"]
                )
                out.write("mke2fs -T %s %s\n" % (p["type"], source))
                out.write(
                    "mount -t %s -o sync %s %s\n" % (p["type"], source, p["mountpoint"])
                )
                out.write("resize2fs %s\n" % (source))
                out.write(
                    "elif [ $DL_FLAG == '0x50524F47' ] || [ -z $ENV_DLFLAG ] || [ $ENV_DLFLAG == 'prog' ]; then\n"
                )
                out.write("resize2fs %s\n" % (source))
                out.write("fi\n")
            elif storage == "spinand":
                ubi_cnt += 1
                source = "/dev/ubi" + str(ubi_cnt) + "_0"
                out.write("ubiattach /dev/ubi_ctrl -m %d\n" % (i))
                if p["type"] != "ubifs":
                    assert "Only supoort ubifs"
                out.write("if [ $? != 0  ]; then\n")
                out.write("ubiformat -y /dev/mtd%d\n" % (i))
                out.write("ubiattach /dev/ubi_ctrl -m %d\n" % (i))
                out.write("ubimkvol /dev/ubi%d -N %s -m\n" % (ubi_cnt, p["label"]))
                out.write("fi\n")
                out.write("if [ ! -c %s ]; then\n" % source)
                out.write("mdev -s\n")
                out.write("fi\n")
                out.write("mount -t ubifs -o sync %s %s\n" % (source, p["mountpoint"]))
                out.write("if [ $? != 0  ]; then\n")
                out.write(
                    "echo 'Mount %s failed, Try formatting and remounting'\n"
                    % p["label"]
                )
                out.write("ubimkvol /dev/ubi%d -N %s -m\n" % (ubi_cnt, p["label"]))
                out.write("mount -t ubifs -o sync %s %s\n" % (source, p["mountpoint"]))
                out.write("fi\n")
            elif storage == "spinor":
                script = """
                if ! mount -t {filesystem} /dev/mtdblock{dev_no} {dev_path}; then
                  echo 'Mount {label} failed, Try erasing and remounting'
                  flash_erase -j /dev/mtd{dev_no} 0 0
                  mount -t {filesystem} /dev/mtdblock{dev_no} {dev_path}
                fi
                """.format(
                    filesystem="jffs2",
                    dev_no=i,
                    dev_path=p["mountpoint"],
                    label=p["label"]
                )
                out.write(script)
        else:
            out.write('printf "Unmounting %s partition\\n"\n' % p["label"])
            out.write("umount %s\n" % p["mountpoint"])

    # Set DL_FLAG flag to complete
    if case == "start" and storage == "emmc":
        # out.write("devmem 0x0e000030 32 0x444F4E45\n")
        out.write("fw_setenv dl_flag done\n")
    out.write(";;\n")


def main():
    args = parse_Args()
    xmlParser = XmlParser(args.xml)
    parts = xmlParser.parse()
    out_path = path.join(args.output, "S10auto_mount")
    try:
        out = open(out_path, "w")
    except Exception:
        logging.error("Create S10auto_mount failed")
        raise
    out.write(
        "#!/bin/sh\n"
        "${CVI_SHOPTS}\n"
        "# This file is automatically generated by create_automount.py\n"
        "# Please do not modify this file manually!\n"
        'case "$1" in\n')
    genCase("start", out, parts, xmlParser.getStorage())
    out.write("esac\n")
    out.close()
    chmod(out_path, 0o755)


if __name__ == "__main__":
    main()
