# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.extension.handpy import *

Board("handpy").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("handpy","COM36").begin()   #windows下指定端口初始化
#Board("handpy","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("handpy","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

rgb[0] = (255, 0, 0)  # 设置为红色，全亮度
rgb[1] = (0, 128, 0)  # 设定为绿色，半亮度
rgb[2] = (0, 0, 64)   # 设置为蓝色，四分之一亮度
rgb.write()

#rgb.fill((0,0,0))
#rgb.write()

#rgb.disable(-1)                                       #关闭LED灯，-1代表3个灯(可以填灯号0,1,2)
#time.sleep(1)
rgb.brightness(2)                                     #设置LED灯的亮度，范围0-9
time.sleep(1)
print(rgb.brightness())                                      #返回LED灯的亮度值
while True:
  pass