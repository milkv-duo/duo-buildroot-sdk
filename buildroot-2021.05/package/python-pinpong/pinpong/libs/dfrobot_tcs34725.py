# -*- coding: utf-8 -*-
import time
import math
from pinpong.board import gboard,I2C

class TCS34725:
  TCS34725_ADDRESS          = (0x29)
  TCS34725_COMMAND_BIT      = (0x80)
  TCS34725_ENABLE           = (0x00)
  TCS34725_ENABLE_AIEN      = (0x10)    #RGBC Interrupt Enable 
  TCS34725_ENABLE_WEN       = (0x08)    #Wait enable - Writing 1 activates the wait timer 
  TCS34725_ENABLE_AEN       = (0x02)    #RGBC Enable - Writing 1 actives the ADC, 0 disables it 
  TCS34725_ENABLE_PON       = (0x01)    #Power on - Writing 1 activates the internal oscillator, 0 disables it 
  TCS34725_ATIME            = (0x01)    #Integration time 
  TCS34725_WTIME            = (0x03)    #Wait time (if TCS34725_ENABLE_WEN is asserted) 
  TCS34725_WTIME_2_4MS      = (0xFF)    #WLONG0 = 2.4ms   WLONG1 = 0.029s 
  TCS34725_WTIME_204MS      = (0xAB)    #WLONG0 = 204ms   WLONG1 = 2.45s  
  TCS34725_WTIME_614MS      = (0x00)    #WLONG0 = 614ms   WLONG1 = 7.4s   
  TCS34725_AILTL            = (0x04)    #Clear channel lower interrupt threshold 
  TCS34725_AILTH            = (0x05)    #
  TCS34725_AIHTL            = (0x06)    #Clear channel upper interrupt threshold 
  TCS34725_AIHTH            = (0x07)    #
  TCS34725_PERS             = (0x0C)    #Persistence register - basic SW filtering mechanism for interrupts 
  TCS34725_PERS_NONE        = (0b0000)  #Every RGBC cycle generates an interrupt                                
  TCS34725_PERS_1_CYCLE     = (0b0001)  #1 clean channel value outside threshold range generates an interrupt   
  TCS34725_PERS_3_CYCLE     = (0b0011)  #3 clean channel values outside threshold range generates an interrupt  
  TCS34725_PERS_5_CYCLE     = (0b0100)  #5 clean channel values outside threshold range generates an interrupt  
  TCS34725_PERS_10_CYCLE    = (0b0101)  #10 clean channel values outside threshold range generates an interrupt 
  TCS34725_PERS_15_CYCLE    = (0b0110)  #15 clean channel values outside threshold range generates an interrupt 
  TCS34725_PERS_20_CYCLE    = (0b0111)  #20 clean channel values outside threshold range generates an interrupt 
  TCS34725_PERS_25_CYCLE    = (0b1000)  #25 clean channel values outside threshold range generates an interrupt 
  TCS34725_PERS_2_CYCLE     = (0b0010)  #2 clean channel values outside threshold range generates an interrupt  
  TCS34725_PERS_30_CYCLE    = (0b1001)  #30 clean channel values outside threshold range generates an interrupt 
  TCS34725_PERS_35_CYCLE    = (0b1010)  #35 clean channel values outside threshold range generates an interrupt 
  TCS34725_PERS_40_CYCLE    = (0b1011)  #40 clean channel values outside threshold range generates an interrupt 
  TCS34725_PERS_45_CYCLE    = (0b1100)  #45 clean channel values outside threshold range generates an interrupt 
  TCS34725_PERS_50_CYCLE    = (0b1101)  #50 clean channel values outside threshold range generates an interrupt 
  TCS34725_PERS_55_CYCLE    = (0b1110)  #55 clean channel values outside threshold range generates an interrupt 
  TCS34725_PERS_60_CYCLE    = (0b1111)  #60 clean channel values outside threshold range generates an interrupt 
  TCS34725_CONFIG           = (0x0D)    #
  TCS34725_CONFIG_WLONG     = (0x02)    #Choose between short and long (12x) wait times via TCS34725_WTIME 
  TCS34725_CONTROL          = (0x0F)    #Set the gain level for the sensor 
  TCS34725_ID               = (0x12)    #0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727 
  TCS34725_STATUS           = (0x13)    #
  TCS34725_STATUS_AINT      = (0x10)    #RGBC Clean channel interrupt 
  TCS34725_STATUS_AVALID    = (0x01)    #Indicates that the RGBC channels have completed an integration cycle 
  TCS34725_CDATAL           = (0x14)    #Clear channel data 
  TCS34725_CDATAH           = (0x15)    #
  TCS34725_RDATAL           = (0x16)    #Red channel data 
  TCS34725_RDATAH           = (0x17)    #
  TCS34725_GDATAL           = (0x18)    #Green channel data 
  TCS34725_GDATAH           = (0x19)    #
  TCS34725_BDATAL           = (0x1A)    #Blue channel data 
  TCS34725_BDATAH           = (0x1B)

  TCS34725_INTEGRATIONTIME_2_4MS  = 0.0024  #2.4ms - 1 cycle    - Max Count: 1024  
  TCS34725_INTEGRATIONTIME_24MS   = 0.024   #24ms  - 10 cycles  - Max Count: 10240 
  TCS34725_INTEGRATIONTIME_50MS   = 0.05    #50ms  - 20 cycles  - Max Count: 20480 
  TCS34725_INTEGRATIONTIME_101MS  = 0.101   #101ms - 42 cycles  - Max Count: 43008 
  TCS34725_INTEGRATIONTIME_154MS  = 0.154   #154ms - 64 cycles  - Max Count: 65535 
  TCS34725_INTEGRATIONTIME_700MS  = 0.7     #700ms - 256 cycles - Max Count: 65535 

  TCS34725_GAIN_1X                = 0x00  # No gain  
  TCS34725_GAIN_4X                = 0x01  # 4x gain  
  TCS34725_GAIN_16X               = 0x02  # 16x gain 
  TCS34725_GAIN_60X               = 0x03  # 60x gain 

  def __init__(self, board=None, it=TCS34725_INTEGRATIONTIME_50MS, gain=TCS34725_GAIN_4X, bus_num=0):
    if isinstance(board, int):
      it = TCS34725_INTEGRATIONTIME_50MS
      board = gboard
    elif board is None:
      board = gboard
      
    self.board = board
    self.i2c = I2C(bus_num)
    self._tcs34725Initialised = False
    self._tcs34725IntegrationTime = it
    self._tcs34725Gain = gain
    self._integrationtime_map={
      self.TCS34725_INTEGRATIONTIME_2_4MS  : 0xFF,
      self.TCS34725_INTEGRATIONTIME_24MS   : 0xF6,
      self.TCS34725_INTEGRATIONTIME_50MS   : 0xEB,
      self.TCS34725_INTEGRATIONTIME_101MS  : 0xD5,
      self.TCS34725_INTEGRATIONTIME_154MS  : 0xC0,
      self.TCS34725_INTEGRATIONTIME_700MS  : 0x00
    }

    # our RGB -> eye-recognized gamma color
    self.gammatable = [0 for i in range(256)]
    # it helps convert RGB colors to what humans see
    for  i in range(256):
      x = i
      x /= 255
      x = math.pow(x, 2.5)
      x *= 255
      self.gammatable[i] = 255.0 - x;

  def write_reg (self, reg, value):
    reg |= self.TCS34725_COMMAND_BIT
    value = [value&0xff]
    self.i2c.writeto_mem(self.TCS34725_ADDRESS, reg, value)


  def read_reg(self, reg):
    reg |= self.TCS34725_COMMAND_BIT
    return self.i2c.readfrom_mem(self.TCS34725_ADDRESS, reg, 1)

  def read_reg_word(self, reg):
    reg |= self.TCS34725_COMMAND_BIT
    return self.i2c.readfrom_mem(self.TCS34725_ADDRESS, reg, 2)

  def enable(self):
    self.write_reg(self.TCS34725_ENABLE, self.TCS34725_ENABLE_PON);
    time.sleep(0.003);
    self.write_reg(self.TCS34725_ENABLE, self.TCS34725_ENABLE_PON | self.TCS34725_ENABLE_AEN)

  def disable(self):
    reg = read_reg(self.TCS34725_ENABLE);
    write_reg(self.TCS34725_ENABLE, reg[0] & ~(self.TCS34725_ENABLE_PON | self.TCS34725_ENABLE_AEN));

  def begin(self):
    # Make sure we're actually connected
    reg = self.read_reg(self.TCS34725_ID)
    if (reg[0] != 0x44) and (reg[0] != 0x10):
      return False
    self._tcs34725Initialised = True;

    # Set default integration time and gain
    self.set_integration_time(self._tcs34725IntegrationTime)
    self.set_gain(self._tcs34725Gain)
    # Note: by default, the device is in power down mode on bootup 
    self.enable()
    return True

  def set_integration_time(self, it):
    # Update the timing register
    self.write_reg(self.TCS34725_ATIME, self._integrationtime_map[it])

    # Update value placeholders 
    self._tcs34725IntegrationTime = it

  def set_gain(self, gain):
    # Update the timing register
    self.write_reg(self.TCS34725_CONTROL, gain)

    # Update value placeholders
    self._tcs34725Gain = gain


  def get_rgbc(self):
    c = self.read_reg_word(self.TCS34725_CDATAL)
    time.sleep(0.01)
    r = self.read_reg_word(self.TCS34725_RDATAL)
    time.sleep(0.01)
    g = self.read_reg_word(self.TCS34725_GDATAL)
    time.sleep(0.01)
    b = self.read_reg_word(self.TCS34725_BDATAL)
    time.sleep(0.01)
    c = c[0]+c[1]*256
    r = r[0]+r[1]*256
    g = g[0]+g[1]*256
    b = b[0]+b[1]*256
    # Set a delay for the integration time
    time.sleep(self._tcs34725IntegrationTime)
    return r,g,b,c

  def calculate_color_temperature(r, g, b):
    # 1. Map RGB values to their XYZ counterparts.    
    # Based on 6500K fluorescent, 3000K fluorescent   
    # and 60W incandescent values for a wide range.   
    # Note: Y = Illuminance or lux                    
    X = (-0.14282 * r) + (1.54924 * g) + (-0.95641 * b);
    Y = (-0.32466 * r) + (1.57837 * g) + (-0.73191 * b);
    Z = (-0.68202 * r) + (0.77073 * g) + ( 0.56332 * b);
  
    # 2. Calculate the chromaticity co-ordinates
    xc = (X) / (X + Y + Z);
    yc = (Y) / (X + Y + Z);
  
    # 3. Use McCamy's formula to determine the CCT
    n = (xc - 0.3320) / (0.1858 - yc);
  
    # Calculate the final CCT
    cct = (449.0 * math.pow(n, 3)) + (3525.0 * math.pow(n, 2)) + (6823.3 * n) + 5520.33;
  
    # Return the results in degrees Kelvin
    return int(cct)

  def calculate_lux(self, r, g, b):
    # This only uses RGB ... how can we integrate clear or calculate lux
    # based exclusively on clear since this might be more reliable?
    illuminance = (-0.32466 * r) + (1.57837 * g) + (-0.73191 * b);
    return int(illuminance)

  def lock(self):
    ret = self.read_reg(self.TCS34725_ENABLE);
    ret[0] |= self.TCS34725_ENABLE_AIEN;
    self.write_reg(self.TCS34725_ENABLE, ret[0]);

  def unlock(self):
    ret = self.read_reg(self.TCS34725_ENABLE);
    ret[0] &= ~self.TCS34725_ENABLE_AIEN;
    self.write_reg(self.TCS34725_ENABLE, ret[0]);

  def clear(self):
    value = [self.TCS34725_COMMAND_BIT | 0x66]
    self.i2c.writeto(self.TCS34725_ADDRESS, value)

  def set_int_limits(self, low, high):
   self.write_reg(0x04, low & 0xFF)
   self.write_reg(0x05, low >> 8)
   self.write_reg(0x06, high & 0xFF)
   self.write_reg(0x07, high >> 8)
