#!/bin/bash
set -e

TOP_DIR="$( cd "$(dirname "$0")" ; pwd -P )/cvitek"
TOOLCHAIN_FILE_PATH=$TOP_DIR/scripts/toolchain-aarch64-elf.cmake
BUILD_PATH=$TOP_DIR/build
INSTALL_PATH=$TOP_DIR/install
#RUN_TYPE=CVIRTOS or BLINKY_DEMO or FULL_DEMO or POSIX_DEMO
RUN_TYPE=CVIRTOS
RUN_CHIP=cv1835

echo "RUN TYPE: " $RUN_TYPE
# clean build and install folder for clean build
rm -rf $BUILD_PATH $INSTALL_PATH

if [ ! -e $BUILD_PATH/arch ]; then
    mkdir -p $BUILD_PATH/arch
fi
pushd $BUILD_PATH/arch
cmake -G Ninja -DCHIP=$RUN_CHIP  \
    -DTOP_DIR=$TOP_DIR \
    -DRUN_TYPE=$RUN_TYPE \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE_PATH \
    $TOP_DIR/arch
cmake --build . --target install -- -v
popd

if [ ! -e $BUILD_PATH/kernel ]; then
    mkdir -p $BUILD_PATH/kernel
fi
pushd $BUILD_PATH/kernel
cmake -G Ninja -DCHIP=$RUN_CHIP  \
    -DTOP_DIR=$TOP_DIR \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE_PATH \
    $TOP_DIR/kernel
cmake --build . --target install -- -v
popd


if [ ! -e $BUILD_PATH/common ]; then
    mkdir -p $BUILD_PATH/common
fi
pushd $BUILD_PATH/common
cmake -G Ninja -DCHIP=$RUN_CHIP  \
    -DTOP_DIR=$TOP_DIR \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE_PATH \
    $TOP_DIR/common
cmake --build . --target install -- -v
popd

if [ ! -e $BUILD_PATH/hal ]; then
    mkdir -p $BUILD_PATH/hal
fi
pushd $BUILD_PATH/hal/
cmake -G Ninja -DCHIP=$RUN_CHIP  \
    -DTOP_DIR=$TOP_DIR \
    -DRUN_TYPE=$RUN_TYPE \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE_PATH \
    $TOP_DIR/hal/$RUN_CHIP
cmake --build . --target install -- -v
popd

if [ ! -e $BUILD_PATH/driver ]; then
    mkdir -p $BUILD_PATH/driver
fi
pushd $BUILD_PATH/driver
cmake -G Ninja -DCHIP=$RUN_CHIP  \
    -DTOP_DIR=$TOP_DIR \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE_PATH \
    $TOP_DIR/driver
cmake --build . --target install -- -v
popd

if [ ! -e $BUILD_PATH/task ]; then
    mkdir -p $BUILD_PATH/task
fi
pushd $BUILD_PATH/task
cmake -G Ninja -DCHIP=$RUN_CHIP  \
    -DRUN_TYPE=$RUN_TYPE \
    -DTOP_DIR=$TOP_DIR \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE_PATH \
    $TOP_DIR/task
cmake --build . --target install -- -v
cmake --build . --target cvirtos.bin -- -v

popd

cp $TOP_DIR/install/bin/cvirtos.bin ./cvirtos.bin
