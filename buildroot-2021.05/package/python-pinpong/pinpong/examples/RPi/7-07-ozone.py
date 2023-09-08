# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_ozone import Ozone

Board("RPI").begin()#初始化


oz = Ozone(0x70, bus_num=1)
#设置模式主动或者被动模式, MEASURE_MODE_AUTOMATIC,MEASURE_MODE_PASSIVE
oz.set_mode(oz.MEASURE_MODE_AUTOMATIC)
collection_times = 20
while True:
    value = oz.read_ozone_data(collection_times)
    print("ozone concentration is %d PPB" %value)
    time.sleep(1)