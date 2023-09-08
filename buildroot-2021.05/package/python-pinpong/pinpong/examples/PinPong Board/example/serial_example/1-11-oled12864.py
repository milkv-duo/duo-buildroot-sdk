# -*- coding: utf-8 -*-

#实验效果：I2C OLED2864屏控制
#接线：使用windows或linux电脑连接一块arduino主控板，OLED2864显示屏接到I2C口SCL及SDA
import time
from pinpong.board import Board, oled12864

Board("PinPong Board").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong Board","COM36").begin()   #windows下指定端口初始化
#Board("PinPong Board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong Board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

oled=oled12864() #初始化屏幕，传入屏幕像素点数

while True:
  oled.fill(1) #全部填充显示
  print("1")
  time.sleep(1)
  
  oled.fill(0) #全部填充熄灭，清屏
  print("0")
  time.sleep(1)
  
  oled.text(1, 0) #显示数字
  oled.text_XY(20, 2, "Hello PinPong") #指定位置显示文字
  time.sleep(1)