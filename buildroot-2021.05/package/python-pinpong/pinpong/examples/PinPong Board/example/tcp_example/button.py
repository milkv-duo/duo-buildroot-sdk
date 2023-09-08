# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin

ip = "192.168.1.116"
port = 8081

Board(ip, port)

btn = Pin(Pin.D8, Pin.IN) #引脚初始化为电平输入
led = Pin(Pin.D7, Pin.OUT)

while True:
  v = btn.read_digital()  #读取引脚电平
  print(v)  #终端打印读取的电平状态
  led.write_digital(v)  #将按钮状态设置给led灯引脚
  time.sleep(0.1)
