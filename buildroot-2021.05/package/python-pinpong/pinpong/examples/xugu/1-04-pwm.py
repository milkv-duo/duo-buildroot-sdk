# -*- coding: utf-8 -*-

#实验效果：使用按钮控制LED呼吸灯
#接线：使用windows或linux电脑连接一块arduino主控板，主控板D6接一个LED灯模块
import time
from pinpong.board import Board,Pin,PWM #导入PWM类实现模拟输出

Board("xugu").begin()

pwm0 = PWM(Pin(Pin.D6)) #将Pin传入PWM中实现模拟输出

while True:
  for i in range(100): #从0到255循环
    pwm0.duty(i)  #设置模拟输出值
    print(i)
    time.sleep(0.05)