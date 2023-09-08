# -*- coding: utf-8 -*-

#实验效果：使用按钮控制LED模块亮度
#接线：使用windows或linux电脑连接一块handpy主控板，主控板P0接一个LED灯模块
import time
from pinpong.board import Board,Pin #导入PWM类实现模拟输出

Board("handpy").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("handpy","COM36").begin()  #windows下指定端口初始化
#Board("handpy","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("handpy","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

pwm0 = Pin(Pin.P0, Pin.PWM)

while True:
  for i in range(1024): #从0到1024循环
    pwm0.write_analog(i)  #设置模拟输出值
    print(i)
    time.sleep(0.05)
