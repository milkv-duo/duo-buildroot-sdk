# -*- coding:utf-8 -*-

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_as7341 import AS7341

Board("MILKV-DUO").begin()

as7341 = AS7341(bus_num=0)

while(as7341.begin() != True):
#    Detect if IIC can communicate properly 
  print("IIC init failed, please check if the wire connection is correct")
  time.sleep(1)

#  Integration time = (ATIME + 1) x (ASTEP + 1) x 2.78µs
#  Set the value of register ATIME, through which the value of Integration time can be calculated. The value represents the time that must be spent during data reading.
#  as7341.set_a_time(29)
#  Set the value of register ASTEP, through which the value of Integration time can be calculated. The value represents the time that must be spent during data reading.
#  as7341.set_a_step(599)
#  Set gain value(0~10 corresponds to X0.5,X1,X2,X4,X8,X16,X32,X64,X128,X256,X512)
#  as7341.set_again(7)
#  Enable LED
#  as7341.enable_led(True)
#  Set pin current to control brightness (1~20 corresponds to current 4mA,6mA,8mA,10mA,12mA,......,42mA)
#  as7341.control_led(10)

while True:
  #开启f1_f2__f3_f4_clear_nir的测量
  as7341.start_measure(0)
  data1 = as7341.read_spectral_data_one()
  print("F1(405-425nm):",data1['adf1'])
  print("F2(435-455nm):",data1['adf2'])
  print("F3(470-490nm):",data1['adf3'])
  print("F4(505-525nm):",data1['adf4'])   
  #开启f5_f6_f7_f8_clear_nir的测量
  as7341.start_measure(1)
  data2 = as7341.read_spectral_data_two()
  print("F5(545-565nm):",data2['adf5'])
  print("F6(580-600nm):",data2['adf6'])
  print("F7(620-640nm):",data2['adf7'])
  print("F8(670-690nm):",data2['adf8'])
  print("Clear:",data2['adclear'])
  print("NIR:"  ,data2['adnir'])

  time.sleep(1)
