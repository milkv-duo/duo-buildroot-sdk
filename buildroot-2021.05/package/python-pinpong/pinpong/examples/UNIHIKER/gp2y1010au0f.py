# -*- coding: utf-8 -*-

#实验效果：舵机控制
#接线：使用电脑连接一块UNIHIKER主控板，P0 P1连接一个空气质量粉尘传感器
import time
from pinpong.board import Board,Pin,GP2Y1010AU0F

Board("UNIHIKER").begin()  #初始化，选择板型，不输入板型则进行自动识别

air = GP2Y1010AU0F(Pin.P0, Pin.P1) #P0模拟引脚 P1数字引脚

while True:
  value = air.dust_density()
  print("Dust Density: %.2f mg/m3" % value)
  time.sleep(1)