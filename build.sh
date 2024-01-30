#!/bin/bash

MILKV_BOARD_ARRAY=
MILKV_BOARD_ARRAY_LEN=
MILKV_BOARD=
MILKV_BOARD_CONFIG=
MILKV_IMAGE_CONFIG=
MILKV_DEFAULT_BOARD=milkv-duo

TOP_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
#echo "TOP_DIR: ${TOP_DIR}"
cd ${TOP_DIR}

function print_info()
{
  printf "\e[1;32m%s\e[0m\n" "$1"
}

function print_err()
{
  printf "\e[1;31mError: %s\e[0m\n" "$1"
}

function get_toolchain()
{
  if [ ! -d host-tools ]; then
    print_info "Toolchain does not exist, download it now..."

    toolchain_url="https://sophon-file.sophon.cn/sophon-prod-s3/drive/23/03/07/16/host-tools.tar.gz"
    echo "toolchain_url: ${toolchain_url}"
    toolchain_file=${toolchain_url##*/}
    echo "toolchain_file: ${toolchain_file}"

    wget ${toolchain_url} -O ${toolchain_file}
    if [ $? -ne 0 ]; then
      print_err "Failed to download ${toolchain_url} !"
      exit 1
    fi

    if [ ! -f ${toolchain_file} ]; then
      print_err "${toolchain_file} not found!"
      exit 1
    fi

    print_info "Extracting ${toolchain_file}..."
    tar -xf ${toolchain_file}
    if [ $? -ne 0 ]; then
      print_err "Extract ${toolchain_file} failed!"
      exit 1
    fi

    [ -f ${toolchain_file} ] && rm -rf ${toolchain_file}

  fi
}

function get_available_board()
{
  MILKV_BOARD_ARRAY=( $(find device -mindepth 1 -maxdepth 1 -not -path 'device/common' -type d -print ! -name "." | awk -F/ '{ print $NF }' | sort -t '-' -k2,2) )
  #echo ${MILKV_BOARD_ARRAY[@]}

  MILKV_BOARD_ARRAY_LEN=${#MILKV_BOARD_ARRAY[@]}
  if [ $MILKV_BOARD_ARRAY_LEN -eq 0 ]; then
    echo "No available config"
    exit 1
  fi

  #echo ${MILKV_BOARD_ARRAY[@]} | xargs -n 1 | sed "=" | sed "N;s/\n/. /"
}

function choose_board()
{
  echo "Select a target to build:"

  echo ${MILKV_BOARD_ARRAY[@]} | xargs -n 1 | sed "=" | sed "N;s/\n/. /"

  local index
  read -p "Which would you like: " index

  if [[ -z $index ]]; then
    echo "Nothing selected."
    exit 0
  fi

  if [[ -n $index && $index =~ ^[0-9]+$ && $index -ge 1 && $index -le $MILKV_BOARD_ARRAY_LEN ]]; then
    MILKV_BOARD="${MILKV_BOARD_ARRAY[$((index - 1))]}"
    #echo "index: $index, Board: $MILKV_BOARD"
  else
    print_err "Invalid input!"
    exit 1
  fi
}

function prepare_env()
{
  source ${MILKV_BOARD_CONFIG}

  source build/${MV_BUILD_ENV} > /dev/null 2>&1
  defconfig ${MV_BOARD_LINK} > /dev/null 2>&1

  echo "OUTPUT_DIR: ${OUTPUT_DIR}"  # @build/milkvsetup.sh

  if [ "${STORAGE_TYPE}" == "sd" ]; then
    MILKV_IMAGE_CONFIG=device/${MILKV_BOARD}/genimage.cfg

    if [ ! -f ${MILKV_IMAGE_CONFIG} ]; then
      print_err "${MILKV_IMAGE_CONFIG} not found!"
      exit 1
    fi
  fi
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
    print_info "Build board ${MILKV_BOARD} success!"
  else
    print_err "Build board ${MILKV_BOARD} failed!"
    exit 1
  fi
}

function milkv_pack_sd()
{
  pack_sd_image

  [ ! -d out ] && mkdir out

  img_in="${OUTPUT_DIR}/${MILKV_BOARD}.img"
  img_out="${MILKV_BOARD}-`date +%Y%m%d-%H%M`.img"

  if [ -f "${img_in}" ]; then
    mv ${img_in} out/${img_out}
    print_info "Create SD image successful: out/${img_out}"
  else
    print_err "Create SD image failed!"
    exit 1
  fi
}

function milkv_pack_emmc()
{
  [ ! -d out ] && mkdir out

  img_in="${OUTPUT_DIR}/upgrade.zip"
  img_out="${MILKV_BOARD}-`date +%Y%m%d-%H%M`.zip"

  if [ -f "${img_in}" ]; then
    mv ${img_in} out/${img_out}
    print_info "Create eMMC image successful: out/${img_out}"
  else
    print_err "Create eMMC image failed!"
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
    print_info "Create spinor/nand img successful: ${img_out_patch}"
  else
    print_err "Create spinor/nand img failed!"
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

function build_info()
{
  print_info "Target Board: ${MILKV_BOARD}"
  print_info "Target Board Storage: ${STORAGE_TYPE}"
  print_info "Target Board Config: ${MILKV_BOARD_CONFIG}"
  if [ "${STORAGE_TYPE}" == "sd" ]; then
    print_info "Target Image Config: ${MILKV_IMAGE_CONFIG}"
  fi
}

get_available_board

function build_usage()
{
  echo "Usage:"
  echo "${BASH_SOURCE[0]}              - Show this menu"
  echo "${BASH_SOURCE[0]} lunch        - Select a board to build"
  echo "${BASH_SOURCE[0]} [board]      - Build [board] directly, supported boards as follows:"

  for board in "${MILKV_BOARD_ARRAY[@]}"; do
    print_info "$board"
  done
}

if [ $# -ge 1 ]; then
  if [ "$1" = "lunch" ]; then
    choose_board || exit 0
  else
    if [[ ${MILKV_BOARD_ARRAY[@]} =~ (^|[[:space:]])"${1}"($|[[:space:]]) ]]; then
      MILKV_BOARD=${1}
      #echo "$MILKV_BOARD"
    else
      print_err "${1} not supported!"
      echo "Available boards: [ ${MILKV_BOARD_ARRAY[@]} ]"
      exit 1
    fi
  fi
else
  build_usage && exit 0
fi

if [ -z "${MILKV_BOARD// }" ]; then
  print_err "No board specified!"
  exit 1
fi

MILKV_BOARD_CONFIG=device/${MILKV_BOARD}/boardconfig.sh

if [ ! -f ${MILKV_BOARD_CONFIG} ]; then
  print_err "${MILKV_BOARD_CONFIG} not found!"
  exit 1
fi

get_toolchain

export MILKV_BOARD="${MILKV_BOARD}"

prepare_env

build_info

milkv_build
milkv_pack
