# -*- coding: utf-8 -*-

#RPi and PythonBoard
#实验效果：使用按钮控制板载亮灭

import time
from pinpong.board import Board,Pin

Board("RPi").begin()

btn = Pin(Pin.D26, Pin.IN) #引脚初始化为电平输入
led = Pin(Pin.D21, Pin.OUT)

while True:
  v = btn.value()  #读取引脚电平
  print(v)  #终端打印读取的电平状态
  led.value(v)  #将按钮状态设置给led灯引脚
  time.sleep(0.1)
