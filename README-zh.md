

[English](./README.md) | 简体中文

<br>

# 项目简介
- Milk-V Duo是一个基于CV1800B芯片的超紧凑嵌入式开发平台。它可以运行Linux和RTOS，为专业人士、工业ODM厂商、AIoT爱好者、DIY爱好者和创作者提供了一个可靠、低成本和高性能的平台。

## 硬件参数
- 处理器: CVITEK CV1800B (C906@1Ghz + C906@700MHz)
- 内存: 64MB
- 网口: 10/100Mbps 以太网 (需外接扩展板)

<br>

# SDK目录结构
```
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

<br>

# 快速开始

## 准备编译环境
- 使用本地的Ubuntu系统，推荐 `Ubuntu 20.04 LTS`
  <br>
  (也可以使用虚拟机中的Ubuntu系统、Windows中WSL安装的Ubuntu、基于Docker的Ubuntu系统)
- 安装串口工具： `mobarXterm` 或者 `Xshell` 或者其他

### Ubuntu 20.04 LTS 下需要安装的工具
安装编译依赖的工具:
```
sudo apt install pkg-config build-essential ninja-build automake autoconf libtool wget curl git gcc libssl-dev bc slib squashfs-tools android-sdk-libsparse-utils jq python3-distutils scons parallel tree python3-dev python3-pip device-tree-compiler ssh cpio fakeroot libncurses5 flex bison libncurses5-dev genext2fs rsync unzip dosfstools mtools tclsh ssh-client android-sdk-ext4-utils
```
注意：`cmake` 版本最低要求 `3.16.5`

查看系统中 `cmake` 的版本号
```
cmake --version
```
当前`Ubuntu 20.04`中用apt安装的cmake版本号为
```
cmake version 3.16.3
```
不满足此SDK最低要求，需要手动安装目前最新的`3.26.4`版本
```
wget https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-linux-x86_64.sh
chmod +x cmake-3.26.4-linux-x86_64.sh
sudo sh cmake-3.26.4-linux-x86_64.sh --skip-license --prefix=/usr/local/
```
手动安装的`cmake`在`/usr/local/bin`中，此时用`cmake --version`命令查看其版本号, 应为
```
cmake version 3.26.4
```

### Ubuntu 22.04 LTS 下需要安装的工具

安装编译依赖的工具:
```
sudo apt install pkg-config build-essential ninja-build automake autoconf libtool wget curl git gcc libssl-dev bc slib squashfs-tools android-sdk-libsparse-utils jq python3-distutils scons parallel tree python3-dev python3-pip device-tree-compiler ssh cpio fakeroot libncurses5 flex bison libncurses5-dev genext2fs rsync unzip dosfstools mtools tcl openssh-client cmake
```

另外，SDK中的mkimage命令依赖的`libssl1.1`，在Ubuntu22.04中已不存在，需要手动安装，以下两种方法都可以

1. 补源安装
   ```
   echo "deb http://security.ubuntu.com/ubuntu focal-security main" | sudo tee /etc/apt/sources.list.d/focal-security.list
   sudo apt update
   sudo apt install libssl1.1
   ```
2. 手动下载deb包安装
   ```
   wget http://security.ubuntu.com/ubuntu/pool/main/o/openssl/libssl1.1_1.1.1-1ubuntu2.1~18.04.23_amd64.deb
   sudo dpkg -i libssl1.1_1.1.1-1ubuntu2.1~18.04.23_amd64.deb
   ```

<br>

## 获取SDK
```
git clone https://github.com/milkv-duo/duo-buildroot-sdk.git
```

## 一键编译
- 执行一键编译脚本`build_milkv.sh`
```
cd duo-buildroot-sdk/
./build_milkv.sh
```
- 编译成功后可以在`out`目录下看到生成的SD卡烧录镜像`milkv-duo-XXX.img`

*注: 第一次编译会自动下载所需的工具链，大小为840M左右，下载完会自动解压到SDK目录下的`host-tools`目录，下次编译时检测到已存在`host-tools`目录，就不会再次下载了*

如有需要分步编译，可依次输入如下命令
```
export MILKV_BOARD=milkv-duo
source milkv/boardconfig-milkv-duo.sh

source build/milkvsetup.sh
defconfig cv1800b_milkv_duo_sd
clean_all
build_all
pack_sd_image
```
生成的固件位置: `install/soc_cv1800b_milkv_duo_sd/milkv-duo.img`

## SD卡烧录

> 注意: 将镜像写入TF卡会擦除卡中原有数据，记得在烧录前备份重要的数据!!!
- Window下使用`balenaEtcher`或者`Rufus`或者`Win32 Disk Imager`等工具将生成的镜像写入TF卡中
- Linux下使用`dd`命令将生成的镜像写入TF卡中，**请务必仔细确认`of`设备`/dev/sdX`为要烧录的TF卡**
  ```
  sudo dd if=milkv-duo-XXX.img of=/dev/sdX
  ```

## 开机
- 将烧录好镜像的TF卡插入 Milk-V Duo 的TF卡槽中
- 接好串口线(可选)
- 将平台上电，Duo会正常开机进入系统
- 如有接串口线，在串口工具中可以看到开机日志，进系统后可通过串口登入终端，执行Linux下的相关命令

### 登陆到Duo终端的方法
- 通过串口线
- 通过USB网卡(RNDIS)方式
- 通过以太网接口(需要扩展板支持)

登陆Duo终端的用户名和密码分别为
```
root
milkv
```

### 禁用LED闪烁
上电后LED会自动闪烁，这个是通过开机脚本实现的，如果需要禁用LED闪烁功能，在Duo的终端中执行:
```
mv /mnt/system/blink.sh /mnt/system/blink.sh_backup && sync
```
也就是将LED闪烁脚本改名，重启Duo后，LED就不闪了
<br>
如果需要恢复LED闪烁，再将其名字改回来，重启即可
```
mv /mnt/system/blink.sh_backup /mnt/system/blink.sh && sync
```

### 使用 IO Board 底板

注意，使用 IO Board 底板时，USB网卡(RNDIS)不可用，如需使用网络功能，请使用底板上的以太网接口

使用底板上的4个USB口，需要修改一下配置，将默认固件中的`usb-rndis`功能修改为`usb-host`
```
rm /mnt/system/usb.sh
ln -s /mnt/system/usb-host.sh /mnt/system/usb.sh
sync
```
修改完，重启或重新上电即可生效

比如底板USB口接入U盘后，可以用`ls /dev/sd*`查看是否有检测到设备

挂载到系统中查看U盘中的内容(以/dev/sda1为例):
```
mkdir /mnt/udisk
mount /dev/sda1 /mnt/udisk
```
查看`/mnt/udisk`目录中的内容是否符合预期
```
ls /mnt/udisk
```

卸载U盘的命令
```
umount /mnt/udisk
```

不使用底板时，恢复USB网卡(RNDIS)的方法
```
rm /mnt/system/usb.sh
ln -s /mnt/system/usb-rndis.sh /mnt/system/usb.sh
sync
```
修改完，需要重启或重新上电生效

<br>

# 常见问题解答

1. 为什么只显示单核?

   CV1800B芯片采用双核设计，当前Linux系统运行在其中的一个核上，另外一个核用来运行实时系统，这个核的SDK尚未公布，待后续更新

2. 为什么查看RAM只显示28M?

   因为有一部分RAM被分配绐了 [ION](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/default/dts/cv180x/cv180x_default_memmap.dtsi#L15)，是在使用摄像头跑算法时需要占用的内存。如果不使用摄像头，您可以修改这个 [ION_SIZE](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/cv180x/cv1800b_milkv_duo_sd/memmap.py#L43) 的值为`0`然后重新编译生成固件

<br>

## 芯片原厂一些资料的链接

- CV181x/CV180x MMF SDK 开发文档汇总
  <br>
  [https://developer.sophgo.com/thread/471.html](https://developer.sophgo.com/thread/471.html)

- CV系列芯片 TPU SDK 开发资料汇总
  <br>
  [https://developer.sophgo.com/thread/473.html](https://developer.sophgo.com/thread/473.html)


<br>

# 关于 Milk-V

- [官方网站](https://milkv.io/)

<br>

# 技术论坛
- [MilkV Community](https://community.milkv.io/)


