#!/bin/bash
set -e
 TOP_DIR="$( cd "$(dirname "$0")" ; pwd -P )"
 TOOLCHAIN_FILE_PATH=$TOP_DIR/scripts/toolchain-riscv64-elf.cmake
 BUILD_FREERTOS_PATH=$TOP_DIR/build
 BUILD_ENV_PATH=$BUILD_PATH
 INSTALL_PATH=$TOP_DIR/install
 RUN_TYPE=CVIRTOS
RUN_CHIP=cv181x
RUN_ARCH=riscv64
 if [ ! -e $BUILD_FREERTOS_PATH/task ]; then
 mkdir -p $BUILD_FREERTOS_PATH/task
 fi
 if [ -f $TOP_DIR/task/isp/libisp.a ]; then
 mkdir -p $BUILD_FREERTOS_PATH/task/isp
 cp $TOP_DIR/task/isp/libisp.a $BUILD_FREERTOS_PATH/task/isp
 cp $TOP_DIR/task/isp/libisp.a $INSTALL_PATH/lib/
 fi
 pushd $BUILD_FREERTOS_PATH/task
 cmake -G Ninja -DCHIP=$RUN_CHIP -DRUN_ARCH=$RUN_ARCH -DRUN_TYPE=$RUN_TYPE -DTOP_DIR=$TOP_DIR -DBUILD_ENV_PATH=$BUILD_ENV_PATH -DBOARD_FPGA=n -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE_PATH $TOP_DIR/task
 cmake --build . --target install -- -v
 cmake --build . --target cvirtos.bin -- -v
 popd
 cp $TOP_DIR/install/bin/cvirtos.bin ../cvirtos.bin
