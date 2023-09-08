# -*- coding: utf-8 -*-

#实验效果：在哈士奇的屏幕上显示文字，清屏

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens

Board("RPI").begin()  #初始化


husky = Huskylens(bus_num=1)

husky.command_request_custom_text("hiiii",25,52) #20字符限制

time.sleep(2)


husky.command_request_clear_text() #清屏
time.sleep(2)

