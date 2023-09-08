# -*- coding: utf-8 -*-

#实验效果：读取模拟量
#接线：使用windows或linux电脑连接一块arduino主控板，16位AD模数模块接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_ads1115 import ADS1115

Board("uno").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

ads = ADS1115() #默认地址0x48
#ads = ADS1115(i2c_addr = 0x49)

while True:
    #channel-0的模拟输出
    print(ads.read_voltage(0))
    time.sleep(0.1)
