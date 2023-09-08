# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_tds import TDS

Board("xugu").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别

tds = TDS(Pin(Pin.A2))

while True:
    tds_value = tds.get_value()
    print("TDS Value: %d ppm" % tds_value)
    time.sleep(1)


  