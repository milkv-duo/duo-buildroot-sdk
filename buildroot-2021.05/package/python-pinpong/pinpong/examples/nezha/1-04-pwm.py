# -*- coding: utf-8 -*-

#Nezha
#实验效果：使用按钮控制LED模块亮度

import time
from pinpong.board import Board,Pin,PWM

Board("nezha").begin()

pwm7 = PWM(Pin(2)) # pwm channel 7
#pwm1 = PWM(Pin(22)) # pwm channel 1

pwm7.freq(1000)

while True:
  for i in range(0,100): #占空比从0%到100%循环
    pwm7.duty(i)  #设置模拟输出值
    print(i)
    time.sleep(0.01)
