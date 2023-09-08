# -*- coding: utf-8 -*-

from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_ili9341 import ILI9341_SPI #导入ili9341库

Board("RPi").begin()

dc  = Pin(21, Pin.OUT)
res = Pin(25, Pin.OUT)

lcd = ILI9341_SPI(width=240, height=320, bus_num=0, device_num=0, dc=dc, res=res) #初始化屏幕，传入屏幕像素点数

"""
MERGE: 像素点合并，例如0XF800(红色) 合并 0x07E0(绿色)，像素点变为0XF800 | 0x07E0 = 0xFFE0(紫色)
REPLACE：像素点取代，例如0XF800(红色) 取代 0x07E0(绿色)，像素点变为0xF800(红色)
"""

lcd.set_blend_mode(lcd.MERGE)#lcd.REPLACE

'''
font='msyh' 当前只支持微软雅黑字体
width = 60, height = 40 表示字体宽度60像素，高度40像素,间距为20像素，间距为0时为默认间距
'''
lcd.set_font(font="msyh", width = 30, height = 20, kerning = 0, rowledge = 0)

lcd.fill(lcd.COLOR_BLACK) #填充屏幕为黑色
lcd.fill_rect(80,0,50,50,lcd.COLOR_GREEN)  #画实心矩形（x,y,w,h,c）
lcd.fill_rect(100,0,50,50,lcd.COLOR_RED)   #画实心矩形
lcd.rect(10,0,50,50,lcd.COLOR_GREEN)      #画空心矩形
lcd.circle(120,100,20,lcd.COLOR_GREEN)     #空心画圆 x,y,r是半径（圆心，半径，color）
lcd.fill_circle(120,100,20,lcd.COLOR_RED)        #实心画圆 x,y,r是半径（圆心，半径，color）

lcd.line(50,100,100,150,lcd.COLOR_GREEN)      #画斜线（x1,y1,x2,y2，color）
lcd.line(120,170,150,200,lcd.COLOR_GREEN)     #画斜线
lcd.line(200,0,200,200,lcd.COLOR_GREEN)       #画直线

lcd.fill_rect(80,-10,130,60,lcd.COLOR_GREEN)   #画越界实心矩形（x,y,w,h,color）
lcd.rect(30,-30,150,50,lcd.COLOR_GREEN)        #画越界空心矩形
lcd.circle(120,100,150,lcd.COLOR_GREEN)        #画越界空心圆 x,y,r是半径（圆心，半径，color）
lcd.fill_circle(240,100,150,lcd.COLOR_GREEN)        #画越界实心圆 x,y,r是半径（圆心，半径，color）
lcd.line(-100,-100,150,200,lcd.COLOR_GREEN)       #画越界直线


string = '測試にほ测试ん2'
lcd.text(string, 100, 100, lcd.COLOR_GREEN)     #打印文字（text,x,y,color,mode）mode=True自动换行