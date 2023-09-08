# -*- coding: utf-8 -*-

#实验效果：读取模拟量
#接线：使用windows或linux电脑连接一块arduino主控板，16位AD模数模块接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_ads1115 import ADS1115

ip = "192.168.0.90"
port = 8081

Board(ip, port)

ads = ADS1115() #默认地址0x48
#ads = ADS1115(i2c_addr = 0x49)

ads.set_gain(ADS1115.GAIN_TWOTHIRDS)
ads.set_comp_mode(ADS1115.COMPMODE_TRAD)
ads.set_comp_pol(ADS1115.COMPPOL_LOW)
ads.set_comp_que(ADS1115.COMPQUE_ONE)
ads.set_comp_lat(ADS1115.COMPLAT_LATCH)
ads.set_high_threshold(32000)
time.sleep(1)

while True:
    print("The voltage difference between A0 and A1 ")
    print(ads.comparator_voltage("01"))
    time.sleep(0.5)

