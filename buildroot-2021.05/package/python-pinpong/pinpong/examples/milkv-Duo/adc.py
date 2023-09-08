# -*- coding: utf-8 -*-
#实验效果：打印UNIHIKER板所有模拟口的值
#接线：使用windows或linux电脑连接一块UNIHIKER主控板
import time
from pinpong.board import Board,Pin,ADC  #导入ADC类实现模拟输入

Board("milkv-duo").begin()  #初始化，选择板型，不输入板型则进行自动识别

#�模�引� ADC0 ADC1
adc0 = ADC(Pin(Pin.A0)) #将Pin传入ADC中实现模拟输入

while True:
  print("P0=", adc0.read())
  print("------------------")
  time.sleep(0.5)
