#!/bin/sh

echo "start export------------------------------------"

export TOOLCHAIN_PATH=$PWD/../../../../../host-tools
export CROSS_COMPILE="$TOOLCHAIN_PATH"/gcc/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
export RAMDISK_PATH=$PWD/../../../../../ramdisk
SYSROOT=$RAMDISK_PATH/sysroot/sysroot-glibc-linaro-2.23-2017.05-arm-linux-gnueabihf

export CFLGAS="-Wall -g  -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -fexpensive-optimizations \
		-fregmove -fschedule-insns2 -fstrength-reduce -std=gnu11 \
		-g -Wall -Wextra -fPIC -O3 -ffunction-sections -fdata-sections -Wl,-gc-sections"
export CXXFLAGS="-std=gnu++11 -g -Wall -Wextra -fPIC -O3 -ffunction-sections -fdata-sections -Wl,--gc-sections"
export LDFLAGS="-L$SYSROOT/usr/lib  -Wl,--gc-sections"
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
	--host=arm-linux-gnueabihf --with-gnu-ld=arm-linux-gnueabihf-ld  \
	--build=arm --enable-fpm=arm CFLAGS=$CFLAGS

echo "start make------------------------------------"
make clean
make -j $NPROC
make install
mkdir -p  $DIR/../libmad_output/
cp -af $DIR/../libmad/install/* $DIR/../libmad_output/
cd ..
rm -rf build

echo "script finished!!!"
