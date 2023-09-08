# -*- coding: utf-8 -*-

#实验效果：读取dht温湿度传感器
#接线：使用windows或linux电脑连接一块handpy主控板
import time
from pinpong.board import Board,Pin,DHT11,DHT22

Board("handpy").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("handpy","COM36").begin()  #windows下指定端口初始化
#Board("handpy","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("handpy","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

dht11 = DHT11(Pin(Pin.P0))
dht22 = DHT22(Pin(Pin.P1))

while True:
  temp = dht11.temp_c() #读取摄氏温度
  humi = dht11.humidity() #读取湿度
  print("dht11 temperature=",temp," humidity=",humi)
  
  temp = dht22.temp_c() #读取摄氏温度
  humi = dht22.humidity() #读取湿度
  print("dht22 temperature=",temp," humidity=",humi)
  time.sleep(1)


