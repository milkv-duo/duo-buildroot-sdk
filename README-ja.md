[English](./README.md) | [简体中文](./README-zh.md) | 日本語

# 概要

Milk-V DuoはCV1800Bをベースにした超小型の組み込みプラットフォームです。LinuxとRTOSが実行可能で、プロフェッショナル、産業用ODM、AIoT、DIY、クリエイターに、高信頼で高費用対効果なプラットフォームを提供します。

## ハードウェア

- CPU: CVITEK CV1800B (C906@1Ghz + C906@700MHz)
- デュアルコアRV64 最大1GHz
- 64MB RAM
- アドオンボード経由での10/100Mbpsイーサネット

# SDKのディレクトリ構造

```
├── build               // コンパイルスクリプトとボード設定
├── build.sh            // 自動コンパイルスクリプト
├── buildroot-2021.05   // Buildrootのソースコード
├── freertos            // FreeRTOSのシステム
├── fsbl                // 完成済みfsblファームウェア
├── install             // 一時イメージの仮置き場
├── isp_tuning          // カメラ効果パラメータ
├── linux_5.10          // Linuxカーネル
├── middleware          // 自家製マルチメディアフレームワーク
├── device              // Milk-Vのコンフィグレーションファイル
├── opensbi             // opensbiライブラリ
├── out                 // 完成したSDカード用イメージはここに出てきます
├── ramdisk             // 完成済みramsidk
└── u-boot-2021.10      // u-bootのソースコード
```

# クイックスタート

> [!TIP]
> For the compilation and use methods of the SDK, you can also go to our [official documentation](https://milkv.io/docs/duo/getting-started/buildroot-sdk) for better information and eading experience. In addition, our official documentation website is also open source. If you are interested in enriching the content of the documentation or translating it into other languages, you can go to [this repository](https://github.com/milk-v/milkv.io/) submit your PR, and we will give out exquisite gifts to contributors from time to time.

コンパイル環境を準備する前に、ローカルのubuntuを使用する際にサポートされているのは`「Ubuntu Jammy 22.04.x amd64」`のみです。

他のLinuxディストロを使用している場合、問題を回避するためにDocker環境を使用することを強くお勧めします。

2つの環境でのコンパイル方法について解説します。

## 1. Ubuntu 22.04を使用してコンパイル

### インストールするパッケージ

コンパイルに必要なパッケージをインストールします。

```bash
sudo apt install -y pkg-config build-essential ninja-build automake autoconf libtool wget curl git gcc libssl-dev bc slib squashfs-tools android-sdk-libsparse-utils jq python3-distutils scons parallel tree python3-dev python3-pip device-tree-compiler ssh cpio fakeroot libncurses5 flex bison libncurses5-dev genext2fs rsync unzip dosfstools mtools tcl openssh-client cmake expect
```

### SDKのソースコードを入手

```bash
git clone https://github.com/milkv-duo/duo-buildroot-sdk.git --depth=1
```

### <1>. 自動コンパイル

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

正常にコンパイルされるとSDカード用イメージ`milkv-duo-*-*.img`が`out`ディレクトリの中に出てきます。

*注意：最初のコンパイル時に必要なツールチェーン(およそ840MB)が自動でダウンロードされます。一度ダウンロードされると`host-tools`内に自動で展開されます。以後のコンパイルでは`host-tools`ディレクトリがある場合再びダウンロードはされません。*

### <2>. 手動コンパイル

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

## 2. Dockerを使用してコンパイル

DockerはホストのLinuxシステムからサポートされている必要があります。Dockerの使い方については[公式ドキュメント](https://docs.docker.com/)とか他の使い方を見てください。

SDKのソースコードをLinuxホストシステムに置いて、Milk-Vから提供されているコンパイル用イメージをDockerで呼び出してコンパイルします。

### SDKのソースコードをホストに持ってくる

```bash
git clone https://github.com/milkv-duo/duo-buildroot-sdk.git --depth=1
```

### SDKのコードディレクトリに入る

```bash
cd duo-buildroot-sdk
```

### Dockerイメージを持ってきて実行する

```bash
docker run -itd --name duodocker -v $(pwd):/home/work milkvtech/milkv-duo:latest /bin/bash
```

コマンド中のパラメータについて:
- `duodocker` Dockerの名前です。好きな名前を使えます。
- `$(pwd)` カレントディレクトリです。ここでは先程cdしたduo-buildroot-sdkを指しています。
- `-v $(pwd):/home/work` 現在のコードディレクトリをDockerイメージの/home/workに結びつけます。
- `milkvtech/milkv-duo:latest` Milk-VによるDockerイメージです。最初にhub.docker.comから自動でダウンロードされます。

Dockerが正常に実行されたら`docker ps -a`コマンドで実行状態を見ることができます。
```bash
$ docker ps -a
CONTAINER ID   IMAGE                        COMMAND       CREATED       STATUS       PORTS     NAMES
8edea33c2239   milkvtech/milkv-duo:latest   "/bin/bash"   2 hours ago   Up 2 hours             duodocker
```

### <1>. Dockerを使用して自動コンパイル

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

コマンド中のパラメータについて:
- `duodocker` 実行中のDockerの名前です。先程設定したものと同じである必要があります。
- `"*"` クオートの中にDockerイメージ中で実行したいコマンドが入ります。
- `cd /home/work` /home/workディレクトリに移動します。このディレクトリは実行時にホストのコードディレクトリに紐付けられているため、Docker中の/home/workディレクトリはSDKのコードディレクトリになります。
- `cat /etc/issue` Dockerで実行されているイメージのバージョンを表示します。これはいまのところ「Ubuntu 22.04.3 LTS」で、デバッグに使われます。
- `./build.sh [board]` 自動コンパイルスクリプトを実行します。

For example, if you need to compile the image of `milkv-duo`, the command is as follows:
```bash
docker exec -it duodocker /bin/bash -c "cd /home/work && cat /etc/issue && ./build.sh milkv-duo"
```

コンパイルが成功すると、`out`ディレクトリの中に`milkv-duo-*-*.img`が出てきます。

### <2>. Dockerを使用して手動コンパイル

If you have not executed the one-click compilation script, you need to manually download the toolchain [host-tools](https://sophon-file.sophon.cn/sophon-prod-s3/drive/23/03/07/16/host-tools.tar.gz) and extract it to the SDK root directory:

```bash
tar -xf host-tools.tar.gz -C /your/sdk/path/
```

手動コンパイルをする場合はDockerにログインする必要があります。`docker ps -a`コマンドを使用して8edea33c2239みたいな形式のコンテナのIDを表示します。

Dockerに入る:
```bash
docker exec -it 8edea33c2239 /bin/bash
```

Dockerに紐付けられたコードディレクトリに入る：
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

コンパイルが完了したら`exit`コマンドでDockerから抜けれます:
```bash
root@8edea33c2239:/home/work# exit
```
生成されたイメージはホストのコードディレクトリからも見れます。

### Dockerを停止する

コンパイルが完了して、もし上のDocker環境がもう必要ないなら止めて削除できます:
```bash
docker stop 8edea33c2239
docker rm 8edea33c2239
```

## 3. その他の環境でのコンパイルに関する注意

もしこのSDKを上の2つの環境以外で行いたいなら、参考までに以下のことに注意してください。

### Cmakeのバージョン

注意：`cmake`は最低でもバージョン`3.16.5`以降が必要です。

システムの`cmake`バージョンの確認

```bash
cmake --version
```

例として、`Ubuntu 20.04`のaptでインストールされる`cmake`のバージョンは

```
cmake version 3.16.3
```
です。条件を満たしていないので手動で最新の`cmake`をインストールしてください

```bash
wget https://github.com/Kitware/CMake/releases/download/v3.27.6/cmake-3.27.6-linux-x86_64.sh
chmod +x cmake-3.27.6-linux-x86_64.sh
sudo sh cmake-3.27.6-linux-x86_64.sh --skip-license --prefix=/usr/local/
```

手動でインストールした`cmake`は`/usr/local/bin`に配置されます。`cmake --version`すると以下のように出力されるはずです。

```
cmake version 3.27.6
```

### Windows Subsystem for Linux(WSL)を使用してコンパイル

WSLでコンパイルするには少し問題があります。
互換性のために$PATHにいくつかのスペース文字を含むWindows用の環境変数が入っています。

解決するには`/etc/wsl.conf`ファイルに以下を追記します。

```
[interop]
appendWindowsPath = false
```

その後、`wsl.exe --reboot`でWSLを再起動する必要があります。そうすれば、自動コンパイルスクリプトを実行するか、手動コンパイルのを行うことができます。
変更を元に戻すには、`/etc/wsl.conf`の`appendWindowsPath`をtrueに設定します。Powershellから`wsl.exe --shutdown`を実行してから`wsl.exe`すれば、また$PATHからWindowsの環境変数が使えるようになります。

## SDカードへの書き込み

> 注意：書き込むとmicroSDカード内のデータはすべて削除されます。重要なデータはバックアップしてください！！！
- Windows上で生成したイメージを書き込む場合`balenaEtcher`や`Rufus`、`Win32 Disk Imager`などのツールを使えます。
- Linux上で生成したイメージを書き込む場合ddコマンドを使えます。 **ddコマンドで書き込む場合指定するデバイスが書き込むmicroSDカードであることを十二分に確認してください**

  ```bash
  sudo dd if=milkv-duo-*-*.img of=/dev/sdX
  ```

## 電源投入

- Milk-V DuoのmicroSDカードスロットにmicroSDカードを挿入します。
- シリアルケーブルを接続します。(必ずしも必要ではありません)
- 電源に接続するとシステムが立ち上がります。
- シリアルケーブルが接続されている場合、ブートログをシリアルコンソールから見ることができます(`mobarXterm`、`Xshell` など)。システムが立ち上がればコンソールからログインしてLinuxコマンドを実行できます。

### Duoのターミナルに入る方法

- シリアルケーブルを使う
- USBネットワークを使う(RNDIS)
- イーサネットインターフェースを使う(アドオンボードが必要)

Duoのターミナルに入る際に必要なユーザー名とパスワードは以下の通りです。

```
root
milkv
```

### LEDの点滅の無効化

もしDuoに載っているLEDの点滅を無効にしたいならDuoのターミナルで以下のコマンドを実行してください。

   ```bash
   mv /mnt/system/blink.sh /mnt/system/blink.sh_backup && sync
   ```

以上のコマンドはLEDの点滅スクリプトをリネームしています。そしてDuoを再起動するとLEDは点滅しなくなります。

DuoのLEDをまた点滅させたい場合、スクリプトのファイル名を元に戻します。

   ```bash
   mv /mnt/system/blink.sh_backup /mnt/system/blink.sh && sync
   ```

### IO-Boardを使う

IO-Boardを使用する場合、USBネットワーク(RNDIS)は使用できないので、IO-Boardのイーサネットインターフェースを使用してください。
IO-BoardのEthernetポートに固定MACアドレスを割り当てる必要がある場合は、以下のコマンドを実行してください(**コマンド中のMACアドレスは設定したいMACアドレスに置き換えてください。また、同一ネットワークセグメント内の異なるデバイスのMACアドレスは重複してはいけません**)。

   ```bash
   echo "pre-up ifconfig eth0 hw ether 78:01:B3:FC:E8:55" >> /etc/network/interfaces"
   ```
   - それからボードを再起動してください。

IO-Board上の4発のUSBポートを有効化する：

   ```bash
   ln -sf /mnt/system/usb-host.sh /mnt/system/usb.sh
   sync
   ```
   - それからボードを再起動してください。

たとえば、USBフラッシュドライブがIO-Board上のUSBポートに接続されている場合`ls /dev/sd*`で認識していることを確認できます。

USBフラッシュドライブをマウントしてその内容を見る(/dev/sda1を例に)。

   ```bash
   mkdir /mnt/udisk
   mount /dev/sda1 /mnt/udisk
   ```

`/mnt/udisk`ディレクトリの内容が予想と一致するか確認する。

   ```bash
   ls /mnt/udisk
   ```

USBフラッシュドライブをアンマウントするコマンド。

   ```bash
   umount /mnt/udisk
   ```

USBネットワーク(RNDIS)の機能をIO-Board不使用時に使う。

   ```bash
   ln -sf /mnt/system/usb-rndis.sh /mnt/system/usb.sh
   sync
   ```
   - それからボードを再起動してください。

# よくある質問

1. なぜ1つのコアしか表示されないのですか。

   CV1800Bチップはデュアルコアですが、現在、Linuxシステムは1つのコアで実行され、もう1つのコアはリアルタイムシステムの実行に使用されています。For the use of this core, please see [official documentation](https://milkv.io/docs/duo/getting-started/rtoscore)。

2. なぜ28MBしかRAMが使えないのですか。
 
   RAMの一部が[ION](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/default/dts/cv180x/cv180x_default_memmap.dtsi#L15)に割り当てられており、カメラを使ってアルゴリズムを実行するときに使用するメモリに割り当てられているからです。カメラを使用しない場合は、この[ION_SIZE](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/cv180x/cv1800b_milkv_duo_sd/memmap.py#L43)の値を0に変更し、再コンパイルしてください(Duo 256M: [ION_SIZE](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/cv181x/cv1812cp_milkv_duo256m_sd/memmap.py#L43))。

## チップ製造元のドキュメント

- CV181x/CV180x MMF SDK開発ドキュメント (英語と中国語): [MultiMedia Framework Software Development Document](https://developer.sophgo.com/thread/471.html)
- CVシリーズのTPU SDK開発ドキュメント (中国語): [CV series chip TPU SDK development data summary](https://developer.sophgo.com/thread/473.html)

# Milk-Vについて

- [公式ウェブサイト](https://milkv.io/)

# フォーラム

- [MilkV Community](https://community.milkv.io/)
