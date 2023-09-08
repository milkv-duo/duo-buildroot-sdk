# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_dht20 import DHT20

Board("UNIHIKER").begin()  #初始化，选择板型，不输入板型则进行自动识别

dht20 = DHT20()

while True:
    print("-------------------------")
    print("temperature: {} ℃".format(dht20.temp_c())) 
    print("humidity: {} %".format(dht20.humidity()))
    time.sleep(0.5)
