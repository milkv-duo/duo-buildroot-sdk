# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_vl53l0 import VL53L0

ip = "192.168.0.15"
port = 8081

Board(ip, port)

vl = VL53L0()

while True:
    distance = vl.get_distance_mm()
    print("distance: %.2f mm"%(distance))
    time.sleep(0.5)
