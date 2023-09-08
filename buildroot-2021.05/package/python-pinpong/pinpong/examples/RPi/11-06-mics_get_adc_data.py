# -*- coding: utf-8 -*-

#SEN0377:MEMS 气体传感器（CO, Alcohol, NO2 & NH3）
import time
from pinpong.board import Board,Pin  #导入ADC类实现模拟输入
from pinpong.libs.dfrobot_mics import DFRobot_MICS_I2C

Board("RPI").begin()  #初始化

CALIBRATION_TIME = 0x03            # calibration time

mics = DFRobot_MICS_I2C(bus_num=1) #根据支持的i2c设备填写

mics.warm_up_time(CALIBRATION_TIME)

while True:
  ox_data = mics.get_adc_data(mics.OX_MODE)
  #red_data = mics.get_adc_data(mics.RED_MODE);
  print ("ox adc is %d"%ox_data)
  #print ("red adc is %d"%red_data)
  time.sleep(1)
  #mics.sleep_mode()