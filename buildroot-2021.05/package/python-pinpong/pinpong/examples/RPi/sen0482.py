# -*- coding: utf-8 -*-

'''
金属风向传感器,
测量范围：16方位,测量精度：3°
启动风速:0.3m/s
'''

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_sen0482 import Wind_Direction

Board("RPi").begin()
wind_direction = Wind_Direction(port="/dev/ttyUSB0",slave=0x02)

#方向字典
direction_dir ={
    0:"北",
    1:"东北偏北",
    2:"东北",
    3:"东北偏东",
    4:"东",
    5:"东南偏东",
    6:"东南",
    7:"东南偏南",
    8:"南",
    9:"西南偏南",   
    10:"西南",
    11:"西南偏西",
    12:"西",
    13:"西北偏西",
    14:"西北",
    15:"西北偏北",
    16:"北"         
}

'''
设置地址（默认地址为0x02）
广播地址为0x00
可写入0x00~0xFF
调用此函数设置地址后需重新上电,并在构造函数重新写入地址
'''
#wind_direction.set_addr(0x02)

while True:
    if wind_direction.get_wind_direction() in direction_dir:
        print("Current wind direction: {}".format(direction_dir[wind_direction.get_wind_direction()]))
        time.sleep(0.5)