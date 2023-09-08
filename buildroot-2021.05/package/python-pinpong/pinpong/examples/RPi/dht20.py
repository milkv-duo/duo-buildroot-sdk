# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_dht20 import DHT20

Board("RPI").begin()#初始化

dht20 = DHT20(bus_num=1) #根据支持的i2c设备填写

while True:
    print("-------------------------")
    print("温度: {} °C".format(dht20.temp_c())) 
    print("湿度: {} %".format(dht20.humidity()))
    time.sleep(0.5)
