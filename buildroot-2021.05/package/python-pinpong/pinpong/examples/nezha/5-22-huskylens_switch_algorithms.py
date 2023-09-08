# -*- coding: utf-8 -*-

#NeZha 
#实验效果：代码切换不同的模式。

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens

Board("nezha").begin()

husky = Huskylens()

husky.command_request_algorthim("ALGORITHM_FACE_RECOGNITION")
time.sleep(2)
husky.command_request_algorthim("ALGORITHM_OBJECT_TRACKING")
time.sleep(2)
husky.command_request_algorthim("ALGORITHM_OBJECT_RECOGNITION")
time.sleep(2)
husky.command_request_algorthim("ALGORITHM_LINE_TRACKING")
time.sleep(2)
husky.command_request_algorthim("ALGORITHM_COLOR_RECOGNITION")
time.sleep(2)
husky.command_request_algorthim("ALGORITHM_TAG_RECOGNITION")
time.sleep(2)
husky.command_request_algorthim("ALGORITHM_OBJECT_CLASSIFICATION")

