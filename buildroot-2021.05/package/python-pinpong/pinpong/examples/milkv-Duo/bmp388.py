# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bmp388 import BMP388

Board("MILKV-DUO").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

bmp = BMP388(bus_num=0)

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
  