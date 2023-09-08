# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bmp280 import BMP280

ip = "192.168.0.15"
port = 8081

Board(ip, port)
bmp = BMP280()

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












































