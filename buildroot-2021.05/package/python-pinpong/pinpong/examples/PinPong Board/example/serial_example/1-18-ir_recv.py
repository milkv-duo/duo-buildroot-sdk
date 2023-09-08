# -*- coding: utf-8 -*-

#实验效果：展示红外遥控功能
#接线：uno支持
import sys
import time
from pinpong.board import Board,IRRecv,Pin

Board("PinPong Board").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong Board","COM36").begin()   #windows下指定端口初始化
#Board("PinPong Board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong Board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

def ir_recv3(data):
  print("------Pin3--------")
  print(hex(data))

ir3 = IRRecv(Pin(3))
#ir3 = IRRecv(Pin(3),ir_recv3)

while True:
    v = ir3.read()
    if v:
      print("------Pin3--------")
      print(hex(v))
    time.sleep(0.1)