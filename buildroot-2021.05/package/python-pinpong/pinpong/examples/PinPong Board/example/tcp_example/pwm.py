# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin,PWM

ip = "192.168.1.116"
port = 8081

Board(ip, port)

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