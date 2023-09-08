# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_ph import PH2

Board().begin()

ph2 = PH2(Pin(Pin.A2))

while True:
    print(ph2.read_ph())
    time.sleep(1)
