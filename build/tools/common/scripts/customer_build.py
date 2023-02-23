#!/usr/bin/python
import argparse
import os
import sys
import json
import logging
import shutil
import subprocess

FORMAT = "%(levelname)s: %(message)s"
logging.basicConfig(level=logging.WARNING, format=FORMAT)


def main():
    parser = argparse.ArgumentParser(description="Overwrite project settings")
    parser.add_argument(
        "json_file_path",
        metavar="path_to_json_file",
        type=str,
        help="the path to the project json file",
    )

    # read json file for project settings
    args = parser.parse_args()
    json_file_path = args.json_file_path
    # check json file vaild
    try:
        with open(json_file_path) as f:
            config = json.load(f)
    except FileNotFoundError:
        logging.error(json_file_path + " is not a json config file")
        parser.print_help()
        return

    # read header
    for m in config["header"]:
        cust_path = os.path.join(m["customer_path"])
        top_path = os.path.join(m["top_path"])
        try:
            os.path.isdir(os.path.join(top_path, cust_path))
        except FileNotFoundError as e:
            logging.error("Your in input path is not a valid path")
            logging.error(e)
            parser.print_help()
            return

    # read project settings
    psrc_file_path = []
    pdest_file_path = []
    for x in config.keys():
        if x == "project_settings":
            for m in config[x]:
                src_file_path = os.path.join(
                    top_path, cust_path, m["repository"], m["file_name"]
                )
                dest_path = os.path.join(top_path, m["repository"], m["file_path"])
                dest_file_path = os.path.join(
                    top_path, m["repository"], m["file_path"], m["file_name"]
                )
                if m["repository"] == "":
                    continue
                if str(os.path.exists(src_file_path)) == "False":
                    if str(os.path.islink(src_file_path)) == "False":
                        print(src_file_path, " - source file not exist ")
                        continue
                if str(os.path.isdir(dest_path)) == "False":
                    try:
                        os.makedirs(dest_path)
                    except FileExistsError:
                        logging.info(dest_path + " exists")
                        continue
                psrc_file_path.append(src_file_path)
                pdest_file_path.append(dest_file_path)
            # copy files to the destination
            for i, d in enumerate(psrc_file_path):
                if pdest_file_path[i] != "":
                    shutil.copy(d, pdest_file_path[i], follow_symlinks=False)

        # patch customize code
        if x == "customize_patches":
            path = os.getcwd()
            for m in config[x]:
                os.chdir(path)
                patch_file_path = os.path.join(
                    top_path, cust_path, m["patch_folder"], m["patch_name"]
                )
                dest_patch_path = os.path.join(top_path, m["repository"])
                if str(os.path.isfile(patch_file_path)) == "False":
                    print(patch_file_path, " - patch file not exist ")
                    continue
                if str(os.path.isdir(dest_patch_path)) == "False":
                    print(dest_patch_path, " - destination path not exist ")
                    continue
                shutil.copyfile(patch_file_path, os.path.join(dest_patch_path, m["patch_name"]))
                os.chdir(str(dest_patch_path))
                ret = subprocess.call(
                    ["git apply " + m["patch_name"]], shell=True, stderr=subprocess.STDOUT
                )
                if ret:
                    print("*** git apply ++", m["patch_name"], "++ failed !! ***")
                    sys.exit(1)
                subprocess.call(["rm -rf " + m["patch_name"]], shell=True)
            os.chdir(path)

        # remove unused files/folders
        if x == "remove_file_folder":
            for m in config[x]:
                if m["repository"] == "":
                    continue
                remove_path = os.path.join(top_path, m["repository"], m["file_folder_name"])
                if str(os.path.exists(remove_path)) == "False":
                    print(remove_path, " - patch file not exist ")
                    continue
                subprocess.call(["rm -rf " + remove_path], shell=True)


if __name__ == "__main__":
    main()
