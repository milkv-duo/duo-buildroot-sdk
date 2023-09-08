# -*- coding: utf-8 -*-
import time
from pinpong.board import Board, Pin
from pinpong.libs.dfrobot_st7789 import ST7789_SPI

Board("RPi").begin()
dc = Pin(pin=21, mode=Pin.OUT)
res = Pin(pin=25, mode=Pin.OUT)
lcd = ST7789_SPI(width=240, height=240, bus_num=0,device_num=0, dc=dc, res=res) #初始化屏幕

lcd.set_blend_mode(lcd.REPLACE)
lcd.fill(lcd.COLOR_BLACK) #填充屏幕为黑色

top = 70
filename = "dfrobot_qrcode.png"

while True:
  lcd.fill_rect(0,top,240,110,lcd.COLOR_WHITE)
  lcd.set_font(font="msyh", width = 20, height = 20, kerning = 0, rowledge = 0)
  label_title  = "成都极趣科技有限公司"
  lcd.text(label_title,20,0+top,0XE4CC)
  
  lable_tel    = "电话："
  lcd.text(lable_tel,20,30+top,0XE4CC)
  
  tel          = "028-xxxxxxxx"
  lcd.text(tel,95,30+top,0XE4CC)
  
  lable_url = "网址："
  lcd.text(lable_url,20,60+top,0XE4CC)
  
  lcd.set_font(font="msyh", width = 20, height = 20, kerning = 4, rowledge = 1)
  url    = "http://www.dfrobot.com.cn"
  lcd.text(url,95,60+top,0XE4CC)
  time.sleep(2)
  lcd.fill(lcd.COLOR_BLACK)

  lcd.picture(filename,20,20,200)
  time.sleep(2)
  lcd.fill(lcd.COLOR_BLACK)

