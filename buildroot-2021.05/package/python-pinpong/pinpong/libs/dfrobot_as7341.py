import time
from pinpong.board import gboard, I2C

class AS7341:
  REG_AS7341_ASTATUS    = 0X60
  
#  REG_AS7341_CH0_DATA_L = 0X61
#  REG_AS7341_CH0_DATA_H = 0X62
#  REG_AS7341_ITIME_L    = 0X63
#  REG_AS7341_ITIME_M    = 0X64
#  REG_AS7341_ITIME_H    = 0X65
#  REG_AS7341_CH1_DATA_L = 0X66
#  REG_AS7341_CH1_DATA_H = 0X67
#  REG_AS7341_CH2_DATA_L = 0X68
#  REG_AS7341_CH2_DATA_H = 0X69
#  REG_AS7341_CH3_DATA_L = 0X6A
#  REG_AS7341_CH3_DATA_H = 0X6B
#  REG_AS7341_CH4_DATA_L = 0X6C
#  REG_AS7341_CH4_DATA_H = 0X6D
#  REG_AS7341_CH5_DATA_L = 0X6E
#  REG_AS7341_CH5_DATA_H = 0X6F    
  
  REG_AS7341_CONFIG      =  0X70
  REG_AS7341_STAT        =  0X71
  REG_AS7341_EDGE        =  0X72
  REG_AS7341_CPIO        =  0X73
  REG_AS7341_LED         =  0X74
  
  REG_AS7341_ENABLE      =  0X80
  REG_AS7341_ATIME       =  0X81
  REG_AS7341_WTIME       =  0X83

  REG_AS7341_SP_TH_L_LSB = 0X84
  REG_AS7341_SP_TH_L_MSB = 0X85
  REG_AS7341_SP_TH_H_LSB = 0X86
  REG_AS7341_SP_TH_H_MSB = 0X87
  REG_AS7341_AUXID       = 0X90
  REG_AS7341_REVID       = 0X91
  
  REG_AS7341_ID          = 0X92
  REG_AS7341_STATUS_1    = 0X93
  REG_AS7341_ASTATUS     = 0X94
  
  REG_AS7341_CH0_DATA_L  = 0X95
  REG_AS7341_CH0_DATA_H  = 0X96
  REG_AS7341_CH1_DATA_L  = 0X97
  REG_AS7341_CH1_DATA_H  = 0X98
  REG_AS7341_CH2_DATA_L  = 0X99
  REG_AS7341_CH2_DATA_H  = 0X9A
  REG_AS7341_CH3_DATA_L  = 0X9B
  REG_AS7341_CH3_DATA_H  = 0X9C
  REG_AS7341_CH4_DATA_L  = 0X9D
  REG_AS7341_CH4_DATA_H  = 0X9E
  REG_AS7341_CH5_DATA_L  = 0X9F
  REG_AS7341_CH5_DATA_H  = 0XA0
  
  REG_AS7341_STATUS_2    = 0XA3
  REG_AS7341_STATUS_3    = 0XA4
  REG_AS7341_STATUS_5    = 0XA6
  REG_AS7341_STATUS_6    = 0XA7
  REG_AS7341_CFG_0       = 0XA9
  REG_AS7341_CFG_1       = 0XAA
  REG_AS7341_CFG_3       = 0XAC
  REG_AS7341_CFG_6       = 0XAF
  REG_AS7341_CFG_8       = 0XB1
  REG_AS7341_CFG_9       = 0XB2
  REG_AS7341_CFG_10      = 0XB3
  REG_AS7341_CFG_12      = 0XB5
  
  REG_AS7341_PERS        =  0XBD
  REG_AS7341_GPIO_2      =  0XBE
  REG_AS7341_ASTEP_L     =  0XCA
  REG_AS7341_ASTEP_H     =  0XCB
  REG_AS7341_AGC_GAIN_MAX=  0XCF
  REG_AS7341_AZ_CONFIG   =  0XD6
  REG_AS7341_FD_TIME_1   =  0XD8
  REG_AS7341_TIME_2      =  0XDA
  REG_AS7341_CFG0        =  0XD7
  REG_AS7341_STATUS      =  0XDB
  REG_AS7341_INTENAB     =  0XF9
  REG_AS7341_CONTROL     =  0XFA
  REG_AS7341_FIFO_MAP    =  0XFC
  REG_AS7341_FIFO_LVL    =  0XFD
  REG_AS7341_FDATA_L     =  0XFE
  REG_AS7341_FDATA_H     =  0XFF
  
  AS7341_GPIO            =   4
  
  ERR_OK           =   0 #OK
  ERR_DATA_BUS     =  -1 #Data Bus error
  ERR_IC_VERSION   =  -2 #Chip version mismatch 

  
  # The measurement mode of spectrum snesor 
  e_mode_t={
    'e_spm'  : 0, #<SPM>
    'e_syns' : 1, #<SYNS>
    'e_synd' : 3 #<SYND>
   }

  e_ch_choose_t={
    'e_f1_to_f4_clear_nir' : 0,
    'e_f5_to_f8_clear_nir' : 1
  }
  
  # Represent 10 different photodiode measurement channels 
  e_channel_t={
    'e_ch_f1   ' : 0,
    'e_ch_f2   ' : 1,
    'e_ch_f3   ' : 2, 
    'e_ch_f4   ' : 3,
    'e_ch_f5   ' : 4,
    'e_ch_f6   ' : 5,
    'e_ch_f7   ' : 6,
    'e_ch_f8   ' : 7,
    'e_ch_clear' : 8,
    'e_ch_nir  ' : 9
  }
  
  # The values of the registers of 6 channels under eF1F4ClearNIR
  s_mode_one_data_t={
    'adf1   ' : 0,
    'adf2   ' : 1,
    'adf3   ' : 2,
    'adf4   ' : 3,
    'adclear' : 4,
    'adnir  ' : 5
  }
  
  # The values of the registers of 6 channels under eF5F8ClearNIR
  s_mode_two_data_t={
    'adf5   ' : 0,
    'adf6   ' : 1,
    'adf7   ' : 2,
    'adf8   ' : 3,
    'adclear' : 4,
    'adnir  ' : 5
  }
  
  measure_mode = e_mode_t
  
  def __init__(self, board=None, i2c_addr= 0x39, bus_num=0):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard
  
    self.board = board
    self.i2c_addr = i2c_addr
    self.i2c = I2C(bus_num)
    self.e_mode_t = self.e_mode_t
  
  def begin(self, mode = e_mode_t['e_spm']):
    self.measure_mode = mode
    rslt = self.enable_as7341(True)
    return rslt
  
  def read_ID(self):
    id = -1
    id = self._read_reg(self.REG_AS7341_ID,id,1)
    if(id ==0 ):
      print("id read error")
      return 0
    else:
      return id
  
  def enable_as7341(self, on):
    data = -1
    data = self._read_reg(self.REG_AS7341_ENABLE,data,1)
#    print("enable_as7341 readreg data:", data)
    if(on == True):
      data = data | (1<<0)
    else:
      data = data & (~1)
#    print("enable_as7341 writereg data:", data)
    self._write_reg(self.REG_AS7341_ENABLE, data, 1)
    if data != -1:
     return True
    else:
      return False
  
  def enable_spectral_measure(self,on):
    data = 0
    data = self._read_reg(self.REG_AS7341_ENABLE,data)
#    print("enable_spectral_measure readreg" ,data)
    if(on == True):
      data = data | (1<<1)
    else:
      data = data & (~(1<<1))
    self._write_reg(self.REG_AS7341_ENABLE,data)
  
  def enable_wait(self,on):
    data = 0
    data = self._read_reg(self.REG_AS7341_ENABLE,data)
    if(on == True):
      data = data | (1<<3)
    else:
      data = data & (~(1<<3))
    self._write_reg(self.REG_AS7341_ENABLE,data)
  
  def enable_SMUX(self,on):
    data = 0
    data = self._read_reg(self.REG_AS7341_ENABLE,data)
    if(on == True):
      data = data | (1<<4)
    else:
      data = data & (~(1<<4))
    self._write_reg(self.REG_AS7341_ENABLE,data)
  
  def enable_filcker_detection(self,on):
    data = 0
    data = self._read_reg(self.REG_AS7341_ENABLE,data)
    if(on == True):
      data = data | (1<<6)
    else:
      data = data & (~(1<<6))
    self._write_reg(self.REG_AS7341_ENABLE,data)
  
  def config(self,mode):
    data = 0
    self.set_bank(1)
    data = self._read_reg(self.REG_AS7341_CONFIG,data)
#    print('config data' ,data)
    if mode in self.e_mode_t:
        data = (data & (~3)) | (self.e_mode_t(mode))
    self._write_reg(self.REG_AS7341_CONFIG,data)
    self.set_bank(0)
  
  def f1_f4_clear_nir(self):
    self._write_reg(0x00,0x30)
    self._write_reg(0x01,0x01)
    self._write_reg(0x02,0x00)
    self._write_reg(0x03,0x00)
    self._write_reg(0x04,0x00)
    self._write_reg(0x05,0x42)
    self._write_reg(0x06,0x00)
    self._write_reg(0x07,0x00)
    self._write_reg(0x08,0x50)
    self._write_reg(0x09,0x00)
    self._write_reg(0x0A,0x00)
    self._write_reg(0x0B,0x00)
    self._write_reg(0x0C,0x20)
    self._write_reg(0x0D,0x04)
    self._write_reg(0x0E,0x00)
    self._write_reg(0x0F,0x30)
    self._write_reg(0x10,0x01)
    self._write_reg(0x11,0x50)
    self._write_reg(0x12,0x00)
    self._write_reg(0x13,0x06)

  def f5_f8_clear_nir(self):
    self._write_reg(0x00, 0x00)
    self._write_reg(0x01, 0x00)
    self._write_reg(0x02, 0x00)
    self._write_reg(0x03, 0x40)
    self._write_reg(0x04, 0x02)
    self._write_reg(0x05, 0x00)
    self._write_reg(0x06, 0x10)
    self._write_reg(0x07, 0x03)
    self._write_reg(0x08, 0x50)
    self._write_reg(0x09, 0x10)
    self._write_reg(0x0A, 0x03)
    self._write_reg(0x0B, 0x00)
    self._write_reg(0x0C, 0x00)
    self._write_reg(0x0D, 0x00)
    self._write_reg(0x0E, 0x24)
    self._write_reg(0x0F, 0x00)
    self._write_reg(0x10, 0x00)
    self._write_reg(0x11, 0x50)
    self._write_reg(0x12, 0x00)
    self._write_reg(0x13, 0x06)
    
  def fd_config(self):
    self._write_reg(0x00, 0x00) 
    self._write_reg(0x01, 0x00) 
    self._write_reg(0x02, 0x00) 
    self._write_reg(0x03, 0x00) 
    self._write_reg(0x04, 0x00) 
    self._write_reg(0x05, 0x00) 
    self._write_reg(0x06, 0x00) 
    self._write_reg(0x07, 0x00) 
    self._write_reg(0x08, 0x00) 
    self._write_reg(0x09, 0x00) 
    self._write_reg(0x0A, 0x00) 
    self._write_reg(0x0B, 0x00) 
    self._write_reg(0x0C, 0x00) 
    self._write_reg(0x0D, 0x00) 
    self._write_reg(0x0E, 0x00) 
    self._write_reg(0x0F, 0x00) 
    self._write_reg(0x10, 0x00) 
    self._write_reg(0x11, 0x00) 
    self._write_reg(0x12, 0x00) 
    self._write_reg(0x13, 0x60) 
  
  def start_measure(self, mode):
    data = -1
    data = self._read_reg(self.REG_AS7341_CFG_0,data)
#    print("start_measure readreg:" ,data)
    data = data & (~(1<<4))
    self._write_reg(self.REG_AS7341_CFG_0,data)
    self.enable_spectral_measure(False)
    self._write_reg(0xAF,0x10)
    if(mode == self.e_ch_choose_t['e_f1_to_f4_clear_nir']):
      self.f1_f4_clear_nir()
    elif(mode == self.e_ch_choose_t['e_f5_to_f8_clear_nir']):
      self.f5_f8_clear_nir()
    self.enable_SMUX(True)
#    print("self.measure_mode:",self.measure_mode)
    if(self.measure_mode == self.e_mode_t['e_syns']):
      self.set_gpio_mode('INPUT')
      self.config(self.measure_mode)
    elif(self.measure_mode == self.e_mode_t['e_spm']):
      self.config(self.measure_mode)
    self.enable_spectral_measure(True)
    if(self.measure_mode == self.e_mode_t['e_spm']):
      while(self.measure_complete() == False):
        time.sleep(0.5)
  
  def read_flicker_data(self):
    filcker = 0
    data    = 0
    data = self._read_reg(self.REG_AS7341_CFG_0,data)
    data = data & (~(1<<4))
    self._write_reg(self.REG_AS7341_CFG_0,data)
    self.enable_spectral_measure(False)
    self._write_reg(0xAF,0x10)
    self.fd_config()
    self.enable_SMUX(True)
    self.enable_spectral_measure(True)
    retry = 100
    if(retry == 0 ): print("data access error")
    self.enable_filcker_detection(True)
    time.sleep(0.6)
    filcker = self._read_reg(self.REG_AS7341_STATUS,filcker)
    self.enable_filcker_detection(False)
    if(filcker == 44):   filcker = 1
    elif(filcker == 45): filcker = 50
    elif(filcker == 46): filcker = 60
    else:                filcker = 0
    return filcker
  
  def measure_complete(self):
    status = 0
    status = self._read_reg(self.REG_AS7341_STATUS_2,status)
    if((status & (1<<6)) != 0):
      return True
    else: 
      return False
  
  def get_channel_data(self,channel):
    data = [0,0]
    channel_data = 0
    data[0] = self._read_reg(self.REG_AS7341_CH0_DATA_L + channel * 2, data[0])
    data[1] = self._read_reg(self.REG_AS7341_CH0_DATA_H + channel * 2, data[1])
    print(data)
    channel_data = (data[1]<<8)|data[0]
    time.sleep(0.1)
    return channel_data
  
  def read_spectral_data_one(self):
    data = {}
    data['adf1']    = self.get_channel_data(0)
    data['adf2']    = self.get_channel_data(1)
    data['adf3']    = self.get_channel_data(2)
    data['adf4']    = self.get_channel_data(3)
    data['adclear'] = self.get_channel_data(4)
    data['adnir']   = self.get_channel_data(5)
    return data
  
  def read_spectral_data_two(self):
    data = {}
    data['adf5']    = self.get_channel_data(0)
    data['adf6']    = self.get_channel_data(1)
    data['adf7']    = self.get_channel_data(2)
    data['adf8']    = self.get_channel_data(3)
    data['adclear'] = self.get_channel_data(4)
    data['adnir']   = self.get_channel_data(5)
    return data

#获取as7341传感器的值
  def get_spectrum_data(self):
    self.start_measure(0)
    data1 = self.read_spectral_data_one()
    self.start_measure(1)
    data2 = self.read_spectral_data_two()
    data = {**data1, **data2}
    del data['adclear']
    del data['adnir']
    return data
  
  def set_gpio(self,connect):
    data = 0
    data = self._read_reg(self.REG_AS7341_CPIO,data)
    if(connect == True): data = data | (1<<0)
    else: data = data & (~(1<<0))
    self._write_reg(self.REG_AS7341_CPIO,data)
  
  def set_gpio_mode(self,mode):
    data = 0
    data = self._read_reg(self.REG_AS7341_GPIO_2,data)
    if(mode == 'INPUT'):
      data = data | (1<<2)
    if(mode == 'OUTPUT'):
      data = data & (~(1<<2))
    self._write_reg(self.REG_AS7341_GPIO_2,data)
    
  def enable_led(self,on):
    data  = 0
    data1 = 0
    self.set_bank(1)
    data  = self._read_reg(self.REG_AS7341_CONFIG,data)
    data1 = self._read_reg(self.REG_AS7341_LED,data1)
    if(on == True):
      data = data | (1<<3)
      data1 = data1 | (1<<7)
    else :
      data = data & (~(1<<3))
      data1 = data1 & (~(1<<7))
    self._write_reg(self.REG_AS7341_CONFIG,data)
    self._write_reg(self.REG_AS7341_LED,data1)
    self.set_bank(0)
    
  def set_bank(self, addr):
    data = 0
    data = self._read_reg(self.REG_AS7341_CFG_0,data)
    if(addr == 1):
        data = data | (1<<4)
    else:
        data = data & (~(1<<4))
    self._write_reg(self.REG_AS7341_CFG_0,data)
  
  def control_led(self,current):
    data = 0
    if(current < 1): current = 1
    current -=1
    if(current > 19): current = 19
    self.set_bank(1)
    data = 0
    data = data | (1<<7)
    data = data | (current & 0x7f)
    self._write_reg(self.REG_AS7341_LED,data)
    time.sleep(0.01)
    self.set_bank(0)
  
  def set_int(self,connect):
    data = 0
    data = self._read_reg(self.REG_AS7341_CPIO,data)
    if(connect == True): data = data | (1<<1)
    else: data = data & (~(1<<1))
    self._write_reg(self.REG_AS7341_CPIO,data)
  
  def enable_sys_int(self,on):
    data = 0
    data = self._read_reg(self.REG_AS7341_INTENAB,data)
    if(on == True): data = data | (1<<0)
    else: data = data & (~(1<<0))
    self._write_reg(self.REG_AS7341_INTENAB,data)
  
  def enable_FIFO_int(self,on):
    data = 0
    data = self._read_reg(self.REG_AS7341_INTENAB,data)
    if(on == True): data = data | (1<<2)
    else: data = data & (~(1<<2))
    self._write_reg(self.REG_AS7341_INTENAB,data)
  
  def enable_spectral_int(self,on):
    data = 0
    data = self._read_reg(self.REG_AS7341_INTENAB,data)
    if(on == True): data = data | (1<<3)
    else: data = data & (~(1<<3))
    self._write_reg(self.REG_AS7341_INTENAB,data)
  
  def end_sleep(self):
    data = 0
    data = self._read_reg(self.REG_AS7341_INTENAB,data)
    data = data | (1<<3)
    self._write_reg(self.REG_AS7341_INTENAB,data)
  
  def clear_FIFO(self):
    data = 0
    data = self._read_reg(self.REG_AS7341_CONTROL,data)
    data = data | (1<<0)
    data = data & (~(1<<0))
    self._write_reg(self.REG_AS7341_CONTROL,data)
  
  def spectral_auto_zero(self):
    data = 0
    data = self._read_reg(self.REG_AS7341_CONTROL,data)
    data = data | (1<<1)
    self._write_reg(self.REG_AS7341_CONTROL,data)

  def enableFlickerInt(self,on):
    data = 0
    data = self._read_reg(self.REG_AS7341_INTENAB,data)
    if(on == True):data = data | (1<<2)
    else :         data = data & (~(1<<2))
    self._write_reg(self.REG_AS7341_INTENAB,data)

  def set_a_time(self,value):
    self._write_reg(self.REG_AS7341_ATIME,value)
  
  def set_again(self,value):
    if(value > 0): value = 10
    self._write_reg(self.REG_AS7341_CFG_1,value)
  
  def set_a_step(self,value):
    lowValue = value & 0x00ff
    highValue = value >> 8 
    self._write_reg(self.REG_AS7341_ASTEP_L,lowValue)
    self._write_reg(self.REG_AS7341_ASTEP_H,highValue)
  
  def get_integration_time(self):
    data = 0
    astep_data = [0,0]
    astep = 0
    data          = self._read_reg(self.REG_AS7341_ATIME,data)
    astep_data[0] = self._read_reg(self.REG_AS7341_ASTEP_L,astep_data[0])
    astep_data[1] = self._read_reg(self.REG_AS7341_ASTEP_L,astep_data[1])
    astep = astep_data[1]
    astep = (astep << 8) | astep_data[0]
    if(data == 0):pass
    elif((data >0) &(data<255)):pass
    elif(data == 255): pass
  
  def set_w_time(self,value):
    self._write_reg(self.REG_AS7341_WTIME,value)
  
  def get_w_time(self):
    value = 0
    data = 0
    data = self._read_reg(self.REG_AS7341_WTIME,data)
    if(data == 0): value = 2.78
    elif(data == 1): value = 5.56
    elif((data > 1) &( data < 255)): value = 2.78*(data+1)
    elif(data == 255): value = 711
    return value
  
  def set_threshold(self,lowTh,highTh):
    if(lowTh >= highTh): return
    self._write_reg(self.REG_AS7341_SP_TH_H_MSB,highTh>>8)
    self._write_reg(self.REG_AS7341_SP_TH_H_LSB,highTh)
    self._write_reg(self.REG_AS7341_SP_TH_L_MSB,lowTh>>8)
    self._write_reg(self.REG_AS7341_SP_TH_L_LSB,lowTh)
    time.sleep(0.01)

  def get_low_threshold(self):
    data = 0
    data = self._read_reg(self.REG_AS7341_SP_TH_L_MSB)
    data = self._read_reg(self.REG_AS7341_SP_TH_L_LSB) | (data<<8)
    return data
  
  def get_high_threshold(self):
    data = 0
    data = self._read_reg(self.REG_AS7341_SP_TH_H_MSB)
    data = self._read_reg(self.REG_AS7341_SP_TH_H_LSB) | (data<<8)
    return data
  
  def clear_interrupt(self):
    self._write_reg(self.REG_AS7341_STATUS_1,0xff)
  
  def enable_spectral_interrupt(self,on):
    data = 0
    data = self._read_reg(self.REG_AS7341_INTENAB)
    if(on == True):
        data = data | (1<<3)
        self._write_reg(self.REG_AS7341_INTENAB,data)
    else:
        data = data & (~(1<<3))
        self._write_reg(self.REG_AS7341_INTENAB,data)
  
  def set_int_channel(self,channel):
    if(channel >= 5): return
    data = self._read_reg(self.REG_AS7341_CFG_12)
    data = data & (~7)
    data = data | channel
    self._write_reg(self.REG_AS7341_CFG_12,data)
  
  def set_APERS(self,num):
    data = self._read_reg(self.REG_AS7341_PERS)
    data = data & (~15)
    data = data | num
    self._write_reg(self.REG_AS7341_PERS,data)
 
  def get_int_source(self):
    return self._read_reg(self.REG_AS7341_STATUS_3)
  
  def interrupt(self):
    data = self._read_reg(self.REG_AS7341_STATUS_1)
    if(data & 0x80): return True
    else: return False
  
  def check_w_time(self):
    status = 0
    value  = 0
    self._read_reg(self.REG_AS7341_STATUS_6,status)
    value = status & (1<<2)
    if(value > 0): return False
    else: return True
    
  def _read_reg(self,reg,data = 0,size=1):
    data = self.i2c.readfrom_mem(self.i2c_addr,reg,size)
#    print("data:",data)
    return data[0]
  
  def _write_reg(self,reg,data,size=1):
    if(type(data)==int):
       data = [data]
#    print("data:",data)
    self.i2c.writeto_mem(self.i2c_addr,reg,data)  

