# -*- coding: utf-8 -*-

#实验效果：代码切换不同的模式。
#接线：使用windows或linux电脑连接一块arduino主控板，哈士奇接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens

ip = "192.168.0.90"
port = 8081

Board(ip, port)

husky = Huskylens()
time.sleep(0.5)

#husky.command_request_algorthim("ALGORITHM_FACE_RECOGNITION")
#husky.command_request_algorthim("ALGORITHM_OBJECT_TRACKING")
husky.command_request_algorthim("ALGORITHM_OBJECT_RECOGNITION")
#husky.command_request_algorthim("ALGORITHM_LINE_TRACKING")
#husky.command_request_algorthim("ALGORITHM_COLOR_RECOGNITION")
#husky.command_request_algorthim("ALGORITHM_TAG_RECOGNITION")
#husky.command_request_algorthim("ALGORITHM_OBJECT_CLASSIFICATION")

