# -*- coding:utf-8 -*-
#IIC转串口模块(DFR0627)

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_uarttoi2c import DFRobot_IIC_Serial

Board("uno").begin()

iic_uart2 = DFRobot_IIC_Serial(DFRobot_IIC_Serial.SUBUART_CHANNEL_2, IA1 = 1, IA0 = 1) 
iic_uart1 = DFRobot_IIC_Serial(DFRobot_IIC_Serial.SUBUART_CHANNEL_1, IA1 = 1, IA0 = 1) 

iic_uart2.begin(baud = 115200, format = iic_uart2.IIC_Serial_8N1)
iic_uart1.begin(baud = 9600, format = iic_uart1.IIC_Serial_8N1)

data = [0x01, 0x02, 0x03]

while True:
    while iic_uart1.available():
        c = iic_uart1.read()
        time.sleep(0.002)
    time.sleep(1)
    iic_uart1.write(data)
    iic_uart2.write("hello")

