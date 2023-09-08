# -*- coding: utf-8 -*-

#SEN0377:MEMS 气体传感器（CO, Alcohol, NO2 & NH3）
import time
from pinpong.board import Board,Pin  #导入ADC类实现模拟输入
from pinpong.libs.dfrobot_mics import DFRobot_MICS_I2C

Board("unihiker").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别

CALIBRATION_TIME = 0x03            # calibration time

mics = DFRobot_MICS_I2C()

mics.warm_up_time(CALIBRATION_TIME)

while True:
  ox_data = mics.get_adc_data(mics.OX_MODE)
  #red_data = mics.get_adc_data(mics.RED_MODE);
  print ("ox adc is %d"%ox_data)
  #print ("red adc is %d"%red_data)
  time.sleep(1)
  #mics.sleep_mode()