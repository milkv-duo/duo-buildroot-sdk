# -*- coding: utf-8 -*-

#实验效果：读取I2C PAJ7620U2手势传感器
#接线：使用windows或linux电脑连接一块arduino主控板，手势传感器接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_paj7620u2 import PAJ7620U2,Gesture

Board("xugu").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别

paj = PAJ7620U2()

paj.set_gesture_high_rate()            #设置高速模式
#paj.set_gesture_low_rate()            #设置低速模式

Gesture.print_all()

while True:
  description,gesture = paj.get_gesture()
  if gesture != Gesture.none:
    print("gesture code         =%s"%gesture)
    print("gesture description  =%s"%description)
