# -*- coding: utf-8 -*-

#实验效果：读取I2C CCS811空气质量传感器

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_ccs811 import CCS811, CCS811_Ecycle, CCS811_Emode


Board("RPI").begin()  #初始化


ccs811 = CCS811(bus_num=1)

#ccs811.write_base_line(baseline)                  #获取到的基线填入
while True:
    if(ccs811.check_data_ready()):
        print("CO2:"+str(ccs811.CO2_PPM())+" ppm")
        print("TVOC:"+str(ccs811.TVOC_PPB())+" ppb")
    else:
        print("data is not ready!")
    time.sleep(1)