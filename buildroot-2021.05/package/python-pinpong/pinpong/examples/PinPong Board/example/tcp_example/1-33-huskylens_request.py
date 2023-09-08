# -*- coding: utf-8 -*-

#实验效果：获取方框的的x,y坐标及长宽。
#接线：使用windows或linux电脑连接一块arduino主控板，哈士奇接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens
#from dfrobot_huskylens import Huskylens

ip = "192.168.0.90"
port = 8081

Board(ip, port)
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
