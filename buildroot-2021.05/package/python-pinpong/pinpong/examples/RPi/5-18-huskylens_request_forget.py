# -*- coding: utf-8 -*-

#实验效果：触发遗忘功能
#接线：使用windows或linux电脑连接一块树莓派主控板，哈士奇接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens

Board("RPI").begin()  #初始化


husky = Huskylens(bus_num=1)

husky.command_request_forget()
time.sleep(2)





