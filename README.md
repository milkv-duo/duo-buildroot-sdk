

简体中文 | [English](./README-en.md)

<br>

# 项目简介
- Milk-V Duo是一个基于CV1800B芯片的超紧凑嵌入式开发平台。它可以运行Linux和RTOS，为专业人士、工业ODM厂商、AIoT爱好者、DIY爱好者和创作者提供了一个可靠、低成本和高性能的平台。

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
- 使用本地的Ubuntu系统，推荐 `Ubuntu 20.04 LTS` (也可以使用虚拟机中的Ubuntu系统、Windows中WSL安装的Ubuntu、基于Docker的Ubuntu系统)
- 安装串口工具： `mobarXterm` 或者 `Xshell` 或者其他
- 安装编译依赖的工具:
```
sudo apt install pkg-config build-essential ninja-build automake autoconf libtool wget curl git gcc libssl-dev bc slib squashfs-tools android-sdk-libsparse-utils android-sdk-ext4-utils jq python3-distutils tclsh scons parallel ssh-client tree python3-dev python3-pip device-tree-compiler ssh cpio fakeroot libncurses5 flex bison libncurses5-dev genext2fs
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
sudo ./cmake-3.26.4-linux-x86_64.sh --skip-license --prefix=/usr/local/
```
手动安装的`cmake`在`/usr/local/bin`中，此时用`cmake --version`命令查看其版本号, 应为
```
cmake version 3.26.4
```

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

## SD卡烧录

> 注意: 将镜像写入TF卡会擦除卡中原有数据，记得在烧录前备份重要的数据!!!
- Window下使用`balenaEtcher`或者`Rufus`或者`Win32 Disk Imager`等工具将生成的镜像写入TF卡中
- Linux下使用`dd`命令将生成的镜像写入TF卡中，**请务必仔细确认`of`设备`/dev/sdX`为要烧录的TF卡**
  ```
  sudo dd if=milkv-duo-XXX.img of=/dev/sdX
  ```

## 开机
- 将烧录好镜像的TF卡插入 Milk-V Duo 的TF卡槽中
- 接好串口线
- 将平台上电，Duo会正常开机进入系统
- 串口工具中可以看到开机日志，进系统后可通过串口登入终端，执行Linux下的相关命令

<br>

# 常见问题解答

1. 为什么只显示单核?

   CV1800B芯片采用双核设计，当前Linux系统运行在其中的一个核上，另外一个核用来运行实时系统，这个核的SDK尚未公布，待后续更新

2. 为什么查看RAM只显示28M?

   因为有一部分RAM被分配绐了 [ION](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/default/dts/cv180x/cv180x_default_memmap.dtsi#L15)，您可以修改这个 [ION_SIZE](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/cv180x/cv1800b_milkv_duo_sd/memmap.py#L43) 的值然后重新编译生成固件.

<br>

# 关于 Milk-V

- [官方网站](https://milkv.io/)

<br>

# 技术论坛
- [MilkV Community](https://community.milkv.io/)


