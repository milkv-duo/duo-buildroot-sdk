# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.extension.handpy import *

Board("handpy").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("handpy","COM36").begin()   #windows下指定端口初始化
#Board("handpy","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("handpy","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

oled.DispChar("PinPong库",1)
#屏幕显示"pinpong库"在第一行
# oled.DispChar("pinpong库", 42, 22)
#屏幕显示"pinpong库"在x,y坐标处，x:0-127,y:0-63
#oled.Bitmap(0,0,50,50,"E:/PinPong/default.png")
#依次是显示的坐标X,Y, 显示的宽和高，图片路径
# oled.clean(1)
#屏幕清除第一行的内容,参数1,2,3,4
#oled.fill(0)
#清屏黑色填0，白色填1
#oled.rotation(0)
#屏幕反转0°或者180°
#oled.pixel(0,0)
#在坐标x,y画点
#oled.set_line_width(1)
#设置线宽范围 1 - 128
#oled.line(0,0,127,63)
#划线，依次是起点坐标x1,y1和终点坐标x2,y2
#oled.circle(63, 31, 20)
#画圆，依次是坐标x, y和 半径, 不填充 
#oled.fill_circle(63, 31, 20)
#画圆，依次是坐标x, y和 半径, 填充
#oled.rect(0,0, 63, 31)
#画矩形，依次是起点坐标x, y, 宽，高, 不填充 
#oled.fill_rect(0,0, 63, 31)
#画矩形，依次是起点坐标x, y, 宽，高, 填充 
oled.show()
while True:
  pass
