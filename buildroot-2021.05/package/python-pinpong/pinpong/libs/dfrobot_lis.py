# -*- coding: utf-8 -*

import time
import numpy as np
from pinpong.board import gboard,I2C

H3LIS200DL                = 0x32  
LIS331HH                  = 0x32  
               
class LIS(object):
  REG_CARD_ID    = 0x0F       #Chip id
  REG_CTRL_REG1  = 0x20       #Control register 1
  REG_CTRL_REG4  = 0x23       #Control register 4
  REG_CTRL_REG2  = 0x21       #Control register 2
  REG_CTRL_REG3  = 0x22       #Control register 3
  REG_CTRL_REG5  = 0x24       #Control register 5
  REG_CTRL_REG6  = 0x25       #Control register 6
  REG_STATUS_REG = 0x27       #Status register
  REG_OUT_X_L    = 0x28       #The low order of the X-axis acceleration register
  REG_OUT_X_H    = 0x29       #The high point of the X-axis acceleration register
  REG_OUT_Y_L    = 0x2A       #The low order of the Y-axis acceleration register
  REG_OUT_Y_H    = 0x2B       #The high point of the Y-axis acceleration register
  REG_OUT_Z_L    = 0x2C       #The low order of the Z-axis acceleration register
  REG_OUT_Z_H    = 0x2D       #The high point of the Z-axis acceleration register
  REG_INT1_THS   = 0x32       #Interrupt source 1 threshold
  REG_INT2_THS   = 0x36       #Interrupt source 2 threshold
  REG_INT1_CFG   = 0x30       #Interrupt source 1 configuration register
  REG_INT2_CFG   = 0x34       #Interrupt source 2 configuration register
  REG_INT1_SRC   = 0x31       #Interrupt source 1 status register
  REG_INT2_SRC   = 0x35       #Interrupt source 2 status register
  SPI_READ_BIT   = 0X80       # bit 0: RW bit. When 0, the data DI(7:0) is written into the device. When 1, the data DO(7:0) from the device is read.
  ENABLE_FILTER  = 0X10       #Enable filter
  __reset = 0
  
  '''
    Power mode selection, determine the frequency of data collection
    Represents the number of data collected per second
  '''
  POWERDOWN_0HZ = 0
  LOWPOWER_HALFHZ = 0X40 
  LOWPOWER_1HZ = 0X60
  LOWPOWER_2HZ = 0X80
  LOWPOWER_5HZ = 0XA0
  LOWPOWER_10HZ = 0XC0
  NORMAL_50HZ = 0X20
  NORMAL_100HZ = 0X28
  NORMAL_400HZ = 0X30
  NORMAL_1000HZ = 0X38
  
  '''
  Sensor range selection
  '''
  H3LIS200DL_100G = 100 #±100G
  H3LIS200DL_200G = 200 #±200G

  LIS331HH_6G      = 6   #±6G
  LIS331HH_12G     = 12  #±12G
  LIS331HH_24G     = 24  #±24G

  '''
  #                           High-pass filter cut-off frequency configuration
  # |--------------------------------------------------------------------------------------------------------|
  # |                |    ft [Hz]      |        ft [Hz]       |       ft [Hz]        |        ft [Hz]        |
  # |   mode         |Data rate = 50 Hz|   Data rate = 100 Hz |  Data rate = 400 Hz  |   Data rate = 1000 Hz |
  # |--------------------------------------------------------------------------------------------------------|
  # |  eCutoffMode1  |     1           |         2            |            8         |             20        |
  # |--------------------------------------------------------------------------------------------------------|
  # |  eCutoffMode2  |    0.5          |         1            |            4         |             10        |
  # |--------------------------------------------------------------------------------------------------------|
  # |  eCutoffMode3  |    0.25         |         0.5          |            2         |             5         |
  # |--------------------------------------------------------------------------------------------------------|
  # |  eCutoffMode4  |    0.125        |         0.25         |            1         |             2.5       |
  # |--------------------------------------------------------------------------------------------------------|
  '''
  CUTOFF_MODE1 = 0
  CUTOFF_MODE2 = 1
  CUTOFF_MODE3 = 2
  CUTOFF_MODE4 = 3
  SHUTDOWN     = 4
  
  '''
  Interrupt event
  '''
  X_LOWERTHAN_TH   = 0X1  #The acceleration in the x direction is less than the threshold
  X_HIGHERTHAN_TH  = 0X2  #The acceleration in the x direction is greater than the threshold
  Y_LOWERTHAN_TH   = 0X4  #The acceleration in the y direction is less than the threshold
  Y_HIGHERTHAN_TH  = 0X8  #The acceleration in the y direction is greater than the threshold
  Z_LOWERTHAN_TH   = 0X10  #The acceleration in the z direction is less than the threshold
  Z_HIGHERTHAN_TH  = 0X20  #The acceleration in the z direction is greater than the threshold
  EVENT_ERROR      = 0  # No event

  #Interrupt pin selection
  INT_1 = 0 #int1
  INT_2 = 1 #int2
  
  def __init__(self):
    __reset = 1

  '''
    @brief Initialize the function
    @return return True(Succeed)/False(Failed)
  '''
  def begin(self):
    identifier = self.read_reg(self.REG_CARD_ID)
    if identifier == H3LIS200DL or identifier == LIS331HH:
      return True
    else:
      return False
      
  '''
    @brief Get chip id
    @return the 8 bit serial number
  '''
  def get_id(self):
    identifier = self.read_reg(self.REG_CARD_ID)
    return identifier

  '''
    @brief Set data measurement rate
    @param rate rate(HZ)
           POWERDOWN_0HZ = 0
           LOWPOWER_HALFHZ = 0X40 
           LOWPOWER_1HZ = 0X60
           LOWPOWER_2HZ = 0X80
           LOWPOWER_5HZ = 0XA0
           LOWPOWER_10HZ = 0XC0
           NORMAL_50HZ = 0X20
           NORMAL_100HZ = 0X28
           NORMAL_400HZ = 0X30
           NORMAL_1000HZ = 0X38
  '''
  def set_acquire_rate(self, rate):    
    value = self.read_reg(self.REG_CTRL_REG1)
    value = value & (~(0x7 << 5))
    value = value & (~(0x3 << 3))
    value = value | rate
    self.write_reg(self.REG_CTRL_REG1,value)

  '''
    @brief Set the threshold of interrupt source 1 interrupt
    @param threshold Threshold(g)
  '''
  def set_int1_th(self,threshold):
    value = (threshold * 128)/_range
    self.write_reg(self.REG_INT1_THS,value)

  '''
    @brief Set interrupt source 2 interrupt generation threshold
    @param threshold Threshold(g)
  '''
  def set_int2_th(self,threshold):
    value = (threshold * 128)/_range
    self.write_reg(self.REG_INT2_THS,value)
  
  '''
    @brief Enable interrupt
    @param source Interrupt pin selection
             INT_1 = 0,/<int pad 1 >/
             INT_2,/<int pad 2>/
    @param event Interrupt event selection
             X_LOWERTHAN_TH     = 0X1  #The acceleration in the x direction is less than the threshold
             X_HIGHERTHAN_TH  = 0X2  #The acceleration in the x direction is greater than the threshold
             Y_LOWERTHAN_TH     = 0X4  #The acceleration in the y direction is less than the threshold
             Y_HIGHERTHAN_TH  = 0X8  #The acceleration in the y direction is greater than the threshold
             Z_LOWERTHAN_TH     = 0X10  #The acceleration in the z direction is less than the threshold
             Z_HIGHERTHAN_TH  = 0X20  #The acceleration in the z direction is greater than the threshold
             EVENT_ERROR      = 0  # No event
  '''
  def enable_int_event(self,source,event):
    value = 0    
    if source == self.INT_1:
      value = self.read_reg(self.REG_INT1_CFG)
    else:
      value = self.read_reg(self.REG_INT2_CFG)
    if self.__reset == 1:
       value = 0
       self.__reset = 0
    value = value | event;
    #print(value)
    if source == self.INT_1:
      self.write_reg(self.REG_INT1_CFG,value)
    else:
      self.write_reg(self.REG_INT2_CFG,value)

  '''
    @brief Check whether the interrupt event'event' is generated in interrupt 1
    @param event:Interrupt event
             X_LOWERTHAN_TH     = 0X1  #The acceleration in the x direction is less than the threshold
             X_HIGHERTHAN_TH  = 0X2  #The acceleration in the x direction is greater than the threshold
             Y_LOWERTHAN_TH     = 0X4  #The acceleration in the y direction is less than the threshold
             Y_HIGHERTHAN_TH  = 0X8  #The acceleration in the y direction is greater than the threshold
             Z_LOWERTHAN_TH     = 0X10  #The acceleration in the z direction is less than the threshold
             Z_HIGHERTHAN_TH  = 0X20  #The acceleration in the z direction is greater than the threshold
             EVENT_ERROR      = 0  # No event
    @return true This event generated
            false This event not generated
  '''
  def get_int1_event(self,event):
    value = self.read_reg(self.REG_INT1_SRC)
    if (value & event) >= 1:
         return True
    else:
         return False
         
  '''
    @brief Check whether the interrupt event'event' is generated in interrupt 2
    @param event:Interrupt event
             X_LOWERTHAN_TH     = 0X1  #The acceleration in the x direction is less than the threshold
             X_HIGHERTHAN_TH  = 0X2  #The acceleration in the x direction is greater than the threshold
             Y_LOWERTHAN_TH     = 0X4  #The acceleration in the y direction is less than the threshold
             Y_HIGHERTHAN_TH  = 0X8  #The acceleration in the y direction is greater than the threshold
             Z_LOWERTHAN_TH     = 0X10  #The acceleration in the z direction is less than the threshold
             Z_HIGHERTHAN_TH  = 0X20  #The acceleration in the z direction is greater than the threshold
             EVENT_ERROR      = 0  # No event
    @return true This event generated
            false This event not generated
  '''
  def get_int2_event(self,event):
    value = self.read_reg(self.REG_INT2_SRC)
    if (value & event) >= 1:
         return True
    else:
         return False
  '''
    @brief Enable sleep wake function
    @param enable:True(enable)/False(disable)
  '''
  def enable_sleep(self, enable):
    value = 0
    if enable == True:
      value = 3
    self.write_reg(self.REG_CTRL_REG5,value)
    return 0

  '''
    @brief Set data filtering mode
    @param mode:Four modes
                CUTOFF_MODE1 = 0
                CUTOFF_MODE2 = 1
                CUTOFF_MODE3 = 2
                CUTOFF_MODE4 = 3
                              High-pass filter cut-off frequency configuration
    |--------------------------------------------------------------------------------------------------------|
    |                |    ft [Hz]      |        ft [Hz]       |       ft [Hz]        |        ft [Hz]        |
    |   mode         |Data rate = 50 Hz|   Data rate = 100 Hz |  Data rate = 400 Hz  |   Data rate = 1000 Hz |
    |--------------------------------------------------------------------------------------------------------|
    |  CUTOFF_MODE1  |     1           |         2            |            8         |             20        |
    |--------------------------------------------------------------------------------------------------------|
    |  CUTOFF_MODE2  |    0.5          |         1            |            4         |             10        |
    |--------------------------------------------------------------------------------------------------------|
    |  CUTOFF_MODE3  |    0.25         |         0.5          |            2         |             5         |
    |--------------------------------------------------------------------------------------------------------|
    |  CUTOFF_MODE4  |    0.125        |         0.25         |            1         |             2.5       |
    |--------------------------------------------------------------------------------------------------------|
  '''
  def set_filter_mode(self,mode):
    value = self.read_reg(self.REG_CTRL_REG2)
    if mode == self.SHUTDOWN:
      value = value & (~ENABLE_FILTER)
    else:
      value = value | ENABLE_FILTER
    value = value & (~3)
    value = value | mode
    self.write_reg(self.REG_CTRL_REG2,value)
  

class H3LIS200DL_I2C(LIS): 
  def __init__(self ,addr = 0x18 ,bus_num = 1):
    self.__addr = addr
    super(H3LIS200DL_I2C, self).__init__()
    self.i2c = I2C(bus_num)
    
  '''
    @brief Set the measurement range
    @param range:Range(g)
                 H3LIS200DL_100G = 100   #±100g
                 H3LIS200DL_200G = 200   #±200g
  '''
  def set_range(self,range_r):
    global _range 
    value = self.read_reg(self.REG_CTRL_REG4)
    _range = range_r
    if range_r == self.H3LIS200DL_100G:
     value = value & (~0x10)
    else:
     value = value | 0x10

  '''
    @brief Get the acceleration in the three directions of xyz
     @return Three-axis acceleration(g), the measurement range is ±100g or ±200g, set by the set_range() function
  '''
  def read_acce_xyz(self):
    global _range 
    value = self.read_reg(self.REG_STATUS_REG)
    data = [0,0,0,0,0,0,0]
    x = 0
    y = 0
    z = 0
    if(value & 0x01) == 1:
      base = self.REG_OUT_X_L
      for i in range(0,6):
        data[i] = self.read_reg(base+i)
      for i in range(0,6):
        data[i] = np.int8(data[i])
      x = (data[1]*_range)/128
      y = (data[3]*_range)/128
      z = (data[5]*_range)/128
    return x,y,z

  '''
    @brief writes data to a register
    @param reg register address
    @param data written data
  '''
  def write_reg(self, reg, data):
        self.i2c.writeto_mem(self.__addr ,reg,[data])
        
  '''
    @brief read the data from the register
    @param reg register address
    @rerun read data
  '''
  def read_reg(self, reg):
    self.i2c.writeto(self.__addr,reg)
    time.sleep(0.01)
    rslt = self.i2c.readfrom(self.__addr,1)
    return rslt[0]

class H3LIS200DL_SPI(LIS): 
  def __init__(self ,cs, bus = 0, dev = 0,speed = 100000):
    super(H3LIS200DL_SPI, self).__init__()
    self.__cs = cs
    self.__spi = spidev.SpiDev()
    self.__spi.open(bus, dev)
    self.__spi.no_cs = True
    self.__spi.max_speed_hz = speed
    
  '''
    @brief Set the measurement range
    @param range:Range(g)
                 H3LIS200DL_100G = 100   #±100g
                 H3LIS200DL_200G = 200   #±200g
  '''
  def set_range(self,range_r):
    global _range 
    value = self.read_reg(self.REG_CTRL_REG4)
    _range = range_r
    if range_r == self.H3LIS200DL_100G:
     value = value & (~0x10)
    else:
     value = value | 0x10

  '''
    @brief Get the acceleration in the three directions of xyz
    @return Three-axis acceleration(g), the measurement range is ±100g or ±200g, set by the set_range() function
  '''
  def read_acce_xyz(self):
    global _range 
    value = self.read_reg(self.REG_STATUS_REG)
    data = [0,0,0,0,0,0,0]
    x = 0
    y = 0
    z = 0
    if(value & 0x01) == 1:
      base = self.REG_OUT_X_L
      for i in range(0,6):
        data[i] = self.read_reg(base+i)
      for i in range(0,6):
        data[i] = np.int8(data[i])
      x = (data[1]*_range)/128
      y = (data[3]*_range)/128
      z = (data[5]*_range)/128

    return x,y,z

  '''
    @brief writes data to a register
    @param reg register address
    @param data written data
  '''
  def write_reg(self, reg, data):
     self.__spi.writebytes([reg,data])
     #self._spi.transfer(data)
  '''
    @brief read the data from the register
    @param reg register address
    @return read data
  '''
  def read_reg(self, reg):
     self.__spi.writebytes([reg|self.SPI_READ_BIT])
     time.sleep(0.01)
     data = self.__spi.readbytes(1)
     return  data[0]
     
class LIS331HH_I2C(LIS): 
  def __init__(self ,addr = 0x18, bus_num = 1):
    self.__addr = addr
    super(LIS331HH_I2C, self).__init__()
    self.i2c = I2C(bus_num)
    
  '''
    @brief Set the measurement range
    @param range:Range(g)
                 LIS331HH_6G  = 6  #±6G
                 LIS331HH_12G = 12 #±12G
                 LIS331HH_24G = 24 #±24G
  '''
  def set_range(self,range_r):
    global _range   
    value = self.read_reg(self.REG_CTRL_REG4)
    _range = range_r
    value = value&(~(3<<4))
    if range_r == self.LIS331HH_6G:
     value = value | (0x0<<4)
    elif range_r == self.LIS331HH_12G:
     value = value | (0x01<<4)
    elif range_r == self.LIS331HH_24G:
     value = value | (0x03<<4)
    self.write_reg(self.REG_CTRL_REG4,value)

  '''
    @brief Get the acceleration in the three directions of xyz
    @return Three-axis acceleration(mg), the measurement range is ±6g, ±12g or ±24g, set by the set_range() function
  '''
  def read_acce_xyz(self):
    global _range   
    value = self.read_reg(self.REG_STATUS_REG)
    data = [0,0,0,0,0,0,0]
    x = 0
    y = 0
    z = 0
    if(value & 0x01) == 1:
      base = self.REG_OUT_X_L
      for i in range(0,6):
        data[i] = self.read_reg(base+i)
      for i in range(0,6):
        data[i] = np.int8(data[i])

      x = data[1]*256+data[0]
      x = (x*1000*_range)/(256*128)
      y = data[3]*256+data[2]
      y = (y*1000*_range)/(256*128)
      z = data[5]*256+data[4]
      z = (z*1000*_range)/(256*128)
    return x,y,z
    
  '''
    @brief writes data to a register
    @param reg register address
    @param data written data
  '''
  def write_reg(self, reg, data):
        self.i2c.writeto_mem(self.__addr ,reg,[data])
  '''
    @brief read the data from the register
    @param reg register address
    @return read data
  '''
  def read_reg(self, reg):
    self.i2c.writeto(self.__addr, [reg])
    time.sleep(0.01)
    rslt = self.i2c.readfrom(self.__addr,1)
    return rslt

class LIS331HH_SPI(LIS): 
  def __init__(self ,cs, bus = 0, dev = 0,speed = 1000000):
    super(LIS331HH_SPI, self).__init__()
    self.__cs = cs
    self.__spi = spidev.SpiDev()
    self.__spi.open(bus, dev)
    self.__spi.no_cs = True
    self.__spi.max_speed_hz = speed
    
  '''
    @brief Set the measurement range
    @param range:Range(g)
                 LIS331HH_6G  = 6  #±6G
                 LIS331HH_12G = 12 #±12G
                 LIS331HH_24G = 24 #±24G
  '''
  def set_range(self,range_r):
    global _range   
    value = self.read_reg(self.REG_CTRL_REG4)
    _range = range_r
    value = value&(~(3<<4))
    if range_r == self.LIS331HH_6G:
     value = value | (0x0<<4)
    elif range_r == self.LIS331HH_12G:
     value = value | (0x01<<4)
    elif range_r == self.LIS331HH_24G:
     value = value | (0x03<<4)
    self.write_reg(self.REG_CTRL_REG4,value)

  '''
    @brief Get the acceleration in the three directions of xyz
    @return Three-axis acceleration(mg), the measurement range is ±6g, ±12g or ±24g, set by the set_range() function
  '''
  def read_acce_xyz(self):
    global _range   
    value = self.read_reg(self.REG_STATUS_REG)
    data = [0,0,0,0,0,0,0]
    x = 0
    y = 0
    z = 0
    #print(_range)
    if(value & 0x01) == 1:
      base = self.REG_OUT_X_L
      for i in range(0,6):
        data[i] = self.read_reg(base+i)
      for i in range(0,6):
        data[i] = np.int8(data[i])

      x = data[1]*256+data[0]
      x = (x*1000*_range)/(256*128)
      y = data[3]*256+data[2]
      y = (y*1000*_range)/(256*128)
      z = data[5]*256+data[4]
      z = (z*1000*_range)/(256*128)
    return x,y,z

  '''
    @brief writes data to a register
    @param reg register address
    @param data written data
  '''
  def write_reg(self, reg, data):
     self.__spi.writebytes([reg,data])
     #self._spi.transfer(data)
  '''
    @brief read the data from the register
    @param reg register address
    @return read data
  '''
  def read_reg(self, reg):
     self.__spi.writebytes([reg|self.SPI_READ_BIT])
     time.sleep(0.01)
     data = self.__spi.readbytes(1)
     #print(data)
     return  data[0]

