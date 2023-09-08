# -*- coding: utf-8 -*-

#Nezha
#实验效果：使用BMI160传感器读取计步数目

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bmi160 import BMI160

Board("nezha").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别

bmi = BMI160()

bmi.begin(bmi.step, 0x69)

while True:
  print(bmi.get_step())
  time.sleep(0.5)