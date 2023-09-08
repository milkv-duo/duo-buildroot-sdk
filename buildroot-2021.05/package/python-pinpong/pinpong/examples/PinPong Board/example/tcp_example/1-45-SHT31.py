# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_sht31 import SHT31

ip = "192.168.1.116"
port = 8081

Board(ip, port)

sht31 = SHT31(0x45)

while True:
    print("-------------------")
    print("Temperature: {} C".format(sht31.temp_c()))
    print("Temperature: {} F".format(sht31.temp_f()))
    print("humidity: {} %RH".format(sht31.humidity()))
    time.sleep(1)