# -*- coding: utf_8 -*-
#风向变送器

import time
from pinpong.board import gboard, ModBus

class Wind_Direction():
  def __init__(self, board=None, port=None, baudrate=9600, slave=0x03):
    self.master   = ModBus(port=port,baudrate=baudrate)
    self.slave    = slave
    self.baudrate = baudrate
  
  def get_wind_direction(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave,0x00,0x01)
    time.sleep(0.03)
    return data[0]
  
  def set_addr(self,addr):
    if(addr>0x00)&(addr<=0xff):
      self.master.config_serial(baudrate=self.baudrate)
      rslt = self.master.write_multiple_reg(self.slave,0x1000,[addr])
      time.sleep(0.03)


