# -*- coding: utf-8 -*-
#实验效果：水质传感器读取水质
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_tds import TDS

Board("milkv-duo").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

tds = TDS(Pin(Pin.A0))

while True:
    tds_value = tds.get_value()
    print("TDS Value: %d ppm" % tds_value)
    time.sleep(1)


  
