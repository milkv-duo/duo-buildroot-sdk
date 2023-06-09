#!/bin/bash

MILKV_BOARD_ARRAY=
MILKV_BOARD_ARRAY_LEN=
MILKV_BOARD=
MILKV_BOARD_CONFIG=

MILKV_BOARD_DIR=milkv


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
  MILKV_BOARD_ARRAY=( $(cd ${MILKV_BOARD_DIR}/; ls boardconfig*.sh | sort | awk -F"[-.]" -v OFS='-' '{print $2, $3}') )
  #echo ${MILKV_BOARD_ARRAY[@]}

  MILKV_BOARD_ARRAY_LEN=${#MILKV_BOARD_ARRAY[@]}
  if [ $MILKV_BOARD_ARRAY_LEN -eq 0 ]; then
    echo "No available board config"
    exit 1
  fi

  #echo ${MILKV_BOARD_ARRAY[@]} | xargs -n 1 | sed "=" | sed "N;s/\n/. /"
}

function choose_milkv_board()
{
  # TODO
  echo "choose"
}

function prepare_env()
{
  source ${MILKV_BOARD_CONFIG}

  source build/${MV_BUILD_ENV} > /dev/null 2>&1
  defconfig ${MV_BOARD_LINK} > /dev/null 2>&1
}

function milkv_duo_build()
{
  # clean old img
  install_dir="install/soc_${MV_BOARD_LINK}"
  old_image_count=`ls ${install_dir}/*.img* | wc -l`
  if [ ${old_image_count} -ge 0 ]; then
    pushd ${install_dir}
    rm -rf *.img*
    popd
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

function milkv_duo_pack()
{
  if [ "$(id -u)" -ne 0 ]; then
    print_info "Creating sd card img requires root privileges"
    sudo echo "Running with root privileges now"
  fi

  pack_sd_image

  [ ! -d out ] && mkdir out

  image_count=`ls ${install_dir}/*.img | wc -l`
  if [ ${image_count} -ge 0 ]; then
    mv ${install_dir}/*.img out/
    pushd out

    for img in sophpi*.img
    do
      #echo $img
      milkv_img=$(echo $img | sed -e 's/sophpi/milkv/')
      #echo $milkv_img
      if [ -f $img ]; then
        mv ${img} ${milkv_img}
        if [ $? -ne 0 ]; then
          print_err "Rename failed!"
          exit 1
        fi
      fi
    done

    popd

    # show latest img
    latest_img=`ls -t out/*.img | head -n1`
    if [ -z "${latest_img// }" ]; then
      print_err "Gen image failed!"
    else
      print_info "Gen image successful: ${latest_img}"
    fi
  else
    print_err "Create sd img failed!"
    exit 1
  fi
}

get_toolchain

get_available_board

if [ $MILKV_BOARD_ARRAY_LEN -eq 1 ]; then
  # Only one board
  print_info "Ready to build: ${MILKV_BOARD_ARRAY[0]}"
  MILKV_BOARD=${MILKV_BOARD_ARRAY[0]}
else
  # no arg
  if [ $# -lt 1 ]; then
    choose_milkv_board || exit 0
  fi

  # with board name
  if [ $# -ge 1 ]; then
    if [[ ${MILKV_BOARD_ARRAY[@]} =~ (^|[[:space:]])"${1}"($|[[:space:]]) ]]; then
      MILKV_BOARD=${1}
      echo "$MILKV_BOARD"
    else
      print_err "${1} not supported!"
      echo "Available boards: [ ${MILKV_BOARD_ARRAY[@]} ]"
      exit 1
    fi
  fi
fi

if [ -z "${MILKV_BOARD// }" ]; then
  print_err "No board specified!"
  exit 1
fi

MILKV_BOARD_CONFIG=${MILKV_BOARD_DIR}/boardconfig-${MILKV_BOARD}.sh

if [ ! -f ${MILKV_BOARD_CONFIG} ]; then
  print_err "${MILKV_BOARD_CONFIG} not found!"
  exit 1
fi

prepare_env

milkv_duo_build
milkv_duo_pack

