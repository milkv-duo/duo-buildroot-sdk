# -*- coding: utf-8 -*-
#实验效果：展示红外接收功能
#接线
import sys
import time
from pinpong.board import Board,IRRecv,Pin

Board("nezha").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化


def ir_recv3(data):
  print("------Recv IR Code--------")
  print(hex(data))

ir3 = IRRecv(ir_recv3)

while(1):
  time.sleep(0.1)