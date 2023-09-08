# -*- coding: utf-8 -*-

#实验效果：使用按钮控制UNIHIKER板载亮灭
#接线：使用电脑连接一块UNIHIKER主控板，主控板P0接一个按钮模块
import time
from pinpong.board import Board,Pin

Board("UNIHIKER").begin()  #初始化，选择板型，不输入板型则进行自动识别

btn = Pin(Pin.P0, Pin.IN) #引脚初始化为电平输入

while True:
  v = btn.value()  #读取引脚电平
  print(v)  #终端打印读取的电平状态
  time.sleep(0.1)
