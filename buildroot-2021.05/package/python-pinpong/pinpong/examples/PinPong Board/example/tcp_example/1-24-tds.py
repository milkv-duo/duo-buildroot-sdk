# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_tds import TDS

ip = "192.168.0.15"
port = 8081

Board(ip, port)

tds = TDS(Pin(Pin.A2))

while True:
    tds_value = tds.get_value()
    print("TDS Value: %d ppm" % tds_value)
    time.sleep(1)


  