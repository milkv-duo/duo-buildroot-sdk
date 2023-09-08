# -*- coding: utf-8 -*-
#实验效果：展示红外接收功能

import sys
import time
from pinpong.board import Board,IRRecv,Pin

Board("UNIHIKER").begin()  #初始化，选择板型，不输入板型则进行自动识别

def ir_recv3(data):
  print("------Receiving IR Code--------")
  print(hex(data))

#ir2 = IRRecv(Pin(Pin.P0))
ir3 = IRRecv(Pin(Pin.P22),ir_recv3)

while(1):
#  v = ir2.read()
#  if v:
#    print("------Pin2--------")
#    print(v)
  time.sleep(0.1)