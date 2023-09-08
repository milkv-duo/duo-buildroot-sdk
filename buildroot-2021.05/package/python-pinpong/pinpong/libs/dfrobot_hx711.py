# -*- coding: utf-8 -*-
import time
from pinpong.board import gboard,Pin

class HX711:
  def __init__(self, board=None, dout_Pin=None, sck_Pin=None):
    if isinstance(board, int):
      sck_Pin = dout_Pin
      dout_Pin = board
      board = gboard
    elif board is None:
      board = gboard
    
    self.board = board
    self.dout_Pin = dout_Pin
    self.sck_Pin = sck_Pin
    
    print(self.dout_Pin)
    print(self.sck_Pin)
    self.sck = Pin(self.sck_Pin, Pin.OUT)
    self.dout = Pin(self.dout_Pin, Pin.IN)
    
    self.sck.value(1)
    time.sleep(0.001)
    self.sck.value(0)
    
    self.average_value()
    self.set_offset(self.average_value())
    self.set_calibration()
  
  def set_calibration(self, scale = 1992.0):
    self.scale = scale
  
  def set_offset(self, offset):
    self.offset = offset
  
  def average_value(self, times = 25):
    sums = 0
    for i in range(times):
      sums += self.get_value()
    return sums / times
      
  def get_value(self):
    data = [0,0,0]
    while self.dout.value():pass
    for i in range(3):
      for j in range(8):
        self.sck.value(1)
        data[2 - i] |= self.dout.value() << (7-j)
#        print(self.dout.value())
        self.sck.value(0)

    
    self.sck.value(1)
    self.sck.value(0)
#    print(data)
#    while True:pass
    ret = (data[2] << 16) | (data[1] << 8) | (data[0]^0x800000)
    return ret
  
  def read_weight(self):
    val = self.average_value() - self.offset
    return val / self.scale
  