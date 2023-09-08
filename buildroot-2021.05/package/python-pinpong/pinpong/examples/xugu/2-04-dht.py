# -*- coding: utf-8 -*-

#实验效果：读取dht温湿度传感器
#接线：使用windows或linux电脑连接一块arduino主控板，dht11连接D6，dht22连接D7
import time
from pinpong.board import Board,Pin,DHT11,DHT22

Board("xugu").begin()

dht11 = DHT11(Pin(Pin.D13))
dht22 = DHT22(Pin(Pin.D8))

while True:
  temp = dht11.temp_c() #读取摄氏温度
  humi = dht11.humidity() #读取湿度
  print("dht11 temperature=",temp," humidity=",humi)
  
  temp = dht22.temp_c() #读取摄氏温度
  humi = dht22.humidity() #读取湿度
  print("dht22 temperature=",temp," humidity=",humi)
  time.sleep(1)


