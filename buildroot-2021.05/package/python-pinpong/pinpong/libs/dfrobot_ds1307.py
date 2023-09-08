# -*- coding: utf-8 -*-
from pinpong.board import gboard,I2C
import time 

class RET:
  def __init__(self):
    self.weeks = ["Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"]
    self.year = 0
    self.month = 0
    self.day = 0
    self.week_num = 0
    self.hours = 0
    self.minute = 0
    self.second = 0
    
class DS1307:
  DS1307_SEC      = 0
  DS1307_MIN      = 1
  DS1307_HR       = 2
  DS1307_DOW      = 3
  DS1307_DATE     = 4
  DS1307_MTH      = 5
  DS1307_YR       = 6
  
  DS1307_BASE_YR  = 2000
  
  HIGH                   = 0
  LOW                    = 1
  DS1307_SQW1HZ          = 2
  DS1307_SQW4KHZ         = 3
  DS1307_SQW8KHZ         = 4
  DS1307_SQW32KHZ        = 5
  
  DS1307_CTRL_ID         = 0x68
  DS1307_CLOCKHALT       = 0x80
  DS1307_LO_BCD          = 0x0f
  DS1307_HI_BCD          = 0xf0
  
  DS1307_HI_SEC          = 0x70
  DS1307_HI_MIN          = 0x70
  DS1307_HI_HR           = 0x30
  DS1307_LO_DOW          = 0x07
  DS1307_HI_DATE         = 0x30
  DS1307_HI_MTH          = 0x30
  DS1307_HI_YR           = 0xf0
  
  DS1307_LOW_BIT         = 0x00
  DS1307_HIGH_BIT        = 0x80
  DS1307_SQW1HZ_BIT      = 0x10
  DS1307_SQW4KHZ_BIT     = 0x11
  DS1307_SQW8KHZ_BIT     = 0x12
  DS1307_SQW32KHZ_BIT    = 0x13
  
  
  
  def __init__(self, board=None, i2c_addr=0x68, bus_num=0):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard
    
    self.board = board
    self.i2c_addr = i2c_addr
    self.i2c = I2C(bus_num)

    self.ret = RET()
    self.rtc = [0 for i in range(7)]

  def get_time(self):
    data = self.read_reg()
    while data is None:
      data = self.read_reg()
      time.sleep(0.01)
    self.ret.second = (10*((data[0] & self.DS1307_HI_SEC)>>4) + (data[0] & self.DS1307_LO_BCD))
    self.ret.minute = (10*((data[self.DS1307_MIN] & self.DS1307_HI_MIN)>>4) + (data[self.DS1307_MIN] & self.DS1307_LO_BCD))
    self.ret.hours  = (10*((data[self.DS1307_HR] & self.DS1307_HI_HR)>>4) + (data[self.DS1307_HR] & self.DS1307_LO_BCD))
    week_num = data[self.DS1307_DOW] & self.DS1307_LO_DOW
    self.ret.week = self.ret.weeks[week_num - 1]
    self.ret.day = (10*((data[self.DS1307_DATE] & self.DS1307_HI_DATE)>>4) + (data[self.DS1307_DATE] & self.DS1307_LO_BCD))
    self.ret.month = (10*((data[self.DS1307_MTH] & self.DS1307_HI_MTH)>>4) + (data[self.DS1307_MTH] & self.DS1307_LO_BCD))
    self.ret.year = (10*((data[self.DS1307_YR] & self.DS1307_HI_YR)>>4) + (data[self.DS1307_YR] & self.DS1307_LO_BCD)) + self.DS1307_BASE_YR
    return self.ret
    
  def set_time(self, value):
    if len(value) == 7:
      if not (0 <= value[6] <= 99):
        raise ValueError("The year is incorrect. Please enter a value between 0 and 99")
      if not (1 <= value[5] <= 12):
        raise ValueError("The month is incorrect. Please enter a value between 1 and 12")
      if not (1 <= value[4] <= 31):
        raise ValueError("The date is incorrect. Please enter a value between 1 and 31")
      if not (1 <= value[3] <= 7):
        raise ValueError("Week is incorrect. Please enter a value between 1 and 7")
      if not (0 <= value[2] <= 23):
        raise ValueError("Hour is incorrect. Please enter a value between 0 and 23")
      if not (0 <= value[1] <= 59):
        raise ValueError("Minute is incorrect. Please enter a value between 0 and 59")
      if not (0 <= value[0] <= 59):
        raise ValueError("Seconds is incorrect. Please enter a value between 0 and 59")
      if value[5] == 2:
        if (value[6]+2000)%400 == 0 or ((value[6]+2000)%4 == 0 and (value[6]+2000)%100 != 0):
          if value[4] > 29:
            raise ValueError("There is no %dth in February %d"%(value[4], (value[6]+2000)))
        else:        
          if value[4] > 28:
            raise ValueError("There is no %dth in February %d"%(value[4], (value[6]+2000)))
      state = self.rtc[self.DS1307_SEC] & self.DS1307_CLOCKHALT
      self.rtc[self.DS1307_SEC] = (state | ((value[0]//10)<<4)) + (value[0] % 10)
      self.rtc[self.DS1307_MIN] = ((value[1] // 10)<<4) + (value[1] % 10)
      self.rtc[self.DS1307_HR] = ((value[2] //10)<<4) + (value[2] % 10)
      self.rtc[self.DS1307_DOW] = value[3]
      self.rtc[self.DS1307_DATE] = ((value[4] // 10)<<4) + (value[4] % 10)
      self.rtc[self.DS1307_MTH] = ((value[5] // 10)<<4) + (value[5] % 10)
      self.rtc[self.DS1307_YR] = ((value[6] // 10)<<4) + (value[6] % 10)
      for i in range(7):
        self.write_reg(0x00,self.rtc)

  def stop(self):
    self.rtc[self.DS1307_SEC] = self.rtc[self.DS1307_SEC] | self.DS1307_CLOCKHALT
    self.write_reg(0x00, self.rtc)

  def start(self):
    self.rtc[self.DS1307_SEC] = self.rtc[self.DS1307_SEC] & (~self.DS1307_CLOCKHALT)
    self.write_reg(0x00, self.rtc)

  def write_reg(self, reg, data):
    self.i2c.writeto_mem(self.i2c_addr, reg, data)

  def read_reg(self, reg = 0x00, lens = 7):
    return self.i2c.readfrom_mem(self.i2c_addr, reg, lens)
    
###############################方波信号输出功能设置############################ 
  
  def set_output(self, set_value):
    if set_value == self.HIGH:
      out = self.DS1307_HIGH_BIT
    elif set_value == self.LOW: 
      out = self.DS1307_LOW_BIT
    elif set_value == self.DS1307_SQW1HZ:
      out = self.DS1307_SQW1HZ_BIT
    elif set_value == self.DS1307_SQW4KHZ:
      out = self.DS1307_SQW4KHZ_BIT
    elif set_value == self.DS1307_SQW8KHZ:
      out = self.DS1307_SQW8KHZ_BIT
    elif set_value == self.DS1307_SQW32KHZ:
      out = self.DS1307_SQW32KHZ_BIT
    else:
      out = self.DS1307_LOW_BIT
    self.write_reg(0x07, [out])
    
