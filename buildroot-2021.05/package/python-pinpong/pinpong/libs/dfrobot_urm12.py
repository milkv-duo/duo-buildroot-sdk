# -*- coding: utf_8 -*-

import time
from pinpong.board import gboard,ModBus

'''
PWM频率高于2K，频率和设定值可能存在较大偏差
频率高于2K请参考使用如下的频率值
46875HZ, 23437HZ, 15625HZ, 11718HZ,
9375HZ, 7812HZ, 6696HZ, 5859HZ, 5208HZ, 4687HZ, 4261HZ,
3906HZ, 3605HZ, 3348HZ, 3125HZ,
2929HZ, 2757HZ, 2604HZ, 2467HZ, 2343HZ, 2232HZ, 2130HZ, 2038HZ,
'''

class URM12():
  def __init__(self, board = None, port=None, baudrate=19200, slave=0x0b):
    self.master = ModBus(port=port,baudrate=baudrate)
    self.slave = slave
    self.baudrate = baudrate
    
  def pid(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 0, 1)
    return data[0]

  def vid(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 1, 1)
    return data[0]

  def slave_addr(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 2, 1)
    return data[0]
  
  '''
  def baudrate(self,baud):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 2, 1)
    return data[0]
  '''
  
  
  
  def distance_cm(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 5, 1)
    time.sleep(0.03)
    return data[0]

  def temp_c(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 6, 1)
    duty = data[0]/255
    time.sleep(0.03)
    return duty

  def temp_compensation(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 7, 1)
    time.sleep(0.03)
    return int(12*1000*1000/256/(data[0]+1))

  def mode(self,auto=False, compensation=False):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 8, 1)
    time.sleep(0.03)
    return data[0]