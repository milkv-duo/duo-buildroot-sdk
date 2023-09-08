# -*- coding: utf-8 -*-
import time
from pinpong.board import gboard,I2C

class VL53L0:
  VL53L0X_REG_VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV       = 0x0089
  VL53L0X_REG_I2C_SLAVE_DEVICE_ADDRESS                = 0x008A
  VL53L0X_REG_IDENTIFICATION_REVISION_ID              = 0x00C2
  VL53L0X_REG_SYSTEM_RANGE_CONFIG                     = 0x0009
  VL53L0X_REG_SYSRANGE_MODE_START_STOP                = 0x0001
  VL53L0X_REG_SYSRANGE_START                          = 0x0000
  VL53L0X_REG_IDENTIFICATION_MODEL_ID                 = 0x00c0
  VL53L0X_REG_SYSRANGE_MODE_BACKTOBACK                = 0x0002
  VL53L0X_REG_RESULT_RANGE_STATUS                     = 0x0014
  VL53L0X_DEVICEMODE_CONTINUOUS_RANGING               = 1
  VL53L0X_DEFAULT_MAX_LOOP                            = 200
  VL53L0X_DEVICEMODE_SINGLE_RANGING                   = 0
  DISABLE                                             = 0
  ENABLE                                              = 1
  High                                                = 0
  Low                                                 = 1
  Single                                              = 0
  Continuous                                          = 1


  def __init__(self, board=None, i2c_addr=0x29, bus_num=0):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard
    
    self.board = board
    self.i2c_addr = i2c_addr
    self.i2c = I2C(bus_num)
    
    self.pre_distance = 0

    self.begin()
    self.set_mode(self.Continuous , self.High) #设置模式
    self.start()

  def begin(self, addr = 0x50):
    try:
      time.sleep(1.5)
#      self.data_init()
#      self.set_device_address(addr)
      vall = self.read_byte_data(self.VL53L0X_REG_IDENTIFICATION_REVISION_ID)
      print("Revision ID: ",end =" ")
      print(hex(vall[0]))
      vall = self.read_byte_data(self.VL53L0X_REG_IDENTIFICATION_MODEL_ID)
      print("Device ID: ",end = " ")
      print(hex(vall[0]))
    except Exception:
      pass

  def get_distance_mm(self):
    self.read_vl53l0()
    if self.distance == 20:
      self.distance = self.pre_distance
    else:
      self.pre_distance = self.distance
    if self.precision == self.High:
      return self.distance / 4.0
    else:
      return self.distance
  
  def read_vl53l0(self):
    data = self.read_data(self.VL53L0X_REG_RESULT_RANGE_STATUS, 12)
    self.ambient_count = ((data[6] & 0xff) << 8) | (data[7] & 0xff)
    self.signal_count = ((data[8] & 0xff) << 8) | (data[9] & 0xff)
    self.distance = ((data[10] & 0xff) << 8) | (data[11] & 0xff)
    self.status = (data[0] & 0x78) >> 3
  
  def read_data(self, reg, lens):
    return self.i2c.readfrom_mem(self.i2c_addr, reg, lens)

  def start(self):
    start_stop_byte = self.VL53L0X_REG_SYSRANGE_MODE_START_STOP
    device_mode = self.mode
    self.write_byte_data(0x80, 0x01)
    self.write_byte_data(0xFF, 0x01)
    self.write_byte_data(0x00, 0x00)
    self.write_byte_data(0x91, 0x3c)
    self.write_byte_data(0x00, 0x01)
    self.write_byte_data(0xFF, 0x00)
    self.write_byte_data(0x80, 0x00)
    if device_mode == self.VL53L0X_DEVICEMODE_SINGLE_RANGING:
      self.write_byte_data(VL53L0X_REG_SYSRANGE_START, 0x01)
      byte = start_stop_byte
      loop = 0
      while (byte & start_stop_byte) == start_stop_byte and loop < self.VL53L0X_DEFAULT_MAX_LOOP:
        if loop > 0:
          byte = self.read_byte_data(self.VL53L0X_REG_SYSRANGE_START)
    elif device_mode == self.VL53L0X_DEVICEMODE_CONTINUOUS_RANGING:
      self.write_byte_data(self.VL53L0X_REG_SYSRANGE_START, self.VL53L0X_REG_SYSRANGE_MODE_BACKTOBACK)
    else:
      print("---Selected mode not supported---")
    
  def set_mode(self, mode, precision):
    self.mode = mode
    if precision == self.High:
      self.high_precision_enable(self.ENABLE)
      self.precision = precision
    else:
      self.high_precision_enable(self.DISABLE)
      self.precision = precision
  
  def high_precision_enable(self, new_state):
    self.write_byte_data(self.VL53L0X_REG_SYSTEM_RANGE_CONFIG, new_state)
    
  def set_device_address(self,addr):
    new_addr = addr & 0x7F
    self.write_byte_data(self.VL53L0X_REG_I2C_SLAVE_DEVICE_ADDRESS, new_addr)
    self.i2c_addr = new_addr
    
  def data_init(self):
    data = self.read_byte_data(self.VL53L0X_REG_VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV)[0]
    data = (data & 0xFE) | 0x01
    self.write_byte_data(self.VL53L0X_REG_VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, data)
    self.write_byte_data(0x88, 0x00)
    self.write_byte_data(0x80, 0x01)
    self.write_byte_data(0xFF, 0x01)
    self.write_byte_data(0x00, 0x00)
    self.read_byte_data(0x91)
    self.write_byte_data(0x91, 0x3c)
    self.write_byte_data(0x00, 0x01)
    self.write_byte_data(0xFF, 0x00)
    self.write_byte_data(0x80, 0x00)
    
  def write_byte_data(self, reg, data):
    self.write_reg(reg, data)
    
  def read_byte_data(self, reg, lens=1):
    return self.read_reg(reg, lens)
    
  def write_reg(self, reg, data):
    self.i2c.writeto_mem(self.i2c_addr, reg, [data])
    
  def read_reg(self, reg, lens):
    return self.i2c.readfrom_mem(self.i2c_addr, reg, lens)
  