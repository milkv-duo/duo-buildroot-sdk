# -*- coding: utf-8 -*-

#SEN0377:MEMS 气体传感器（CO, Alcohol, NO2 & NH3）
import time
from pinpong.board import Board,Pin  #导入ADC类实现模拟输入
from pinpong.libs.dfrobot_mics import DFRobot_MICS_I2C

Board("milkv-duo").begin()  #初始化，选择板型

CALIBRATION_TIME = 0x03            # calibration time

mics = DFRobot_MICS_I2C(bus_num=1)

mics.warm_up_time(CALIBRATION_TIME)

while True:
  '''
    Type of detection gas
    CO       = 0x01  (Carbon Monoxide)
    CH4      = 0x02  (Methane)
    C2H5OH   = 0x03  (Ethanol)
    H2       = 0x06  (Hydrogen)
    NH3      = 0x08  (Ammonia)
    NO2      = 0x0A  (Nitrogen Dioxide)
  '''
  gas_concentration = mics.get_gas_ppm(mics.C2H5OH)
  print("gas concentration is %.1f"%gas_concentration)
  time.sleep(1)
  