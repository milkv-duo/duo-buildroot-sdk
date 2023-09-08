# -*- coding:utf-8 -*-
#RPi 
#实验效果：ST7789 TFT屏幕的基本使用
from pinpong.board import Board, Pin
from pinpong.libs.dfrobot_st7789 import ST7789_SPI

Board("RPi").begin()
dc = Pin(pin=21, mode=Pin.OUT)
res = Pin(pin=25, mode=Pin.OUT)
lcd = ST7789_SPI(width=240, height=240, bus_num=0,device_num=0, dc=dc, res=res) #初始化屏幕

"""
MERGE: 像素点合并，例如0XF800(红色) 合并 0x07E0(绿色)，像素点变为0XF800 | 0x07E0 = 0xFFE0(紫色)
REPLACE：像素点取代，例如0XF800(红色) 取代 0x07E0(绿色)，像素点变为0xF800(红色)
"""

lcd.set_blend_mode(lcd.MERGE)#lcd.REPLACE

'''
font='msyh' 当前只支持微软雅黑字体
size=30 表示中文字体宽度30像素，高度30像素，英文宽度15，高度30
'''
lcd.set_font(font="msyh", width = 30, height = 20, kerning = 0, rowledge = 0)

lcd.fill(lcd.COLOR_BLACK) #填充屏幕为黑色
lcd.fill_rect(80,0,50,50,lcd.COLOR_GREEN)   #画实心矩形（x,y,w,h,c）
lcd.fill_rect(100,0,50,50,lcd.COLOR_RED)    #画实心矩形
lcd.rect(150,0,50,50,lcd.COLOR_GREEN)       #画空心矩形
lcd.circle(120,100,20,lcd.COLOR_GREEN)     #空心画圆 x,y,r是半径（圆心，半径，color）
lcd.fill_circle(120,100,20,lcd.COLOR_RED)        #实心画圆 x,y,r是半径（圆心，半径，color）

lcd.line(50,100,100,150,lcd.COLOR_GREEN)      #画斜线（x1,y1,x2,y2，color）
lcd.line(120,170,150,200,lcd.COLOR_GREEN)     #画斜线
lcd.line(200,0,200,200,lcd.COLOR_GREEN)       #画直线

lcd.fill_rect(80,-10,130,60,lcd.COLOR_GREEN)  #画越界实心矩形（x,y,w,h,color）
lcd.rect(80,-30,130,60,lcd.COLOR_GREEN)       #画越界空心矩形
lcd.circle(120,100,150,lcd.COLOR_GREEN)        #画越界空心圆 x,y,r是半径（圆心，半径，color）
lcd.fill_circle(240,100,150,lcd.COLOR_GREEN)        #画越界实心圆 x,y,r是半径（圆心，半径，color）
lcd.line(-100,-100,150,200,lcd.COLOR_GREEN)       #画越界直线

#string = '测试'
#string = '測試にほ测试ん2'
#string = 'faeujfgkfadsafkqeucwfjvcascvxeqwegulkgufjhnjghmgmuqweqwerwdasmccavsjvcsvckasvcs'
#string = 'DFRobot'
#string = '用 glyph->bitmap_left 和 glyph->bitmap_top 来指定起始位置。bitmap_left 是从字形位图当前笔位置到最左边界的水平距离，而 bitmap_top 是从笔位置（位于基线）到最高边界得垂直距离。'
#lcd.text(string, 100, 30, lcd.COLOR_GREEN)     #打印文字（text,x,y,color）
