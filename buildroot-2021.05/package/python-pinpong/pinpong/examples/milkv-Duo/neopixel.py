# -*- coding: utf-8 -*-

#实验效果：控制WS2812单线RGB LED灯
#接线：使用windows或linux电脑连接一块arduino主控板，ws2812灯接到D9口
import time
from pinpong.board import Board,Pin,NeoPixel

NEOPIXEL_PIN = Pin.P21
PIXELS_NUM = 4 #灯数

Board("UNIHIKER").begin()  #初始化，选择板型，不输入板型则进行自动识别

np = NeoPixel(Pin(NEOPIXEL_PIN), PIXELS_NUM)

while True:
  np[0] = (0, 255 ,0) #设置第一个灯RGB亮度
  np[1] = (255, 0, 0) #设置第二个灯RGB亮度
  np[2] = (0, 0, 255) #设置第三个灯RGB亮度
  np[3] = (255, 0, 255) #设置第四个灯RGB亮度
  time.sleep(1)

  np[1] = (0, 255, 0)
  np[2] = (255, 0, 0)
  np[3] = (255, 255, 0)
  np[0] = (0, 0, 255)
  time.sleep(1)

  np.rainbow(0,7,0,0x0022FF)
  time.sleep(1)
  for i in range(3):
    np.rotate(1)
    time.sleep(1)

  for i in range(3):
    np.shift(1)
    time.sleep(1)

  np.clear()
  time.sleep(1)

  np.range_color(0,4,0xFF0000)
  time.sleep(1)
  np.range_color(0,4,0x00FF00)
  time.sleep(1)  
  np.range_color(0,4,0x0000FF)
  time.sleep(1)
  
  np.clear()
  time.sleep(1)
  