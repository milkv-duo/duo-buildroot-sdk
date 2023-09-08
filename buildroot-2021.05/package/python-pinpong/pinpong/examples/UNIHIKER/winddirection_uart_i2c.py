# -*- coding: utf-8 -*-
#SEN0482:风向测量,需搭配RS485转UART模块(DFR0845)、IIC转串口模块(DFR0627)使用

import time
from pinpong.board import Board
from pinpong.libs.rs485winddirection_rs485touart_uarttoi2c import IICSerialWindDirection

Board("UNIHIKER").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别

wx = IICSerialWindDirection(IICSerialWindDirection.SUBUART_CHANNEL_1, IA1 = 1, IA0 = 1)

wx.modify_address(0, 2)

while True:
    print(wx.read_wind_direction())
    print(wx.get_wind_angle())          #V2才有读取角度
    time.sleep(0.3)