# -*- coding:utf-8 -*-
#NeZha 
#实验效果：ST7789 TFT屏幕的基本使用
from pinpong.board import Board, Pin
from pinpong.libs.dfrobot_st7789 import ST7789_SPI

Board("nezha").begin()
dc = Pin(pin=24, mode=Pin.OUT)
res = Pin(pin=25, mode=Pin.OUT)
lcd = ST7789_SPI(width=240, height=240, bus_num=1,device_num=0, dc=dc, res=res) #初始化屏幕
lcd.reset()

"""
MERGE: 像素点合并，例如0XF800(红色) 合并 0x07E0(绿色)，像素点变为0XF800 | 0x07E0 = 0xFFE0(紫色)
REPLACE：像素点取代，例如0XF800(红色) 取代 0x07E0(绿色)，像素点变为0xF800(红色)
"""

lcd.blend_mode(lcd.MERGE)#lcd.REPLACE
'''
font='msyh' 当前只支持微软雅黑字体
size=30 表示中文字体宽度30像素，高度30像素，英文宽度15，高度30
'''
lcd.set_font(font="msyh",size = 30)

lcd.fill(lcd.COLOR_BLACK) #填充屏幕为黑色
lcd.fill_rect(80,0,50,50,lcd.COLOR_GREEN)  #画实心矩形（x,y,w,h,c）
lcd.fill_rect(100,0,50,50,lcd.COLOR_RED)   #画实心矩形
lcd.rect(150,0,50,50,lcd.COLOR_GREEN)      #画空心矩形
lcd.circle(120,100,20,lcd.COLOR_GREEN)     #空心画圆 x,y,r是半径（圆心，半径，color）
lcd.fill_circle(120,100,20,lcd.COLOR_RED)        #实心画圆 x,y,r是半径（圆心，半径，color）

lcd.line(50,100,100,150,lcd.COLOR_GREEN)      #画斜线（x1,y1,x2,y2，color）
lcd.line(120,170,150,200,lcd.COLOR_GREEN)     #画斜线
lcd.line(0,230,150,230,lcd.COLOR_GREEN)       #画直线

lcd.fill_rect(80,-10,130,60,lcd.COLOR_GREEN)  #画越界实心矩形（x,y,w,h,color）
lcd.circle(120,100,150,lcd.COLOR_GREEN)        #画越界空心圆 x,y,r是半径（圆心，半径，color）

string = '测试'
#string = '測試にほ测试ん2'
#string = 'DFRobot'
#string = '从字形度量这个名字可以想到，字形度量是关联每一个字形的确定距离，以此描述如何使用该距离来排版文本。'
lcd.text(string, 0, 100, lcd.COLOR_GREEN)     #打印文字（text,x,y,color）
