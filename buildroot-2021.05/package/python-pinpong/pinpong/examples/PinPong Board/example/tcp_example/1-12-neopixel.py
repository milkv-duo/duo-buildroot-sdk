# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin,NeoPixel

ip = "192.168.1.116"
port = 8081

Board(ip, port)

NEOPIXEL_PIN = Pin.D9
PIXELS_NUM = 3 #灯数

np = NeoPixel(Pin(NEOPIXEL_PIN), PIXELS_NUM)

while True:
  np[0] = (0, 255 ,0) #设置第一个灯RGB亮度
  np[1] = (255, 0, 0) #设置第二个灯RGB亮度
  np[2] = (0, 0, 255) #设置第三个灯RGB亮度
  time.sleep(1)

  np[1] = (0, 255, 0)
  np[2] = (255, 0, 0)
  np[0] = (0, 0, 255)
  time.sleep(1)
  
  np.clear()          #清除RGB亮度
  time.sleep(1)
  
  np.range_color(0,3,0xFF0000)
  time.sleep(1)
  np.range_color(0,3,0x00FF00)
  time.sleep(1)  
  np.range_color(0,3,0x0000FF)
  time.sleep(1)

  np.clear()
  time.sleep(1)

