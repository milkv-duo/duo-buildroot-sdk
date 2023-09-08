# -*- coding: utf-8 -*-
#SEN0482:风向测量,需搭配RS485转UART模块使用(DFR0845)
#与unihiker硬串口1连接,P0-Rx,P3-Tx

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_wind_direction import Winddirection #从libs导入mlx90614库

Board("UNIHIKER").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别

w = Winddirection()

w.modify_address(0, 2)

while True:
    print(w.read_wind_direction())
    print(w.get_wind_angle())          #V2才有读取角度
    time.sleep(1)
