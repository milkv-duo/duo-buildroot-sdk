# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_vl53l0 import VL53L0

Board("RPI").begin()#初始化


vl = VL53L0(bus_num=1) #根据树莓派支持i2c设备填写

while True:
    distance = vl.get_distance_mm()
    print("distance: %.2f mm"%(distance))
    time.sleep(0.5)
