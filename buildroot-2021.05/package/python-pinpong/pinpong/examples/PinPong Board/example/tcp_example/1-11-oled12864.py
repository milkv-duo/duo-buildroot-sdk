# -*- coding: utf-8 -*-
import time
from pinpong.board import Board, oled12864

ip = "192.168.1.166"
port = 8081

Board(ip, port)

oled=oled12864() #初始化屏幕，传入屏幕像素点数

while True:
  oled.fill(1) #全部填充显示
  print("1")
  time.sleep(1)
  
  oled.fill(0) #全部填充熄灭，清屏
  print("0")
  time.sleep(1)
  
  oled.text(1, 123) #显示数字
  oled.text_XY(20, 2, "Hello PinPong") #指定位置显示文字
  time.sleep(2)

