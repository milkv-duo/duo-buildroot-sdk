# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bmi160 import BMI160

Board("RPI").begin()#初始化

bmi = BMI160(bus_num=1)  #根据树莓派支持i2c设备填写

bmi.begin(bmi.step, 0x69)

while True:
  print(bmi.get_step())
  time.sleep(0.5)