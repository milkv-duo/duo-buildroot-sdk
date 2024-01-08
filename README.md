English | [简体中文](./README-zh.md) | [日本語](./README-ja.md)

# Project Introduction

Milk-V Duo is an ultra-compact embedded development platform based on the CV1800B chip. It can run Linux and RTOS, providing a reliable, low-cost, and high-performance platform for professionals, industrial ODMs, AIoT enthusiasts, DIY hobbyists, and creators.

## Hardware

- CPU: CVITEK CV1800B (C906@1Ghz + C906@700MHz)
- Dual RV64 Core up to 1GHz
- 64MB RAM
- Provides 10/100Mbps Ethernet via optional add-on board

# SDK Directory Structure

```
├── build               // compilation scripts and board configs
├── build.sh            // one-click compilation script
├── buildroot-2021.05   // buildroot source code
├── freertos            // freertos system
├── fsbl                // fsbl firmware in prebuilt form
├── install             // temporary images stored here
├── isp_tuning          // camera effect parameters
├── linux_5.10          // linux kernel
├── middleware          // self-developed multimedia framework
├── device              // configuration files for milkv
├── opensbi             // opensbi library
├── out                 // final image for SD card
├── ramdisk             // prebuilt ramdisk
└── u-boot-2021.10      // u-boot source code
```

# Quick Start

> [!TIP]
> For the compilation and use methods of the SDK, you can also go to our [official documentation](https://milkv.io/docs/duo/getting-started/buildroot-sdk) for better information and eading experience. In addition, our official documentation website is also open source. If you are interested in enriching the content of the documentation or translating it into other languages, you can go to [this repository](https://github.com/milk-v/milkv.io/) submit your PR, and we will give out exquisite gifts to contributors from time to time.

Prepare the Compilation Environment. Using a local Ubuntu system, the officially supported compilation environment is `Ubuntu Jammy 22.04.x amd64` only!

If you are using other Linux distributions, we strongly recommend that you use the Docker environment to compile to reduce the probability of compilation errors.

The following describes the compilation methods in the two environments.

## 1. Compiled using Ubuntu 22.04

### Packages to be installed

Install the packages that compile dependencies:

```bash
sudo apt install -y pkg-config build-essential ninja-build automake autoconf libtool wget curl git gcc libssl-dev bc slib squashfs-tools android-sdk-libsparse-utils jq python3-distutils scons parallel tree python3-dev python3-pip device-tree-compiler ssh cpio fakeroot libncurses5 flex bison libncurses5-dev genext2fs rsync unzip dosfstools mtools tcl openssh-client cmake expect
```

### Get SDK Source Code

```bash
git clone https://github.com/milkv-duo/duo-buildroot-sdk.git --depth=1
```

### <1>. One-click Compilation

Execute one-click compilation script `build.sh`：
```bash
cd duo-buildroot-sdk/
./build.sh
```
You will see tips on how to use the compiled script:
```bash
# ./build.sh
Usage:
./build.sh              - Show this menu
./build.sh lunch        - Select a board to build
./build.sh [board]      - Build [board] directly, supported boards asfollows:
milkv-duo
milkv-duo-lite
milkv-duo256m
milkv-duo256m-lite
```
Listed at the bottom is the list of currently supported target versions. Those with the `lite` suffix is a simplified version and does not include libraries and application packages such as python, pip, pinpong, etc.

As shown in the prompt, there are two ways to compile the target version.

The first method is to execute `./build.sh lunch` to bring up the interactive menu, select the version number to be compiled, and press Enter:
```bash
# ./build.sh lunch
Select a target to build:
1. milkv-duo
2. milkv-duo-lite
3. milkv-duo256m
4. milkv-duo256m-lite
Which would you like:
```

The second method is to put the name of the target version after the script and compile it directly. For example, if you need to compile the image of `milkv-duo`, the command is as follows:
```bash
# ./build.sh milkv-duo
```

After a successful compilation, you can find the generated SD card burning image `milkv-duo-*-*.img` in the `out` directory.

*Note: The first compilation will automatically download the required toolchain, which is approximately 840MB in size. Once downloaded, it will be automatically extracted to the `host-tools` directory in the SDK directory. For subsequent compilations, if the `host-tools` directory is detected, the download will not be performed again*.

### <2>. Step-by-step Compilation

If you have not executed the one-click compilation script, you need to manually download the toolchain [host-tools](https://sophon-file.sophon.cn/sophon-prod-s3/drive/23/03/07/16/host-tools.tar.gz) and extract it to the SDK root directory:

```bash
tar -xf host-tools.tar.gz -C /your/sdk/path/
```

Then enter the following commands in sequence to complete the step-by-step compilation. Replace `[board]` and `[config]` in the command with the version that needs to be compiled. The currently supported `board` and corresponding `config` are as follows:
```
milkv-duo               cv1800b_milkv_duo_sd
milkv-duo-lite          cv1800b_milkv_duo_sd
milkv-duo256m           cv1812cp_milkv_duo256m_sd
milkv-duo256m-lite      cv1812cp_milkv_duo256m_sd
```

```bash
source device/[board]/boardconfig.sh

source build/milkvsetup.sh
defconfig [config]
clean_all
build_all
pack_sd_image
```

For example, if you need to compile the image of `milkv-duo`, the step-by-step compilation command is as follows:
```bash
source device/milkv-duo/boardconfig.sh

source build/milkvsetup.sh
defconfig cv1800b_milkv_duo_sd
clean_all
build_all
pack_sd_image
```

Generated firmware location:
```
Duo:      install/soc_cv1800b_milkv_duo_sd/[board].img
Duo256M:  install/soc_cv1812cp_milkv_duo256m_sd/[board].img
```

## 2. Compiled using Docker

Docker support is required on hosts running Linux systems. For how to use Docker, please refer to the [official documentation](https://docs.docker.com/) or other tutorials.

We put the SDK source code on the Linux host system and call the Docker image environment provided by Milk-V to compile it.

### Pull SDK code on Linux host

```bash
git clone https://github.com/milkv-duo/duo-buildroot-sdk.git --depth=1
```

### Enter the SDK code directory

```bash
cd duo-buildroot-sdk
```

### Pull the Docker image and run

```bash
docker run -itd --name duodocker -v $(pwd):/home/work milkvtech/milkv-duo:latest /bin/bash
```

Description of some parameters in the command:
- `duodocker` Docker name, you can use the name you want to use.
- `$(pwd)` The current directory, here is the duo-buildroot-sdk directory that was 'cd' to in the previous step.
- `-v $(pwd):/home/work`  Bind the current code directory to the /home/work directory in the Docker image.
- `milkvtech/milkv-duo:latest` The Docker image provided by Milk-V will be automatically downloaded from hub.docker.com for the first time.

After Docker runs successfully, you can use the `docker ps -a` command to view the running status:
```bash
$ docker ps -a
CONTAINER ID   IMAGE                        COMMAND       CREATED       STATUS       PORTS     NAMES
8edea33c2239   milkvtech/milkv-duo:latest   "/bin/bash"   2 hours ago   Up 2 hours             duodocker
```

### <1>. One-click compilation using Docker

```bash
docker exec -it duodocker /bin/bash -c "cd /home/work && cat /etc/issue && ./build.sh [board]"
```

Note that the `./build.sh [board]` at the end of the command is the same as the previous usage in the one-click compilation instructions in Ubuntu 22.04. Use `./build.sh` can see how to use the command, use `./ build.sh lunch` can bring up the interactive selection menu, use `./build.sh [board]` to directly compile the target version, `[board]` can be replaced with:
```
milkv-duo
milkv-duo-lite
milkv-duo256m
milkv-duo256m-lite
```
*Versions with the `lite` suffix is a simplified version and does not include libraries and application packages such as python, pip, pinpong, etc.*

Description of some parameters in the command:
- `duodocker` The name of the running Docker must be consistent with the name set in the previous step.
- `"*"` In quotes is the shell command to be run in the Docker image.
- `cd /home/work` Switch to the /home/work directory. Since this directory has been bound to the host's code directory during runtime, the /home/work directory in Docker is the source code directory of the SDK.
- `cat /etc/issue` Displays the version number of the image used by Docker. It is currently Ubuntu 22.04.3 LTS and is used for debugging.
- `./build.sh [board]` Execute one-click compilation script.

For example, if you need to compile the image of `milkv-duo`, the command is as follows:
```bash
docker exec -it duodocker /bin/bash -c "cd /home/work && cat /etc/issue && ./build.sh milkv-duo"
```

After successful compilation, you can see the generated SD card burning image `[board]-*-*.img` in the `out` directory.

### <2>. Compile step by step using Docker

If you have not executed the one-click compilation script, you need to manually download the toolchain [host-tools](https://sophon-file.sophon.cn/sophon-prod-s3/drive/23/03/07/16/host-tools.tar.gz) and extract it to the SDK root directory:

```bash
tar -xf host-tools.tar.gz -C /your/sdk/path/
```

Step-by-step compilation requires logging into Docker to operate. Use the command `docker ps -a` to view and record the ID number of the container, such as 8edea33c2239.

Enter Docker:
```bash
docker exec -it 8edea33c2239 /bin/bash
```

Enter the code directory bound in Docker：
```bash
root@8edea33c2239:/# cd /home/work/
```

Then enter the following commands in sequence to complete the step-by-step compilation. Replace `[board]` and `[config]` in the command with the version that needs to be compiled. The currently supported `board` and corresponding `config` are as follows:
```
milkv-duo               cv1800b_milkv_duo_sd
milkv-duo-lite          cv1800b_milkv_duo_sd
milkv-duo256m           cv1812cp_milkv_duo256m_sd
milkv-duo256m-lite      cv1812cp_milkv_duo256m_sd
```

```bash
source device/[board]/boardconfig.sh

source build/milkvsetup.sh
defconfig [config]
clean_all
build_all
pack_sd_image
```

For example, if you need to compile the image of `milkv-duo`, the step-by-step compilation command is as follows:
```bash
source device/milkv-duo/boardconfig.sh

source build/milkvsetup.sh
defconfig cv1800b_milkv_duo_sd
clean_all
build_all
pack_sd_image
```

Generated firmware location:
```
Duo:      install/soc_cv1800b_milkv_duo_sd/[board].img
Duo256M:  install/soc_cv1812cp_milkv_duo256m_sd/[board].img
```

Generated firmware location: `install/soc_cv1800b_milkv_duo_sd/milkv-duo.img`.

After compilation is completed, you can use the `exit` command to exit the Docker environment:
```bash
root@8edea33c2239:/home/work# exit
```
The generated firmware can also be seen in the host code directory.

### Stop Docker

After compilation is completed, if the above Docker running environment is no longer needed, you can stop it first and then delete it:
```bash
docker stop 8edea33c2239
docker rm 8edea33c2239
```

## 3. Other compilation considerations

If you want to try to compile this SDK in an environment other than the above two environments, the following are things you may need to pay attention to, for reference only.

### cmake version

Note：`cmake` minimum version requirement is `3.16.5`.

Check the version of `cmake` in the system:

```bash
cmake --version
```

For example, the version of `cmake` installed using apt in the `Ubuntu 20.04` is:

```
cmake version 3.16.3
```

The minimum requirement of this SDK is not met. Manual installation of the latest version `3.27.6` is needed:

```bash
wget https://github.com/Kitware/CMake/releases/download/v3.27.6/cmake-3.27.6-linux-x86_64.sh
chmod +x cmake-3.27.6-linux-x86_64.sh
sudo sh cmake-3.27.6-linux-x86_64.sh --skip-license --prefix=/usr/local/
```

When manually installed, `cmake` is located in `/usr/local/bin`. To check its version, use the command `cmake --version`, which should display:

```
cmake version 3.27.6
```

### Compiling with Windows Linux Subsystem (WSL)

If you wish to perform the compilation with WSL, there's an small issue building the image.
The $PATH, due Windows interoperability, has Windows environment variables which include some spaces between the paths.

To solve this problem you need to change the `/etc/wsl.conf` file and add the following lines:

```
[interop]
appendWindowsPath = false
```

After that, you need to reboot the WSL with `wsl.exe --reboot`. Then you able to run the `./build.sh` script or the `build_all` line in the step-by-step compilation method.
To rollback this change in `/etc/wsl.conf` file set `appendWindowsPath` as true. To reboot the WSL, can you use the Windows PowerShell command `wsl.exe --shutdown` then `wsl.exe`, after that the Windows environment variables become avaliable again in $PATH.

## SD card burning

> Note: Writing the image to the microSD card will erase the existing data on the card. Remember to back up important data before burning!!!
- To write the generated image to a microSD card on Windows, you can use tools like `balenaEtcher`, `Rufus`, or `Win32 Disk Imager`.
- To write the generated image to a microSD card on Linux, use the `dd` command. **Please make sure to carefully confirm that the `of` device `/dev/sdX` corresponds to the microSD card you want to burn**:
  ```bash
  sudo dd if=milkv-duo-*-*.img of=/dev/sdX
  ```

## Power ON

- Insert the microSD card into the microSD card slot of the Milk-V Duo.
- Connect the serial cable (optional).
- Power on, the Duo will boot up and enter the system normally.
- If a serial cable is connected, you can view the boot logs in the serial console (`mobarXterm`, `Xshell` or others). After entering the system, you can use the serial console to log in to the terminal and execute relevant Linux commands.

### The method to log in to the Duo terminal

- Using a serial cable.
- Using a USB network (RNDIS).
- Using the Ethernet interface (requires the IO-Board).

The username and password for logging into the Duo terminal are as follows:

```
root
milkv
```

### To disable LED blinking

If you want to disable the LED blinking feature on the Duo, you can execute the following command in the Duo terminal:

```bash
mv /mnt/system/blink.sh /mnt/system/blink.sh_backup && sync
```

This means renaming the LED blinking script, and after restarting the Duo, the LED will no longer blink.

If you want to restore LED blinking, rename it back to its original name and restart the device:

```bash
mv /mnt/system/blink.sh_backup /mnt/system/blink.sh && sync
```

### Using the IO-Board baseboard

Note that when using the IO-Board, the USB network (RNDIS) is not available, Please use the Ethernet interface on the IO-Board.

If you need to assign a fixed MAC address to the Ethernet port of the IO-Board, please execute the following command(**Replace the MAC address in the command with the MAC address you want to set, and please note that MAC addresses of different devices within the same network segment must not be duplicated**):

```bash
echo "pre-up ifconfig eth0 hw ether 78:01:B3:FC:E8:55" >> /etc/network/interfaces
```

then reboot the board.

Enable the 4 USB ports on the IO-Board:

```bash
ln -sf /mnt/system/usb-host.sh /mnt/system/usb.sh
sync
```

then reboot the board.

For example, if a USB flash drive is connected to the USB port on the IO-Board, you can use the command `ls /dev/sd*` to check if the device is detected.

To mount the USB drive and view its contents in the system (taking /dev/sda1 as an example):

```bash
mkdir /mnt/udisk
mount /dev/sda1 /mnt/udisk
```

Verify if the contents in the `/mnt/udisk` directory match the expectations:

```bash
ls /mnt/udisk
```

The command to unmount a USB flash drive:

```bash
umount /mnt/udisk
```

To restore the functionality of the USB network (RNDIS) when not using the IO-Board, you can follow these steps:

```bash
ln -sf /mnt/system/usb-rndis.sh /mnt/system/usb.sh
sync
```

then reboot the board.

# FAQs

1. Why is only a single core being displayed?

   The CV1800B chip adopts a dual-core design. Currently, the Linux system runs on one of the cores, while the other core is used for running a real-time system. For the use of this core, please see [official documentation](https://milkv.io/docs/duo/getting-started/rtoscore).

2. Why does it only show 28M when viewing the RAM?
 
   Because a portion of the RAM is allocated to [ION](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/default/dts/cv180x/cv180x_default_memmap.dtsi#L15), which is the memory used when running algorithms with the camera. If you're not using the camera, you can modify the value of this [ION_SIZE](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/cv180x/cv1800b_milkv_duo_sd/memmap.py#L43) to 0 and then recompile to generate the image(Duo 256M: [ION_SIZE](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/cv181x/cv1812cp_milkv_duo256m_sd/memmap.py#L43)).

## Links to some documentation from the chip manufacturer

- CV181x/CV180x MMF SDK Development Documents (English and Chinese): [MultiMedia Framework Software Development Document](https://developer.sophgo.com/thread/471.html).
- CV Series Chip TPU SDK Development Documentation Compilation (Chinese): [CV series chip TPU SDK development data summary](https://developer.sophgo.com/thread/473.html).

# About Milk-V

- [Official Website](https://milkv.io/)

# FORUM

- [MilkV Community](https://community.milkv.io/)
