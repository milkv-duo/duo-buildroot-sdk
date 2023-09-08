# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin,PWM

ip = "192.168.1.116"
port = 8081

Board(ip, port)

pwm0 = PWM(Pin(Pin.D7)) #将Pin传入PWM中实现模拟输出

while True:
  for i in range(255): #从0到255循环
    pwm0.duty(i)  #设置模拟输出值
    print(i)
    time.sleep(0.05)
