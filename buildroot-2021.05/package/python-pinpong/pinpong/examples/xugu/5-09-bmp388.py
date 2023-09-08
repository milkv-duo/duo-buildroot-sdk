# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bmp388 import BMP388

Board("xugu").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别

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
  