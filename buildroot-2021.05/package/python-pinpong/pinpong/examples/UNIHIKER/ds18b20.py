# -*- coding: utf-8 -*-

#实验效果：读取DS18B20防水温度传感器
import time
from pinpong.board import Board,Pin,DS18B20

Board("UNIHIKER").begin()  #初始化，选择板型，不输入板型则进行自动识别

ds18b20 = DS18B20(Pin(Pin.P21))
while True:
  temp = ds18b20.temp_c()
  print("temperature = ",temp)
  time.sleep(1)
