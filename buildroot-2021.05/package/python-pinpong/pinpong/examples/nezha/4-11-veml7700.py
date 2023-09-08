# -*- coding: utf-8 -*-

#Nezha
#实验效果：使用VEML7700传感器测量环境光强度 

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_veml7700 import VEML7700_I2C

Board("nezha").begin()

veml7700 = VEML7700_I2C()


while (veml7700.begin() == False):
  print ('Please check that the veml7700 sensor is properly connected')
  time.sleep(3)
print("sensor begin successfully!!!")

while True:
  ''' 直接读取当前测量的数据，单位勒克斯（lx） '''
  lux = veml7700.get_ALS_lux()
  print('ALS_LUX :', lux, 'lx')
