# -*- coding: utf_8 -*-
#风速仪

import time
from pinpong.board import gboard, ModBus

class Wind_Speed():

  def __init__(self, board=None, port=None, baudrate=9600, slave=0x02):
    self.master   = ModBus(port=port,baudrate=baudrate)
    self.slave    = slave
    self.baudrate = baudrate

  def get_wind_speed(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave,0x00,0x01)
    time.sleep(0.03)
    data = data[0]/10
    return data

  def set_addr(self,addr):
    if(addr>0x00)&(addr<=0xff):
      self.master.config_serial(baudrate=self.baudrate)
      rslt = self.master.write_multiple_reg(self.slave,0x1000,[addr])
      time.sleep(0.03)