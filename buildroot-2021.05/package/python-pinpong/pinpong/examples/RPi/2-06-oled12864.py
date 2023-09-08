# -*- coding: utf-8 -*-

#RPi and PythonBoard
#实验效果：I2C OLED2864屏控制

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_ssd1306 import SSD1306_I2C #导入ssd1306库

Board("RPi").begin()

oled=SSD1306_I2C(width=128, height=64, bus_num=1) #初始化屏幕，传入屏幕像素点数
#kerning  字间距，0为系统默认值 其他值为自定义间距，单位为像素
#rowledge 行距，默认为0，单位为像素
#oled.set_font(font="msyh", width = 15, height = 15, kerning = 0, rowledge = 0)
#oled.set_font(font="msyh", width = 15, height = 15)
oled.set_font(font="default", width = 15, height = 15)

if True:

  oled.fill(1) #全部填充显示
  time.sleep(0.5)
  
  oled.fill(0) #全部填充熄灭，清屏
  time.sleep(0.5)

  oled.fill_rect(10,10,50,45,1)  #画实心矩形（x,y,w,h,c）
  time.sleep(1)

  oled.rect(10,0,50,50,1)      #画空心矩形
  time.sleep(1)

  oled.circle(20,20,10,1)     #空心画圆 x,y,r是半径（圆心，半径，color）
  time.sleep(1)

  oled.fill_circle(20,20,10,0)        #实心画圆 x,y,r是半径（圆心，半径，color）
  time.sleep(1)

  oled.line(10,10,100,0,1)      #画斜线（x1,y1,x2,y2，color）
  time.sleep(1)

  oled.line(20,0,20,40,1)       #画直线
  time.sleep(1)

  oled.fill_rect(80,10,130,60,1)   #画越界实心矩形（x,y,w,h,color）
  time.sleep(1)

  oled.rect(30,30,20,80,1)        #画越界空心矩形
  time.sleep(1)

  oled.circle(20,20,60,1)        #画越界空心圆 x,y,r是半径（圆心，半径，color）
  time.sleep(1)

  oled.fill_circle(20,20,30,0)        #画越界实心圆 x,y,r是半径（圆心，半径，color）
  time.sleep(1)

  oled.text("測試にほ测试ん2",30,30)
#  oled.text("Hello PinPong",0,20) #指定位置显示文字
  time.sleep(2)
