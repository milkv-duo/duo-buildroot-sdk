# -*- coding: utf-8 -*-

#实验效果：I2C OLED2864屏控制
#接线：使用windows或linux电脑连接一块microbit主控板，OLED2864显示屏接到I2C口SCL及SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_ssd1306 import SSD1306_I2C #导入ssd1306库

Board("microbit").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("microbit","COM36").begin()  #windows下指定端口初始化
#Board("microbit","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("microbit","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

oled=SSD1306_I2C(width=128, height=64) #初始化屏幕，传入屏幕像素点数

while True:
  oled.fill(1) #全部填充显示
  oled.show() #显示生效
  print("1")
  time.sleep(1)
  
  oled.fill(0) #全部填充熄灭，清屏
  oled.show() #显示生效
  print("0")
  time.sleep(1)
  
  oled.text(0) #显示数字
  oled.text("Hello PinPong",8,8) #指定位置显示文字
  oled.show()  #显示生效
  time.sleep(2)
