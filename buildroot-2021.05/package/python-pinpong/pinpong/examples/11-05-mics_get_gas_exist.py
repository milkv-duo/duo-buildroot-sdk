# -*- coding: utf-8 -*-

#SEN0377:MEMS 气体传感器（CO, Alcohol, NO2 & NH3）
import time
from pinpong.board import Board,Pin  #导入ADC类实现模拟输入
from pinpong.libs.dfrobot_mics import DFRobot_MICS_I2C

Board("uno").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()   #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化


CALIBRATION_TIME = 0x03            # calibration time

mics = DFRobot_MICS_I2C(bus_num=4)

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
  if mics.get_gas_exist(mics.C2H5OH) == mics.ERROR:
    print ("This gas does not exist!")
  else:
    print ("This gas exists!")
