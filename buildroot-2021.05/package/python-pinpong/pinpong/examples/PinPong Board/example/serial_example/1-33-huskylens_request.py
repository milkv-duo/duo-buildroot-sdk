# -*- coding: utf-8 -*-

#实验效果：获取方框的的x,y坐标及长宽。
#接线：使用windows或linux电脑连接一块arduino主控板，哈士奇接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens

Board("PinPong Board").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong Board","COM36").begin()   #windows下指定端口初始化
#Board("PinPong Board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong Board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

husky = Huskylens()


while True:
    data = husky.command_request()
    if (data):
        x_center = data[0]
        y_center = data[1]
        width = data[2]
        height = data[3]
        ID = data[4]
        print("X_center:",x_center)
        print("y_center:",y_center)
        print("width:",width)
        print("height:",height)
        print("ID:",ID)

    time.sleep(2)
