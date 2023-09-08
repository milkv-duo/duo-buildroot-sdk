# -*- coding: utf-8 -*-

#实验效果：使用按钮控制microbit板连接的LED亮灭
#接线：使用windows或linux电脑连接一块microbit主控板，主控板P0接一个按钮模块
import time
from pinpong.board import Board,Pin

Board("microbit").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("microbit","COM36").begin()   #windows下指定端口初始化
#Board("microbit","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("microbit","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

btn = Pin(Pin.P0, Pin.IN) #引脚初始化为电平输入
led = Pin(Pin.P1, Pin.OUT)

while True:
  v = btn.value()  #读取引脚电平
  print(v)  #终端打印读取的电平状态
  led.value(v)  #将按钮状态设置给led灯引脚
  time.sleep(0.1)