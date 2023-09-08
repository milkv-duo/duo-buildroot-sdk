# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bmp388 import BMP388

ip = "192.168.0.15"
port = 8081

Board(ip, port)

bmp = BMP388()

while True:
  altitude = bmp.cal_altitude_m()
  pressure = bmp.pressure_pa()
  temp = bmp.temp_C()
  print("-----------------------------")
  print("altitude = %.2f m"% altitude)
  print("pressure = %.2f pa"% pressure)
  print("read_temperature = %.2f C"% temp)
  print("-----------------------------")
  time.sleep(0.5)
  