# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bme280 import BME280

Board("xugu").begin()               #初始化，选择板型和端口号，不输入端口号则进行自动识别化

bme = BME280()

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