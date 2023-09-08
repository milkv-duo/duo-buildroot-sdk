# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_aht20 import AHT20

ip = "192.168.1.116"
port = 8081

Board(ip, port)

aht = AHT20()

while True:
    print("humidity = {} %RH".format(aht.humidity()))
    print("template = {} ℃".format(aht.temp_c()))
    print("---------------------------")
    time.sleep(0.2)