# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_max17043 import MAX17043

Board("PinPong board").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong board","COM36").begin()  #windows下指定端口初始化
#Board("PinPong board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

gauge = MAX17043()

while not gauge.begin():
    print("gauge begin faild!")
    time.sleep(2)

print("gauge begin successful!")
while True:
    print("voltage: {} mV".format(gauge.read_voltage()))
    print("percentage: {} %".format(gauge.read_percentage()))
    time.sleep(1)