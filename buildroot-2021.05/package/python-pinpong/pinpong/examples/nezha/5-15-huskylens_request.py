# -*- coding: utf-8 -*-

#NeZha 
#实验效果：获取方框的的x,y坐标及长宽。

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens

Board("nezha").begin()

husky = Huskylens()

while True:
    data = husky.command_request()
    if (data):
        num_of_objects = int(len(data)/5)
        for i in range(num_of_objects):
            x_center = data[5*i]
            y_center = data[5*i+1]
            width = data[5*i+2]
            height = data[5*i+3]
            ID = data[5*i+4]
            print("X_center:",x_center)
            print("y_center:",y_center)
            print("width:",width)
            print("height:",height)
            print("ID:",ID)

    time.sleep(2)
