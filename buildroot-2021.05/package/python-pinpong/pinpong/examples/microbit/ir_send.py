# -*- coding: utf-8 -*-
#实验效果：红外发射模块
import sys
import time
from pinpong.board import Board,IRRemote,Pin

Board("HANDPY").begin()  #初始化，选择板型，不输入板型则进行自动识别

ir = IRRemote(Pin(Pin.P3))

while True:
    ir.send(0xfd807f) #VOL+
    time.sleep(0.5)
    ir.send(0xfd906f) #VOL-
    time.sleep(0.5)