# -*- coding: utf-8 -*-

#RPi and PythonBoard
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_urm09 import URM09 #从libs中导入URM09库

Board("RPi").begin()

urm = URM09(i2c_addr=0x11, bus_num=1) #初始化传感器，设置I2C地址
urm.set_mode_range(urm._MEASURE_MODE_AUTOMATIC ,urm._MEASURE_RANG_500) #设置URM09模式为自动检测，最大测量距离500cm

while True:
  dist = urm.distance_cm() #读取距离数据，单位厘米（cm）
  temp = urm.temp_c() #读取传感器温度，单位摄氏度（℃）

  print("Distance is %d cm         "%dist)
  print("Temperature is %.2f .c    "%temp)
  time.sleep(0.5)
