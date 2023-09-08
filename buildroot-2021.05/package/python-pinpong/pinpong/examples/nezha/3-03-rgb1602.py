# -*- coding: utf-8 -*-

#Nezha
#实验效果：控制I2C RGB彩色背光LCD1602液晶屏幕

import sys
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_rgb1602 import RGB1602 #从libs导入rgb1602库

Board("nezha").begin()

lcd = RGB1602()

print("I2C RGB1602 TEST...")

lcd.set_rgb(0,50,0);       #设置RGB值

lcd.print("PinPong")       #显示 "PinPong"

lcd.set_cursor(1,1)        #设置光标位置
lcd.print(1234)            #显示数字

while True:
  time.sleep(1)
  lcd.scroll_left()           #滚动显示
