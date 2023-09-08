# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.extension.handpy import *
from pinpong.libs.microbit_motor import DFMotor

Board("handpy").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("handpy","COM36").begin()  #windows下指定端口初始化
#Board("handpy","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("handpy","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

M1 = DFMotor(1)
#1,2,3,4代表电机M1,M2,M3,M4, CW代表正转，CCW代表反转，200是速度，范围0-255
while True:
    M1.speed(200)
    M1.run(M1.CW)
    sleep_ms(2000)
    M1.stop()
    sleep_ms(2000)
    M1.speed(200)
    M1.run(M1.CCW)
    sleep_ms(2000)
    M1.stop()
    sleep_ms(2000)
