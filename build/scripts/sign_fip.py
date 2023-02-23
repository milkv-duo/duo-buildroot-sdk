#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK

import logging
import argparse
import os
import stat
import re
import subprocess
import tempfile
import os.path
import tarfile

import build_helper

try:
    import argcomplete
except ImportError:
    argcomplete = None


build_helper.check_python_min_version()


def parse_args():
    parser = argparse.ArgumentParser(
        description="Scan boards to generate env and configs"
    )
    parser.add_argument("-v", "--verbose", default="INFO")
    parser.add_argument("--logfile", type=str)
    parser.add_argument("--sign-atf", type=str)
    parser.add_argument("--sign-bld", type=str)

    if argcomplete:
        argcomplete.autocomplete(parser)

    return parser.parse_args()


def sign_atf(fip_path):
    keyserver = os.environ["KEYSERVER"]
    keyserver_sshkey_path = os.environ["KEYSERVER_SSHKEY_PATH"]
    logging.info(
        "keyserver=%s keyserver_sshkey_path=%s", keyserver, keyserver_sshkey_path
    )

    os.chmod(keyserver_sshkey_path, stat.S_IRUSR)

    logging.info("scp %s to keyserver", fip_path)
    ret = subprocess.run(
        [
            "scp",
            "-i",
            keyserver_sshkey_path,
            fip_path,
            "service_sign@%s:fip.bin" % keyserver,
        ],
        stderr=subprocess.PIPE,
        check=True,
    )
    print(ret.stderr.decode())

    m = re.search(r"TOKEN:(.{32})", ret.stderr.decode())
    token = m.group(1)
    logging.info("token=%s", token)

    logging.info("sign in keyserver")
    ret = subprocess.run(
        [
            "ssh",
            "-i",
            keyserver_sshkey_path,
            "service_sign@%s" % keyserver,
            "sign_fip",
            "--chip=cv1835",
            "--token=%s" % token,
        ],
        check=False,
    )
    logging.debug("%r" % ret)

    fip_signed_path = os.path.splitext(fip_path.replace("_key0", ""))
    fip_signed_path = fip_signed_path[0] + "_key1" + fip_signed_path[1]

    logging.info("copy %s from keyserver", fip_signed_path)
    ret = subprocess.run(
        [
            "scp",
            "-i",
            keyserver_sshkey_path,
            "service_sign@%s:fip_ID%s_signed_encrypted.bin" % (keyserver, token),
            fip_signed_path,
        ],
        check=False,
    )
    logging.debug("%r" % ret)


def extract_bld_bin(bld_tar_path, dst):
    with tarfile.open(bld_tar_path, "r") as tf:
        tf.extract("BLD.bin", dst)

    return os.path.join(dst, "BLD.bin")


def sign_bld(bld_path, chip_arch, chip):
    logging.info("chip_arch=%s:%s bld_path=%s", chip_arch, chip, bld_path)
    fip_atf_key0_path = os.path.join(
        os.environ["RELEASE_BIN_ATF_DIR"],
        "fip_atf_%s_key0.bin" % chip_arch,
    )

    logging.debug("fip_atf_key0_path=%s", fip_atf_key0_path)
    assert os.path.exists(fip_atf_key0_path)

    with tempfile.TemporaryDirectory() as tmpdir:
        logging.debug("tmpdir=%s", tmpdir)

        fip_w_bld_path = os.path.join(
            tmpdir, os.path.basename(fip_atf_key0_path).replace("_key0", "")
        )
        pack_fip_path = os.path.join("build/tools", chip_arch, "pack_fip", "pack_fip.py")

        bld_bin_path = bld_path
        if os.path.splitext(bld_path)[1] == ".tar":
            bld_bin_path = extract_bld_bin(bld_path, tmpdir)
        else:
            bld_bin_path = bld_path

        if os.path.getsize(bld_bin_path) % 16 != 0:
            raise ValueError("bld_bin_path size is %d", os.path.getsize(bld_bin_path))

        subprocess.run(
            [
                "python3",
                pack_fip_path,
                fip_atf_key0_path,
                "--output",
                fip_w_bld_path,
                "--add-bld",
                bld_bin_path,
            ],
            check=True,
        )

        subprocess.run(
            ["python3", pack_fip_path, "--parse", fip_w_bld_path], check=True
        )

        sign_atf(fip_w_bld_path)

        fip_signed_path = os.path.splitext(fip_w_bld_path)
        fip_signed_path = fip_signed_path[0] + "_key1" + fip_signed_path[1]
        logging.debug("fip_signed_path=%s", fip_signed_path)
        assert os.path.exists(fip_signed_path)

        subprocess.run(
            ["python3", pack_fip_path, "--unpack", fip_signed_path],
            check=True,
        )

        bld_tar_path = os.path.splitext(bld_path)[0].replace("_key0", "") + "_key1.tar"
        logging.info("bld_tar_path=%s", bld_tar_path)
        members = [
            "BLD_CONTENT_CERT.bin",
            "BLD2_KEY_CERT.bin",
            "BLD1_KEY_CERT.bin",
            "BLD.bin",
        ]
        with tarfile.open(bld_tar_path, "w") as tf:
            for m in members:
                s = os.path.splitext(fip_signed_path)[0] + "_" + m
                logging.debug("Add %s", s)
                tf.add(s, arcname=m, recursive=False)


def main():
    args = parse_args()

    build_helper.init_logging(args.logfile, stdout_level=args.verbose)

    if args.sign_atf:
        sign_atf(args.sign_atf)

    if args.sign_bld:
        chip_arch = os.environ["CHIP_ARCH_L"]
        chip = os.environ["CHIP"]
        sign_bld(args.sign_bld, chip_arch, chip)

    logging.info("END")


if __name__ == "__main__":
    main()
