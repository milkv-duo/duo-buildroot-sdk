#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK

import logging
import os
import re
import argparse
import itertools
import collections
import json
import os.path
import subprocess
import glob
from datetime import datetime

import build_helper
import sign_fip

try:
    import argcomplete
except ImportError:
    argcomplete = None


build_helper.check_python_min_version()

Board = collections.namedtuple("Board", "chip, board, ddr_cfg, info")
Target = collections.namedtuple("Target", "chip_arch chip is_multi_fip atf_key")

GEN_REL_BIN_EXCLUDE_CHIPS = [
    "cv181x",
    "cv180x",
]

GEN_REL_BIN_EXCLUDE = [
    "cv1835_fpga",
    "cv1835_palladium",
    "cv1822_fpga",
    "cv1822_palladium",
    "cv181x_fpga",
    "cv181x_fpga_c906",
    "cv181x_riscv-fpga",
    "cv181x_palladium",
    "cv1826_wevb_0005a_alios_spinand",
]

ATF_REPO_BRANCH = {}

ATF_REPO_PATH = "arm-trusted-firmware"
BLD_REPO_PATH = "bm_bld"
REL_BIN_ATF = "rel_bin/release_bin_atf"
REL_BIN_BLDS = {
    "bld": "rel_bin/release_bin_bld",
    "bldp": "rel_bin/release_bin_bldp",
    "blp": "rel_bin/release_bin_blp",
}

KEYSERVER = "10.18.98.102"
KEYSERVER_SSHKEY_PATH = os.path.join(
    ATF_REPO_PATH, "tools/build_script/service_sign@cvi_keyserver.pem"
)


def parse_args():
    parser = argparse.ArgumentParser(
        description="Scan boards to generate env and configs"
    )
    parser.add_argument(
        "-v",
        "--verbose",
        default="INFO",
        choices=["CRITICAL", "DEBUG", "ERROR", "INFO", "NOTSET", "WARNING"],
    )
    parser.add_argument("--logfile", type=str)
    parser.add_argument("--gen-atf", action="store_true")
    parser.add_argument("--gen-bld", action="store_true")
    parser.add_argument("--push", action="store_true")
    parser.add_argument("--exclude", action="append")

    if argcomplete:
        argcomplete.autocomplete(parser)

    return parser.parse_args()


def board_dir_to_name(board_dir):
    chips = build_helper.get_chip_list()
    chip_list = list(itertools.chain(*chips.values()))

    m = re.search(
        r"^([0-9a-z]+)_(.+)$", os.path.basename(board_dir), flags=re.IGNORECASE
    )
    chip, br_name = m.groups()
    if chip not in chip_list:
        raise Exception(
            "%r of %r is unknown (missing in chip_list.json?)" % (chip, board_dir)
        )

    for chip_arch, xlist in chips.items():
        if chip in xlist:
            break
    else:
        raise Exception("Can't find CHIP_ARCH for %r" % chip)

    return chip_arch, chip, br_name


def get_current_branch(git_path):
    ret = subprocess.run(
        ["git", "rev-parse", "--abbrev-ref", "HEAD"],
        cwd=git_path,
        check=True,
        stdout=subprocess.PIPE,
    )
    return ret.stdout.decode().strip()


def get_atf_branch(chip_arch, is_multi_fip):
    if is_multi_fip:
        return "cv1835_multibin_boot"

    try:
        atf_branch = [b for b, a in ATF_REPO_BRANCH.items() if chip_arch in a][0]
    except IndexError:
        atf_branch = "master"

    return atf_branch


def checkout_atf_branch(atf_branch):
    current = get_current_branch(ATF_REPO_PATH)

    if current == atf_branch:
        return

    logging.info("checkout_atf_branch: %s", atf_branch)

    if atf_branch != "master":
        git_cmd = [
            "git",
            "fetch",
            "--depth",
            "1",
            "origin",
            "%s:%s" % (atf_branch, atf_branch),
        ]
        subprocess.run(git_cmd, cwd=ATF_REPO_PATH, check=True)

    subprocess.run(["git", "checkout", atf_branch], cwd=ATF_REPO_PATH, check=True)
    subprocess.run(["git", "clean", "-fd"], cwd=ATF_REPO_PATH, check=True)


def list_board_dirs():
    for board_conf in sorted(glob.glob(build_helper.BOARD_KCONFIG_SAVED_GLOB)):
        board_dir = os.path.dirname(board_conf)

        if "/default/" in board_dir:
            continue

        if not os.path.isdir(board_dir):
            continue

        yield board_dir


def gen_bld_for_board(chip_arch, board, atf_key):
    logging.info("gen_bld_rel_bin_for_board=%s (%s)", board, atf_key)

    for i in [ATF_REPO_PATH, BLD_REPO_PATH]:
        if not os.path.exists(i):
            raise Exception("%s doesn't exist" % i)

    chip_arch, chip, br_name = board_dir_to_name(board)
    logging.info("%r", [chip_arch, chip, br_name])
    atf_branch = get_atf_branch(chip_arch, "rtos" in br_name)

    checkout_atf_branch(atf_branch)

    cj_path = os.path.join(
        "build", build_helper.BOARD_DIR, chip_arch, board, "config.json"
    )
    with open(cj_path, "r", encoding="utf-8") as fp:
        cj = json.load(fp)
        ddr_cfg_list = cj["ddr_cfg_list"]

    ddr_cfg_list = [i for i in ddr_cfg_list if i]
    if not ddr_cfg_list:
        ddr_cfg_list = ["none"]

    # Build for all DDR_CFG
    if atf_key != "":
        atf_key = "setconfig ATF_KEY_SEL_%s=y" % atf_key

    for d in ddr_cfg_list:
        logging.info("ddr_cfg=%s", d)

        script = """
        set -eo pipefail
        source build/envsetup_soc.sh f
        defconfig %(board)s
        setconfig DDR_CFG_%(ddr_cfg)s=y
        %(atf_key)s
        clean_bld
        clean_atf
        build_atf
        """ % {
            "board": board,
            "ddr_cfg": d,
            "atf_key": atf_key,
        }
        subprocess.run(["bash"], input=script.encode(), shell=True, check=True)


def gen_bld(push):
    boards = [board_dir_to_name(e) for e in list_board_dirs()]
    boards = [
        (get_atf_branch(chip_arch, False), chip_arch, chip, br)
        for chip_arch, chip, br in boards
    ]
    boards.sort()
    for n, (_, chip_arch, chip, br) in enumerate(boards):
        if chip in GEN_REL_BIN_EXCLUDE_CHIPS:
            continue

        fullname = chip + "_" + br
        if fullname in GEN_REL_BIN_EXCLUDE:
            continue

        gen_bld_for_board(chip_arch, fullname, "")
        if chip_arch == "cv183x" and "rtos" not in fullname:
            gen_bld_for_board(chip_arch, fullname, "clear")

    checkout_atf_branch("master")

    for i in sorted(glob.glob(os.path.join(REL_BIN_BLDS["bld"], "*_key0.tar"))):
        if "_single" in i:
            continue
        if "_rtos" in i:
            continue

        b = re.findall(r'^bld_(.*?)_(.*)_key0.tar', os.path.basename(i))[0][1]
        chip_arch, chip, _ = board_dir_to_name(b)
        sign_fip.sign_bld(i, chip_arch, chip)

    git_cmd = ["git", "log", r"--pretty=format:%h %aI%n%s%n%b", "-n", "1"]
    ret = subprocess.run(git_cmd, cwd=BLD_REPO_PATH, check=True, stdout=subprocess.PIPE)
    message = ret.stdout.decode()
    message = "\n".join([">>> " + i for i in message.split("\n")])
    logging.info("message=%r", message)

    for path in REL_BIN_BLDS.values():
        subprocess.run(["git", "add", "."], cwd=path, check=True)
        ret = subprocess.run(
            ["git", "status", "--porcelain"],
            cwd=path,
            check=True,
            stdout=subprocess.PIPE,
        )

        if not ret.stdout.decode().strip():
            logging.info("%s: nothing to commit", path)
            continue

        subprocess.run(
            ["git", "commit", "-F", "-"],
            cwd=path,
            input=message.encode(),
            check=True,
        )
        if push:
            subprocess.run(
                ["git", "push", "origin", "HEAD:master"], cwd=path, check=True
            )


def gen_atf_for_chip(target):
    logging.info("gen_atf_for_chip=%r", target)

    for i in [ATF_REPO_PATH, BLD_REPO_PATH]:
        if not os.path.exists(i):
            raise Exception("%s doesn't exist" % i)

    boards = [board_dir_to_name(e) for e in list_board_dirs()]

    for _, chip, br in boards:
        if "fpga" in br or "palladium" in br:
            continue

        if target.chip in chip:
            break
    else:
        logging.warning("No board for %s", target.chip)
        return

    board = chip + "_" + br

    atf_branch = get_atf_branch(target.chip_arch, target.is_multi_fip)
    logging.info("atf_branch=%s board=%s", atf_branch, board)
    checkout_atf_branch(atf_branch)

    git_cmd = ["git", "log", r"--pretty=format:%h %aI%n%s%n%b", "-n", "1"]
    ret = subprocess.run(git_cmd, cwd=ATF_REPO_PATH, check=True, stdout=subprocess.PIPE)
    message = ret.stdout.decode()

    # Build fip.bin
    atf_key = ""
    if target.atf_key:
        atf_key = "setconfig ATF_KEY_SEL_%s=y" % target.atf_key

    script = """
    set -eo pipefail
    source build/envsetup_soc.sh f
    defconfig %(board)s
    %(multibin)s
    %(atf_key)s
    clean_bld
    clean_atf
    build_atf
    """ % {
        "board": board,
        "multibin": "setconfig MULTI_FIP=y" if target.is_multi_fip else "",
        "atf_key": atf_key,
    }
    subprocess.run(["bash"], input=script.encode(), shell=True, check=True)

    # Check status
    ret = subprocess.run(
        ["git", "status", "--porcelain", "-z"],
        cwd=REL_BIN_ATF,
        check=True,
        stdout=subprocess.PIPE,
    )
    out = ret.stdout.strip(b"\0").split(b"\0")
    out = [i.decode() for i in out]

    fip = []
    txt = []
    for i in out:
        if re.search(r"^( [MA]|\?\?) .+\.bin", i):
            fip.append(i)
        elif re.search(r"^( [MA]|\?\?) .+\.txt", i):
            txt.append(i)

    if len(fip) != 1:
        raise ValueError("Only one fip.bin should be generated (%r)", fip)
    if len(txt) > 1:
        raise ValueError("Only one fip.txt should be generated (%r)", txt)

    # Add fip.bin
    logging.info("add fip %s", fip[0])
    subprocess.run(["git", "add", fip[0][3:]], cwd=REL_BIN_ATF, check=True)
    if txt:
        logging.info("add txt %s", txt[0])
        subprocess.run(["git", "add", txt[0][3:]], cwd=REL_BIN_ATF, check=True)

    if fip[0][3:].endswith("_key0.bin"):
        sign_fip.sign_atf(os.path.join(REL_BIN_ATF, fip[0][3:]))
        subprocess.run(
            ["git", "add", fip[0][3:].replace("_key0", "_key1")],
            cwd=REL_BIN_ATF,
            check=True,
        )

    return message


def gen_atf(push):
    chip_list = build_helper.get_chip_list()

    xlist = [
        (get_atf_branch(chip_arch, False), chip_arch, chips, False)
        for chip_arch, chips in chip_list.items()
    ]
    xlist.sort(reverse=True)

    targets = [Target("cv183x", "cv1835", None, "clear")]
    for _, chip_arch, chips, is_multi_fip in xlist:
        if not chips:
            continue
        t = Target(chip_arch, chips[0], is_multi_fip, None)
        targets.append(t)

    message = set()
    for t in targets:
        m = gen_atf_for_chip(t)
        if not m:
            continue
        message.add(m.strip())

    message = "\n\n".join(message)
    message = "\n".join([">>> " + i for i in message.split("\n")])
    logging.info("message=%r", message)

    subprocess.run(
        ["git", "commit", "-F", "-"],
        cwd=REL_BIN_ATF,
        input=message.encode(),
        check=True,
    )

    if push:
        subprocess.run(
            ["git", "push", "origin", "HEAD:master"], cwd=REL_BIN_ATF, check=True
        )


def main():
    args = parse_args()

    build_helper.init_logging(args.logfile, stdout_level=args.verbose)
    logging.debug("[%s] start", datetime.now().isoformat())

    # The location of the top Kconfig
    os.environ["srctree"] = build_helper.BUILD_REPO_DIR

    os.environ["KEYSERVER"] = KEYSERVER
    os.environ["KEYSERVER_SSHKEY_PATH"] = KEYSERVER_SSHKEY_PATH
    os.environ["RELEASE_BIN_ATF_DIR"] = REL_BIN_ATF

    if args.exclude:
        GEN_REL_BIN_EXCLUDE.extend(args.exclude)

    if args.gen_bld:
        gen_bld(args.push)

    if args.gen_atf:
        gen_atf(args.push)

    logging.debug("[%s] finished", datetime.now().isoformat())


if __name__ == "__main__":
    main()
