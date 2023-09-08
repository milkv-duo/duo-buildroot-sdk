# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_tm1650 import TM1650

Board("PinPong board").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong board","COM36").begin()  #windows下指定端口初始化
#Board("PinPong board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

tm = TM1650()

tm.display_string("1.246")                   #四位数码管显示字符串xxxx
time.sleep(3)
tm.clear()                                   #四位数码管清屏
tm.set_dot(4, True)                          #四位数码管点亮或熄灭第几位小数点

while True:
    pass