# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin

ip = "192.168.1.116"
port = 8081

Board(ip, port)

led = Pin(Pin.D7, Pin.OUT)

while True:
  led.value(1) #输出高电平
  print("1") #终端打印信息
  
  time.sleep(1) #等待1秒 保持状态

  led.value(0) #输出低电平
  print("0") #终端打印信息
  time.sleep(1) #等待1秒 保持状态
