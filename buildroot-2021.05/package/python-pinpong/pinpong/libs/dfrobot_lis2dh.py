# -*- coding: utf-8 -*-
import time
from pinpong.board import gboard,I2C

class LIS2DH:
  LIS2DH12_RANGE_2GA	     = 0x00
  LIS2DH12_RANGE_4GA	     = 0x10
  LIS2DH12_RANGE_8GA	     = 0x20
  LIS2DH12_RANGE_16GA	     = 0x30

  def __init__(self, board=None, i2c_addr=0x18, bus_num=0):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard
    
    self.board = board
    self.i2c_addr = i2c_addr
    self.i2c = I2C(bus_num)
    self.x = 0
    self.y = 0
    self.z = 0

  def init(self, range):
    self.set_range(range)
    value = [0x2F, 0x00, 0x00, range, 0x00, 0x00]
    self.write_reg(0xA0, value)
    return True

  def read_XYZ(self):
    data = self.read_reg(0xA8, 6)
    for i in range(len(data)):
      if data[i] > 127:
        data[i] = data[i] - 256
    x = data[1] * 256 + data[0]
    y = data[3] * 256 + data[2]
    z = data[5] * 256 + data[4]
    self.x = -(x*1000 // (1024 * self.scale_vel))
    self.y = -(y*1000 // (1024 * self.scale_vel))
    self.z = -(z*1000 // (1024 * self.scale_vel))

  def set_range(self, range):
    if range == self.LIS2DH12_RANGE_2GA:
        self.scale_vel = 16
    elif range == self.LIS2DH12_RANGE_4GA:
        self.scale_vel = 8
    elif range == self.LIS2DH12_RANGE_8GA:
        self.scale_vel = 4
    elif range == self.LIS2DH12_RANGE_16GA:
        self.scale_vel = 2
    else:
        self.scale_vel = 16

  def write_reg(self, reg, data):
    self.i2c.writeto_mem(self.i2c_addr, reg, data)
  
  def read_reg(self, reg, lens):
    reg = reg | 0x80
    return self.i2c.readfrom_mem(self.i2c_addr, reg, lens)
