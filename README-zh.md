[English](./README.md) | 简体中文 | [日本語](./README-ja.md)


# 项目简介

Milk-V Duo 是一个基于 CV1800B 芯片的超紧凑嵌入式开发平台。它可以运行 Linux 和RTOS，为专业人士、工业 ODM 厂商、AIoT 爱好者、DIY 爱好者和创作者提供了一个可靠、低成本和高性能的平台。

## 硬件参数

- 处理器: CVITEK CV1800B (C906@1Ghz + C906@700MHz)
- 内存: 64MB
- 网口: 10/100Mbps 以太网 (需外接扩展板)

# SDK目录结构

```
├── build               // 编译目录，存放编译脚本以及各board差异化配置
├── build.sh            // Milk-V Duo 一键编译脚本
├── buildroot-2021.05   // buildroot 开源工具
├── freertos            // freertos 系统
├── fsbl                // fsbl启动固件，prebuilt 形式存在
├── install             // 执行一次完整编译后，临时存放各 image 路径
├── isp_tuning          // 图像效果调试参数存放路径
├── linux_5.10          // 开源 linux 内核
├── middleware          // 自研多媒体框架，包含 so 与 ko
├── device              // 存放 Milk-V Duo 相关配置及脚本文件的目录
├── opensbi             // 开源 opensbi 库
├── out                 // Milk-V Duo 最终生成的 SD 卡烧录镜像所在目录
├── ramdisk             // 存放最小文件系统的 prebuilt 目录
└── u-boot-2021.10      // 开源 uboot 代码
```

# 快速开始

> [!TIP]
> 以下 SDK 的编译和使用方法，您也可以转到我们的 [官方文档](https://milkv.io/zh/docs/duo/getting-started/buildroot-sdk) 中查看，会有更好的阅读体验。另外，我们的官方文档网站也是开源的，如果您有兴趣来丰富文档的内容或者翻译成其他语言，可以在 [这个仓库](https://github.com/milk-v/milkv.io/) 中提交您的 PR，我们会不定期为贡献者赠送精美的礼物。

准备编译环境，使用本地的 Ubuntu 系统，官方支持的编译环境为 `Ubuntu Jammy 22.04.x amd64`。

如果您使用的是其他的 Linux 发行版，我们强烈建议您使用 Docker 环境来编译，以降低编译出错的概率。

以下分别介绍两种环境下的编译方法。

## 一、使用 Ubuntu 22.04 编译

### 安装编译依赖的工具包

```bash
sudo apt install -y pkg-config build-essential ninja-build automake autoconf libtool wget curl git gcc libssl-dev bc slib squashfs-tools android-sdk-libsparse-utils jq python3-distutils scons parallel tree python3-dev python3-pip device-tree-compiler ssh cpio fakeroot libncurses5 flex bison libncurses5-dev genext2fs rsync unzip dosfstools mtools tcl openssh-client cmake expect
```

### 获取 SDK

```bash
git clone https://github.com/milkv-duo/duo-buildroot-sdk.git --depth=1
```

### 1、一键编译

执行一键编译脚本 `build.sh`：
```bash
cd duo-buildroot-sdk/
./build.sh
```
会看到编译脚本的使用方法提示：
```bash
# ./build.sh
Usage:
./build.sh              - Show this menu
./build.sh lunch        - Select a board to build
./build.sh [board]      - Build [board] directly, supported boards asfollows:
milkv-duo
milkv-duo-lite
milkv-duo-spinand
milkv-duo-spinor
milkv-duo256m
milkv-duo256m-lite
```
最下边列出的是当前支持的目标版本列表，带 `lite` 后缀的为精简版，不包含 python，pip, pinpong 等库和应用包。带`spinor`或者`spinand` 后缀的为基于IOB板载NOR FLASH或者NAND FLASH的版本。

如提示中所示，有两种方法来编译目录版本。

第一种方法是执行 `./build.sh lunch` 调出交互菜单，选择要编译的版本序号，回车：
```bash
# ./build.sh lunch
Select a target to build:
1. milkv-duo
2. milkv-duo-lite
3. milkv-duo-spinand
4. milkv-duo-spinor
5. milkv-duo256m
6. milkv-duo256m-lite
7. milkv-duos
Which would you like:
```

第二种方法是脚本后面带上目标版本的名字，比如要编译 `milkv-duo` 的镜像:
```bash
# ./build.sh milkv-duo
```

编译成功后可以在 `out` 目录下看到生成的SD卡烧录镜像 `milkv-duo-*-*.img`，或者NOR FLASH/NAND FLASH 的烧录文件目录`milkv-duo-*-*`。

*注: 第一次编译会自动下载所需的工具链，大小为 840M 左右，下载完会自动解压到 SDK 目录下的 `host-tools` 目录，下次编译时检测到已存在 `host-tools` 目录，就不会再次下载了*

### 2、分步编译

如果未执行过一键编译脚本，需要先手动下载工具链 [host-tools](https://sophon-file.sophon.cn/sophon-prod-s3/drive/23/03/07/16/host-tools.tar.gz)，并解压到 SDK 根目录：

```bash
tar -xf host-tools.tar.gz -C /your/sdk/path/
```

再依次输入如下命令完成分步编译，命令中的 `[board]` 和 `[config]` 替换为需要编译的版本，当前支持的 `board` 和对应的 `config` 如下：
```
milkv-duo               cv1800b_milkv_duo_sd
milkv-duo-lite          cv1800b_milkv_duo_sd
milkv-duo-spinand       cv1800b_milkv_duo_spinand
milkv-duo-spinor        cv1800b_milkv_duo_spinor
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

比如需要编译 `milkv-duo` 的镜像，分步编译命令如下：
```bash
source device/milkv-duo/boardconfig.sh

source build/milkvsetup.sh
defconfig cv1800b_milkv_duo_sd
clean_all
build_all
pack_sd_image
```

生成的固件位置：
```
Duo:      	install/soc_cv1800b_milkv_duo_sd/[board].img
Duo(nor): 	install/soc_cv1800b_milkv_duo_sd/fip.bin、boot.spinor、rootfs.spinor
Duo(nand):	install/soc_cv1800b_milkv_duo_sd/fip.bin、boot.spinand、rootfs.spinand、system.spinand、cfg.spinand
Duo256M:  	install/soc_cv1812cp_milkv_duo256m_sd/[board].img
```

## 二、使用 Docker 编译

需要在运行 Linux 系统的主机上支持 Docker。 Docker 的使用方法请参考[官方文档](https://docs.docker.com/)或其他教程。

我们将 SDK 代码放在 Linux 主机系统上，调用 Milk-V 提供的 Docker 镜像环境来编译。

### 在 Linux 主机上拉 SDK 代码

```bash
git clone https://github.com/milkv-duo/duo-buildroot-sdk.git --depth=1
```

### 进入 SDK 代码目录

```bash
cd duo-buildroot-sdk
```

### 拉取 Docker 镜像并运行

```bash
docker run -itd --name duodocker -v $(pwd):/home/work milkvtech/milkv-duo:latest /bin/bash
```

命令中部分参数说明:
- `duodocker` docker 运行时名字，可以使用自己想用的名字
- `$(pwd)` 当前目录，这里是上一步 cd 到的 duo-buildroot-sdk 目录
- `-v $(pwd):/home/work`  将当前的代码目录绑定到 Docker 镜像里的 /home/work 目录
- `milkvtech/milkv-duo:latest` Milk-V 提供的 Docker 镜像，第一次会自动从 hub.docker.com 下载

Docker 运行成功后，可以用 `docker ps -a` 命令查看运行状态：
```bash
$ docker ps -a
CONTAINER ID   IMAGE                        COMMAND       CREATED       STATUS       PORTS     NAMES
8edea33c2239   milkvtech/milkv-duo:latest   "/bin/bash"   2 hours ago   Up 2 hours             duodocker
```

### 1. 使用 Docker 一键编译

```bash
docker exec -it duodocker /bin/bash -c "cd /home/work && cat /etc/issue && ./build.sh [board]"
```

注意命令最后的 `./build.sh [board]` 和前面在 Ubuntu 22.04 中一键编译说明中的用法是一样的，直接 `./build.sh` 可以查看命令的使用方法，用 `./build.sh lunch` 可以调出交互选择菜单，用 `./build.sh [board]` 可以直接编译目标版本，`[board]` 可以替换为:
```
milkv-duo
milkv-duo-lite
milkv-duo-spinand
milkv-duo-spinor
milkv-duo256m
milkv-duo256m-lite
```
*带 `lite` 后缀的版本为精简版，不包含 python，pip, pinpong 等库和应用包*

命令中部分参数说明:
- `duodocker` 运行的 Docker 名字, 与上一步中设置的名字要保持一致
- `"*"` 双引号中是要在 Docker 镜像中运行的 Shell 命令
- `cd /home/work` 是切换到 /home/work 目录，由于运行时已经将该目录绑定到主机的代码目录，所以在 Docker 中 /home/work 目录就是该 SDK 的源码目录
- `cat /etc/issue` 显示 Docker 使用的镜像的版本号，目前是 Ubuntu 22.04.3 LTS，调试用
- `./build.sh [board]` 执行一键编译脚本

比如需要编译 `milkv-duo` 的镜像，编译命令如下:
```bash
docker exec -it duodocker /bin/bash -c "cd /home/work && cat /etc/issue && ./build.sh milkv-duo"
```

编译成功后可以在 `out` 目录下看到生成的SD卡烧录镜像 `[board]-*-*.img`

### 2. 使用 Docker 分步编译

如果未执行过一键编译脚本，需要先手动下载工具链 [host-tools](https://sophon-file.sophon.cn/sophon-prod-s3/drive/23/03/07/16/host-tools.tar.gz)，并解压到 SDK 根目录：

```bash
tar -xf host-tools.tar.gz -C /your/sdk/path/
```

分步编译需要登陆到 Docker 中进行操作，用命令 `docker ps -a` 查看并记录容器的 ID 号，比如 8edea33c2239。

登陆到 Docker 中:
```bash
docker exec -it 8edea33c2239 /bin/bash
```

进入 Docker 中绑定的代码目录：
```bash
root@8edea33c2239:/# cd /home/work/
```

再依次输入如下命令完成分步编译，命令中的 `[board]` 和 `[config]` 替换为需要编译的版本，当前支持的 `board` 和对应的 `config` 如下：
```
milkv-duo               cv1800b_milkv_duo_sd
milkv-duo-lite          cv1800b_milkv_duo_sd
milkv-duo-spinand       cv1800b_milkv_duo_spinand
milkv-duo-spinor        cv1800b_milkv_duo_spinor
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

比如需要编译 `milkv-duo` 的镜像，分步编译命令如下：
```bash
source device/milkv-duo/boardconfig.sh

source build/milkvsetup.sh
defconfig cv1800b_milkv_duo_sd
clean_all
build_all
pack_sd_image
```

生成的固件位置：
```
Duo:      	install/soc_cv1800b_milkv_duo_sd/[board].img
Duo(nor): 	install/soc_cv1800b_milkv_duo_sd/fip.bin、boot.spinor、rootfs.spinor
Duo(nand):	install/soc_cv1800b_milkv_duo_sd/fip.bin、boot.spinand、rootfs.spinand、system.spinand、cfg.spinand
Duo256M:  	install/soc_cv1812cp_milkv_duo256m_sd/[board].img
```

编译完成后可以用 `exit` 命令退出 Docker 环境：
```bash
root@8edea33c2239:/home/work# exit
```
在主机代码目录中同样也可以看到生成的固件。

### 停用 Docker

编译完成后，如果不再需要以上的 Docker 运行环境，可先将其停止，再删除:
```bash
docker stop 8edea33c2239
docker rm 8edea33c2239
```

## 三、其他编译注意事项

如果您想尝试在以上两种环境之外的环境下编译本 SDK，下面是可能需要注意的事项，仅供参考。

### cmake 版本号

注意：`cmake` 版本最低要求 `3.16.5`

查看系统中 `cmake` 的版本号

```bash
cmake --version
```

比如在`Ubuntu 20.04`中用 apt 安装的 cmake 版本号为

```
cmake version 3.16.3
```

不满足此SDK最低要求，需要手动安装目前最新的 `3.27.6` 版本

```bash
wget https://github.com/Kitware/CMake/releases/download/v3.27.6/cmake-3.27.6-linux-x86_64.sh
chmod +x cmake-3.27.6-linux-x86_64.sh
sudo sh cmake-3.27.6-linux-x86_64.sh --skip-license --prefix=/usr/local/
```
手动安装的 `cmake` 在 `/usr/local/bin` 中，此时用 `cmake --version` 命令查看其版本号, 应为

```
cmake version 3.27.6
```

### 使用 Windows Linux 子系统 (WSL) 进行编译

如果您希望使用 WSL 执行编译，则构建镜像时会遇到一个小问题，WSL 中的 $PATH 具有 Windows 环境变量，其中路径之间包含一些空格。

要解决此问题，您需要更改 `/etc/wsl.conf` 文件并添加以下行：

```
[interop]
appendWindowsPath = false
```

然后需要使用 `wsl.exe --reboot` 重新启动 WSL。再运行 `./build.sh` 脚本或分步编译命令。

要恢复 `/etc/wsl.conf` 文件中的此更改，请将 `appendWindowsPath` 设置为 `true`。 要重新启动 WSL，您可以使用 Windows PowerShell 命令 `wsl.exe --shutdown`，然后使用`wsl.exe`，之后 Windows 环境变量在 $PATH 中再次可用。

## SD卡烧录

> 注意: 将镜像写入 TF 卡会擦除卡中原有数据，记得在烧录前备份重要的数据!!!
- Window 下使用 `balenaEtcher` 或者 `Rufus` 或者 `Win32 Disk Imager` 等工具将生成的镜像写入 TF 卡中
- Linux 下使用 `dd` 命令将生成的镜像写入 TF 卡中，**请务必仔细确认 `of` 设备 `/dev/sdX` 为要烧录的 TF 卡**
  ```bash
  sudo dd if=milkv-duo-XXX.img of=/dev/sdX
  ```
## IOB板FLASH烧录

- 需要IOB板FLASH位置处焊接上NOR或者NAND FLASH(需要用户后期自己动手)
- 准备一张没有烧录SD镜像的SD卡，将out下`milkv-duo-spinor-*-*`或者`milkv-duo-spinand-*-*`目录下全部文件拷贝至内存卡根目录
- 将拷贝好镜像的 TF 卡插入 Milk-V Duo 的 TF 卡槽中
- 接好串口线，可观察刻录进度
- 开机上电即开始刻录镜像到NOR或者NAND，等待uboot中自动刻录镜像完成
- 拔掉Milk-V Duo 的 TF 卡槽中的 TF 卡，重新上电即可从NOR或者NAND启动

## 开机

- 将烧录好镜像的 TF 卡插入 Milk-V Duo 的 TF 卡槽中
- 接好串口线(可选)
- 将平台上电，Duo 会正常开机进入系统
- 如有接串口线，在串口工具(`mobarXterm`, `Xshell` 或者其他)中可以看到开机日志，进系统后可通过串口登入终端，执行 Linux 下的相关命令

### 登陆到 Duo 终端的方法

- 通过串口线
- 通过 USB 网卡 (RNDIS) 方式
- 通过以太网接口(需要扩展板支持)

登陆 Duo 终端的用户名和密码分别为：

```
root
milkv
```

### 禁用LED闪烁

上电后 LED 会自动闪烁，这个是通过开机脚本实现的，如果需要禁用 LED 闪烁功能，在 Duo 的终端中执行:

```bash
mv /mnt/system/blink.sh /mnt/system/blink.sh_backup && sync
```
也就是将 LED 闪烁脚本改名，重启 Duo 后，LED 就不闪了。

如果需要恢复 LED 闪烁，再将其名字改回来，重启即可：
```bash
mv /mnt/system/blink.sh_backup /mnt/system/blink.sh && sync
```

### 使用 IO Board 底板

注意，使用 IO Board 底板时，USB 网卡 (RNDIS) 不可用，如需使用网络功能，请使用底板上的以太网接口。

使用底板上的 4 个 USB 口，需要修改一下配置，将默认固件中的 `usb-rndis` 功能修改为 `usb-host`：

```bash
ln -sf /mnt/system/usb-host.sh /mnt/system/usb.sh
sync
```
修改完，重启或重新上电即可生效。

比如底板 USB 口接入 U 盘后，可以用 `ls /dev/sd*` 查看是否有检测到设备。

挂载到系统中查看U盘中的内容(以 `/dev/sda1` 为例)：

```bash
mkdir /mnt/udisk
mount /dev/sda1 /mnt/udisk
```

查看 `/mnt/udisk` 目录中的内容是否符合预期：

```bash
ls /mnt/udisk
```

卸载U盘的命令：

```bash
umount /mnt/udisk
```

不使用底板时，恢复 USB 网卡 (RNDIS) 的方法：

```bash
ln -sf /mnt/system/usb-rndis.sh /mnt/system/usb.sh
sync
```

修改完，需要重启或重新上电生效。

# 常见问题解答

1. 为什么只显示单核?

   CV1800B 芯片采用双核设计，当前 Linux 系统运行在其中的一个核上，另外一个核用来运行实时系统，该核的使用请查看[官方文档](https://milkv.io/zh/docs/duo/getting-started/rtoscore)。

2. 为什么查看 RAM 只显示 28M?

   因为有一部分RAM被分配绐了 [ION](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/default/dts/cv180x/cv180x_default_memmap.dtsi#L15)，是在使用摄像头跑算法时需要占用的内存。如果不使用摄像头，您可以修改这个 [ION_SIZE](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/cv180x/cv1800b_milkv_duo_sd/memmap.py#L43) 的值为 `0` 然后重新编译生成固件(Duo 256M: [ION_SIZE](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/cv181x/cv1812cp_milkv_duo256m_sd/memmap.py#L43))。

## 芯片原厂一些资料的链接

- CV181x/CV180x MMF SDK 开发文档汇总 (英文和中文)：[CV181x/CV180x MMF SDK 开发文档汇总](https://developer.sophgo.com/thread/471.html)

- CV系列芯片 TPU SDK 开发资料汇总 (中文)：[CV系列芯片 TPU SDK 开发资料汇总](https://developer.sophgo.com/thread/473.html)

# 关于 Milk-V

- [官方网站](https://milkv.io/)

# 技术论坛

- [MilkV Community](https://community.milkv.io/)
