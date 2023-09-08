# -*- coding:utf-8 -*-
import time
from pinpong.board import Board, Pin
from pinpong.libs.dfrobot_as7341 import AS7341
from pinpong.libs.dfrobot_st7789 import ST7789_SPI
from pinpong.libs.dfrobot_ui     import HISTOGRAM

Board("RPi").begin()

#所需的参数列表
text_list          = ['F1','F2','F3','F4','F5','F6','F7','F8']
color_list         = [0x284B,0x194F,0x130D,0x266D,0x5F64,0xF548,0xF982,0x60A0]

#设置LCD屏dc和res引脚
dc = Pin(pin=21, mode=Pin.OUT)
res = Pin(pin=25, mode=Pin.OUT)

#构造外设对象
as7341 = AS7341()
lcd = ST7789_SPI(width=240, height=240, bus_num=0,device_num=0, dc=dc, res=res) 
histogram = HISTOGRAM(lcd,50,0,8)

#设置外设参数
lcd.set_font(font="msyh", width = 30, height = 30, kerning = 0, rowledge = 0)

#在LCD上打印F1-F8
def text_print():
  for index,text in enumerate(text_list):
    lcd.text(text,0,(index)*30,color_list[index])

#初始化外设
lcd.fill(lcd.COLOR_BLACK) #填充屏幕为黑色
while(as7341.begin() != True):
    lcd.text('AS7341 ERROR', 0, 0, lcd.COLOR_WHITE)
    time.sleep(0.5)

#在LCD上打印F1-F8
text_print()
while True:
  #获取数据
  spectrum_data = as7341.get_spectrum_data()
  #打印携带数据的柱状图
  histogram.histogram_with_data(spectrum_data,color_list) 
