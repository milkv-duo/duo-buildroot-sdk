# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_paj7620u2 import PAJ7620U2,Gesture

ip = "192.168.0.15"
port = 8081

Board(ip, port)

paj = PAJ7620U2()

paj.set_gesture_high_rate()            #设置高速模式
#paj.set_gesture_low_rate()            #设置低速模式

Gesture.print_all()

while True:
  description,gesture, = paj.get_gesture()
  if gesture != Gesture.none:
    print("gesture code         =%s"%gesture)
    print("gesture description  =%s"%description)
