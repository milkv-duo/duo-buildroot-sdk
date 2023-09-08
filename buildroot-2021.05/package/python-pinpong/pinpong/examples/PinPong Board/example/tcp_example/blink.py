# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin

ip = "192.168.1.116"
port = 8081

Board(ip, port)

led = Pin(Pin.D7, Pin.OUT) #引脚初始化为电平输出

while True:
  led.write_digital(1) #输出高电平
  time.sleep(1) #等待1秒 保持状态

  led.write_digital(0) #输出低电平
  time.sleep(1) #等待1秒 保持状态
