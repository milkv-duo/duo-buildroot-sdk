# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_dht20 import DHT20

Board("uno").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

dht20 = DHT20()

while True:
    print("-------------------------")
    print("Temperature: {} ℃".format(dht20.temp_c())) 
    print("humidity: {} %".format(dht20.humidity()))
    time.sleep(0.5)
