# Project Description
Milk-V Duo is an ultra-compact embedded development platform based on the CV1800B chip. It can run Linux and RTOS, providing a reliable, low-cost and high-performance platform for professionals, industrial ODM manufacturers, AIoT enthusiasts, DIY enthusiasts and creators.
hardware parameters
Processor: CVITEK CV1800B (C906@1Ghz + C906@700MHz)
Memory: 64MB
Network port: 10/100Mbps Ethernet (external expansion board required)
```
SDK directory structure
├── build               // 编译目录，存放编译脚本以及各board差异化配置
├── build_milkv.sh      // Milk-V Duo 一键编译脚本
├── buildroot-2021.05   // buildroot开源工具
├── freertos            // freertos系统
├── fsbl                // fsbl启动固件，prebuilt形式存在
├── install             // 执行一次完整编译后，临时存放各image路径
├── isp_tuning          // 图像效果调试参数存放路径
├── linux_5.10          // 开源linux内核
├── middleware          // 自研多媒体框架，包含so与ko
├── milkv               // 存放 Milk-V Duo 相关配置及脚本文件的目录
├── opensbi             // 开源opensbi库
├── out                 // Milk-V Duo 最终生成的SD卡烧录镜像所在目录
├── ramdisk             // 存放最小文件系统的prebuilt目录
└── u-boot-2021.10      // 开源uboot代码
```
# Quick start
Prepare the compilation environment
Use the local Ubuntu system, recommended Ubuntu 20.04 LTS
(you can also use the Ubuntu system in the virtual machine, the Ubuntu installed in WSL in Windows, and the Ubuntu system based on Docker)
Install serial tool: mobarXtermor Xshellor other
Tools that need to be installed under Ubuntu 20.04 LTS
Install the tools that compile dependencies:
```
sudo apt install pkg-config build-essential ninja-build automake autoconf libtool wget curl git gcc libssl-dev bc slib squashfs-tools android-sdk-libsparse-utils jq python3-distutils scons parallel tree python3-dev python3-pip device-tree-compiler ssh cpio fakeroot libncurses5 flex bison libncurses5-dev genext2fs rsync unzip dosfstools mtools tclsh ssh-client android-sdk-ext4-utils
Note: cmakeminimum version requirements3.16.5
```
Check cmake version number in the system
```
cmake --version
```
The current Ubuntu 20.04version number of cmake installed with apt is

cmake version 3.16.3
Does not meet the minimum requirements of this SDK, you need to manually install the latest 3.26.4version
```
wget https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-linux-x86_64.sh
chmod +x cmake-3.26.4-linux-x86_64.sh
sudo sh cmake-3.26.4-linux-x86_64.sh --skip-license --prefix=/usr/local/
```
If you installed it manually cmake, /usr/local/binyou can use cmake --versionthe command to view its version number at this time, it should be

cmake version 3.26.4
Tools that need to be installed under Ubuntu 22.04 LTS
Install the tools that compile dependencies:
```
sudo apt install pkg-config build-essential ninja-build automake autoconf libtool wget curl git gcc libssl-dev bc slib squashfs-tools android-sdk-libsparse-utils jq python3-distutils scons parallel tree python3-dev python3-pip device-tree-compiler ssh cpio fakeroot libncurses5 flex bison libncurses5-dev genext2fs rsync unzip dosfstools mtools tcl openssh-client cmake
```
In addition, the libssl1.1 that the mkimage command in the SDK depends on does not exist in Ubuntu 22.04 and needs to be installed manually. The following two methods are available

# Source installation
```
echo "deb http://security.ubuntu.com/ubuntu focal-security main" | sudo tee /etc/apt/sources.list.d/focal-security.list
sudo apt update
sudo apt install libssl1.1
Manually download deb package installation
wget http://security.ubuntu.com/ubuntu/pool/main/o/openssl/libssl1.1_1.1.1-1ubuntu2.1~18.04.23_amd64.deb
sudo dpkg -i libssl1.1_1.1.1-1ubuntu2.1~18.04.23_amd64.deb
```

# Get the SDK
```
git clone https://github.com/milkv-duo/duo-buildroot-sdk.git
```
# One-click compilation
Execute one-click compilation scriptbuild_milkv.sh
```
cd duo-buildroot-sdk/
./build_milkv.sh
```
After the compilation is successful, you can outsee the generated SD card burning image in the directorymilkv-duo-XXX.img
Note: The first compilation will automatically download the required tool chain, which is about 840M in size. After downloading, it will be automatically decompressed to the directory under the SDK directory. When the next compilation detects that the existing directory host-toolsexists host-tools, it will not be downloaded again

If you need to compile step by step, you can enter the following commands in sequence
```
export MILKV_BOARD=milkv-duo
source milkv/boardconfig-milkv-duo.sh

source build/milkvsetup.sh
defconfig cv1800b_milkv_duo_sd
clean_all
build_all
pack_sd_image
Generated firmware location:install/soc_cv1800b_milkv_duo_sd/milkv-duo.img
```
# SD card burning
Note: Writing the image to the TF card will erase the original data in the card, remember to back up important data before burning!!!

balenaEtcherUse tools such as or Rufusor Win32 Disk Imagerto write the generated image to the TF card under Windows
Use commands under Linux ddto write the generated image into the TF card, please be sure to carefully confirm that ofthe device /dev/sdXis the TF card to be burned
```
sudo dd if=milkv-duo-XXX.img of=/dev/sdX
```
# Start up
Insert the TF card with burned image into the TF card slot of Milk-V Duo
Connect the serial cable (optional)
Power on the platform, Duo will boot into the system normally
If there is a serial port connection, you can see the boot log in the serial port tool. After entering the system, you can log in to the terminal through the serial port and execute related commands under Linux.
How to log in to the Duo terminal
via serial line
Through the USB network card (RNDIS) method
Through the Ethernet interface (requires expansion board support)
The username and password to log in to the Duo terminal are

root
milkv
Disable LED blinking
The LED will blink automatically after power-on. This is achieved through the boot script. If you need to disable the LED blinking function, execute in the Duo terminal:
```
mv /mnt/system/blink.sh /mnt/system/blink.sh_backup && sync
```
That is to change the name of the LED blinking script. After restarting the Duo, the LED will not blink.
If you need to restore the LED blinking, change its name back and restart it.
```
mv /mnt/system/blink.sh_backup /mnt/system/blink.sh && sync
```
# Frequently Asked Questions
Why is it only showing single core?

The CV1800B chip adopts a dual-core design. The current Linux system runs on one of the cores, and the other core is used to run the real-time system. The SDK of this core has not yet been released and will be updated later.

# Why does checking RAM only show 28M?

Because a part of RAM is allocated to ION , it is the memory that needs to be occupied when using the camera to run the algorithm. If you don't use the camera, you can modify the value of ION_SIZE0 and recompile to generate the firmware


# Links to some information from the original chip factory
CV181x/CV180x MMF SDK development document summary
https://developer.sophgo.com/thread/471.html

# CV series chip TPU SDK development data summary
https://developer.sophgo.com/thread/473.html


# About Milk-V
Official website

# Technology Forum
MilkV Community
