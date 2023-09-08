# -*- coding: utf-8 -*-
#SEN0483:风速测量,需搭配RS485转UART模块使用(DFR0845)

import time
from pinpong.board import Board
from pinpong.libs.rs485windspeed_rs485touart_uarttoi2c import IICSerial #从libs导入mlx90614库

Board().begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别

ws = IICSerial(sub_uart_channel = IICSerial.SUBUART_CHANNEL_2, IA1 = 1, IA0 = 1)

a = ws.modify_address(0, 2)
print(a)
while True:
    #print(w.read_wind_speed())
    time.sleep(1)
