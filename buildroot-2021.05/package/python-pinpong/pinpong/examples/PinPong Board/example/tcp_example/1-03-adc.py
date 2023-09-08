# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin,ADC

ip = "192.168.1.116"
port = 8081

Board(ip, port)

adc0 = ADC(Pin(Pin.A0)) #将Pin传入ADC中实现模拟输入
times = time.time()
while True:
  v = adc0.read()  #读取A0口模拟信号数值
  print("A0=", v)
  time.sleep(0.5)
  
