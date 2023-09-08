# -*- coding: utf-8 -*-

#实验效果：触发拍照/截图保存至SD卡

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens

Board("RPI").begin()  #初始化


husky = Huskylens(bus_num=1)

husky.command_request_photo() #保存照片
time.sleep(2)


husky.command_request_screenshot() #保存截图
time.sleep(2)




