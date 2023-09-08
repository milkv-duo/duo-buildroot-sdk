# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin,DHT11,DHT22

ip = "192.168.0.15"
port = 8081

Board(ip, port)

dht11 = DHT11(Pin(Pin.D6))
dht22 = DHT22(Pin(Pin.D7))

while True:
  temp = dht11.temp_c() #读取摄氏温度
  humi = dht11.humidity() #读取湿度
  print("dht11 temperature=",temp," humidity=",humi)
  
  temp = dht22.temp_c() #读取摄氏温度
  humi = dht22.humidity() #读取湿度
  print("dht22 temperature=",temp," humidity=",humi)
  time.sleep(1)
  