# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_ina219 import INA219 #从libs中导入INA291库

Board("uno").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

ina = INA219(i2c_addr=0x45, bus_num=4) #初始化传感器，设置I2C地址

ina.begin()
#ina.linear_calibrate(1000)

while True:
  vol = ina.get_bus_voltage_mv()
  print("vol==%.2f mV"%vol)
  time.sleep(0.5)

  vol = ina.get_current_ma()
  print("vol==%.2f mA"%vol)
  time.sleep(0.5)

  vol = ina.get_power_mw()
  print("vol==%.2f mW"%vol)
  time.sleep(0.5)

  vol = ina.get_shunt_voltage_mv()
  print("vol==%.2f mV"%vol)
  time.sleep(0.5)  

  print("-------------------")
