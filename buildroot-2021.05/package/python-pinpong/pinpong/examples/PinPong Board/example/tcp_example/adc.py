# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin,ADC

ip = "192.168.1.116"
port = 8081

Board(ip, port)

adc0 = Pin(Pin.A0, Pin.ANALOG) #引脚初始化为电平输出

while True:
  v = adc0.read_analog()  #读取A0口模拟信号数值
  print("A0=", v)
  time.sleep(0.5)
