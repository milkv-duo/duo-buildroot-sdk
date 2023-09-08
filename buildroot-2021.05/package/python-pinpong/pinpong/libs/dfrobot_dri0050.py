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

class DRI0050():
  def __init__(self, board = None, port=None, baudrate=9600, slave=0x32):
    self.master = ModBus(port=port,baudrate=baudrate)
    self.slave = slave
    self.baudrate = baudrate
    
  def get_pid(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 0, 1)
    time.sleep(0.03)
    return data[0]

  def get_vid(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 1, 1)
    time.sleep(0.03)
    return data[0]

  def get_addr(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 2, 1)
    time.sleep(0.03)
    return data[0]

  def get_version(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 5, 1)
    time.sleep(0.03)
    return data[0]

  def get_duty(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 6, 1)
    duty = data[0]/255
    time.sleep(0.03)
    return duty

  def get_freq(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 7, 1)
    time.sleep(0.03)
    return int(12*1000*1000/256/(data[0]+1))

  def get_enable(self):
    self.master.config_serial(baudrate=self.baudrate)
    data = self.master.read_holding_reg(self.slave, 8, 1)
    time.sleep(0.03)
    return data[0]

  def set_duty(self, duty):
    #if duty > 1 or duty < 0:
    #  raise raise ValueError("duty must be in [0,1]")
    v = int(duty*255)
    self.master.config_serial(baudrate=self.baudrate)
    rslt = self.master.write_single_reg(self.slave, 6, output_value=v)
    time.sleep(0.03)

  def set_freq(self, freq):
    #if freq < 183 or freq > 46875:
    #  raise raise ValueError("duty must be in [183,46875]")
    #v = int((freq-48000)/(90000)*255) #0-255 (138HZ-46875HZ)
    #12*1000/256/30
    v = int(12*1000*1000/256/freq) - 1
    self.master.config_serial(baudrate=self.baudrate)
    rslt = self.master.write_single_reg(self.slave, 7, output_value=v)
    time.sleep(0.03)

  def set_enable(self, enable):
    self.master.config_serial(baudrate=self.baudrate)
    rslt = self.master.write_single_reg(self.slave, 8, output_value=enable)
    time.sleep(0.03)

  def pwm(self, freq, duty):
    v=[]
    v.append(int(duty*255))
    v.append(int(12*1000*1000/256/freq) - 1)
    self.master.config_serial(baudrate=self.baudrate)
    rslt = self.master.write_multiple_reg(self.slave, 6, output_value=v)
    time.sleep(0.03)
