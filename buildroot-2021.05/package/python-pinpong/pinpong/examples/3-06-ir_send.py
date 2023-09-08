# -*- coding: utf-8 -*-
#实验效果：红外发射模块
import sys
import time
from pinpong.board import Board,IRRemote,Pin

Board("uno").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

ir = IRRemote(Pin(Pin.D4))

while True:
    ir.send(0xD3)               #一次只能8位数据
    time.sleep(0.5)