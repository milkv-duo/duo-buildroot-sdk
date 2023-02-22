#!/bin/sh

echo "start export------------------------------------"

export TOOLCHAIN_PATH=$PWD/../../../../../host-tools
export CROSS_COMPILE="$TOOLCHAIN_PATH"/gcc/arm-cvitek-linux-uclibcgnueabihf/bin/arm-cvitek-linux-uclibcgnueabihf-
export RAMDISK_PATH=$PWD/../../../../../ramdisk
SYSROOT=$RAMDISK_PATH/sysroot/sysroot-uclibc

export CFLGAS="-Wall -g  -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -fexpensive-optimizations \
		-fregmove -fschedule-insns2 -fstrength-reduce -std=gnu11 \
		-g -Wall -Wextra -fPIC -Os -ffunction-sections -fdata-sections -Wl,--gc-sections"
export CXXFLAGS="-std=gnu++11 -g -Wall -Wextra -fPIC -Os -ffunction-sections -fdata-sections -Wl,--gc-sections"
export LDFLAGS="-L$SYSROOT/usr/lib -Wl,--gc-sections"
export CC="${CROSS_COMPILE}gcc --sysroot=${SYSROOT}"
export CXX="${CROSS_COMPILE}g++ --sysroot=${SYSROOT}"
export AR="$CROSS_COMPILE"ar
export LD="$CROSS_COMPILE"ld

DIR=`pwd`
NPROC=`nproc`

echo "start configure ------------------------------"

mkdir build
cd build
$DIR/../libmad/configure --prefix=$DIR/../libmad/install --enable-static --enable-shared \
	--host=arm-cvitek-linux-uclibcgnueabihf --with-gnu-ld=arm-cvitek-linux-uclibcgnueabihf-ld \
	--build=arm

echo "start make------------------------------------"
make clean
make -j $NPROC
make install
mkdir -p  $DIR/../libmad_output/
cp -af $DIR/../libmad/install/* $DIR/../libmad_output/
cd ..
rm -rf build

echo "script finished!!!"
