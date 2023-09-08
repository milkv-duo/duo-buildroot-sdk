# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bme280 import BME280

Board("RPI").begin()          

bme = BME280(bus_num=1)  #根据树莓派支持的i2c设备填写

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