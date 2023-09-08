# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.extension.handpy import *

Board("handpy").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("handpy","COM36").begin()   #windows下指定端口初始化
#Board("handpy","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("handpy","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

tune = ["C4:4", "D4:4", "E4:4", "C4:4", "C4:4", "D4:4", "E4:4", "C4:4",
        "E4:4", "F4:4", "G4:4", "E4:4", "F4:4", "G4:4"]
music.play(tune)

#music.set_tempo(4,60)
#设置每一拍等同于4分音符，每分钟节拍数
#music.play("C4:4")
#time.sleep(2)
#music.stop()
#停止后台播放
while True:
    pass











