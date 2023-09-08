# -*- coding: utf-8 -*-

#NeZha 
#实验效果：使用SGP40传感器读取环境VOC的值

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_sgp40 import SGP40 #从libs中导入SGP40库

Board("nezha").begin()


sgp40=SGP40()

#set Warm-up time
print('Please wait 10 seconds...')
sgp40.begin(10)

#sgp40.set_envparams(50,25)

while True:
    print('Voc index : %d'%(sgp40.get_voc_index()))
    time.sleep(1)