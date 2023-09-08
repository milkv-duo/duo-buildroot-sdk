# -*- coding: utf-8 -*-

#实验效果：打印UNO板A0口模拟值
#接线：使用windows或linux电脑连接一块arduino主控板，主控板A0接一个旋钮模块
import time
from pinpong.board import Board,Pin,ADC  #导入ADC类实现模拟输入

Board("xugu").begin()

adc0 = Pin(Pin.A0, Pin.ANALOG) #引脚初始化为电平输出

while True:
  v = adc0.read_analog()  #读取A0口模拟信号数值
  print("A0=", v)
  time.sleep(0.5)
