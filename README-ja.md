[English](./README.md) | [简体中文](./README-zh.md) | 日本語

# 概要

Milk-V DuoはCV1800Bをベースにした超小型の組み込みプラットフォームです。LinuxとRTOSが実行可能で、プロフェッショナル、産業用ODM、AIoT、DIY、クリエイターに、高信頼で高費用対効果なプラットフォームを提供します。

## ハードウェア

- CPU: CVITEK CV1800B (C906@1Ghz + C906@700MHz)
- デュアルコアRV64 最大1GHz
- 64MB RAM
- アドオンボード経由での10/100Mbpsイーサネット

# SDKのディレクトリ構造

```text
├── build               // compilation scripts and board configs コンパイルスクリプトとボード設定
├── build_milkv.sh      // one-click compilation script 自動コンパイルスクリプト
├── buildroot-2021.05   // buildroot source code Buildrootのソースコード
├── freertos            // freertos system freeRTOSのシステム
├── fsbl                // fsbl firmware in prebuilt form 完成済みfsblファームウェア
├── install             // temporary images stored here 一時イメージの仮置き場
├── isp_tuning          // camera effect parameters カメラ効果パラメータ
├── linux_5.10          // linux kernel Linuxカーネル
├── middleware          // self-developed multimedia framework 自家製マルチメディアフレームワーク
├── milkv               // configuration files for milkv Milk-Vのコンフィグレーションファイル
├── opensbi             // opensbi library opensbiライブラリ
├── out                 // final image for SD card 完成したSDカード用イメージはここに出てきます
├── ramdisk             // prebuilt ramdisk 完成済みramsidk
└── u-boot-2021.10      // u-boot source code u-bootのソースコード
```

# クイックスタート

## コンパイル環境の準備

- ローカルのUbuntuを使う場合`Ubuntu 20.04 LTS`をお勧めします。
  <br>
  (仮想マシンにインストールされたUbuntuやWSLやUbuntuのDockerも使えます。)
- `mobaXterm`とか`Xshell`のようなシリアルポートツールをインストールしてください

### Ubuntu 20.04 LTSに必要なツール

依存関係をコンパイルするためにインストールするもの

```bash
sudo apt install -y pkg-config build-essential ninja-build automake autoconf libtool wget curl git gcc libssl-dev bc slib squashfs-tools android-sdk-libsparse-utils jq python3-distutils scons parallel tree python3-dev python3-pip device-tree-compiler ssh cpio fakeroot libncurses5 flex bison libncurses5-dev genext2fs rsync unzip dosfstools mtools tclsh ssh-client android-sdk-ext4-utils
```

注意: `cmake`は最低でも`3.16.5`以降が必要です。

システムにインストールされている`cmake`のバージョンを確認してください

```bash
cmake --version
```

`Ubuntu 20.04`のaptでデフォルトでインストールされている`cmake`のバージョンは

```text
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

### Ubuntu 22.04 LTSに必要なツール

依存関係をコンパイルするためにインストールするもの

```bash
sudo apt install -y pkg-config build-essential ninja-build automake autoconf libtool wget curl git gcc libssl-dev bc slib squashfs-tools android-sdk-libsparse-utils jq python3-distutils scons parallel tree python3-dev python3-pip device-tree-compiler ssh cpio fakeroot libncurses5 flex bison libncurses5-dev genext2fs rsync unzip dosfstools mtools tcl openssh-client cmake
```

SDKのmkimageコマンドは`libssl1.1`に依存しています。`dpkg -s libssl1.1`でインストールされているか確認してください

インストールされていない場合、2通りの方法でインストールできます。

1. 追加のリポジトリからインストール

   ```bash
   echo "deb http://security.ubuntu.com/ubuntu focal-security main" | sudo tee /etc/apt/sources.list.d/focal-security.list
   sudo apt update
   sudo apt install libssl1.1
   ```

2. debパッケージを手動でダウンロードしてインストール

   ```bash
   wget http://archive.ubuntu.com/ubuntu/pool/main/o/openssl/libssl1.1_1.1.0g-2ubuntu4_amd64.deb
   sudo dpkg -i libssl1.1_1.1.0g-2ubuntu4_amd64.deb
   ```

## イメージのコンパイル

### SDKのソースコードをダウンロード

```bash
git clone https://github.com/milkv-duo/duo-buildroot-sdk.git --depth=1
```

### 自動コンパイル

- 自動コンパイルスクリプト`build_milkv.sh`を実行

```
cd duo-buildroot-sdk/
./build_milkv.sh
```

- 正常にコンパイルされるとSDカード用イメージ`milkv-duo-*-*.img`が`out`ディレクトリの中に出てきます。

*注意：最初のコンパイル時に必要なツールチェーン(およそ840MB)が自動でダウンロードされます。一度ダウンロードされると`host-tools`内に自動で展開されます。以後のコンパイルでは`host-tools`ディレクトリがある場合再びダウンロードはされません。*

### 手動コンパイル

手動コンパイルをしたい場合は以下のコマンドを順に実行します。

```bash
export MILKV_BOARD=milkv-duo
source milkv/boardconfig-milkv-duo.sh

source build/milkvsetup.sh
defconfig cv1800b_milkv_duo_sd
clean_all
build_all
pack_sd_image
```

生成されたイメージは`install/soc_cv1800b_milkv_duo_sd/milkv-duo.img`に出てきます。

## SDカードへの書き込み

> 注意：書き込むとmicroSDカード内のデータはすべて削除されます。重要なデータはバックアップしてください！！！
- Windows上で生成したイメージを書き込む場合`balenaEtcher`や`Rufus`、`Win32 Disk Imager`などのツールを使えます。
- Linux上で生成したイメージを書き込む場合ddコマンドを使えます。 **ddコマンドで書き込む場合指定するデバイスが書き込むmicroSDカードであることを十二分に確認してください**

  ```
  sudo dd if=milkv-duo-*-*.img of=/dev/sdX
  ```

## 電源投入

- Milk-V DuoのmicroSDカードスロットにmicroSDカードを挿入します。
- シリアルケーブルを接続します。(必ずしも必要ではありません)
- 電源に接続するとシステムが立ち上がります。
- シリアルケーブルが接続されている場合、ブートログをシリアルコンソールから見ることができます。システムが立ち上がればコンソールからログインしてLinuxコマンドを実行できます。

### Duoのターミナルに入る方法

- シリアルケーブルを使う
- USBネットワークを使う(RNDIS)
- イーサネットインターフェースを使う(アドオンボードが必要)

Duoのターミナルに入る際に必要なユーザー名とパスワードは以下の通りです。

```text
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

それからボードを再起動してください。

IO-Board上の4発のUSBポートを有効化する：

```bash
rm /mnt/system/usb.sh
ln -s /mnt/system/usb-host.sh /mnt/system/usb.sh
sync
```

それからボードを再起動してください。

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

USBフラッシュドライブをアンマウントするコマンド

```bash
umount /mnt/udisk
```

USBネットワーク(RNDIS)の機能をIO-Board不使用時に使う場合は以下のコマンドを実行してください

```bash
rm /mnt/system/usb.sh
ln -s /mnt/system/usb-rndis.sh /mnt/system/usb.sh
sync
```

それからボードを再起動してください。

# よくある質問

1. なぜ1つのコアしか表示されないのですか。

   CV1800Bチップはデュアルコアですが、現在、Linuxシステムは1つのコアで実行され、もう1つのコアはリアルタイムシステムの実行に使用されています。このコアのSDKはまだリリースされておらず、今後アップデートされる予定です。

2. なぜ28MBしかRAMが使えないのですか。
 
   RAMの一部が[ION](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/default/dts/cv180x/cv180x_default_memmap.dtsi#L15)に割り当てられており、カメラを使ってアルゴリズムを実行するときに使用するメモリだからです。カメラを使用しない場合は、この[ION_SIZE](https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/build/boards/cv180x/cv1800b_milkv_duo_sd/memmap.py#L43)の値を0に変更し、再コンパイルしてください。

## チップ製造元のドキュメント

- CV181x/CV180x MMF SDK開発ドキュメント (英語と中国語): [MultiMedia Framework Software Development Document](https://developer.sophgo.com/thread/471.html)
- CVシリーズのTPU SDK開発ドキュメント (中国語): [CV series chip TPU SDK development data summary](https://developer.sophgo.com/thread/473.html)

# Milk-Vについて

- [公式ウェブサイト](https://milkv.io/)

# フォーラム

- [MilkV Community](https://community.milkv.io/)
