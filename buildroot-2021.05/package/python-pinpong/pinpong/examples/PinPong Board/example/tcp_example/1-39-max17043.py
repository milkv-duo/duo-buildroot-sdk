# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_max17043 import MAX17043

ip = "192.168.1.116"
port = 8081

Board(ip, port)

gauge = MAX17043()

while not gauge.begin():
    print("gauge begin faild!")
    time.sleep(2)

print("gauge begin successful!")
while True:
    print("voltage: {} mV".format(gauge.read_voltage()))
    print("percentage: {} %".format(gauge.read_percentage()))
    time.sleep(1)