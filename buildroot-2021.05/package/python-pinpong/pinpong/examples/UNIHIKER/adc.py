# -*- coding: utf-8 -*-
#实验效果：打印UNIHIKER板所有模拟口的值
#接线：使用windows或linux电脑连接一块UNIHIKER主控板
import time
from pinpong.board import Board,Pin,ADC  #导入ADC类实现模拟输入

Board("UNIHIKER").begin()  #初始化，选择板型，不输入板型则进行自动识别

#模拟引脚P0 P1 P2 P3 P4 P10 P21 P22
adc0 = ADC(Pin(Pin.P0)) #将Pin传入ADC中实现模拟输入
adc1 = ADC(Pin(Pin.P1))
adc2 = ADC(Pin(Pin.P2))
adc3 = ADC(Pin(Pin.P3))
adc4 = ADC(Pin(Pin.P4))
adc10 = ADC(Pin(Pin.P10))
adc21 = ADC(Pin(Pin.P21))
adc22 = ADC(Pin(Pin.P22))

while True:
  print("P0=", adc0.read())
  print("P1=", adc1.read())
  print("P2=", adc2.read())
  print("P3=", adc3.read())
  print("P4=", adc4.read())
  print("P10=", adc10.read())
  print("P21=", adc21.read())
  print("P22=", adc22.read())
  print("------------------")
  time.sleep(0.5)
