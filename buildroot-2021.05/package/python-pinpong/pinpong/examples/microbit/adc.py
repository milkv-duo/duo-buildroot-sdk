# -*- coding: utf-8 -*-

#实验效果：使用microbit板连接一个模拟传感器，并读出数据
#接线：使用windows或linux电脑连接一块microbit主控板，主控板P2接一个模拟传感器
import time
from pinpong.board import Board,Pin,ADC

Board("microbit").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("microbit","COM36").begin()   #windows下指定端口初始化
#Board("microbit","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("microbit","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

adc0 = ADC(Pin(Pin.P2)) #将Pin传入ADC中实现模拟输入

while True:
  v = adc0.read()  #读取A0口模拟信号数值
  print("A0=", v)
  time.sleep(0.5)

