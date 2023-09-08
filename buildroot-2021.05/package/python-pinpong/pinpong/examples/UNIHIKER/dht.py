# -*- coding: utf-8 -*-

#实验效果：读取dht温湿度传感器
#接线：使用电脑连接一块UNIHIKER主控板，dht11连接P21，dht22连接P22
import time
from pinpong.board import Board,Pin,DHT11,DHT22

Board("UNIHIKER").begin()  #初始化，选择板型，不输入板型则进行自动识别

dht11 = DHT11(Pin(Pin.P21))
dht22 = DHT22(Pin(Pin.P22))

while True:
  temp = dht11.temp_c() #读取摄氏温度
  humi = dht11.humidity() #读取湿度
  print("dht11 temperature=",temp," humidity=",humi)
  
  temp = dht22.temp_c() #读取摄氏温度
  humi = dht22.humidity() #读取湿度
  print("dht22 temperature=",temp," humidity=",humi)
  time.sleep(1)

