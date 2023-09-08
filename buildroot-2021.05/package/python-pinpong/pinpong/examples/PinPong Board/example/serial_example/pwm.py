# -*- coding: utf-8 -*-

#实验效果：使用按钮控制LED模块亮度
#接线：使用windows或linux电脑连接一块arduino主控板，主控板D6接一个LED灯模块
import time
from pinpong.board import Board,Pin #导入PWM类实现模拟输出

Board("PinPong Board").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong Board","COM36").begin()   #windows下指定端口初始化
#Board("PinPong Board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong Board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

pwm0 = Pin(Pin.D7, Pin.PWM)

while True:
  pwm0.write_analog(100)
  time.sleep(1)
  pwm0.write_analog(0)
  time.sleep(1)
  pwm0.write_analog(60)
  time.sleep(1)
  pwm0.write_analog(0)
  time.sleep(1)
  pwm0.write_analog(80)
  time.sleep(1)
  pwm0.write_analog(0)
  time.sleep(1)
'''
  for i in range(100): #从0到255循环
    pwm0.write_analog(i)  #设置模拟输出值
    print(i)
    time.sleep(0.05)
'''