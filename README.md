English | [简体中文](./README-zh.md)

# Project Introduction

Milk-V Duo is an ultra-compact embedded development platform based on the CV1800B chip. It can run Linux and RTOS, providing a reliable, low-cost, and high-performance platform for professionals, industrial ODMs, AIoT enthusiasts, DIY hobbyists, and creators.

## Hardware

- CPU: CVITEK CV1800B (C906@1Ghz + C906@700MHz)
- Dual RV64 Core up to 1GHz
- 64MB RAM
- Provides 10/100Mbps Ethernet via optional add-on board

# SDK Directory Structure

```text
├── build               // compilation scripts and board configs
├── build_milkv.sh      // one-click compilation script
├── buildroot-2021.05   // buildroot source code
├── freertos            // freertos system
├── fsbl                // fsbl firmware in prebuilt form
├── install             // temporary images stored here
├── isp_tuning          // camera effect parameters
├── linux_5.10          // linux kernel
├── middleware          // self-developed multimedia framework
├── milkv               // configuration files for milkv
├── opensbi             // opensbi library
├── out                 // final image for SD card
├── ramdisk             // prebuilt ramdisk
└── u-boot-2021.10      // u-boot source code
```

# Quick Start

## Prepare the Compilation Environment

- Using a local Ubuntu system, `Ubuntu 20.04 LTS` is recommended.
  <br>
  (You can also use Ubuntu installed in a virtual machine, Ubuntu installed through WSL on Windows, or Ubuntu-based systems using Docker)
- Install a serial port tool: `mobaXterm` or `Xshell` or others.

### Tools to be installed on Ubuntu 20.04 LTS

Install the tools that compile dependencies:

```bash
sudo apt install -y pkg-config build-essential ninja-build automake autoconf libtool wget curl git gcc libssl-dev bc slib squashfs-tools android-sdk-libsparse-utils jq python3-distutils scons parallel tree python3-dev python3-pip device-tree-compiler ssh cpio fakeroot libncurses5 flex bison libncurses5-dev genext2fs rsync unzip dosfstools mtools tclsh ssh-client android-sdk-ext4-utils
```

Note：`cmake` minimum version requirement is `3.16.5`

Check the version of `cmake` in the system

```bash
cmake --version
```

The version of `cmake` installed using apt in the current `Ubuntu 20.04` is

```text
cmake version 3.16.3
```

The minimum requirement of this SDK is not met. Manual installation of the latest version `3.27.6` is needed.

```bash
wget https://github.com/Kitware/CMake/releases/download/v3.27.6/cmake-3.27.6-linux-x86_64.sh
chmod +x cmake-3.27.6-linux-x86_64.sh
sudo sh cmake-3.27.6-linux-x86_64.sh --skip-license --prefix=/usr/local/
```

When manually installed, `cmake` is located in `/usr/local/bin`. To check its version, use the command `cmake --version`, which should display

```
cmake version 3.27.6
```

### Tools to be installed on Ubuntu 22.04 LTS

Install the tools that compile dependencies:

```bash
sudo apt install -y pkg-config build-essential ninja-build automake autoconf libtool wget curl git gcc libssl-dev bc slib squashfs-tools android-sdk-libsparse-utils jq python3-distutils scons parallel tree python3-dev python3-pip device-tree-compiler ssh cpio fakeroot libncurses5 flex bison libncurses5-dev genext2fs rsync unzip dosfstools mtools tcl openssh-client cmake
```
Additionally, the mkimage command in the SDK relies on `libssl1.1`. Sometimes the system won't contain that because it has been deprecated, use `dpkg -s libssl1.1` to check if you have installed it.

If not, the following two methods are both applicable:

1. Installation with additional repositories

   ```bash
   echo "deb http://security.ubuntu.com/ubuntu focal-security main" | sudo tee /etc/apt/sources.list.d/focal-security.list
   sudo apt update
   sudo apt install libssl1.1
   ```

2. Manual download and installation of the deb package

   ```bash
   wget http://security.ubuntu.com/ubuntu/pool/main/o/openssl/libssl1.1_1.1.1f-1ubuntu2.19_amd64.deb
   sudo dpkg -i libssl1.1_1.1.1f-1ubuntu2.19_amd64.deb
   ```
### Compiling with Windows Linux Subsystem (WSL)

If you wish to perform the compilation with WSL, there's an small issue building the image.
The $PATH, due Windows interoperability, has Windows environment variables which include some spaces between the paths.

To solve this problem you need to change the `/etc/wsl.conf` file and add the following lines:

```bash
[interop]
appendWindowsPath = false
```

After that, you need to reboot the WSL with `wsl.exe --reboot`. Then you able to run the `./build_milkv.sh` script or the `build_all` line in the step-by-step compilation method.
To rollback this change in `/etc/wsl.conf` file set `appendWindowsPath` as true.  To reboot the WSL, can you use the Windows PowerShell command `wsl.exe --shutdown` then `wsl.exe`, after that the Windows environment variables become avaliable again in $PATH.

## Compile the Image

### Get SDK Source Code

```bash
git clone https://github.com/milkv-duo/duo-buildroot-sdk.git --depth=1
```

### One-click Compilation

- Execute the one-click compilation script `build_milkv.sh`

```
cd duo-buildroot-sdk/
./build_milkv.sh
```

- After a successful compilation, you can find the generated SD card burning image `milkv-duo-*-*.img` in the `out` directory

*Note: The first compilation will automatically download the required toolchain, which is approximately 840MB in size. Once downloaded, it will be automatically extracted to the `host-tools` directory in the SDK directory. For subsequent compilations, if the `host-tools` directory is detected, the download will not be performed again*

### Step-by-step Compilation

If you wish to perform step-by-step compilation, you can enter the following commands sequentially

```bash
export MILKV_BOARD=milkv-duo
source milkv/boardconfig-milkv-duo.sh

source build/milkvsetup.sh
defconfig cv1800b_milkv_duo_sd
clean_all
build_all
pack_sd_image
```

Location of the generated image: `install/soc_cv1800b_milkv_duo_sd/milkv-duo.img`

## SD card burning

> Note: Writing the image to the microSD card will erase the existing data on the card. Remember to back up important data before burning!!!
- To write the generated image to a microSD card on Windows, you can use tools like `balenaEtcher`, `Rufus`, or `Win32 Disk Imager`
- To write the generated image to a microSD card on Linux, use the `dd` command. **Please make sure to carefully confirm that the `of` device `/dev/sdX` corresponds to the microSD card you want to burn**
  ```
  sudo dd if=milkv-duo-*-*.img of=/dev/sdX
  ```

## Power ON

- Insert the microSD card into the microSD card slot of the Milk-V Duo
- Connect the serial cable (optional)
- Power on, the Duo will boot up and enter the system normally
- If a serial cable is connected, you can view the boot logs in the serial console. After entering the system, you can use the serial console to log in to the terminal and execute relevant Linux commands

### The method to log in to the Duo terminal

- Using a serial cable
- Using a USB network (RNDIS)
- Using the Ethernet interface (requires the IO-Board)

The username and password for logging into the Duo terminal are as follows:

```text
root
milkv
```

### To disable LED blinking

If you want to disable the LED blinking feature on the Duo, you can execute the following command in the Duo terminal:

```bash
mv /mnt/system/blink.sh /mnt/system/blink.sh_backup && sync
```

This means renaming the LED blinking script, and after restarting the Duo, the LED will no longer blink.

If you want to restore LED blinking, rename it back to its original name and restart the device.

```bash
mv /mnt/system/blink.sh_backup /mnt/system/blink.sh && sync
```

### Using the IO-Board baseboard

Note that when using the IO-Board, the USB network (RNDIS) is not available, Please use the Ethernet interface on the IO-Board

If you need to assign a fixed MAC address to the Ethernet port of the IO-Board, please execute the following command(**Replace the MAC address in the command with the MAC address you want to set, and please note that MAC addresses of different devices within the same network segment must not be duplicated**)

```bash
echo "pre-up ifconfig eth0 hw ether 78:01:B3:FC:E8:55" >> /etc/network/interfaces
```

then reboot the board

Enable the 4 USB ports on the IO-Board:

```bash
rm /mnt/system/usb.sh
ln -s /mnt/system/usb-host.sh /mnt/system/usb.sh
sync
```

then reboot the board

For example, if a USB flash drive is connected to the USB port on the IO-Board, you can use the command `ls /dev/sd*` to check if the device is detected

To mount the USB drive and view its contents in the system (taking /dev/sda1 as an example):

```bash
mkdir /mnt/udisk
mount /dev/sda1 /mnt/udisk
```

Verify if the contents in the `/mnt/udisk` directory match the expectations

```bash
ls /mnt/udisk
```

The command to unmount a USB flash drive

```bash
umount /mnt/udisk
```

To restore the functionality of the USB network (RNDIS) when not using the IO-Board, you can follow these steps

```bash
rm /mnt/system/usb.sh
ln -s /mnt/system/usb-rndis.sh /mnt/system/usb.sh
sync
```

then reboot the board

# FAQs

1. Why is only a single core being displayed?

   The CV1800B chip adopts a dual-core design. Currently, the Linux system runs on one of the cores, while the other core is used for running a real-time system. The SDK for this core has not been released yet and will be updated in the future

2. Why does it only show 28M when viewing the RAM?
 
   Because a portion of the RAM is allocated to [ION](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/default/dts/cv180x/cv180x_default_memmap.dtsi#L15), which is the memory used when running algorithms with the camera. If you're not using the camera, you can modify the value of this [ION_SIZE](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/cv180x/cv1800b_milkv_duo_sd/memmap.py#L43) to 0 and then recompile to generate the image

## Links to some documentation from the chip manufacturer

- CV181x/CV180x MMF SDK Development Documents (Chinese): [MultiMedia Framework Software Development Document](https://developer.sophgo.com/thread/471.html)
- CV Series Chip TPU SDK Development Documentation Compilation (Chinese): [CV series chip TPU SDK development data summary](https://developer.sophgo.com/thread/473.html)

# About Milk-V

- [Official Website](https://milkv.io/)

# FORUM

- [MilkV Community](https://community.milkv.io/)
