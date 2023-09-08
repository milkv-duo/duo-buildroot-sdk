# -*- coding: utf-8 -*-

#实验效果：控制WS2812单线RGB LED灯
#接线：使用windows或linux电脑连接一块arduino主控板，ws2812灯接到D9口
import time
from pinpong.board import Board,Pin,NeoPixel

NEOPIXEL_PIN = Pin.D9
PIXELS_NUM = 3 #灯数

Board("PinPong Board").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong Board","COM36").begin()   #windows下指定端口初始化
#Board("PinPong Board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong Board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

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
  