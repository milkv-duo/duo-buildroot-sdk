# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_heartrate import HEARTRATE

ip = "192.168.0.15"
port = 8081

Board(ip, port)

DIGITAL_MODE = 1
ANALOG_MODE = 0

heart_rate = HEARTRATE(DIGITAL_MODE, Pin.A1)    # ANALOG_MODE or DIGITAL_MODE 方式

while True:
  heart_rate.get_value()
  rate_value = heart_rate.get_rate()
  if rate_value:
    print(rate_value)
  time.sleep(0.02)


















