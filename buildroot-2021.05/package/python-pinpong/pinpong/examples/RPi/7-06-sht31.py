# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_sht31 import SHT31

Board("RPI").begin()#初始化

sht31 = SHT31(0x45,bus_num=1)

while True:
    print("-------------------")
    print("Temperature: {} C".format(sht31.temp_c()))
    print("Temperature: {} F".format(sht31.temp_f()))
    print("humidity: {} %RH".format(sht31.humidity()))
    time.sleep(1)