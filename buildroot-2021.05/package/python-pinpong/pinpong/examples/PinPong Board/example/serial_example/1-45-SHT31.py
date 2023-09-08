# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_sht31 import SHT31

Board("PinPong board").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong board","COM36").begin()  #windows下指定端口初始化
#Board("PinPong board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

sht31 = SHT31(0x45)

while True:
    print("-------------------")
    print("Temperature: {} C".format(sht31.temp_c()))
    print("Temperature: {} F".format(sht31.temp_f()))
    print("humidity: {} %RH".format(sht31.humidity()))
    time.sleep(1)