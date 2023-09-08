# -*- coding: utf-8 -*-

#NeZha 
#实验效果：读取VOC的值

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_veml6075 import VEML6075 #从libs中导入SGP40库

Board("nezha").begin()

veml=VEML6075()


while veml.begin() != True:
    print("VEML6075 begin faild")
    time.sleep(2)
print("VEML6075 begin succeed")

while True:
    Uva = veml.getUva()           # get UVA
    Uvb = veml.getUvb()           # get UVB
    Uvi = veml.getUvi(Uva, Uvb)   # get UVI
    print("")
    print("======== start print ========")
    print("UVA:     %.2f" %(Uva))
    print("UVB:     %.2f" %(Uvb))
    print("UVI:     %.2f" %(Uvi))
    print("mw/cm^2: %.2f" %(veml.Uvi2mwpcm2(Uvi)))
    print("======== end print =========")
    time.sleep(1)
