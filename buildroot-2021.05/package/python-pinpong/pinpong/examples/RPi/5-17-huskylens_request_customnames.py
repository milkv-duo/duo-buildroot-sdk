# -*- coding: utf-8 -*-

#实验效果：自定义ID名字

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens

Board("RPI").begin()  #初始化


husky = Huskylens(bus_num=1)

husky.command_request_customnames(2,"TEST2") 

time.sleep(2)


