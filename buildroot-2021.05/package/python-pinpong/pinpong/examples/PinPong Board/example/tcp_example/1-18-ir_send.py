# -*- coding: utf-8 -*-

#实验效果：展示红外遥控功能
import sys
import time
from pinpong.board import Board,IRRemote,Pin

ip = "192.168.0.15"
port = 8081

Board(ip, port)

ir = IRRemote(Pin(Pin.D4))

while True:
    ir.send(0x1122)
    time.sleep(0.5)