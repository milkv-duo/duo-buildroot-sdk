# -*- coding: utf-8 -*-
import time
from pinpong.board import gboard,I2C

class INA219_EDUCATION:
  INA219_IIC_ADDRESS1=0x40   #A0 = 0  A1 = 0
  INA219_IIC_ADDRESS2=0x41   #A0 = 1  A1 = 0
  INA219_IIC_ADDRESS3=0x44   #A0 = 0  A1 = 1
  INA219_IIC_ADDRESS4=0x45   #A0 = 1  A1 = 1

  ina219_reg_config                   =0x00
  ina219_config_reset                 =0x8000
  ina219_config_busvoltage_range_mask  =0x2000

  '''Shunt Voltage Register'''
  ina219_reg_shunt_voltage=0x01
  '''Bus Voltage Register'''
  ina219_reg_bus_voltage=0x02
  '''Power Register'''
  ina219_reg_power=0x03
  '''Current Register'''
  ina219_reg_current=0x04 
  '''Register Calibration'''
  ina219_reg_calibration=0x05  

  ina219_bus_volrange_16v = 0
  ina219_bus_volrange_32v = 1

  ina219_pga_bit_1 = 0
  ina219_pga_bit_2 = 1
  ina219_pga_bit_4 = 2
  ina219_pga_bit_8 = 3

  ina219_adc_bit_9  = 0
  ina219_adc_bit_10 = 1
  ina219_adc_bit_11 = 2
  ina219_adc_bit_12 = 3

  ina219_adc_sample_1  = 0
  ina219_adc_sample_2  = 1
  ina219_adc_sample_4  = 2
  ina219_adc_sample_8  = 3
  ina219_adc_sample_16 = 4
  ina219_adc_sample_32 = 5
  ina219_adc_sample_64 = 6
  ina219_adc_sample_128= 7

  ina219_power_down    = 0
  ina219_s_vol_trig     = 1
  ina219_b_vol_trig     = 2
  ina219_s_and_b_vol_trig = 3
  ina219_adc_0ff       = 4
  ina219_s_vol_con      = 5
  ina219_b_vol_con      = 6
  ina219_s_and_b_vol_con  = 7

  last_operate_status = 0

  cal_value=0x00

  def __init__(self, board=None, i2c_addr=INA219_IIC_ADDRESS4, bus_num=0):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard

    '''The i2c default device address is 0x11'''
    self.i2c_addr = i2c_addr
    self.board = board
    self.i2c = I2C(bus_num)
    self.offset = 0

  def begin(self):
    self.set_bus_range(self.ina219_bus_volrange_32v)
    self.set_pga(self.ina219_pga_bit_8)
    self.set_bus_adc(self.ina219_adc_bit_12, self.ina219_adc_sample_8)
    self.set_shunt_adc(self.ina219_adc_bit_12, self.ina219_adc_sample_8)
    self.set_mode(self.ina219_s_and_b_vol_con)
    self.cal_value = 4096
    buf=[self.cal_value>>8,self.cal_value & 0xff]
    self.write_ina_reg(self.ina219_reg_calibration, buf)


  def linear_calibrate(self,extmeter_Reading_ma):
    global cal_value
    ina219_reading_ma = 0
    for i in range(0,10):
      ina219_reading_ma += self.get_current_ma()
      time.sleep(0.1)
    ina219_reading_ma /= 10.0
    if ((ina219_reading_ma != 0) and (extmeter_Reading_ma != 0)):
      self.cal_value = (int)((extmeter_Reading_ma / ina219_reading_ma) * self.cal_value) & 0xFFFE
    buf=[self.cal_value>>8,self.cal_value & 0xff]
    self.write_ina_reg(self.ina219_reg_calibration,buf)

  '''Sets Bus Voltage Range(default value is 32V)'''
  def set_bus_range(self,value):
    conf = self.read_ina_reg(self.ina219_reg_config)
    conf &= ~(1 << 13)
    conf |= value << 13
    buf=[conf>>8,conf & 0xff]
    #print("data==%d"%buf[0])
    self.write_ina_reg(self.ina219_reg_config, buf)

  def set_pga(self,bits):
    conf = self.read_ina_reg(self.ina219_reg_config)
    conf &= ~(0x03 << 11)
    conf |= bits << 11
    buf=[conf>>8,conf & 0xff]
    self.write_ina_reg(self.ina219_reg_config, buf)

  '''These bits adjust the Bus ADC resolution (9-, 10-, 11-, or 12-bit) 
  or set the number of samples used when averaging results for the Bus Voltage Register'''
  def set_bus_adc(self,bits,sample):
    if((bits < self.ina219_adc_bit_12) and (sample > self.ina219_adc_sample_1)):
      return
    if(bits < self.ina219_adc_bit_12):
        value = bits
    else:
        value = 0x08 | sample
    conf = self.read_ina_reg(self.ina219_reg_config)
    conf &= ~(0x0f << 7)
    conf |= value << 7
    buf=[conf>>8,conf & 0xff]
    self.write_ina_reg(self.ina219_reg_config, buf)

  '''These bits adjust the Shunt ADC resolution (9-, 10-, 11-, or 12-bit) 
  or set the number of samples used when averaging results for the Shunt Voltage Register'''
  def set_shunt_adc(self,bits,sample):
    if(bits < self.ina219_adc_bit_12 and sample > self.ina219_adc_sample_1):
       return
    if(bits < self.ina219_adc_bit_12):
        value = bits
    else:
        value = 0x08 | sample
    conf = self.read_ina_reg(self.ina219_reg_config)
    conf &= ~(0x0f << 3)
    conf |= value << 3
    buf=[conf>>8,conf & 0xff]
    self.write_ina_reg(self.ina219_reg_config, buf)

  '''Selects continuous, triggered, or power-down mode of operation'''
  def set_mode(self,mode):
    conf = self.read_ina_reg(self.ina219_reg_config)
    conf &= ~(0x07)
    conf |= mode
    buf=[conf>>8,conf & 0xff]
    self.write_ina_reg(self.ina219_reg_config, buf); 

  def get_bus_voltage_v(self):
    return round(float(self.read_ina_reg(self.ina219_reg_bus_voltage) >> 3) * 0.004, 2)

  def get_shunt_voltage_mv(self):
    return round(float(self.read_ina_reg(self.ina219_reg_shunt_voltage) * 0.01), 2)

  def get_current_ma(self):
    return round(float(self.read_ina_reg(self.ina219_reg_current) + self.offset) , 2)

  # def get_power_mw(self):
  #   return round(float(self.read_ina_reg(self.ina219_reg_power) * 20), 2)

  def set_offset(self, offset):
    self.offset = offset

  def read_ina_reg(self,reg):
    buf=self.read_reg(reg,2)
    if (buf[0] & 0x80):
      return - 0x10000 + ((buf[0] << 8) | (buf[1]))
    else:
      return (buf[0] << 8) | (buf[1])

  ''' Write data to the i2c register '''
  def write_ina_reg(self ,reg ,data):
    #self.i2c.writeto(self.i2c_addr, [reg] + data)

    #print("data==%d"%data[0])
    self.i2c.writeto_mem(self.i2c_addr,reg,data)

  def read_reg(self,reg,len):
    rslt=self.i2c.readfrom_mem(self.i2c_addr,reg,len)
    return rslt
