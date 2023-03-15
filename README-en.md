
[简体中文](./README.md) | English

<br>

# Table of Contents

- [Table of Contents](#table-of-contents)
- [Project Introduction](#project-introduction)
  - [Hardware Information](#hardware-information)
  - [SOC SPEC](#soc-spec)
- [SDK Directory Structure](#sdk-directory-structure)
- [Quick Start](#quick-start)
  - [Prepare the compilation environment](#prepare-the-compilation-environment)
  - [Get SDK source code](#get-sdk-source-code)
  - [Prepare cross-compilation tools](#prepare-cross-compilation-tools)
  - [compile](#compile)
  - [SD card burning](#sd-card-burning)
- [About SOPHGO](#about-sophgo)
  - [Related Efforts](#related-efforts)
- [FORUM](#forum)


# Project Introduction
- This is a release SDK repository for the [SOPHGO](https://www.sophgo.com/) `cv181x` and `cv180x` series of Edge chips.
- Mainly applicable to official EVB

<br>

## Hardware Information
- [《CV180xB EVB板硬件指南》](https://sophon-file.sophon.cn/sophon-prod-s3/drive/23/03/14/14/CV180xB_EVB%E6%9D%BF%E7%A1%AC%E4%BB%B6%E6%8C%87%E5%8D%97_V1.0.pdf)
- [《CV180xC EVB板硬件指南》](https://sophon-file.sophon.cn/sophon-prod-s3/drive/23/03/18/18/CV180xC_EVB%E6%9D%BF%E7%A1%AC%E4%BB%B6%E6%8C%87%E5%8D%97_V1.0.pdf)
- [《CV181xC EVB板硬件指南》](https://sophon-file.sophon.cn/sophon-prod-s3/drive/23/03/15/14/CV181xC_QFN_EVB%E6%9D%BF%E7%A1%AC%E4%BB%B6%E6%8C%87%E5%8D%97_V1.0.pdf)
- [《CV181xH EVB板硬件指南》](https://sophon-file.sophon.cn/sophon-prod-s3/drive/23/03/15/15/CV181xH_EVB%E6%9D%BF%E7%A1%AC%E4%BB%B6%E6%8C%87%E5%8D%97_V1.0.pdf)

<br>

## SOC SPEC
- [SOC Product Brief](https://www.sophgo.com/product/index.html)

<br><br>

# SDK Directory Structure
```
.
├── build               // 编译目录，存放编译脚本以及各board差异化配置
├── buildroot-2021.05   // buildroot开源工具
├── freertos            // freertos系统
├── fsbl                // fsbl启动固件，prebuilt形式存在
├── install             // 执行一次完整编译后，各image的存放路径
├── isp_tuning          // 图像效果调试参数存放路径
├── linux_5.10          // 开源linux内核
├── middleware          // 自研多媒体框架，包含so与ko
├── opensbi             // 开源opensbi库
├── ramdisk             // 存放最小文件系统的prebuilt目录
└── u-boot-2021.10      // 开源uboot代码
```

<br><br>

# Quick Start

## Prepare the compilation environment
1. Install a virtual machine, or use a local ubuntu system, recommend`Ubuntu 20.04 LTS`
2. Install the serial port tool: `mobarXterm` or `xshell` or other tools.
3. Install the tools that compile dependencies.
```
sudo apt install pkg-config
sudo apt install build-essential
sudo apt install ninja-build
sudo apt install automake
sudo apt install autoconf
sudo apt install libtool
sudo apt install wget
sudo apt install curl
sudo apt install git
sudo apt install gcc
sudo apt install libssl-dev
sudo apt install bc
sudo apt install slib
sudo apt install squashfs-tools
sudo apt install android-sdk-libsparse-utils
sudo apt install android-sdk-ext4-utils
sudo apt install jq
sudo apt install cmake
sudo apt install python3-distutils
sudo apt install tclsh
sudo apt install scons
sudo apt install parallel
sudo apt install ssh-client
sudo apt install tree
sudo apt install python3-dev
sudo apt install python3-pip
sudo apt install device-tree-compiler
sudo apt install libssl-dev
sudo apt install ssh
sudo apt install cpio
sudo apt install squashfs-tools
sudo apt install fakeroot
sudo apt install libncurses5
sudo apt install flex
sudo apt install bison
```
- Note: The minimum version of cmake requires 3.16.5

## Get SDK source code
```
git clone https://github.com/sophgo/cvi_mmf_sdk.git
```

## Prepare cross-compilation tools

- Get the cross-compilation toolchain
```
wget https://sophon-file.sophon.cn/sophon-prod-s3/drive/23/03/07/16/host-tools.tar.gz
```
- Unpack the toolchain and link to the SDK directory
```
tar xvf host-tools.tar.gz
cd cvi_mmf_sdk/
ln -s ../host-tools ./
```

## compile
- Take `cv1812h_wevb_0007a_emmc` as an example
```
cd cvi_mmf_sdk/
source build/cvisetup.sh
defconfig cv1812h_wevb_0007a_emmc
build_all
```
- After the compilation is successful, you can see the generated image in the `install` directory

## SD card burning
- Connect the serial cable of the EVB board
- Format the SD card into FAT32 format
- Put the image in the `install` directory into the root directory of the SD card
```
.
├── boot.emmc
├── cfg.emmc
├── fip.bin
├── fw_payload_uboot.bin
├── rootfs.emmc
└── system.emmc
```
- Insert the SD card into the SD card slot
- Power on the platform again, and it will automatically enter the burning process when it is turned on. The burning process log is as follows:
```
Hit any key to stop autoboot:  0
## Resetting to default environment
Start SD downloading...
mmc1 : finished tuning, code:60
465408 bytes read in 11 ms (40.3 MiB/s)
mmc0 : finished tuning, code:27
switch to partitions #1, OK
mmc0(part 1) is current device

MMC write: dev # 0, block # 0, count 2048 ... 2048 blocks written: OK in 17 ms (58.8 MiB/s)

MMC write: dev # 0, block # 2048, count 2048 ... 2048 blocks written: OK in 14 ms (71.4 MiB/s)
Program fip.bin done
mmc0 : finished tuning, code:74
switch to partitions #0, OK
mmc0(part 0) is current device
64 bytes read in 3 ms (20.5 KiB/s)
Header Version:1
2755700 bytes read in 40 ms (65.7 MiB/s)

MMC write: dev # 0, block # 0, count 5383 ... 5383 blocks written: OK in 64 ms (41.1 MiB/s)
64 bytes read in 4 ms (15.6 KiB/s)
Header Version:1
13224 bytes read in 4 ms (3.2 MiB/s)

MMC write: dev # 0, block # 5760, count 26 ... 26 blocks written: OK in 2 ms (6.3 MiB/s)
64 bytes read in 4 ms (15.6 KiB/s)
Header Version:1
11059264 bytes read in 137 ms (77 MiB/s)

MMC write: dev # 0, block # 17664, count 21600 ... 21600 blocks written: OK in 253 ms (41.7 MiB/s)
64 bytes read in 3 ms (20.5 KiB/s)
Header Version:1
4919360 bytes read in 65 ms (72.2 MiB/s)

MMC write: dev # 0, block # 158976, count 9608 ... 9608 blocks written: OK in 110 ms (42.6 MiB/s)
64 bytes read in 4 ms (15.6 KiB/s)
Header Version:1
10203200 bytes read in 128 ms (76 MiB/s)

MMC write: dev # 0, block # 240896, count 19928 ... 19928 blocks written: OK in 228 ms (42.7 MiB/s)
Saving Environment to MMC... Writing to MMC(0)... OK
mars_c906#
```
- Burning is successful, unplug the SD card, power on the board again, and enter the system.

<br><br>

# About SOPHGO
** SOPHGO is committed to becoming the world's leading general computing power provider.<br>
SOPHGO focuses on the development and promotion of AI, RISC-V CPU and other computing products. With the self-developed chips as the core, SOPHGO has created a matrix of computing power products, which covers the whole scene of "cloud, edge and terminal" and provides computing power products and overall solutions for urban brains, intelligent computing centers, intelligent security, intelligent transportation, safety production, industrial quality inspection, intelligent terminals and others.SOPHGO has set up R&D centers in more than 10 cities and countries, including Beijing, Shanghai, Shenzhen, Qingdao, Xiamen, the United States and Singapore. **
- [Official Website](https://www.sophgo.com/)

<br><br>

## Related Efforts
- [sophpi-huashan](https://github.com/sophgo/sophpi-huashan)
- [sophpi-duo](https://github.com/sophgo/sophpi-duo)

# FORUM
- [Discussions - Open Hardware sophpi](https://developer.sophgo.com/forum/index/25/51.html)
