# -*- coding: utf-8 -*-

'''
金属风速传感器,
测量范围：0-32.4m/s,测量精度：0.3m/s
启动风速:0.2-0.4m/s
'''

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_sen0483 import Wind_Speed

Board("UNO").begin()
wind_speed = Wind_Speed(port="COM5",slave=0x02)


'''
设置地址（默认地址为0x02）
广播地址为0x00
可写入0x01~0xFF
调用此函数设置地址后需重新上电,并在构造函数重新写入地址
'''
#wind_speed.set_addr(0x02)

while True:
    print("当前风速：{}m/s".format(wind_speed.get_wind_speed()))#读取风速数据，单位米每秒（m/s）
    time.sleep(0.5)