# -*- coding: utf-8 -*-

#Nezha
#实验效果：使用BMI160传感器读取三轴加速度和陀螺仪值


import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bmi160 import BMI160

Board("nezha").begin()

bmi = BMI160()

bmi.begin(bmi.acc)

while True:
  gyr_x = bmi.get_gyr_x()
  acc_x = bmi.get_acc_x()
  gyr_y = bmi.get_gyr_y()
  acc_y = bmi.get_acc_y()
  gyr_z = bmi.get_gyr_z()
  acc_z = bmi.get_acc_z()
  print("\n--------------------------------")
  print("acc_x=%f, acc_y=%f, acc_z=%f"%(acc_x,acc_y,acc_z))
  print("gyr_x=%f, gyr_y=%f, gyr_z=%f"%(gyr_x,gyr_y,gyr_z))
  time.sleep(0.5)