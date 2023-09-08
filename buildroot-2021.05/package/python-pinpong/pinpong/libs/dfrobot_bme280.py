# -*- coding: utf-8 -*-
import time
import math
from pinpong.board import gboard,I2C

class BME280:
  BME280_REG_START                               = 0x88
  BME280_REG_CHIP_ID_DEFAULT                     = 0x60
  BME280_CTRL_MEAS                               = 0xF4
  BME280_CTRL_HUM                                = 0xF2
  BME280_CONFIG                                  = 0xF5
  BME280_TEMP                                    = 0xFA
  BME280_PRESS                                   = 0xF7
  BME280_HUMI                                    = 0xFD

  Sampling_no                                    = 0
  Sampling_X1                                    = 1
  Sampling_X2                                    = 2
  Sampling_X4                                    = 3
  Sampling_X8                                    = 4
  Sampling_X16                                   = 5
 
  ConfigFilter_off                               = 0
  ConfigFilter_X2                                = 1
  ConfigFilter_X4                                = 2
  ConfigFilter_X8                                = 3
  ConfigFilter_X16                               = 4

  ConfigTStandby_0_5                            = 0
  ConfigTStandby_62_5                           = 1
  ConfigTStandby_125                            = 2
  ConfigTStandby_250                            = 3
  ConfigTStandby_500                            = 4
  ConfigTStandby_1000                           = 5
  ConfigTStandby_10                             = 6
  ConfigTStandby_20                             = 7

  CtrlMeasMode_sleep                            = 0
  CtrlMeasMode_forced                           = 1
  CtrlMeasMode_normal                           = 0x03

  def __init__(self, board=None, i2c_addr=0x77, bus_num=0):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard
      
    self.i2c_addr = i2c_addr
    self.board = board
    self.i2c = I2C(bus_num)
    
    self.reset()
    while not self.begin():
        print("bme begin faild")
        time.sleep(2)
    time.sleep(0.1)

  def begin(self):
    chip_id = 0xd0
    temp = self.get_reg(chip_id)[0]
    if temp == self.BME280_REG_CHIP_ID_DEFAULT:
        self.reset()
        time.sleep(0.3)
        self.get_calibrate()
        self.set_ctrl_meas_sampling_press(self.Sampling_X8)
        self.set_ctrl_meas_sampling_temp(self.Sampling_X8)
        self.set_ctrl_humi_sampling(self.Sampling_X8)
        self.set_config_filter(self.ConfigFilter_off)
        self.set_config_TStandby(self.ConfigTStandby_125)
        self.set_ctrl_meas_mode(self.CtrlMeasMode_normal)
    return True
  
  def humidity(self):
    self.temp_c()
    raw = self.get_humidity_raw()
    v1 = self._t_fine - 76800
    v1 = (((((raw <<14) - ((self._sCalibHumi_h4) << 20) - ((self._sCalibHumi_h5) * v1)) + \
         (16384)) >> 15) * (((((((v1 * (self._sCalibHumi_h6)) >> 10) * (((v1 * \
         (self._sCalibHumi_h3)) >> 11) + (32768))) >> 10) + (2097152)) * \
         (self._sCalibHumi_h2) + 8192) >> 14))
    v1 = (v1 - (((((v1 >> 15) * (v1 >> 15)) >> 7) * ( self._sCalibHumi_h1)) >> 4))
    if v1 < 0:
        return 0
    419430400 if v1 > 419430400 else v1
    return round((v1 >> 12) / 1024, 2)

  def get_humidity_raw(self):
    data = self.read_reg(self.BME280_HUMI, 2)
    val = data[0] << 8 | data[1]
    return val
  
  def get_sealevel(self):
    pressure = self.press_pa()
    return (pressure / pow(1.0 - (525 / 44330), 5.255))
  
  def cal_altitudu(self):
    sea_level_pressure = self.get_sealevel()
    pressure = self.press_pa()
    return round(((1.0 - pow(pressure / sea_level_pressure, 0.190284)) * 287.15 / 0.0065), 2)
    
  def press_pa(self):
    self.temp_c()
    raw = self.get_pressure_raw()
    v1 = self._t_fine - 128000
    v2 = v1 * v1 * self._sCalib_p6
    v2 = v2 + ((v1 * self._sCalib_p5) << 17)
    v2 = v2 +((self._sCalib_p4) << 35)
    v1 = ((v1 * v1 * self._sCalib_p3) >> 8) + ((v1 * self._sCalib_p2) << 12)
    v1 = (((1 << 47) + v1) * self._sCalib_p1) >> 33
    if v1 == 0:
      return 0
    rslt = 1048576 - raw
    rslt = (((rslt << 31) - v2) * 3125) // v1
    v1 = ((self._sCalib_p9) * (rslt >> 13) * (rslt >> 13)) >> 25
    v2 = ((self._sCalib_p8) * rslt) >> 19
    rslt = ((rslt + v1 + v2) >> 8) + ((self._sCalib_p7) << 4)
    ret = (int)(rslt / 256)
    return ret

  def get_pressure_raw(self):
    data = self.read_reg(self.BME280_PRESS , 3)
    temp = data[2] >> 4
    val = (data[0] << 12) | (data[1] << 4) | temp
    return val

  def temp_c(self):
    raw = self.get_temperature_raw()
    v1 = (((raw >> 3) - (self._sCalib_t1 << 1)) * (self._sCalib_t2)) >> 11
    v2 = (((((raw >> 4) - (self._sCalib_t1)) * ((raw >> 4) - (self._sCalib_t1))) >> 12) * (self._sCalib_t3)) >> 14
    self._t_fine = v1 + v2
    rslt = (self._t_fine * 5 + 128) >> 8
    return (rslt / 100)
  
  def get_temperature_raw(self):
    data = self.read_reg(self.BME280_TEMP, 3)
    temp = data[2] >> 4
    val = (data[0] << 12) | (data[1] << 4) | temp
    return val

  def set_ctrl_meas_mode(self, mode):
    reg_Field = 0x03
    if mode == self.CtrlMeasMode_sleep:
      reg_val = 0x00
    elif mode == self.CtrlMeasMode_forced:
      reg_val = 0x01
    elif mode == self.CtrlMeasMode_normal:
      reg_val = 0x03
    self.write_reg_bits(self.BME280_CTRL_MEAS, reg_Field, reg_val)

  def set_config_TStandby(self, TStandby):
    reg_Field = 0xe0
    if TStandby == self.ConfigTStandby_0_5:
      reg_val = 0x00
    elif TStandby == self.ConfigTStandby_62_5:
      reg_val = 0x20
    elif TStandby == self.ConfigTStandby_125:
      reg_val = 0x40
    elif TStandby == self.ConfigTStandby_250:
      reg_val = 0x60
    elif TStandby == self.ConfigTStandby_500:
      reg_val = 0x80
    elif TStandby == self.ConfigTStandby_1000:
      reg_val = 0xa0
    elif TStandby == self.ConfigTStandby_10:
      reg_val = 0xc0
    elif TStandby == self.ConfigTStandby_20:
      reg_val = 0xe0
    self.write_reg_bits(self.BME280_CONFIG, reg_Field, reg_val)

  def set_config_filter(self, Filter):
    reg_Field = 0x1c
    if Filter == self.ConfigFilter_off:
      reg_val = 0x00
    elif Filter == self.ConfigFilter_X2:
      reg_val = 0x04
    elif Filter == self.ConfigFilter_X4:
      reg_val = 0x08
    elif Filter == self.ConfigFilter_X8:
      reg_val = 0x0c
    elif Filter == self.ConfigFilter_X16:
      reg_val = 0x10
    self.write_reg_bits(self.BME280_CONFIG, reg_Field, reg_val)

  def set_ctrl_humi_sampling(self, Sampling):
    reg_Field = 0x07
    if Sampling == self.Sampling_no:
      reg_val = 0x00
    elif Sampling == self.Sampling_X1:
      reg_val = 0x01
    elif Sampling == self.Sampling_X2:
      reg_val = 0x02
    elif Sampling == self.Sampling_X4:
      reg_val = 0x03
    elif Sampling == self.Sampling_X8:
      reg_val = 0x04
    elif Sampling == self.Sampling_X16:
      reg_val = 0x05
    self.write_reg_bits(self.BME280_CTRL_HUM, reg_Field, reg_val)

  def set_ctrl_meas_sampling_temp(self, Sampling):
    reg_Field = 0xe0
    if Sampling == self.Sampling_no:
      reg_val = 0x00
    elif Sampling == self.Sampling_X1:
      reg_val = 0x20
    elif Sampling == self.Sampling_X2:
      reg_val = 0x40
    elif Sampling == self.Sampling_X4:
      reg_val = 0x60
    elif Sampling == self.Sampling_X8:
      reg_val = 0x80
    elif Sampling == self.Sampling_X16:
      reg_val = 0xa0
    self.write_reg_bits(self.BME280_CTRL_MEAS, reg_Field, reg_val)

  def set_ctrl_meas_sampling_press(self, Sampling):
    reg_Field = 0x1c
    if Sampling == self.Sampling_no:
      reg_val = 0x00
    elif Sampling == self.Sampling_X1:
      reg_val = 0x04
    elif Sampling == self.Sampling_X2:
      reg_val = 0x08
    elif Sampling == self.Sampling_X4:
      reg_val = 0x0c
    elif Sampling == self.Sampling_X8:
      reg_val = 0x10
    elif Sampling == self.Sampling_X16:
      reg_val = 0x14
    self.write_reg_bits(self.BME280_CTRL_MEAS, reg_Field, reg_val)
    
  def write_reg_bits(self, reg, reg_Field, reg_val):
    temp = self.read_reg(reg, 1)[0]
    temp &= ~reg_Field
    temp |= reg_val
    self.write_reg(reg, [temp])

  def get_calibrate(self):
    _sCalib = self.read_reg(0x88, 24)
    self._sCalib_t1 = _sCalib[1] << 8 | _sCalib[0]
    self._sCalib_t2 = _sCalib[3] << 8 | _sCalib[2]
    self._sCalib_t3 = _sCalib[5] << 8 | _sCalib[4]
    self._sCalib_p1 = _sCalib[7] << 8 | _sCalib[6]
    self._sCalib_p2 = self.bin2dec_auto(_sCalib[8], _sCalib[9])
    self._sCalib_p3 = _sCalib[11] << 8 | _sCalib[10]
    self._sCalib_p4 = _sCalib[13] << 8 | _sCalib[12]
    self._sCalib_p5 = self.bin2dec_auto(_sCalib[14], _sCalib[15])
    self._sCalib_p6 = self.bin2dec_auto(_sCalib[16], _sCalib[17])
    self._sCalib_p7 = _sCalib[19] << 8 | _sCalib[18]
    self._sCalib_p8 = self.bin2dec_auto(_sCalib[20], _sCalib[21])
    self._sCalib_p9 = _sCalib[23] << 8 | _sCalib[22]
    self._sCalibHumi_h1 = self.read_reg(0xa1, 1)[0]
    temp = self.read_reg(0xe1, 2)
    self._sCalibHumi_h2 = temp[1] << 8 | temp[0]
    self._sCalibHumi_h3 = self.read_reg(0xe3, 1)[0]
    temp = self.read_reg(0xe4, 2)
    self._sCalibHumi_h4 = temp[1] << 8 | temp[0]
    temp = self.read_reg(0xe5, 2)
    self._sCalibHumi_h5 = temp[1] << 8 | temp[0]
    self._sCalibHumi_h6 = self.read_reg(0xe7, 1)[0]
    self._sCalibHumi_h4 = ((self._sCalibHumi_h4 >> 8) & 0x0f) | ((self._sCalibHumi_h4 & 0x00ff) << 4)
    self._sCalibHumi_h5 = ((self._sCalibHumi_h5 & 0xff00) >> 4) | ((self._sCalibHumi_h5 & 0x00f0) >> 4)

  def get_reg(self, reg, lens=1):
    return self.read_reg(reg, lens)

  def reset(self):
    temp = [0xb6]
    self.write_reg(0xe0, temp)
    time.sleep(0.1)
  
  def read_reg(self, reg, lens):
    return self.i2c.readfrom_mem(self.i2c_addr, reg, lens)

  def write_reg(self, reg, data):
    self.i2c.writeto_mem(self.i2c_addr, reg, data)

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