# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_vl53l0 import VL53L0

Board("MILKV-DUO").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

vl = VL53L0(bus_num=1)

while True:
    distance = vl.get_distance_mm()
    print("distance: %.2f mm"%(distance))
    time.sleep(0.5)
