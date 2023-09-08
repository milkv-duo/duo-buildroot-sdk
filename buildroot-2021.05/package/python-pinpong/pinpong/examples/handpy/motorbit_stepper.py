# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.extension.handpy import *
from pinpong.libs.microbit_motor import DFStepper

Board("handpy").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("handpy","COM36").begin()  #windows下指定端口初始化
#Board("handpy","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("handpy","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

M1_M2=DFStepper(1)

while True:
    M1_M2.circle(1,M1_M2.CW)
    sleep_ms(2000)
    M1_M2.stop()
    sleep_ms(2000)
    M1_M2.circle(1,M1_M2.CCW)
    sleep_ms(2000)
    M1_M2.stop()
    sleep_ms(2000)

