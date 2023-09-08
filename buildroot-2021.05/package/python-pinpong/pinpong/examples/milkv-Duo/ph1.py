# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_ph import PH

Board().begin()

ph2 = PH(Pin(Pin.A0))   #需供电5V

while True:
    print(ph2.read_ph())
    time.sleep(1)
