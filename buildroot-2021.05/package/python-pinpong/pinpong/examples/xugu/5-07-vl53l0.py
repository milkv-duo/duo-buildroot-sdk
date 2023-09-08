# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_vl53l0 import VL53L0

Board("xugu").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别

vl = VL53L0()

while True:
    distance = vl.get_distance_mm()
    print("distance: %.2f mm"%(distance))
    time.sleep(0.5)
