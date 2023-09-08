# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_tm1650 import TM1650

ip = "192.168.1.116"
port = 8081

Board(ip, port)

tm = TM1650()

tm.display_string("1.246")                   #四位数码管显示字符串xxxx
time.sleep(3)
tm.clear()                                   #四位数码管清屏
tm.set_dot(4, True)                          #四位数码管点亮或熄灭第几位小数点

while True:
    pass