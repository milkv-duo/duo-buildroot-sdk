# -*- coding: utf-8 -*-

#实验效果：读取I2C CCS811空气质量传感器
#接线：使用windows或linux电脑连接一块arduino主控板，空气质量传感器接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_ccs811 import CCS811, CCS811_Ecycle, CCS811_Emode

ip = "192.168.0.15"
port = 8081

Board(ip, port)

ccs811 = CCS811()

while True:
    if(ccs811.check_data_ready()):
        print("baseline:",ccs811.read_baseline())
    else:
        print("data is not ready!")
    time.sleep(0.3)
