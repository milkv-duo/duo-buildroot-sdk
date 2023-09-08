# -*- coding: utf-8 -*-
import time
from pinpong.board import gboard,I2C

class URM09:
  ''' Configuration mode and parameters '''
  _MEASURE_MODE_AUTOMATIC = 0x80          # automatic mode
  _MEASURE_MODE_PASSIVE   = 0x00          # passive mode
  
  _CMD_DISTANCE_MEASURE   = 0x01          # passive mode configure registers
  
  _MEASURE_RANG_500       = 0x20          # Ranging from 500
  _MEASURE_RANG_300       = 0x10          # Ranging from 300 
  _MEASURE_RANG_150       = 0x00          # Ranging from 100
  
  ''' Enum register configuration '''
  SLAVEADDR_INDEX = 0
  PID_INDEX       = 1
  VERSION_INDEX   = 2
  DIST_H_INDEX    = 3
  DIST_L_INDEX    = 4
  
  TEMP_H_INDEX    = 5
  TEMP_L_INDEX    = 6

  CFG_INDEX       = 7
  CMD_INDEX       = 8
  REG_NUM         = 9
  
  ''' Conversion data '''
  txbuf      = [0]
  
  def __init__(self, board=None, i2c_addr=0x11, bus_num=0):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard

    ''' The i2c default device address is 0x11 '''
    self.i2c_addr = i2c_addr
    self.board = board
    self.i2c = I2C(board=board, bus_num=bus_num)

  ''' Set the automatic mode or the passive mode and the measurement distance '''
  def set_mode_range(self , Range , SetMode):
    self.txbuf = [Range | SetMode]
    self.URM09_set_regs(self.CFG_INDEX ,self.txbuf)

  ''' Write command registers and send ranging commands in passive mode '''
  def set_measurement(self):
    self.txbuf = [self._CMD_DISTANCE_MEASURE]
    self.URM09_set_regs(self.CMD_INDEX ,self.txbuf)

  ''' Get the temperature data of the register '''
  def temp_c(self):
    self.set_measurement()
    time.sleep(0.2)
    rslt = self.URM09_get_regs(self.TEMP_H_INDEX ,2)
    return ((rslt[0] << 8) + rslt[1])/10.0

  ''' Get the distance data of the register '''
  def distance_cm(self):
    self.set_measurement()
    time.sleep(0.2)
    rslt = self.URM09_get_regs(self.DIST_H_INDEX ,2)
    if ((rslt[0] << 8) + rslt[1]) < 32768:
      return ((rslt[0] << 8) + rslt[1])
    else : 
      return (((rslt[0] << 8) + rslt[1]) - 65536)

  ''' Modify i2c device address '''
  def modify_device_address(self ,Address):
    self.txbuf = [Address]
    self.URM09_set_regs(self.SLAVEADDR_INDEX ,self.txbuf)

  ''' read i2c device address '''
  def read_device_address(self):
    rslt = self.URM09_get_regs(self.SLAVEADDR_INDEX ,1)
    return rslt[0]

  ''' Write data to the i2c register '''
  def URM09_get_regs(self ,reg ,len):
    rslt = self.i2c.readfrom_mem(self.i2c_addr,reg,len)
    return rslt

  ''' Write data to the i2c register '''
  def URM09_set_regs(self ,reg ,data):
    self.i2c.writeto(self.i2c_addr, [reg] + data)
