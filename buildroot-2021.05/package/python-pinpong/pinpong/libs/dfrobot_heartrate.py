# -*- coding: utf-8 -*-
import time
import timeit
from pinpong.board import gboard,Pin,ADC

class HEARTRATE:
  SAMPLE_NUMBER                  = 100
  def __init__(self, board=None, mode =None, pin= None):
    if isinstance(board, int):
      pin = mode
      mode = board
      board = gboard
    elif board is None:
      board = gboard
    
    self.pin = pin
    self.board = board
    self.mode = mode
    self.value = [0 for i in range(self.SAMPLE_NUMBER)]
    self.value_count = 255
    self.now_time = 0
    self.last_time = 0
    self.sample_time = [0 for i in range(10)]
    self.time_flag = 0
    self.value_flag = 0
    
  def get_value(self):
    self.adc = ADC(Pin(self.pin))
    self.value_count = self.value_count + 1
    if self.value_count >= self.SAMPLE_NUMBER:
      self.value_count = 0
    self.value[self.value_count] = self.adc.read()
    return self.value[self.value_count]
    
  def get_rate(self):
    num = 1
    first = True
    told = 0
    t = 0
    value = 0
    tnew= time.time() * 1000
    while not value:
      self.get_value()
      if self.mode == 1:
        value = self.digital_get_rate()
      else:
        value = self.analog_get_rate()
      if value:
        num = 1
        told = time.time() * 1000
        break
      time.sleep(0.02)
      if time.time() * 1000 - told > 2000 or first:
        if first:
          first = False
          if num:
            print("Wait for valid data !")
            num = 0
      if time.time() * 1000 - tnew >15000:
        num = 1
        print("time out")
        return value
    return value
  
  def digital_get_rate(self):
    if self.value_count:
      count = self.value_count - 1
    else:
      count = self.SAMPLE_NUMBER - 1
    if self.value[self.value_count] > 1000 and self.value[count] < 20:
      self.now_time = time.time() * 1000
      dif_time = self.now_time - self.last_time
      self.last_time = self.now_time
      if dif_time > 300 or dif_time < 2000:
        self.sample_time[self.time_flag] = dif_time
        self.time_flag = self.time_flag + 1
        if self.time_flag > 9:
          self.time_flag = 0
        if 0 == self.sample_time[9]:
#          print("Wait for valid data !")
          return 0
        arrange = [0 for i in range(10)]
        for i in range(10):
          arrange[i] = self.sample_time[i]
        arrange_ = 0
        for i in range(9,-1,-1):
          for j in range(i):
            if arrange[j] > arrange[j+1]:
              arrange_ = arrange[j]
              arrange[j] = arrange[j+1]
              arrange[j+1] = arrange_
        if arrange[7] - arrange[3] > 120:
#          print("Wait for valid data !")
          return 0
        arrange_ = 0
        for i in range(3,8):
          arrange_ += arrange[i]
        value_time = (int)(300000 / arrange_)
        return value_time
        
  def analog_get_rate(self):
    self.min_number(self.value_count)
    if self.max_number(self.value_count):
      self.now_time = time.time() * 1000
      dif_time = self.now_time - self.last_time
      self.last_time = self.now_time
      
      if dif_time > 300 or dif_time < 2000:
        self.sample_time[self.time_flag] = dif_time
        self.time_flag = self.time_flag + 1
        if self.time_flag > 9:
          self.time_flag = 0
        if 0 == self.sample_time[9]:
#          print("Wait for valid data !")
          return 0
        arrange = [0 for i in range(10)]
        for i in range(10):
          arrange[i] = self.sample_time[i]
        arrange_ = 0
        for i in range(9,-1,-1):
          for j in range(i):
            if arrange[j] > arrange[j+1]:
              arrange_ = arrange[j]
              arrange[j] = arrange[j+1]
              arrange[j+1] = arrange_
        if arrange[7] - arrange[3] > 150:
          print("Wait for valid data !")
          return 0
        arrange_ = 0
        for i in range(3,8):
          arrange_ += arrange[i]
        value_time = (int)(300000 / arrange_)
        return value_time
    return 0
        
  def max_number(self, count):
    for i in range(4):
      if count < i:
        temp1 = self.SAMPLE_NUMBER + count - i
      else:
        temp1 = count - i
      if count < (i+1):
        temp2 = self.SAMPLE_NUMBER + (count - 1) - i
      else:
        temp2 = (count - 1) - i
      if self.value[temp1] <= self.value[temp2]:
        return 0
    if self.value_flag:
      self.value_flag = 1
      return 0
    else:
      self.value_flag = 1
      return 1
    
  def min_number(self, count):
    for i in range(4):
      if count < i:
        temp1 = self.SAMPLE_NUMBER + count - i
      else:
        temp1 = count - i
      if count < (i+1):
        temp2 = self.SAMPLE_NUMBER + (count - 1) - i
      else:
        temp2 = (count - 1) - i
      if self.value[temp1] >= self.value[temp2]:
        return 
    self.value_flag = 0
  