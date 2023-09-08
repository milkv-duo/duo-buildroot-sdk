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
    Gets the power mode of the sensor
    The sensor is in sleep mode when power is on,so it needs to wake up the sensor. 
    The data obtained in sleep mode is wrong
  '''
  if mics.get_power_mode() == mics.SLEEP_MODE:
    mics.wakeup_mode()
    print("sleep mode,   wake up sensor success!")
  else:
    mics.sleep_mode()
    print("wake up mode, sleep sensor success!")
  time.sleep(3)
