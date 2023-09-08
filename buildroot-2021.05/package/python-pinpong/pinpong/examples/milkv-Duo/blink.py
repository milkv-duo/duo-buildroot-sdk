# -*- coding: utf-8 -*-

#实验效果：控制UNIHIKER板载LED灯一秒闪烁一次
#接线：使用电脑连接一块UNIHIKER主控板

import time
from pinpong.board import Board,Pin

Board("MILKV-DUO").begin()  #初始化，选择板型，不输入板型则进行自动识别

led = Pin(Pin.D14, Pin.OUT) #引脚初始化为电平输出

while True:
  led.value(1) #输出高电平
  print("1") #终端打印信息
  time.sleep(1) #等待1秒 保持状态

  led.value(0) #输出低电平
  print("0") #终端打印信息
  time.sleep(1) #等待1秒 保持状态
