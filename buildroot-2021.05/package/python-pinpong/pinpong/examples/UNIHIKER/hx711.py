# -*- coding: utf-8 -*-

#实验效果：读取重量传感器
import time
from pinpong.board import Board,Pin,HX711

dout_pin = Pin.D4
sck_pin = Pin.D5
scale = 2121                       #校验值

Board("UNIHIKER").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

hx = HX711(dout_pin, sck_pin, scale)

while True:
  value = hx.read_weight()
  print("weight = %.2f g" %value)
  time.sleep(0.2)