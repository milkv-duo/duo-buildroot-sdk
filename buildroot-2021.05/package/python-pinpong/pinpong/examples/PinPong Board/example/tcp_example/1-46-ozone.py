# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_ozone import Ozone

ip = "192.168.1.116"
port = 8081

Board(ip, port)

oz = Ozone(0x73)
#设置模式主动或者被动模式, MEASURE_MODE_AUTOMATIC,MEASURE_MODE_PASSIVE
oz.set_mode(oz.MEASURE_MODE_AUTOMATIC)
collection_times = 20

while True:
    value = oz.read_ozone_data(collection_times)
    print("ozone concentration is %d PPB" %value)
    time.sleep(1)