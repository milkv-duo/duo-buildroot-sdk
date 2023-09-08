# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bmi160 import BMI160

ip = "192.168.1.116"
port = 8081

Board(ip, port)

bmi = BMI160()

bmi.begin(bmi.acc)

while True:
  gyr_x = bmi.get_gyr_x()
  acc_x = bmi.get_acc_x()
  gyr_y = bmi.get_gyr_y()
  acc_y = bmi.get_acc_y()
  gyr_z = bmi.get_gyr_z()
  acc_z = bmi.get_acc_z()
  print("{}  {}  {}  {}  {}  {}".format(gyr_x, gyr_y, gyr_z, acc_x, acc_y, acc_z))
  time.sleep(0.5)