# -*- coding: utf-8 -*-

#RPi and PythonBoard
#实验效果：使用按钮控制LED模块亮度

import time
from pinpong.board import Board,Pin,PWM

Board("RPi").begin()

pwm0 = PWM(Pin(Pin.D21)) #将Pin传入PWM中实现模拟输出

pwm0.freq(300)
while True:
  for i in range(100): #从0到255循环
    pwm0.duty(i)  #设置模拟输出值
    print(i)
    time.sleep(0.05)
