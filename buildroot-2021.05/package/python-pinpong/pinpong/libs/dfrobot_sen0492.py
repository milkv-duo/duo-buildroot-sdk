# -*- coding: utf_8 -*-

import time
from pinpong.board import gboard, ModBus

class Laser_Ranging():
  slave = 0x50
  baudrate = 115200
  mode_dir = {0x01,0x02,0x03}
  baudrate_dir = {
      2400   : 0x00,
      4800   : 0x01,
      9600   : 0x02,
      19200  : 0x03,
      38400  : 0x04,
      57600  : 0x05,
      115200 : 0x06,
      230400 : 0x07,
      460800 : 0x08,
      921600 : 0x09
  }

  def __init__(self, board = None, port=None, baudrate=115200, slave=0x50):
    self.master   = ModBus(port=port,baudrate=baudrate)
    self.slave    = slave
    self.baudrate = baudrate
  
  def get_distance_mm(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave,0x34,0x01)
    time.sleep(0.03)
    return data[0]
  
  def get_input_status(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave,0x35,0x01)
    time.sleep(0.03)
    return data[0]
  
  def recovery_sys(self):
    self.master.config_serial(baudrate=self.baudrate)
    rslt = self.master.write_single_reg(self.slave,0x00,0x01)
    time.sleep(0.03)

  def set_threshold_mm(self,threshold_value):
    #set max threshold value 4000mm
    #set min threshold value 40mm
    if threshold_value > 4000 : threshold_value = 4000
    if threshold_value < 40   : threshold_value = 40
    self.master.config_serial(baudrate=self.baudrate)
    rslt = self.master.write_single_reg(self.slave,0x03,output_value=threshold_value)
    time.sleep(0.03)
  
  def set_baudrate(self,baudrate):
    if baudrate in self.baudrate_dir:
      self.master.config_serial(baudrate=self.baudrate)
      rslt = self.master.write_single_reg(self.slave,0x04,self.baudrate_dir[baudrate])

  def set_addr(self,addr):
    if(addr<0xFF)&(addr>0x00):
      self.master.config_serial(baudrate=self.baudrate)
      rslt = self.master.write_single_reg(self.slave,0x1a,addr)
  
  def set_meature_mode(self,mode):
    if mode in self.mode_dir:
      self.master.config_serial(baudrate=self.baudrate)
      rslt = self.master.write_single_reg(self.slave,0x36,mode)

#校准模式未完成 WIKI上通讯协议
#  def _get_calibrat_status(self):
#    self.master.config_serial(baudrate=self.baudrate)
#    calibrat_status = self.master.read_holding_reg(self.slave,0x37,0x01)
#    print(calibrat_status)
#
#  def calibrat(self):
#    self.master.config_serial(baudrate=self.baudrate)
#    rslt = self.master.write_single_reg(self.slave,0x37,0x04)
#    self._get_calibrat_status()




