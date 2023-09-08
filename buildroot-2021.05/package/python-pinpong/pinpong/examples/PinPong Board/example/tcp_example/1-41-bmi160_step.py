# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bmi160 import BMI160

ip = "192.168.1.116"
port = 8081

Board(ip, port)

bmi = BMI160()

bmi.begin(bmi.step, 0x69)

while True:
  print(bmi.get_step())
  time.sleep(0.5)