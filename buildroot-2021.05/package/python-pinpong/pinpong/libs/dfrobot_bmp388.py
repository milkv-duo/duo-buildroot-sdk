# -*- coding: utf-8 -*-
import time
from math import pow
from pinpong.board import gboard,I2C

class BMP388:
  BMP3_I2C_INTF                          = 1
  BMP3_CHIP_ID_ADDR                      = 0x00
  BMP3_CHIP_ID                           = 0x50
  BMP3_CMD_ADDR                          = 0x7E
  BMP3_ERR_REG_ADDR                      = 0x02
  BMP3_SENS_STATUS_REG_ADDR              = 0x03
  BMP3_CMD_RDY                           = 0x10
  BMP3_CMD_ERR                           = 0x02
  BMP3_CALIB_DATA_ADDR                   = 0x31
  BMP3_DATA_ADDR                         = 0x04
  BMP3_PWR_CTRL_ADDR                     = 0x1B
  
  BMP3_PRESS                             = 1
  BMP3_PRESS_EN_SEL                      = 2
  BMP3_OK                                = 0
  BMP3_E_DEV_NOT_FOUND                   = -2
  BMP3_E_CMD_EXEC_FAILED                 = -4  
  BMP3_TEMP_EN_SEL                       = 4
  BMP3_CALIB_DATA_LEN                    = 21
  BMP3_P_T_DATA_LEN                      = 6
  
  BMP3_ENABLE                            = 0x01
  BMP3_NO_OVERSAMPLING                   = 0x00
  BMP3_ODR_200_HZ                        = 0x00
  
  def __init__(self, board=None, i2c_addr=0x77, bus_num=0):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard
    
    self.board = board
    self.i2c_addr = i2c_addr
    self.i2c = I2C(bus_num)
    
    self.par_t1 = 0
    self.par_t2 = 0
    self.par_t3 = 0
    self.par_p1 = 0
    self.par_p2 = 0
    self.par_p3 = 0
    self.par_p4 = 0
    self.par_p5 = 0
    self.par_p6 = 0
    self.par_p7 = 0
    self.par_p8 = 0
    self.par_p9 = 0
    self.par_p10 = 0
    self.par_p11 = 0
    self.op_mode = 0
    self.t_lin = 0
    self.temperature = 0
    self.pressure = 0
    
    self.begin()

    time.sleep(0.1)
    
    self.sea_level = self.read_sea_level(525.0)
    
  def begin(self):
    rslt = self.bmp3_get_regs(self.BMP3_CHIP_ID_ADDR, 1)[0]
    if rslt == self.BMP3_CHIP_ID:
      self.chip_id = rslt
      rslt = self.reset()
      self.get_calib_data()
      self.set_config()
  
  def set_config(self):
    self.press_en = self.BMP3_ENABLE
    self.temp_en = self.BMP3_ENABLE
    self.press_os = self.BMP3_NO_OVERSAMPLING
    self.temp_os = self.BMP3_NO_OVERSAMPLING
    self.odr = self.BMP3_ODR_200_HZ
    settings_sel = 2|4|16|32|128
    self.bmp3_set_sensor_settings(settings_sel)
    self.op_mode = 0x03
    self.write_power_mode()
    
  def bmp3_set_sensor_settings(self,settings_sel):
    temp = self.bmp3_get_regs(self.BMP3_PWR_CTRL_ADDR,1)
    reg_data = temp[0]
    if(settings_sel & self.BMP3_PRESS_EN_SEL):
      reg_data = (reg_data&~(0x01))|(0x01&0x01)
    if(settings_sel & self.BMP3_TEMP_EN_SEL):
      reg_data = (reg_data&~(0x02))|((0x01<<0x01)&0x02)
    data = [reg_data]
    self.bmp3_set_regs(self.BMP3_PWR_CTRL_ADDR, data)
  
  def write_power_mode(self):
    op_mode_reg_val = self.bmp3_get_regs(self.BMP3_PWR_CTRL_ADDR,1)[0]
    op_mode_reg_val = (op_mode_reg_val&~(0x30))|((self.op_mode<<0x04)&0x30)
    self.bmp3_set_regs(self.BMP3_PWR_CTRL_ADDR,[op_mode_reg_val])
    
  def parse_calib_data(self, calib):
    calib = self.bmp3_get_regs(0x31,21)
    self.parse_calib_data(calib)
  
  def read_sea_level(self, altitude):
    pressure = self.pressure_pa()
    return round(pressure / pow(1.0 - (altitude / 44330.0), 5.255),2)
    
  def read_altitude(self):
    pressure = self.pressure_pa()
    return round((1.0 - pow(pressure / 101325, 0.190284)) * 287.15 / 0.0065,2)
    
  def pressure_pa(self):
    self.bmp3_get_sensor_data(1)
    return round(self.pressure, 2)
  
  def temp_C(self):
    self.bmp3_get_sensor_data(2)
    return round(self.temperature,2)
    
  def cal_altitude_m(self):
    pressure = self.pressure_pa()
    return round((1.0 - pow(pressure / self.sea_level, 0.190284)) * 287.15 / 0.0065,2)
    
  def bmp3_get_sensor_data(self, sensor_comp):
    rslt = self.bmp3_get_regs(self.BMP3_DATA_ADDR, 6)
    xlsb = rslt[0]
    lsb = rslt[1] << 8
    msb = rslt[2] << 16
    uncomp_pressure = msb|lsb|xlsb
    xlsb = rslt[3]
    lsb = rslt[4] << 8
    msb = rslt[5] << 16
    uncomp_temperature = msb|lsb|xlsb
    self.compensate_data(sensor_comp, uncomp_pressure, uncomp_temperature)
    
  def compensate_data(self, sensor_comp, uncomp_pressure, uncomp_temperature):
    if(sensor_comp & 0x03):
      self.temperature = self.compensate_temperature(uncomp_temperature)
    if(sensor_comp & 0x01):
      self.pressure = self.compensate_pressure(uncomp_pressure)
    
  def compensate_temperature(self, uncomp_temperature):
    uncomp_temp = uncomp_temperature
    partial_data1 = (uncomp_temp - self.par_t1)
    partial_data2 = (partial_data1 * self.par_t2)
    self.t_lin = partial_data2 + (partial_data1 * partial_data1)*self.par_t3
    return self.t_lin
    
  def compensate_pressure(self, uncomp_pressure, ):
    partial_data1 = self.par_p6 * self.t_lin
    partial_data2 = self.par_p7 * pow(self.t_lin, 2)
    partial_data3 = self.par_p8 * pow(self.t_lin, 3)
    partial_out1 = self.par_p5 + partial_data1 + partial_data2 + partial_data3
    partial_data1 = self.par_p2 * self.t_lin
    partial_data2 = self.par_p3 * pow(self.t_lin, 2)
    partial_data3 = self.par_p4 * pow(self.t_lin, 3)
    partial_out2 = uncomp_pressure *(self.par_p1-0.000145 +partial_data1+ partial_data2 + partial_data3)

    partial_data1 = pow(uncomp_pressure, 2)
    partial_data2 = self.par_p9 + self.par_p10 * self.t_lin
    partial_data3 = partial_data1 * partial_data2
    partial_data4 = partial_data3 + pow(uncomp_pressure, 3) * self.par_p11
    comp_press = partial_out1 + partial_out2 + partial_data4
    return comp_press;
  
  def uint8_int(self, num):
    if(num>127):
      num = num - 256
    return num
  
  def parse_calib_data(self,calib): # Parse
    temp_var = 0.00390625
    self.par_t1 = ((calib[1]<<8)|calib[0])/temp_var
    
    temp_var = 1073741824
    self.par_t2 = ((calib[3]<<8)|calib[2])/temp_var
    
    temp_var = 281474976710656
    calibTemp = self.uint8_int(calib[4])
    self.par_t3 = (calibTemp)/temp_var
    
    temp_var = 1048576
    calibTempA = self.uint8_int(calib[6])
    calibTempB = self.uint8_int(calib[5])
    self.par_p1 = ((calibTempA|calibTempB)-16384)/temp_var
    
    temp_var = 536870912
    calibTempA = self.uint8_int(calib[8])
    calibTempB = self.uint8_int(calib[7])
    self.par_p2 = (((calibTempA<<8)|calibTempB)-16384)/temp_var
    
    temp_var = 4294967296
    calibTemp = self.uint8_int(calib[9])
    self.par_p3 = calibTemp/temp_var
    
    temp_var = 137438953472
    calibTemp = self.uint8_int(calib[10])
    self.par_p4 = calibTemp/temp_var
    
    temp_var = 0.125
    self.par_p5 = ((calib[12]<<8)|calib[11])/temp_var
    
    temp_var = 64
    self.par_p6 = ((calib[14]<<8)|calib[13])/temp_var
    
    temp_var = 256
    calibTemp = self.uint8_int(calib[15])
    self.par_p7 = calibTemp/temp_var
    
    temp_var = 32768
    calibTemp = self.uint8_int(calib[16])
    self.par_p8 = calibTemp/temp_var
    
    temp_var = 281474976710656
    self.par_p9 = ((calib[18]<<8)|calib[17])/temp_var
    
    temp_var = 281474976710656
    calibTemp = self.uint8_int(calib[19])
    self.par_p10 = (calibTemp)/temp_var
    
    temp_var = 36893488147419103232
    calibTemp = self.uint8_int(calib[20])
    self.par_p11 = (calibTemp)/temp_var

  def get_calib_data(self):
    rslt = self.bmp3_get_regs(self.BMP3_CALIB_DATA_ADDR, self.BMP3_CALIB_DATA_LEN)
    self.parse_calib_data(rslt)
    
  def reset(self):
    reg_addr = 0x7E
    soft_rst_cmd = 0xB6
    rslt = self.bmp3_get_regs(self.BMP3_SENS_STATUS_REG_ADDR, 1)[0]
    if rslt & self.BMP3_CMD_RDY:
      self.bmp3_set_regs(self.BMP3_CMD_ADDR, [soft_rst_cmd])
      time.sleep(0.002)
      ret = self.bmp3_get_regs(self.BMP3_ERR_REG_ADDR, 1)[0]
      if ret & self.BMP3_CMD_ERR:
        ret = self.BMP3_E_CMD_EXEC_FAILED
    else:
      ret = self.BMP3_E_CMD_EXEC_FAILED
    return ret
  
  
  def bmp3_get_regs(self, reg_addr, lens):
    return self.i2c.readfrom_mem(self.i2c_addr, reg_addr, lens)
    
  def bmp3_set_regs(self, reg_addr, value):
    self.i2c.writeto_mem(self.i2c_addr, reg_addr, value)
    
    
    
    
    