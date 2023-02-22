# CVI_MP3PLAYER

## Briefing
This README.md files describe following information and instruction for compiling:
- The License claim of the 3rdparty lib and cvi_mp3_plyaer.c interface codes.
- The requirement and build steps to produce cvi_mp3player on your embedded platform.

## Introduction
cvi_mp3player exe bin file allow user to play mp3 files whilch encoded by the specification of MPEG-1 Audio Layer III/MPEG-2 Audio Lyaer III format. cvi_mp3_player provide and simple interface to use the 3rdparty library(MAD/libmad) to do the mp3 decode job. Once user build successfully the outcome:cvi_mp3player, it can be placed into the corresponding embedded board of same toolchain based system. User can play out the sound by command: cvi_mp3player [$filename].mp3.

User should keep in mind that 3rdparty agreement and license should totally comply by the user not by the Cvitek. All of the usage and build is responsible by user since Cvitek do not modify any code of MAD/libmad. User should follow the license rules and restriction of MAD/libmad.

## COPYRIGHT
All of the copyright should based on the MAD/libmad copyright proclamation first. The code are under restriction of distributed under the terms of the GNU General Public License (GPL) by whom apply it. Cvitek do not have any authority or license right for the MAD/libmad. Thus, Cvitek only public the interface sample code and do not responsible for any other owners'/venders' commercial usage. The user should take the full responsibility when using MAD/libmad.
Here's the link:https://github.com/markjeee/libmad/blob/master/COPYRIGHT.

## Requirement
To build out the cvi_mp3player, user should satisfy below condtion(files/libs):
-  libcvi_mp3.so
-  libmad.so (user should download open source and build it by oneself/ Check build 3rdparty script segment below)
-  cvi_mp3_player(.c file and .h file, already release in current folder)

## Build
After user export the enviornment and rewrite the toolchain path in Makefile. User can build out cvi_mp3player with previous libs.
example 1:
```sh
export SDK_VER=32bit
make clean
make
ls -la cvi_mp3player  // this is the output file
```
example 2:
```sh
export SDK_VER=uclibc
make clean
make
ls -la cvi_mp3player  // this is the output file
```
## Usage
Put the cvi_mp3player and other mp3 files to the embedded board. Execute by typing input command:./cvi_mp3player [$filename].mp3

## Build 3rdparty Script: MAD/libmad
- Download the libmad(version:0.15.1b) source code from web, user can find it in sourceforge:https://sourceforge.net/projects/mad/files/libmad/0.15.1b/ or other public link as https://github.com/markjeee/libmad
- Build the libmad by official make(configure) rule in libmad source code. User should setup correct cross compile toolchain that fit the corresponding embedded platform.
- User can reference the ./reference_script/build_xxx.sh to check how to build out libmad with compatible configure.


### What is MAD/libmad
MAD (libmad) is a high-quality MPEG audio decoder. It currently supports
  MPEG-1 and the MPEG-2 extension to Lower Sampling Frequencies, as well as
  the so-called MPEG 2.5 format. All three audio layers (Layer I, Layer II,
  and Layer III a.k.a. MP3) are fully implemented.
  MAD does not yet support MPEG-2 multichannel audio (although it should be
  backward compatible with such streams) nor does it currently support AAC.
  Because MAD is distributed under the terms of the GPL, its redistribution
  is not generally restricted, so long as the terms of the GPL are followed.
  This means MAD can be incorporated into other software as long as that
  software is also distributed under the GPL. (Should this be undesirable,
  alternate arrangements may be possible by contacting Underbit.)(More Details: https://www.underbit.com/products/mad/)


