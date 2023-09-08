# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bme280 import BME280

ip = "192.168.0.15"
port = 8081

Board(ip, port)

bme = BME280()

while True:
    temp = bme.temp_c()
    press = bme.press_pa()
    alti = bme.cal_altitudu()
    humi = bme.humidity()
    print("-----------------------")
    print("temperature: %.2f C" % temp)
    print("preass: %d pa"% press)
    print("altitudu: %.2f m"% alti)
    print("humitity: %.2f"% humi)
    print("-----------------------")
    time.sleep(1)