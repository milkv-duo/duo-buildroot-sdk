# -*- coding: utf-8 -*-

#实验效果：读取I2C BMP280气压传感器

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bmp280 import BMP280

Board("RPi").begin()  #初始化

bmp = BMP280(bus_num=1)

while not bmp.begin():
  print("bmp begin faild")
  time.sleep(2)

while True:
  temp = bmp.temp_c()
  press = bmp.pressure_p()
  alti = bmp.altitude_m(press)
  print("=========== start print ===========")
  print("temperature: %.2f Celsius" % temp)
  print("pressure: %d pa" % press)
  print("altitude: %.2f meter" % alti)
  print("===========  end print  ===========")
  time.sleep(1)












































