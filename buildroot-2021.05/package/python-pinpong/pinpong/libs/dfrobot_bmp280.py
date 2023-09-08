# -*- coding: utf-8 -*-
import time
import math
from pinpong.board import gboard,I2C

class BMP280:
  sdo_low                       = 0
  sdo_high                      = 1
  
  sampling_no                   = 0
  sampling_X1                   = 1
  sampling_X2                   = 2
  sampling_X4                   = 3
  sampling_X8                   = 4
  sampling_X16                  = 5
  
  config_filter_off              = 0
  config_filter_X2               = 1
  config_filter_X4               = 2
  config_filter_X8               = 3
  config_filter_X16              = 4
  
  config_standby_0_5             = 0
  config_standby_62_5            = 1
  config_standby_125             = 2
  config_standby_250             = 3
  config_standby_500             = 4
  config_standby_1000            = 5
  config_standby_2000            = 6
  config_standby_4000            = 7
                                 
  ctrl_meas_mode_sleep            = 0
  ctrl_meas_mode_forced           = 1
  ctrl_meas_mode_normal           = 3
  
  reset_addr                     = 0xe0
  chip_id_addr                   = 0xd0
  calib_addr                     = 0x88
  ctrlMeas_addr                  = 0xf4
  config_addr                    = 0xf5
  temp_addr                      = 0xfa
  press_addr                     = 0xf7
  
  BMP280_REG_CHIP_ID_DEFAULT     = 0x58
  
  def __init__(self, board=None, i2c_addr= 0x77, bus_num=0, mode=1):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard
    self.board = board
    self.i2c = I2C(bus_num)
    if mode == self.sdo_low:
      self.i2c_addr = 0x76
    else:self.i2c_addr = 0x77
    self._sCalib = [0]
    self._t_fine = 0
    self.reset()
    
  def begin(self):
    temp = self.get_reg(self.chip_id_addr)
    if temp[0]== self.BMP280_REG_CHIP_ID_DEFAULT:
      self.reset()
      time.sleep(0.2)
      self.get_calibrate()
      self.set_ctrl_meas_sampling_press(self.sampling_X8)
      time.sleep(0.01)
      self.set_ctrl_meas_sampling_temp(self.sampling_X8)
      time.sleep(0.01)
      self.set_config_filter(self.config_filter_off)
      time.sleep(0.01)
      self.set_config_standby(self.config_standby_125)
      time.sleep(0.01)
      self.set_ctrl_meas_mode(self.ctrl_meas_mode_normal)
      time.sleep(0.01)
    else:
      return False
    time.sleep(0.1)
    self.seaLevelPressure = self.read_sea_level(525)
    return True
  
  def read_sea_level(self, altitude):
    pressure = self.pressure_p()
    return (pressure / math.pow(1.0 - (altitude / 44330.0), 5.255))
  
  def altitude_m(self, pressure):
    return (1.0 - math.pow(pressure / self.seaLevelPressure, 0.190284)) * 287.15 / 0.0065

  def pressure_p(self):
    self.temp_c()
    raw = self.get_pressure_raw()
    v1 = self._t_fine - 128000
    v2 = v1 * v1 * self._sCalib[8]
    v2 = v2 + ((v1 * self._sCalib[7]) << 17)
    v2 = v2 +((self._sCalib[6]) << 35)
    v1 = ((v1 * v1 * self._sCalib[5]) >> 8) + ((v1 * self._sCalib[4]) << 12)
    v1 = (((1 << 47) + v1) * self._sCalib[3]) >> 33
    if v1 == 0:
      return 0
    rslt = 1048576 - raw
    rslt = (((rslt << 31) - v2) * 3125) // v1
    v1 = ((self._sCalib[11]) * (rslt >> 13) * (rslt >> 13)) >> 25
    v2 = ((self._sCalib[10]) * rslt) >> 19
    rslt = ((rslt + v1 + v2) >> 8) + ((self._sCalib[9]) << 4)
    ret = (int)(rslt / 256)
    return ret
  
  def get_pressure_raw(self):
    data = self.read_reg(self.press_addr , 3)
    temp = data[2] >> 4
    val = (data[0] << 12) | (data[1] << 4) | temp
    return val
  
  def temp_c(self):
    raw = self.get_temperature_raw()
    v1 = (((raw >> 3) - (self._sCalib[0] << 1)) * (self._sCalib[1])) >> 11
    v2 = (((((raw >> 4) - (self._sCalib[0])) * ((raw >> 4) - (self._sCalib[0]))) >> 12) * (self._sCalib[2])) >> 14
    self._t_fine = v1 + v2
    rslt = (self._t_fine * 5 + 128) >> 8
    return (rslt / 100)
  
  def get_temperature_raw(self):
    data = self.read_reg(self.temp_addr, 3)
    temp = data[2] >> 4
    val = (data[0] << 12) | (data[1] << 4) | temp
    return val
    
  def set_ctrl_meas_mode(self, eCtrlMeasMode):
    reg_field = 0x3
    reg_val = eCtrlMeasMode
    self.write_reg_bits(self.ctrlMeas_addr, reg_field, reg_val)
  
  def set_config_standby(self, config_standby):
    reg_field = 0xe0
    if config_standby == self.config_standby_0_5 or config_standby == self.config_standby_62_5 or config_standby == self.config_standby_125 or config_standby == self.config_standby_250 or config_standby == self.config_standby_500 or config_standby == self.config_standby_1000 or config_standby == self.config_standby_2000 or config_standby == self.config_standby_4000:
      reg_val = 32 * config_standby
    else:
      reg_val = 0x40
    self.write_reg_bits(self.config_addr, reg_field, reg_val)
  
  def set_config_filter(self, config_filter):
    reg_field = 0x1c
    if config_filter == self.config_filter_off or config_filter == self.config_filter_X2 or config_filter == self.config_filter_X4 or config_filter == self.config_filter_X8 or config_filter == self.config_filter_X16:
      reg_val = 4 * config_filter
    else:
      reg_val = 0x0
    self.write_reg_bits(self.config_addr, reg_field, reg_val)
  
  def write_reg_bits(self, reg, field, val):
    data = self.read_reg(reg, 1)
    reg_val = data[0] & (~field)
    reg_val = reg_val | val
    self.write_reg(reg, [reg_val])
  
  def set_ctrl_meas_sampling_temp(self, sampling):
    reg_field = 0xe0
    if sampling == self.sampling_no or sampling == self.sampling_X1 or sampling == self.sampling_X2 or sampling == self.sampling_X4 or sampling == self.sampling_X8 or sampling == self.sampling_X16:
      reg_val = 32 * sampling
    else:
      reg_val = 0x80
    self.write_reg_bits(self.ctrlMeas_addr, reg_field, reg_val)
    
  def set_ctrl_meas_sampling_press(self, sampling):
    reg_field = 0x1c
    if sampling == self.sampling_no or sampling == self.sampling_X1 or sampling == self.sampling_X2 or sampling == self.sampling_X4 or sampling == self.sampling_X8 or sampling == self.sampling_X8 or sampling == self.sampling_X16:
      reg_val = 4 * sampling
    else:
      reg_val = 0x10
    self.write_reg_bits(self.ctrlMeas_addr, reg_field, reg_val)
  
  def get_calibrate(self):
    value = list()
    data = self.read_reg(self.calib_addr, 26)
    val = data[1] << 8 | data[0]
    value.append(val)
    num = 0
    for i in range(2):
      val = self.bin2dec_auto(data[num+2],data[num+3])
      value.append(val)
      num += 2
    val = data[7] << 8 | data[6]
    value.append(val)
    num += 2
    for i in range(8):
      val = self.bin2dec_auto(data[num+2],data[num+3])
      value.append(val)
      num += 2
    self._sCalib = value
  
  def get_reg(self, reg):
    return self.read_reg(reg, 1)
   
  def reset(self):
    temp = [0xb6]
    self.write_reg(self.reset_addr, temp)
    time.sleep(0.1)
    
  def write_reg(self, reg, data):
    self.i2c.writeto_mem(self.i2c_addr, reg, data)
    
  def read_reg(self, reg, len):
    return self.i2c.readfrom_mem(self.i2c_addr, reg, len)
    
  def bin2dec(self, a):
        a_reverse = self.reverse(a)  # 取反
        a_add_1 = self.add_1(a_reverse)  # 二进制加1
        a_int = -int(a_add_1, 2)
        return a_int
 
  def bin2dec_auto(self, a, b):
      tmp = (b << 8) | a
      my_list = list(bin(tmp))
      c = len(my_list)
      if c < 18:
        for i in range(18-c):
          my_list.insert(2, '0')
      value = my_list[2:]
      if value[0] == '1':  # 如果首位是1，复数转换
          a_output = self.bin2dec(value)
      else:
          a_output = tmp
      return a_output
 
  def add_1(self, binary_inpute):  # 二进制编码加1
      _, out = bin(int(binary_inpute, 2) + 1).split("b")
      return out
 
  def reverse(self, binary_inpute):  # 取反操作
      binary_out = list(binary_inpute)
      for epoch, i in enumerate(binary_out):
          if i == "0":
              binary_out[epoch] = "1"
          else:
              binary_out[epoch] = "0"
      return "".join(binary_out)