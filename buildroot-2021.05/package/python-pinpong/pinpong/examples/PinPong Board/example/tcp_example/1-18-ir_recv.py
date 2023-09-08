# -*- coding: utf-8 -*-

#实验效果：展示红外遥控功能
#接线：uno支持
import sys
import time

from pinpong.board import Board,IRRecv,Pin


ip = "192.168.1.116"
port = 8081

Board(ip, port)

def ir_recv3(data):
  print("------Pin3--------")
  print(hex(data))

#ir3 = IRRecv(Pin(3))
ir3 = IRRecv(Pin(3),ir_recv3)

while(1):
#  v = ir3.read()
#  if v:
#   print("------Pin3--------")
#   print(hex(v))
  time.sleep(0.1)