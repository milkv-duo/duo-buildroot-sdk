# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_rgb1602 import RGB1602
 
ip = "192.168.0.15"
port = 8081

Board(ip, port)

lcd = RGB1602() #初始化LCD的I2C地址

print("I2C RGB1602 TEST...")

lcd.set_rgb(0,50,0);       #设置RGB值

lcd.print("PinPong")       #显示 "PinPong"

lcd.set_cursor(1,1)        #设置光标位置
lcd.print(1234)            #显示数字

while True:
  time.sleep(1)
  lcd.scroll_left()           #滚动显示
