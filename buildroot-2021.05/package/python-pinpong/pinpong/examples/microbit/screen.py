# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.extension.microbit import *

Board("microbit").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("microbit","COM36").begin()   #windows下指定端口初始化
#Board("microbit","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("microbit","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

#'HAPPY','HEART','HEART_SMALL','ALL_ARROWS','ALL_CLOCKS','ANGRY','ARROW_E','ARROW_N','ARROW_NE','ARROW_NW','ARROW_S','ARROW_SE','ARROW_SW','ARROW_W','ASLEEP','BUTTERFLY','CHESSBOARD','CLOCK1','CLOCK10','CLOCK11','CLOCK12','CLOCK2','CLOCK3','CLOCK4','CLOCK5','CLOCK6','CLOCK7','CLOCK8','CLOCK9','CONFUSED','COW','DIAMOND','DIAMOND_SMALL','DUCK','FABULOUS','GHOST','GIRAFFE','HOUSE','MEH','MUSIC_CROTCHET','MUSIC_QUAVER','MUSIC_QUAVERS','NO','PACMAN','PITCHFORK','RABBIT','ROLLERSKATE','SAD','SILLY','SKULL','SMILE','SNAKE','SQUARE','SQUARE_SMALL','STICKFIGURE','SURPRISED','SWORD','TARGET','TORTOISE','TRIANGLE','TRIANGLE_LEFT','TSHIRT','UMBRELLA','XMAS','YES'
display.show(Image.HEART)
#显示图案
#display.scroll("hello world")
#显示字符串
#display.set_pixel(0,0)
#点亮坐标x,y的灯
#display.off_pixel(0,0)
#熄灭坐标x,y的灯
#display.set_brightness(1)
#设置灯的亮度
#display.clear()
#关闭所有点阵
while True:
    pass



