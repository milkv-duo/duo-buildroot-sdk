# -*- coding: utf_8 -*-

import time
from pinpong.board import gboard,ModBus

class XY_MD02():
  def __init__(self, board = None, uart=None, baudrate=None, host=None, slave=2):
    self.master = ModBus(uart=uart,baudrate=baudrate)
    self.slave = 2
    self.baudrate = baudrate
    
  def get_temp(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_input_reg(self.slave, 1, 1)
    return data[0]/10

  def get_humi(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_input_reg(self.slave, 2, 1)
    return data[0]/10
