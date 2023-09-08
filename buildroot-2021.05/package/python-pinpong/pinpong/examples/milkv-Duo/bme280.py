# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bme280 import BME280

Board("milkv-duo").begin()               #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()      #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin() #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化
bme = BME280(bus_num=1)

while True:
    temp = bme.temp_c()
    press = bme.press_pa()
    alti = bme.cal_altitudu()
    humi = bme.humidity()
    print("temperature: %.2f C" % temp)
    print("preass: %d pa"% press)
    print("altitudu: %.2f m"% alti)
    print("humitity: %.2f"% humi)
    time.sleep(1)