#!/bin/bash

# milkv sd image generator

# usage
if [ "$#" -ne "1" ]; then
  echo "usage: ${0} OUTPUT_DIR"
  echo ""
  echo "       The script is used to create a sdcard image with two partitions, "
  echo "       one is fat32 with 128MB, the other is ext4 with 256MB."
  echo "       You can modify the capacities in genimage cfg as you wish!"
  echo "       genimage cfg: milkv/genimage-milkv-duo.cfg"
  echo ""
  echo "Note:  Please backup you sdcard files before using this image!"

  exit
fi

echo "BR_DIR: $BR_DIR"
echo "BR_BOARD: $BR_BOARD"

# genimage command in buildroot host bin
BR_HOST_BIN="${BR_DIR}/output/${BR_BOARD}/host/bin"
if [ ! -d ${BR_HOST_BIN} ]; then
  echo "host/bin not found, check buildroot output dir!"
  exit 1
fi

export PATH=${BR_HOST_BIN}:${PATH}

output_dir=$1
echo ${output_dir}
pushd ${output_dir}

[ -d tmp ] && rm -rf tmp

genimage --config ${TOP_DIR}/device/${MV_BOARD}/genimage.cfg --rootpath fs/ --inputpath ${PWD} --outputpath ${PWD}
if [ $? -eq 0 ]; then
    echo "gnimage for ${MV_BOARD} success!"
else
    echo "gnimage for ${MV_BOARD} failed!"
fi

popd
