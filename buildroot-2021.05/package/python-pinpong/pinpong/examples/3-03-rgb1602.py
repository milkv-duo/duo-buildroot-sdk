# -*- coding: utf-8 -*-

#实验效果：控制I2C RGB彩色LCD1602液晶屏幕
#接线：使用windows或linux电脑连接一块arduino主控板，RGBLCD1602接到I2C口SCL SDA
import sys
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_rgb1602 import RGB1602 #从libs导入rgb1602库

Board("uno").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

lcd = RGB1602(bus_num=4)

print("I2C RGB1602 TEST...")

lcd.set_rgb(0,50,0);       #设置RGB值

lcd.print("PinPong")       #显示 "PinPong"

lcd.set_cursor(1,1)        #设置光标位置
lcd.print(1234)            #显示数字

while True:
  time.sleep(1)
  lcd.scroll_left()           #滚动显示
