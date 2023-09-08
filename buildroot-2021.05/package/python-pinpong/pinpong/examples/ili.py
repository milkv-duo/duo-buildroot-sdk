# -*- coding: utf-8 -*-

import time
from pinpong.board import Pin
from pinpong.libs.dfrobot_ili9341 import ILI9341_SPI #导入ili9341库

#board = Board()  #初始化，选择板型和端口号，不输入则留空进行自动识别

dc = Pin(pin=21, mode=Pin.OUT)
res = Pin(pin=26, mode=Pin.OUT)

lcd = ILI9341_SPI(width=240, height=320, device_num=1, dc=dc, res=res) #初始化屏幕，传入屏幕像素点数
lcd.begin()

while True:
  lcd.fill(lcd.COLOR_BLACK)
  time.sleep(1)
  lcd.fill(lcd.COLOR_NAVY)
  time.sleep(1)
  lcd.fill(lcd.COLOR_DGREEN)
  time.sleep(1)
