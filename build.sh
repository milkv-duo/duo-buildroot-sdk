#!/bin/bash

TOP_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
cd ${TOP_DIR}

function show_info()
{
  printf "\e[1;94m%s\e[0m\n" "$1"
}

function show_err()
{
  printf "\e[1;31mError: %s\e[0m\n" "$1"
}

function milkv_build()
{
  # clean old img
  old_image_count=`ls ${OUTPUT_DIR}/*.img* | wc -l`
  if [ ${old_image_count} -ge 0 ]; then
    pushd ${OUTPUT_DIR}
    rm -rf *.img*
    popd
  fi

  # clean emmc/nor/nand img
  if [ -f "${OUTPUT_DIR}/upgrade.zip" ]; then
    rm -rf ${OUTPUT_DIR}/*
  fi
   
  clean_all
  build_all
  if [ $? -eq 0 ]; then
    show_info "Build board ${MILKV_BOARD} success!"
  else
    show_err "Build board ${MILKV_BOARD} failed!"
    exit 1
  fi
}

function milkv_pack_sd()
{
  pack_sd_image

  [ ! -d out ] && mkdir out

  img_in="${OUTPUT_DIR}/${MILKV_BOARD}.img"
  img_out="${MILKV_BOARD}_`date +%Y-%m%d-%H%M`.img"

  if [ -f "${img_in}" ]; then
    mv ${img_in} out/${img_out}
    show_info "Create SD image successful: out/${img_out}"
  else
    show_err "Create SD image failed!"
    exit 1
  fi
}

function milkv_pack_emmc()
{
  [ ! -d out ] && mkdir out

  img_in="${OUTPUT_DIR}/upgrade.zip"
  img_out="${MILKV_BOARD}_`date +%Y-%m%d-%H%M`.zip"

  if [ -f "${img_in}" ]; then
    mv ${img_in} out/${img_out}
    show_info "Create eMMC image successful: out/${img_out}"
  else
    show_err "Create eMMC image failed!"
    exit 1
  fi
}

function milkv_pack_nor_nand()
{
  [ ! -d out ] && mkdir out

  if [ -f "${OUTPUT_DIR}/upgrade.zip" ]; then
    img_out_patch=${MILKV_BOARD}-`date +%Y%m%d-%H%M`
    mkdir -p out/$img_out_patch

    if [ "${STORAGE_TYPE}" == "spinor" ]; then
        cp ${OUTPUT_DIR}/fip.bin out/$img_out_patch
        cp ${OUTPUT_DIR}/*.spinor out/$img_out_patch
    else
        cp ${OUTPUT_DIR}/fip.bin out/$img_out_patch
        cp ${OUTPUT_DIR}/*.spinand out/$img_out_patch
    fi

    echo "Copy all to a blank tf card, power on and automatically download firmware to NOR or NAND in U-boot." >> out/$img_out_patch/how_to_download.txt
    show_info "Create spinor/nand img successful: ${img_out_patch}"
  else
    show_err "Create spinor/nand img failed!"
    exit 1
  fi
}

function milkv_pack()
{
  if [ "${STORAGE_TYPE}" == "sd" ]; then
    milkv_pack_sd
  elif [ "${STORAGE_TYPE}" == "emmc" ]; then
    milkv_pack_emmc
  else
    milkv_pack_nor_nand
  fi
}

function list_boards()
{
  for board in "${MILKV_BOARD_ARRAY[@]}"; do
    show_info "$board"
  done
}

function get_toolchain()
{
  if [ ! -d host-tools ]; then
    show_info "Toolchain does not exist, download it now..."

    toolchain_url="https://github.com/milkv-duo/host-tools.git"
    echo "toolchain_url: ${toolchain_url}"

    git clone ${toolchain_url}
    if [ $? -ne 0 ]; then
      show_err "Failed to download ${toolchain_url} !"
      exit 1
    fi
  fi
}

function build_usage()
{
  echo "Usage:"
  echo "${BASH_SOURCE[0]}              - Show this menu"
  echo "${BASH_SOURCE[0]} lunch        - Select a board to build"
  echo "${BASH_SOURCE[0]} [board]      - Build [board] directly, supported boards as follows:"
  list_boards
}

if [ $# -ge 1 ]; then
  if [ "$1" = "lunch" ]; then
    source ${TOP_DIR}/build/envsetup_milkv.sh lunch || exit 1
  else
    source ${TOP_DIR}/build/envsetup_milkv.sh "list" || exit 1
    if [[ ${MILKV_BOARD_ARRAY[@]} =~ (^|[[:space:]])"${1}"($|[[:space:]]) ]]; then
      #MILKV_BOARD=${1}
      check_board ${1} || exit $?
      build_info || exit $?
    else
      show_err "${1} not supported!"
      echo "Available boards:"
      list_boards
      exit $?
    fi
  fi
else
  source ${TOP_DIR}/build/envsetup_milkv.sh list || exit 1
  build_usage && exit 0
fi

get_toolchain

milkv_build
milkv_pack
