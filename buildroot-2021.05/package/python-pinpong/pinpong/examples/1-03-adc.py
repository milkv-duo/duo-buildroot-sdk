# -*- coding: utf-8 -*-

#实验效果：打印UNO板A0口模拟值
#接线：使用windows或linux电脑连接一块arduino主控板，主控板A0接一个模拟传感器
import time
from pinpong.board import Board,Pin,ADC  #导入ADC类实现模拟输入

Board("UNIHIKER").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()   #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

adc0 = ADC(Pin(Pin.A22)) #将Pin传入ADC中实现模拟输入
#adc1 = ADC(Pin(Pin.A1))
#adc2 = ADC(Pin(Pin.A2))
#adc3 = ADC(Pin(Pin.A3))
#adc4 = ADC(Pin(Pin.A4))
#adc10 = ADC(Pin(Pin.A10))
while True:
  v0 = adc0.read()  #读取A0口模拟信号数值
  '''
  v1 = adc1.read()  #读取A1口模拟信号数值
  v2 = adc2.read()  #读取A2口模拟信号数值
  v3 = adc3.read()  #读取A3口模拟信号数值
  v4 = adc4.read()  #读取A4口模拟信号数值
  v10 = adc10.read()  #读取A10口模拟信号数值
  '''
  
  print("A0=", v0)
  '''
  print("A1=", v1)
  print("A2=", v2)
  print("A3=", v3)
  print("A4=", v4)
  print("A10=", v10)
  '''
  time.sleep(0.5)
