# -*- coding: utf-8 -*-

#实验效果：I2C LCD1602控制
#接线：使用windows或linux电脑连接一块arduino主控板，LCD1602显示屏接到I2C口SCL及SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_lcd1602 import LCD1602_I2C #从libs中导入lcd1602_i2c库

Board("xugu").begin()

lcd = LCD1602_I2C(i2c_addr=0x20) #初始化LCD的I2C地址

print("I2C LCD1602 TEST...")
lcd.backlight(True)  #打开背光
lcd.clear()   #清屏
lcd.set_cursor(0,0)  #设置光标位置
lcd.print("Hello World") #显示 "Hello World",1602屏像素点少，不能显示汉字

lcd.set_cursor(1,1)  #设置光标位置
lcd.print(1234) #显示数字1234

while True:
  time.sleep(1)
  lcd.scroll_left() #滚动显示
