# -*- coding: utf-8 -*-
from pinpong.board import gboard,I2C

class MLX90614:
  MLX90614_IIC_ADDR   = (0x5A)
  MLX90614_TA         = (0x06)
  MLX90614_TOBJ1      = (0x07)
  
  def __init__(self, board=None, i2c_addr=MLX90614_IIC_ADDR,bus_num=0):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard
      
    self.board = board
    self.i2c_addr = i2c_addr
    self.i2c = I2C(bus_num)

  def obj_temp_c(self):
    return round(self.__temperature(self.MLX90614_TOBJ1),2)    #Get celsius temperature of the object 

  def env_temp_c(self):
    return round(self.__temperature(self.MLX90614_TA),2)    #Get celsius temperature of the ambient

  def obj_temp_f(self):
    return round((self.__temperature(self.MLX90614_TOBJ1) * 9 / 5) + 32,2)  #Get fahrenheit temperature of the object

  def env_temp_f(self):
    return round((self.__temperature(self.MLX90614_TA) * 9 / 5) + 32, 2) #Get fahrenheit temperature of the ambient

  def __temperature(self,reg):
    temp = self.__get_reg(reg)*0.02-273.15             #Temperature conversion
    return temp

  def __get_reg(self,reg):
    data = self.i2c.readfrom_mem_restart_transmission(self.i2c_addr, reg, 3)
    result = (data[1]<<8) | data[0]
    return result
