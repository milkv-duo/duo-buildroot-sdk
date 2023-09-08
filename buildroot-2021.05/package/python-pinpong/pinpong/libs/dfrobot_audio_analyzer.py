# -*- coding: utf-8 -*-
import time
from pinpong.board import gboard,Pin,ADC

class AUDIO:
  def __init__(self, board=None, StrobePin=None, RstPin=None, AnalogPin=None):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard
    
    self.board = board
    self._strobe_pin = StrobePin
    self._RST_pin = RstPin
    self._DC_pin = AnalogPin
    self.rst_state = False

  def begin(self,  StrobePin, RstPin, AnalogPin):
    self._strobe_pin = StrobePin
    self._RST_pin = RstPin
    self._DC_pin = AnalogPin   
    self.strobe = Pin(self._strobe_pin, Pin.OUT)
    self.RST = Pin(self._RST_pin, Pin.OUT)
    self.rst_module()
  
  def rst_module(self):
    self.strobe.value(0)
    self.RST.value(1)
    self.strobe.value(1)
    self.strobe.value(0)
    self.RST.value(0)
    time.sleep(0.000072)
  
  def read_freq(self):
    # if not self.rst_state:
    #   self._TimepointSt = time.time() * 1000
    #   self.rst_state = True
    # else:
    #   self._TimepointNow = time.time() * 1000
    #   if self._TimepointNow - self._TimepointSt > 3000:
    #     self.rst_module()
    #     self.rst_state = False
    # value = [0 for i in range(7)]
    # self.DC = ADC(Pin(Pin.A5))
    # for i in range(7):
    #   time.sleep(0.00001)
    #   value[i] = self.DC.read()
    #   time.sleep(0.00005)
    #   self.strobe.value(1)
    #   time.sleep(0.000018)
    #   self.strobe.value(0)
    # return value

    for i in range(7):
      time.sleep(0.00005)
      self.strobe.value(1)
      time.sleep(0.000018)
      self.strobe.value(0)
    return 0