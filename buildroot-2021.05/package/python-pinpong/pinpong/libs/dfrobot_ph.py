import time
import sys
from pinpong.board import gboard, Pin, ADC

_temperature      = 25.0
_phValue          = 7.0
_acidVoltage      = 2032.44
_neutralVoltage   = 1500.0
_voltage          = 1500.0
class PH2():
  def __init__(self, board=None, pin=None):
    if isinstance(board, Pin):
      pin = board
      board = gboard
    elif board is None:
      board = gboard
      pin = board
    self.board = board
    self.adc = ADC(pin)
    self.map = 4095
    self.aref = 3300

  def read_ph(self,temperature=_temperature):
    global _acidVoltage
    global _neutralVoltage
    if self.adc.board.boardname == 'UNIHIKER' or self.adc.board.boardname == 'HANDPY':
      self.map = 4095
      self.aref = 3300
    elif self.adc.board.boardname == 'MICROBIT':
      self.map = 1024
      self.aref = 3300
    else:
      self.map = 1023
      self.aref = 5000
    voltage = self.adc.read() / self.map * self.aref
    slope     = (7.0-4.0)/((_neutralVoltage-1500.0)/3.0 - (_acidVoltage-1500.0)/3.0)
    intercept = 7.0 - slope*(_neutralVoltage-1500.0)/3.0
    _phValue  = slope*(voltage-1500.0)/3.0+intercept
    return round(_phValue,2)

class PH():
  def __init__(self, board=None, pin=None):
    if isinstance(board, Pin):
      pin = board
      board = gboard
    elif board is None:
      board = gboard
      pin = board
    self.board = board
    self.adc = ADC(pin)
    self.pHArray = [0 for i in range(20)]
    self.first = True
    self.pHArrayIndex = 0

  def read_ph(self,temperature=_temperature):
    count = 1
    if self.first:
      count = 20
      self.first = False
    for i in range(count):
      analog = self.adc.read()
      self.pHArray[self.pHArrayIndex] = analog
      self.pHArrayIndex = (self.pHArrayIndex + 1) % 20
      time.sleep(0.02)
    voltage = self.avergeArray() * 5.0 / 1024
    return round(3.5 * voltage + 0.0, 2)
  
  def avergeArray(self):
    amount=0
    if(self.pHArray[0] < self.pHArray[1]):
      min = self.pHArray[0]
      max = self.pHArray[1]
    else:
      min = self.pHArray[1]
      max = self.pHArray[0]
    for i in range(20):
      if(self.pHArray[i] < min):
        amount += min
        min = self.pHArray[i]
      else:
        if(self.pHArray[i] > max):
          amount += max
          max = self.pHArray[i]
        else:
          amount += self.pHArray[i]
    avg = amount / 20
    return avg