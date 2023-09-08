# -*- coding: utf-8 -*-

#实验效果：I2C OLED2864屏控制
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_ssd1306 import SSD1306_I2C #导入ssd1306库

Board("jh7100").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别

oled=SSD1306_I2C(width=128, height=64) #初始化屏幕，传入屏幕像素点数
#oled.set_font(font="msyh", width = 15, height = 15, kerning = 0, rowledge = 0)

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
