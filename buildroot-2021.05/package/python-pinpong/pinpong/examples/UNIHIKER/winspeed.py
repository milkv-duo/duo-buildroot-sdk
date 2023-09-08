# -*- coding: utf-8 -*-
#SEN0483:风速测量,需搭配RS485转UART模块(DFR0845)、IIC转串口模块(DFR0627)使用

import time
from pinpong.board import Board
from pinpong.libs.rs485windspeed_rs485touart_uarttoi2c import IICSerialWindSpeed

Board("UNIHIKER").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别

ws = IICSerialWindSpeed(IICSerialWindSpeed.SUBUART_CHANNEL_1, IA1 = 0, IA0 = 0)

ws.modify_address(0, 2)
while True:
    print(ws.read_wind_speed())
    time.sleep(0.3)