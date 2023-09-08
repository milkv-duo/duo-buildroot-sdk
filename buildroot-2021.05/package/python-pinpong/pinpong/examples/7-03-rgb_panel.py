# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_rgb_panel import RGBpanel

Board("uno").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

panel = RGBpanel(bus_num=4)

s = "PinPong"

panel.clear()                       #清除表情
panel.display(6,panel.YELLOW)       #显示第x个表情，anel.YELLOW颜色
time.sleep(2)

#panel.clear()
#panel.scroll(panel.Left);          #设置往左滚动
#panel.print(s,panel.BLUE);         #设置显示的字符
#time.sleep(8)

#panel.clear()
#panel.pixel(2,2,panel.QUENCH)      #x,y坐标显示panel.QUENCH颜色
#panel.pixel(3,2,panel.RED)         #x,y坐标显示panel.RED颜色
#panel.pixel(4,2,panel.GREEN)       #x,y坐标显示panel.GREEN颜色
#panel.pixel(5,2,panel.YELLOW)      #x,y坐标显示panel.YELLOW颜色
#panel.pixel(6,2,panel.BLUE)        #x,y坐标显示panel.BLUE颜色
#panel.pixel(7,2,panel.PURPLE)      #x,y坐标显示panel.PURPLE颜色
#panel.pixel(8,2,panel.CYAN)        #x,y坐标显示panel.CYAN颜色
#panel.pixel(9,2,panel.WHITE)       #x,y坐标显示panel.WHITE颜色
#time.sleep(3)

#panel.clear()
#panel.fillScreen(panel.RED)        #屏幕全部显示panel.RED
#time.sleep(3)

#panel.clear();
#panel.scroll(panel.Left);
#panel.print(12, panel.BLUE);
#time.sleep(3)
