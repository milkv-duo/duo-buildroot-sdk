# -*- coding: utf-8 -*-

import time
from pinpong.board import Board,Pin

Board("handpy").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("handpy","COM36").begin()   #windows下指定端口初始化
#Board("handpy","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("handpy","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

btn = Pin(Pin.P0, Pin.IN) #引脚初始化为电平输入
led = Pin(Pin.P1, Pin.OUT)

while True:
  v = btn.value()  #读取引脚电平
  print(v)  #终端打印读取的电平状态
  led.value(v)  #将按钮状态设置给led灯引脚
  time.sleep(0.1)