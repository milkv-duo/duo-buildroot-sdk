#!/bin/bash
#
# Usage:
#    The common functions for envsetup_soc.sh
#
# Partition generation :
# pack_boot -> boot.emmc / boot.spinand (kernel+dtb)
# pack_rootfs -> rootfs.emmc / rootfs.spinand (squashFS rootfs)
# pack_system -> system.emmc / system.spinand (3rd/sdk shared libraries, spinand-ubifs/ emmc-ext4)
# pack_gpt -> gpt.img (emmc only)
# pack_cfg -> cfg.emmc / cfg.spinand (cofigs partition for saving configs and isp pq bin)
#
function print_error()
{
  printf "\e[1;31;47m\tERROR\t %s \e[0m\n" "$1"
}

function print_notice()
{
  printf "\e[1;34;47m %s \e[0m\n" "$1"
}

# $1 : The path for removing files
function remove_unused_files()
{
  delFiles=("*.a" "kdump" "*.la" "*DS_Store*" "include")
  for file in "${delFiles[@]}";do
    find "$1" -name "$file" -printf 'removing %p from rootfs\n' -prune -exec rm -rf {} \;
  done
}

function create_ramdisk_folder
{(
  CHIP_ARCH_LOWER=$(echo "${CHIP_ARCH}" | tr A-Z a-z)
  CUST_FOLDER_NAME="$PROJECT_FULLNAME"

  CHIP_FOLDER_PATH="$RAMDISK_PATH/rootfs/overlay/$CHIP"
  CUST_FOLDER_PATH="$RAMDISK_PATH/rootfs/overlay/$CUST_FOLDER_NAME"
  SDK_VER_FOLDER_PATH="$RAMDISK_PATH/rootfs/overlay/${CHIP_ARCH_LOWER}_${SDK_VER}"

  pushd "$BUILD_PATH"
  export RAMDISK_OUTPUT_BASE CHIP_FOLDER_PATH CUST_FOLDER_PATH SDK_VER_FOLDER_PATH
  make ramdisk || return "$?"
  popd

  # copy overlay/public folder to overlay/$CHIP
  local tmp_dir
  tmp_dir=$(mktemp -d)
  local ROOTFS_SCRIPT=
  if [ -e rootfs_"$PROJECT_FULLNAME".json ]; then
    ROOTFS_SCRIPT=rootfs_"$PROJECT_FULLNAME".json
  else
    ROOTFS_SCRIPT=rootfs_"${CHIP}_${STORAGE_TYPE}".json
  fi

  # create config folder with priority: project->chip
  if [ -d "$RAMDISK_PATH"/configs/"$CUST_FOLDER_NAME" ] ; then
    command cp "$RAMDISK_PATH"/configs/"$CUST_FOLDER_NAME"/* "$RAMDISK_PATH"/"$RAMDISK_OUTPUT_BASE"/configs
  fi
  if [ -d "$RAMDISK_PATH"/configs/"$CHIP"/"$SDK_VER" ]; then
    command cp "$RAMDISK_PATH"/configs/"$CHIP"/"$SDK_VER"/* "$RAMDISK_PATH"/"$RAMDISK_OUTPUT_BASE"/configs
  fi
  if [ -d "$RAMDISK_PATH"/configs/"$CHIP" ]; then
    command cp "$RAMDISK_PATH"/configs/"$CHIP"/* "$RAMDISK_PATH"/"$RAMDISK_OUTPUT_BASE"/configs
  fi
  if [ -d "$RAMDISK_PATH"/configs/"$CHIP_ARCH_LOWER" ]; then
    command cp "$RAMDISK_PATH"/configs/"$CHIP_ARCH_LOWER"/* "$RAMDISK_PATH"/"$RAMDISK_OUTPUT_BASE"/configs
  fi

  command cp -rf "$RAMDISK_PATH"/"$RAMDISK_OUTPUT_BASE"/target/system/* "$SYSTEM_OUT_DIR"
  command rm -rf "$RAMDISK_PATH"/"$RAMDISK_OUTPUT_BASE"/target/system
)}

function build_ramboot
{(
  print_notice "Run ${FUNCNAME[0]}() function"
  create_ramdisk_folder || return "$?"
  _build_kernel_env
  cd "$BUILD_PATH" || return
  make ramboot
)}

function pack_boot
{(
  print_notice "Run ${FUNCNAME[0]}() function"
  create_ramdisk_folder

  pushd "$RAMDISK_PATH"/"$RAMDISK_OUTPUT_FOLDER"

  _build_kernel_env
  pushd "$BUILD_PATH"
  make boot || return "$?"
  popd

  mkdir -p "$OUTPUT_DIR"/rawimages
  # Only pack header when SUBTYPE is asic to avoid storage_type is null
  if [[ ${BOARD} != "fpga" &&  ${BOARD} != "palladium" ]]; then
    command cp ./boot.itb "$OUTPUT_DIR"/rawimages/boot."$STORAGE_TYPE"
    python3 "$IMGTOOL_PATH"/raw2cimg.py "$OUTPUT_DIR"/rawimages/boot."$STORAGE_TYPE" "$OUTPUT_DIR" "$FLASH_PARTITION_XML"
  else
    command cp ./boot.itb "$OUTPUT_DIR"/boot.itb
  fi
  popd
)}

# $1 : The path for rootfs partition
function pack_rootfs
{(
   echo "pack_rootfs"
  print_notice "Run ${FUNCNAME[0]}_${STORAGE_TYPE}() function"

  CHIP_ARCH_LOWER=$(echo "${CHIP_ARCH}" | tr A-Z a-z)
  CUST_FOLDER_NAME="$PROJECT_FULLNAME"
  CHIP_FOLDER_PATH="$RAMDISK_PATH"/rootfs/overlay/"$CHIP"
  SDK_VER_FOLDER_PATH="$RAMDISK_PATH"/rootfs/overlay/"${CHIP_ARCH_LOWER}_${SDK_VER}"
  CUST_FOLDER_PATH="$RAMDISK_PATH"/rootfs/overlay/"$CUST_FOLDER_NAME"
  echo "CUST_FOLDER_NAME = ${CUST_FOLDER_NAME}"
  echo "CHIP_FOLDER_PATH = ${CHIP_FOLDER_PATH}"
  echo "SDK_VER_FOLDER_PATH = ${SDK_VER_FOLDER_PATH}"
  echo "CUST_FOLDER_PATH = ${CUST_FOLDER_PATH}"

  export ROOTFS_DIR COMMON_TOOLS_PATH FLASH_PARTITION_XML STORAGE_TYPE
  export CHIP_FOLDER_PATH SDK_VER_FOLDER_PATH CUST_FOLDER_PATH

  cd "$BUILD_PATH" || return
  make rootfs
)}

function pack_data
{(
  print_notice "Run ${FUNCNAME[0]}_${STORAGE_TYPE}() function"

  export ROOTFS_DIR COMMON_TOOLS_PATH FLASH_PARTITION_XML STORAGE_TYPE
  export CHIP_FOLDER_PATH SDK_VER_FOLDER_PATH CUST_FOLDER_PATH
  mkdir -p "$OUTPUT_DIR"/data
  pushd "$OUTPUT_DIR"/data;echo "If you can dream it, you can do it." > sample;popd
  cd "$BUILD_PATH" || return
  make jffs2
)}

function clean_rootfs
{(
  export ROOTFS_DIR STORAGE_TYPE

  cd "$BUILD_PATH" || return
  make rootfs-clean
)}

# $1 : The path for system partition
function pack_system
{(
  print_notice "Run ${FUNCNAME[0]}_${STORAGE_TYPE}() function"
  export TOOLS_PATH COMMON_TOOLS_PATH STORAGE_TYPE FLASH_PARTITION_XML

  cd "$BUILD_PATH" || return
  if [ "$STORAGE_TYPE" == "emmc" ] || [ "$STORAGE_TYPE" == "spinor" ] || [ "$STORAGE_TYPE" == "spinand" ]; then
    make system
  fi
)}

function pack_gpt
{(
  if [[ $STORAGE_TYPE != "emmc" ]]; then
    return 0
  fi
  print_notice "Run ${FUNCNAME[0]}() function"
  pushd "$EMMCTOOL_PATH"
  mkdir -p "$OUTPUT_DIR"/rawimages
  make gpt.img PARTITION_XML="$FLASH_PARTITION_XML" INSTALL_DIR="$OUTPUT_DIR"/rawimages
  python3 "$IMGTOOL_PATH"/raw2cimg.py "$OUTPUT_DIR"/rawimages/gpt.img "$OUTPUT_DIR" "$FLASH_PARTITION_XML"
  popd
)}

function pack_cfg
{(
  print_notice "Run ${FUNCNAME[0]}_${STORAGE_TYPE}() function"

  pushd "$ISP_TUNING_PATH"
  if [ $STORAGE_TYPE == "spinor" ]; then
    ./copyBin.sh "$OUTPUT_DIR"/rootfs/mnt/cfg/param/ "$SENSOR_TUNING_PARAM"
  else
    ./copyBin.sh "$OUTPUT_DIR"/rootfs/mnt/cfg/tmp_secure/ "$SENSOR_TUNING_PARAM"
  fi
  popd

  export TOOLS_PATH COMMON_TOOLS_PATH STORAGE_TYPE FLASH_PARTITION_XML ROOTFS_DIR

  cd "$BUILD_PATH" || return
  if [ $STORAGE_TYPE != "sd" ]; then
    make cfg
  fi
)}

function copy_tools
{(
  # Copy USB_DL, partition.xml and bootlogo
  if [[ "${chip_cv[*]}" =~ "$CHIP" ]] && [[ ${BOARD} != "fpga" &&  ${BOARD} != "palladium" ]]; then
    command rm -rf "$OUTPUT_DIR"/tools
    command mkdir -p "$OUTPUT_DIR"/tools/
    command cp -rf "$TOOLS_PATH"/"${CHIP_ARCH,,}"/usb_dl/ "$OUTPUT_DIR"/tools/
    if [ "$ENABLE_BOOTLOGO" -eq 1 ];then
      python3 "$IMGTOOL_PATH"/raw2cimg.py "$BOOTLOGO_PATH" "$OUTPUT_DIR" "$FLASH_PARTITION_XML"
    fi
    command cp --remove-destination "$FLASH_PARTITION_XML" "$OUTPUT_DIR"/
  fi
)}

function pack_upgrade
{(
  local TMPDIR
  TMPDIR=$(mktemp -d)
  local extra_files_args="-f utils $TOOLS_PATH/common/ota_tool/utils/cimg2raw"
  if [[ $STORAGE_TYPE == "spinand" ]]; then
    extra_files_args="$extra_files_args -f utils $TOOLS_PATH/common/ota_tool/utils/nandwrite"
  fi

  # Customer can customize
  {
  echo "Version=1"
  echo "STORAGE_TYPE=$STORAGE_TYPE"
  echo "CHIP=$CHIP"
  echo "BOARD=$BOARD"
  } >> "$TMPDIR"/misc_info.txt

  extra_files_args="$extra_files_args -f META $TMPDIR/misc_info.txt"
  # Generating default ota script.
  # Customer can customize thils script. Beware the script should be utils/update-script.sh
  if [[ $STORAGE_TYPE == "emmc" || $STORAGE_TYPE == "spinand" ]]; then
    python3 "$TOOLS_PATH"/common/ota_tool/gen_ota_script "$FLASH_PARTITION_XML" "$TMPDIR"
    extra_files_args="$extra_files_args -f utils $TMPDIR/update-script.sh"
  # Customize example
  # Rename original update-script.sh so we can invoke it later by our own wrapped one
  mv "$TMPDIR"/update-script.sh "$TMPDIR"/update-script.sh_
  extra_files_args=$extra_files_args"_"
  fi

  # Below is an example application to support panel display during updating
  for each in "$TOOLS_PATH"/common/ota_tool/utils/example/*
  do
  extra_files_args="$extra_files_args -f utils $each"
  done

  python3 "$IMGTOOL_PATH"/mk_package.py "$FLASH_PARTITION_XML" "$OUTPUT_DIR" -o "$OUTPUT_DIR"/upgrade.zip $extra_files_args
  command rm -rf "$TMPDIR"
)}

function pack_sd_image
{(
  pushd "$BUILD_PATH"
  make sd_image || return "$?"
  popd
)}

function pack_prog_img
{(
  local tmp_dir
  tmp_dir="$OUTPUT_DIR"/temp
  mkdir -p "$tmp_dir"
  rm -rf "${tmp_dir:?}/"*
  if [[ "$STORAGE_TYPE" = "spinand" ]]; then
    pushd "$SPINANDTOOL_PATH"/sv_tool
    make
    ./create_sv -c 5 -o "$tmp_dir"/sv.bin
    popd
    cp "$OUTPUT_DIR"/rawimages/*."$STORAGE_TYPE" "$tmp_dir"
    cp "$OUTPUT_DIR"/*.xml "$tmp_dir"
    cp "$OUTPUT_DIR"/fip.bin "$tmp_dir"
    # Tar images
    tar -caf "$OUTPUT_DIR"/prog_img.tar.gz -C "$tmp_dir" .
    # List images in tar
    tar -tzvf "$OUTPUT_DIR"/prog_img.tar.gz
  elif [[ "$STORAGE_TYPE" = "emmc" ]]; then
    cp "$OUTPUT_DIR"/fip.bin "$OUTPUT_DIR"/rawimages/
    python3 "$IMGTOOL_PATH"/pack_emmc_bin.py "$FLASH_PARTITION_XML" "$OUTPUT_DIR"/rawimages/ "$tmp_dir" -v
  fi
  # Tar images
  tar -caf "$OUTPUT_DIR"/prog_img.tar.gz -C "$tmp_dir" .
  # List images in tar
  tar -tzvf "$OUTPUT_DIR"/prog_img.tar.gz

  rm -rf "$tmp_dir"
)}

pathremove()
{
  local IFS=':'
  local NEWPATH
  local DIR
  local PATHVARIABLE=${2:-PATH}
  for DIR in ${!PATHVARIABLE} ; do
    if [ "$DIR" != "$1" ] ; then
      NEWPATH=${NEWPATH:+$NEWPATH:}$DIR
    fi
  done
  export $PATHVARIABLE="$NEWPATH"
}

pathprepend()
{
  pathremove $1 $2
  local PATHVARIABLE=${2:-PATH}
  export $PATHVARIABLE="$1${!PATHVARIABLE:+:${!PATHVARIABLE}}"
}

pathappend()
{
  pathremove $1 $2
  local PATHVARIABLE=${2:-PATH}
  export $PATHVARIABLE="${!PATHVARIABLE:+${!PATHVARIABLE}:}$1"
}

function _setconfig_complete()
{
    local cur prev

    COMPREPLY=()
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    cur="${COMP_WORDS[COMP_CWORD]}"

    if [[ -z "${cur}" ]]; then
        COMPREPLY=("${!_BUILD_KCONFIG_MAP[@]}")
    elif [[ "${cur}" =~ ^[nym]$ ]] && [[ "${prev}" = "=" ]]; then
        COMPREPLY=("${cur} ")
    else
      if [[ "${cur}" = "=" ]]; then
        case "${_BUILD_KCONFIG_MAP[${prev}=]}" in
        "<bool>")
          COMPREPLY=("y" "n")
          ;;
        "<tristate>")
          COMPREPLY=("y" "n" "m")
          ;;
        *)
          COMPREPLY=()
        esac
      else
        mapfile -t COMPREPLY < <(compgen -W "${_BUILD_KCONFIG_MAP_STR}" -- "${cur}")
      fi
    fi

    return 0
}

function _call_kconfig_script()
{
  local ret script

  script=$1
  shift

  print_notice "Run ${script} function"
  (
    set -e
    cd "$BUILD_PATH" || return
    "${BUILD_PATH}/scripts/${script}.py" "$@"
    "${BUILD_PATH}/scripts/savedefconfig.py" --out "${BUILD_PATH}/.defconfig"
  )
  ret=$?; if [ $ret -ne 0 ]; then return $ret; fi

  cvi_setup_env || return $?
  cvi_print_env
  (
    set -e
    cd "$BUILD_PATH" || return
    "${BUILD_PATH}/scripts/gencmakeconfig.py" "${BUILD_PATH}/.config"
  )
  ret=$?; if [ $ret -ne 0 ]; then return $ret; fi

  source "$BUILD_PATH/output/config_map.sh"
  _BUILD_KCONFIG_MAP_STR="${!_BUILD_KCONFIG_MAP[*]}"
  complete -o nospace -F _setconfig_complete setconfig
}

function oldconfig()
{
  _call_kconfig_script "${FUNCNAME[0]}"
}

function olddefconfig()
{
  _call_kconfig_script "${FUNCNAME[0]}"
}

function defconfig()
{
  local chip_arch
  local board

  board=$1
  chip_arch=`"${BUILD_PATH}/scripts/boards_scan.py" --get-chip-arch --board_name ${board}`

  # if input is chip series, then list boards by chip series
  if [ "${chip_arch}" != "" ]; then
    _call_kconfig_script "${FUNCNAME[0]}" "${BUILD_PATH}/boards/${chip_arch}/${board}/${board}_defconfig"
  else
    "${BUILD_PATH}/scripts/boards_scan.py" --list-boards ${board}
  fi
}

function menuconfig()
{
  _call_kconfig_script "${FUNCNAME[0]}"
}

function guiconfig()
{
  _call_kconfig_script "${FUNCNAME[0]}"
}

function setconfig()
{
  _call_kconfig_script "${FUNCNAME[0]}" "$@"
}

function _build_add_bash_completion()
{
  _boards=$(find "${BUILD_PATH}/boards" -mindepth 2 -maxdepth 2 -type d -not -path '*/default/*' -printf '%f ')
  complete -W "$_boards" defconfig
  complete -r setconfig 2> /dev/null || return 0
}

function _gen_build_env()
{
  local _tmp ret
  _tmp=$(python3 "${TOP_DIR}/build/scripts/boards_scan.py" --scan-boards-config)
  ret=$?
  [[ "$ret" == 0 ]] || return $ret
  # shellcheck disable=SC1090
  source <(echo "${_tmp}")
}

 _gen_build_env || return $?
 _build_add_bash_completion
