from pinpong.board import gboard,I2C
import time

class ADS1115:
  ADS1115_IIC_ADDRESS0         = 0x48
  ADS1115_IIC_ADDRESS1         = 0x49
  DFROBOT_ADS1115_POINTER_LOWTHRESH = 0x02
  ADS1115_REG_POINTER_HITHRESH = 0x03

  DFROBOT_ADS1115_POINTER_CONFIG      = 0x01
  DFROBOT_ADS1115_POINTER_CONVERT     = 0x00
  ADSMUX_MASK     = 0x0007
  ADSMUX_1        = 0x0000
  ADSMUX_2        = 0x0001
  ADSMUX_3        = 0x0002
  ADSMUX_4        = 0x0003
  ADSMUX_5        = 0x0004
  ADSMUX_6        = 0x0005
  ADSMUX_7        = 0x0006
  ADSMUX_8        = 0x0007

  #ADSCompMod_t
  COMPMODE_TRAD   = 0
  COMPMODE_WINDOW = 1

  #ADSCompPol_t
  COMPPOL_LOW  =  0
  COMPPOL_HIGH =  1


  #ADSCompLat_t
  COMPLAT_NONLAT = 0
  COMPLAT_LATCH  = 1

  #ADSCompQue_t
  COMPQUE_ONE   = 0
  COMPQUE_TWO   = 1
  COMPQUE_FOUR  = 2
  COMPQUE_NONE  = 3

  #ADSOSMod_t
  OSMODE_SINGLE  = 1
  OSMODE_BUSY    = 0
  OSMODE_NOTBUSY = 1

  #ADSRat_t
  RATE_8               = 0
  RATE_16              = 1
  RATE_32              = 2
  RATE_64              = 3
  RATE_128             = 4
  RATE_250             = 5
  RATE_475             = 6
  RATE_860             = 7

  #ADSGain_t
  GAIN_TWOTHIRDS = 0
  GAIN_ONE       = 1
  GAIN_TWO       = 2
  GAIN_FOUR      = 3
  GAIN_EIGHT     = 4
  GAIN_SIXTEEN   = 5
  
  #ADSMod_t
  MODE_CONTIN = 0
  MODE_SINGLE = 1

  ads_conversionDelay = 100

  def __init__(self, board=None, i2c_addr=ADS1115_IIC_ADDRESS0,bus_num=0):
    if isinstance(board, int):
      i2c_addr = board 
      board = gboard 

    self.board = board
    self.i2c_addr = i2c_addr
    self.i2c = I2C(bus_num)
    self.buf=[]
    self.coefficient = 0.0625
    #print("default:" + str(self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)))
    self.set_gain(self.GAIN_TWOTHIRDS)
    #print("set gain:" + str(self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)))
    self.set_mode(self.MODE_SINGLE)
    #print("set mode:" + str(self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)))
    self.set_rate(self.RATE_128)
    #print("set rate:" + str(self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)))
    self.set_OSMode(self.OSMODE_SINGLE)
    #print("set OSMode:" + str(self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)))
    self.set_high_threshold(80000)



  def comparator_voltage(self, channel):
    if channel == "01":
      self.set_mux(self.ADSMUX_1)
    elif channel =="03":
      self.set_mux(self.ADSMUX_2)
    elif channel =="13":
      self.set_mux(self.ADSMUX_3)
    elif channel =="23":
      self.set_mux(self.ADSMUX_4)
    else:
      print("incorrect channel")

    time.sleep(self.ads_conversionDelay/1000)
    raw_data = self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONVERT)
    if raw_data > 32767:
      raw_data -= 65535
    voltage = raw_data*self.coefficient
    return int(voltage)



  def read_voltage(self, channel):
    if channel >3:
      return 0
    self.set_comp_que(self.COMPQUE_NONE)
    self.set_comp_lat(self.COMPLAT_NONLAT)
    self.set_comp_pol(self.COMPPOL_LOW)
    self.set_comp_mode(self.COMPMODE_TRAD)
    if channel == 0:
      self.set_mux(self.ADSMUX_5)
    elif channel ==1:
      self.set_mux(self.ADSMUX_6)
    elif channel ==2:
      self.set_mux(self.ADSMUX_7)
    elif channel ==3:
      self.set_mux(self.ADSMUX_8)
    else:
      print("incorrect channel")

    #print("after configuring the mux:" + str(self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)))
    time.sleep(self.ads_conversionDelay/1000)
    #print(self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONVERT))
    #print("self.coefficient:")
    #print(self.coefficient)
    voltage = (self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONVERT))*self.coefficient
    return int(voltage)


  def set_high_threshold(self, threshold):
    self.ads_highthreshold = threshold
    self.__write_ads_reg(self.ADS1115_REG_POINTER_HITHRESH, self.ads_highthreshold) 

  def set_mux(self, value):
    conf = self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)
    conf &= ~(0x07<<12)
    conf |= (value<<12)
    #print("set mux:" + str(conf))
    self.__write_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG, conf)   


  def set_comp_que(self, value):
    conf = self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)
    conf &= ~(0x03)
    conf |= value
    self.__write_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG, conf)   

  def set_comp_lat(self, value):
    conf = self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)
    conf &= ~(0x01<<2)
    conf |= (value<<2)
    self.__write_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG, conf)   

  def set_comp_pol(self, value):
    conf = self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)
    conf &= ~(0x01<<3)
    conf |= (value<<3)
    self.__write_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG, conf)   

  def set_comp_mode(self, value):
    conf = self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)
    conf &= ~(0x01<<4)
    conf |= (value<<4)
    self.__write_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG, conf)   


  def set_OSMode(self, value):
    conf = self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)
    conf &= ~(0x01<<15)
    conf |= (value<<15)
    self.__write_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG, conf)    



  def set_rate(self, value):
    conf = self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)
    conf &= ~(0x07<<5)
    conf |= (value<<5)
    self.__write_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG, conf)    

  def set_mode(self, value):
    conf = self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)
    conf &= ~(0x01<<8)
    conf |= (value<<8)
    self.__write_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG, conf)

  def set_gain(self, value):
    conf = self.__read_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG)
    conf &= ~(0x07 << 9)
    conf |= (value << 9)
    self.__write_ads_reg(self.DFROBOT_ADS1115_POINTER_CONFIG, conf)
    if value == self.GAIN_TWO:
      self.coefficient = 0.0625
    elif value == self.GAIN_TWOTHIRDS:
      self.coefficient = 0.1875
    elif value == self.GAIN_ONE:
      self.coefficient = 0.125
    elif value == self.GAIN_FOUR:
      self.coefficient = 0.03125
    elif value == self.GAIN_EIGHT:
      self.coefficient = 0.015625
    elif value == self.GAIN_SIXTEEN:
      self.coefficient = 0.0078125


  def __read_ads_reg(self,reg):
    data = self.i2c.readfrom_mem(self.i2c_addr, reg, 2)
    return (data[0] << 8) | data[1]

  def __write_ads_reg(self,reg,data):
    data = [data>>8,data&0xff]
    self.i2c.writeto_mem(self.i2c_addr, reg, data)
