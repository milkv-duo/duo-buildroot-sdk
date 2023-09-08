# -*- coding: utf-8 -*-

#实验效果：打印handpy板P0口模拟值
#接线：使用windows或linux电脑连接一块handpy主控板，主控板P0接一个旋钮模块
import time
from pinpong.board import Board,Pin,ADC  #导入ADC类实现模拟输入

Board("handpy").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("handpy","COM36").begin()  #windows下指定端口初始化
#Board("handpy","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("handpy","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

adc0 = Pin(Pin.P0, Pin.ANALOG) #引脚初始化为电平输出

while True:
  v = adc0.read_analog()  #读取A0口模拟信号数值
  print("A0=", v)
  time.sleep(0.5)
