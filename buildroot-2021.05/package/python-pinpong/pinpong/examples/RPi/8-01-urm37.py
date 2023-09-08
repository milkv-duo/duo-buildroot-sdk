# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_urm37 import URM37

Board("RPi").begin()

urm37 = URM37("/dev/ttyS1")

while urm37.begin() == False:
    print("Communication with device failed, please check connection")
    time.sleep(1)

while True:
    print("-------------------------")
    print("temperature: {}".format(urm37.temp_c())) 
    print("distance: {}".format(urm37.distance_cm()))    #获取安全等级
