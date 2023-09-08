# -*- coding: utf-8 -*-
import time
from pinpong.board import gboard,Pin,ADC

class TDS:
  SCOUNT                               = 30
  temperature                          = 25
  def __init__(self, board=None, tds_pin=None):
    if isinstance(board, Pin):
      tds_pin = board
      board = gboard
    elif board is None:
      board = gboard
    
    self.board = board
    self.tds_pin = tds_pin
    self.adc0 = ADC(self.tds_pin)
    time.sleep(0.1)
    # Pin(self.tds_pin, Pin.IN)
    # self.adc0 = ADC(Pin(self.tds_pin))
#    self.adc0 = Pin(self.tds_pin, Pin.ANALOG)
    self.analog_buffer = [0 for i in range(30)]
    self.analog_buffer_temp = [0 for i in range(30)]
    self.analog_buffer_index = 0

  def get_value(self):
    analog_sample_timepoint = time.time()
    while (time.time() - analog_sample_timepoint)*1000 < 40: pass
    analog_sample_timepoint = time.time()
    for i in range(30):
        self.analog_buffer[i] = self.adc0.read()
        
    print_timepoint = time.time()
    while (time.time() - print_timepoint)*1000 > 800: pass
    print_timepoint = time.time()
    for i in range(30):
        self.analog_buffer_temp[i] = self.analog_buffer[i]
    average_voltage = self.get_median_num() * 5 / 1024
    compensation_coefficient = 1 + 0.02 * (self.temperature - 25)
    compensation_volatge = average_voltage / compensation_coefficient
    tds_value = (int)((133.42 * compensation_volatge * compensation_volatge * compensation_volatge - 255.86 * compensation_volatge * compensation_volatge + 857.39 * compensation_volatge) * 0.5)
    return tds_value

  def get_median_num(self):
    for j in range(30 - 1):
        for i in range(30 - j - 1):
            if self.analog_buffer_temp[i] > self.analog_buffer_temp[i+1]:
                temp = self.analog_buffer_temp[i]
                self.analog_buffer_temp[i] = self.analog_buffer_temp[i+1]
                self.analog_buffer_temp[i+1] = temp
    if self.SCOUNT & 1 > 0:
        btemp = self.analog_buffer_temp[(self.SCOUNT - 1) / 2]
    else:
        
        #btemp = (self.analog_buffer_temp[int(self.SCOUNT / 2)] + self.analog_buffer_temp[int(self.SCOUNT / 2 - 1)]) / 2
        btemp = int((self.analog_buffer_temp[14] + self.analog_buffer_temp[15])) / 2
    return btemp