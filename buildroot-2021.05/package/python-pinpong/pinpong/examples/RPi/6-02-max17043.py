# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_max17043 import MAX17043

Board("RPI").begin()#初始化

gauge = MAX17043(bus_num=1)

while not gauge.begin():
    print("gauge begin faild!")
    time.sleep(2)

print("gauge begin successful!")
while True:
    print("voltage: {} mV".format(gauge.read_voltage()))
    print("percentage: {} %".format(gauge.read_percentage()))
    time.sleep(1)