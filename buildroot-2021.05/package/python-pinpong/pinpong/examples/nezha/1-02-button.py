# -*- coding: utf-8 -*-

#NeZha
#实验效果：使用按钮控制板载亮灭

import time
from pinpong.board import Board,Pin

Board("nezha").begin()

btn = Pin(6, Pin.IN) #引脚初始化为电平输入,接按钮
led = Pin(3, Pin.OUT)#引脚初始化为电平输出，接LED

while True:
  v = btn.value()  #读取引脚电平
  print(v)  #终端打印读取的电平状态
  led.value(v)  #将按钮状态设置给led灯引脚
  time.sleep(0.1)
