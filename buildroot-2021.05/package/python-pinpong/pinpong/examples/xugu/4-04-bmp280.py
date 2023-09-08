# -*- coding: utf-8 -*-

#实验效果：读取I2C BMP280气压传感器
#接线：使用windows或linux电脑连接一块arduino主控板，气压传感器接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bmp280 import BMP280

Board("xugu").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别

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












































