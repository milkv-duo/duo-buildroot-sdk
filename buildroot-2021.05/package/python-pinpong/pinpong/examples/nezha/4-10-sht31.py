# -*- coding: utf-8 -*-

#Nezha
#实验效果：使用sht31传感器读取环境温度和湿度

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_sht3x import SHT3x

Board("nezha").begin()

sht31 =  SHT3x(iic_addr = 0x45,bus_num=4)



while sht31.begin() != 0:
  print("The initialization of the chip is failed, please confirm whether the chip connection is correct")
  time.sleep(1)

'''
   read_serial_number Read the serial number of the chip
   @return Return 32-digit serial number
'''
print("The chip serial number = %d "%sht31.read_serial_number())

'''
   softReset Send command resets via iiC, enter the chip's default mode single-measure mode, turn off the heater, 
   and clear the alert of the ALERT pin.
   @return Read the status register to determine whether the command was executed successfully, and returning true indicates success.
'''
if sht31.soft_reset() == False:
  print("Failed to reset the chip")

print("------------------Read data in single measurement mode-----------------------")

while True:
  '''
     @brief Get the measured temperature (in degrees Celsius)
     @return Return the float temperature data 
  '''
  print("\n-------------------------------------------")
  print("environment temperature(°C): %f C"%sht31.get_temperature_C())

  '''
     @brief Get the measured temperature (in degrees Fahrenheit)
     @return Return the float temperature data 
  '''
  print("environment temperature(F): %f F"%sht31.get_temperature_F())

  '''
     @brief Get measured humidity(%RH)
     @return Return the float humidity data
  '''
  print("relative humidity(%%RH): %f %%RH"%sht31.get_humidity_RH())
  time.sleep(1)
