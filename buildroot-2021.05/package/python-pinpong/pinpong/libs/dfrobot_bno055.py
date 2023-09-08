# -*- coding: utf-8 -*-
import time
from pinpong.board import gboard,I2C

class BNO055:
  axis_acc                  = 0
  axis_mag                  = 1
  axis_gyr                  = 2
  axis_lia                  = 3
  axis_grv                  = 4
                            
  single_axis_X              = 0
  single_axis_Y              = 1
  single_axis_Z              = 2
                            
  power_mode_normal           = 0
  power_mode_low_power        = 1
  power_mode_suspend          = 2
  
  acc_range_2G              = 0
  acc_range_4G              = 1
  acc_range_8G              = 2
  acc_range_16G             = 3
  
  acc_bandwidth_7_81        = 0
  acc_bandwidth_15_63       = 1
  acc_bandwidth_31_25       = 2
  acc_bandwidth_62_5        = 3
  acc_bandwidth_125         = 4
  acc_bandwidth_250         = 5
  acc_bandwidth_500         = 6
  acc_bandwidth_1000        = 7
  
  acc_power_mode_normal        = 0
  acc_power_mode_suspend       = 1
  acc_power_mode_low_power1    = 2
  acc_power_mode_standby       = 3
  acc_power_mode_low_power2    = 4
  acc_power_mode_deep_suspend  = 5
  
  mag_data_rate_2            = 0
  mag_data_rate_6            = 1
  mag_data_rate_8            = 2
  mag_data_rate_10           = 3
  mag_data_rate_15           = 4
  mag_data_rate_20           = 5
  mag_data_rate_25           = 6
  mag_data_rate_30           = 7
  
  mag_opr_mode_low_power        = 0
  mag_opr_mode_regular          = 1
  mag_opr_mode_enhanced_regular = 2
  mag_opr_mode_high_accuracy    = 3
  
  mag_power_mode_normal        = 0
  mag_power_mode_sleep         = 1
  mag_power_mode_suspend       = 2
  mag_power_mode_force         = 3
  
  gyr_range_2000            = 0
  gyr_range_1000            = 1
  gyr_range_500             = 2
  gyr_range_250             = 3
  gyr_range_125             = 4
                            
  gyr_bandwidth_523         = 0
  gyr_bandwidth_230         = 1
  gyr_bandwidth_116         = 2
  gyr_bandwidth_47          = 3
  gyr_bandwidth_23          = 4
  gyr_bandwidth_12          = 5
  gyr_bandwidth_64          = 6
  gyr_bandwidth_32          = 7
  
  gyr_power_mode_normal              = 0
  gyr_power_mode_fast_power_up       = 1
  gyr_power_mode_deep_suspend        = 2
  gyr_power_mode_suspend             = 3
  gyr_power_mode_advanced_powersave  = 4
  
  acc_sleep_mode_event_driven        = 0
  acc_sleep_mode_equidstant_sampling = 1
  
  acc_sleep_duration_0_5     = 5
  acc_sleep_duration_1       = 6
  acc_sleep_duration_2       = 7
  acc_sleep_duration_4       = 8
  acc_sleep_duration_6       = 9
  acc_sleep_duration_10      = 10
  acc_sleep_duration_25      = 11
  acc_sleep_duration_50      = 12
  acc_sleep_duration_100     = 13
  acc_sleep_duration_500     = 14
  acc_sleep_duration_1000    = 15
  
  gyr_sleep_duration_2       = 0
  gyr_sleep_duration_4       = 1
  gyr_sleep_duration_5       = 2
  gyr_sleep_duration_8       = 3
  gyr_sleep_duration_10      = 4
  gyr_sleep_duration_15      = 5
  gyr_sleep_duration_18      = 6
  gyr_sleep_duration_20      = 7
  
  gyr_auto_sleep_duration_No  = 0
  gyr_auto_sleep_duration_4   = 1
  gyr_auto_sleep_duration_5   = 2
  gyr_auto_sleep_duration_8   = 3
  gyr_auto_sleep_duration_10  = 4
  gyr_auto_sleep_duration_15  = 5
  gyr_auto_sleep_duration_20  = 6
  gyr_auto_sleep_duration_40  = 7
  
  acc_int_set_amnm_X_axis       = (0x01 << 2)
  acc_int_set_amnm_Y_axis       = (0x01 << 3)
  acc_int_set_amnm_Z_axis       = (0x01 << 4)
  acc_int_set_hg_X_axis         = (0x01 << 5)
  acc_int_set_hg_Y_axis         = (0x01 << 6)
  acc_int_set_hg_Z_axis         = (0x01 << 7)
  acc_int_set_all             = 0xfc

  gyr_int_set_am_X_axis         = (0x01 << 0)
  gyr_int_set_am_Y_axis         = (0x01 << 1)
  gyr_int_set_am_Z_axis         = (0x01 << 2)
  gyr_int_set_hr_X_axis         = (0x01 << 3)
  gyr_int_set_hr_Y_axis         = (0x01 << 4)
  gyr_int_set_hr_Z_axis         = (0x01 << 5)
  gyr_int_set_am_filt          = (0x01 << 6)
  gyr_int_set_hr_filt          = (0x01 << 7)
  gyr_int_set_all             = 0x3f
  
  opr_mode_config        = 0
  opr_mode_acc_only       = 1
  opr_mode_mag_only       = 2
  opr_mode_gyr_only      = 3  
  opr_mode_acc_mag        = 4
  opr_mode_acc_Gyr       = 5
  opr_mode_mag_gyr       = 6
  opr_mode_AMG           = 7
  opr_mode_Imu           = 8
  opr_mode_compass       = 9
  opr_mode_M4G           = 10
  opr_mode_ndofFmc_off    = 11
  opr_mode_ndof          = 12
  
  SYS_TRIGGER               = 0x3f
  OPR_MODE                  = 0x3d
  UNIT_SEL                  = 0x3b
  ACC_CONFIG                = 0x8
  GYR_CONFIG0               = 0xa
  PWR_MODE                  = 0x3e

  ACC_DATA                  = 0x8
  MAG_DATA                  = 0xe
  GYR_DATA                  = 0x14
  LIA_DATA                  = 0x28
  GRV_DATA                  = 0x2e
  EUL_DATA                  = 0x1A
  QUA_DATA                  = 0x20
  GYR_CONFIG1               = 0xb
  MAG_CONFIG                = 0x9
  
  def __init__(self, board=None, i2c_addr=0x28, bus_num=0):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard
      
    self.i2c_addr = i2c_addr
    self.board = board
    self.i2c = I2C(bus_num)
    
    self._currentPage = 0xff
    self._acc_range = 0
    self.__gyr_range = 0
    
    self.reset()                  #重置传感器
  
  def begin(self):
    temp = self.get_reg(0x00, 0);
    if temp[0] == 0xa0:
      timeOut = 0
      self.reset()
      while True:
        temp = self.get_reg(0x39, 0)
        time.sleep(0.01)
        timeOut = timeOut + 1
        if (temp != 0) and (timeOut<100):
          break
      if timeOut == 100:
        return False
      else:
        self.set_power_mode(self.opr_mode_config)
        time.sleep(0.05)
        self.set_unit()
        self.set_acc_range(self.acc_range_4G)
        self.set_gyr_range(self.gyr_range_2000)
        self.set_power_mode(self.power_mode_normal)
        self.set_power_mode(self.opr_mode_ndof)
        time.sleep(0.05)
    else:
      return False
    return True
  
  def get_qua(self):
    qua = RET()
    data = self.get_qua_raw()
    ret_value = [0,0,0,0]
    ret_value[0] = data[0] / 16384.0
    ret_value[1] = data[1] / 16384.0
    ret_value[2] = data[2] / 16384.0
    ret_value[3] = data[3] / 16384.0
    qua.w = ret_value[0]
    qua.x = ret_value[1]
    qua.y = ret_value[2]
    qua.z = ret_value[3]
    return qua
    
  def get_qua_raw(self):
    self.set_to_page(0)
    val = self.read_reg(self.QUA_DATA, 8)
    data = [0,0,0,0]
    data[0] = self.bin2dec_auto(val[0],val[1])
    data[1] = self.bin2dec_auto(val[2],val[3])
    data[2] = self.bin2dec_auto(val[4],val[5])
    data[3] = self.bin2dec_auto(val[6],val[7])
    return data
    
  def get_eul(self):
    eul = RET()
    data = self.get_eul_raw()
    ret_value = [0,0,0]
    ret_value[0] = data[0] / 16.0
    ret_value[1] = data[1] / 16.0
    ret_value[2] = data[2] / 16.0
    eul.head = ret_value[0]
    eul.roll = ret_value[1]
    eul.pitch = ret_value[2]
    return eul
  
  def get_eul_raw(self):
    self.set_to_page(0)
    val = self.read_reg(self.EUL_DATA, 6)
    data = [0,0,0]
    data[0] = self.bin2dec_auto(val[0],val[1])
    data[1] = self.bin2dec_auto(val[2],val[3])
    data[2] = self.bin2dec_auto(val[4],val[5])
    return data
    
  def get_axis_acc(self, Axis= 0):
    acc = RET()
    value = self.get_axis_raw(Axis)
    factor = 1.0
    if Axis == self.axis_acc: 
      factor = 1.0
    elif Axis == self.axis_lia:
      factor = 1.0
    elif Axis == self.axis_grv:
      factor = 1.0
    elif Axis == self.axis_mag:
      factor = 16.0
    elif Axis == self.axis_gyr:
      factor = 16.0
    ret_value = [0,0,0]
    ret_value[0] = value[0] / factor
    ret_value[1] = value[1] / factor
    ret_value[2] = value[2] / factor
    acc.x = ret_value[0]
    acc.y = ret_value[1]
    acc.z = ret_value[2]
    return acc
  
  def get_axis_mag(self, Axis = 1):
    mag = RET()
    value = self.get_axis_raw(Axis)
    factor = 1.0
    if Axis == self.axis_acc: 
      factor = 1.0
    elif Axis == self.axis_lia:
      factor = 1.0
    elif Axis == self.axis_grv:
      factor = 1.0
    elif Axis == self.axis_mag:
      factor = 16.0
    elif Axis == self.axis_gyr:
      factor = 16.0
    ret_value = [0,0,0]
    ret_value[0] = value[0] / factor
    ret_value[1] = value[1] / factor
    ret_value[2] = value[2] / factor
    mag.x = ret_value[0]
    mag.y = ret_value[1]
    mag.z = ret_value[2]
    return mag
  
  def get_axis_gyr(self, Axis = 2):
    gyr = RET()
    value = self.get_axis_raw(Axis)
    factor = 1.0
    if Axis == self.axis_acc: 
      factor = 1.0
    elif Axis == self.axis_lia:
      factor = 1.0
    elif Axis == self.axis_grv:
      factor = 1.0
    elif Axis == self.axis_mag:
      factor = 16.0
    elif Axis == self.axis_gyr:
      factor = 16.0
    ret_value = [0,0,0]
    ret_value[0] = value[0] / factor
    ret_value[1] = value[1] / factor
    ret_value[2] = value[2] / factor
    gyr.x = ret_value[0]
    gyr.y = ret_value[1]
    gyr.z = ret_value[2]
    return gyr
  
  def get_axis_lia(self, Axis = 3):
    lia = RET()
    value = self.get_axis_raw(Axis)
    factor = 1.0
    if Axis == self.axis_acc: 
      factor = 1.0
    elif Axis == self.axis_lia:
      factor = 1.0
    elif Axis == self.axis_grv:
      factor = 1.0
    elif Axis == self.axis_mag:
      factor = 16.0
    elif Axis == self.axis_gyr:
      factor = 16.0
    ret_value = [0,0,0]
    ret_value[0] = value[0] / factor
    ret_value[1] = value[1] / factor
    ret_value[2] = value[2] / factor
    lia.x = ret_value[0]
    lia.y = ret_value[1]
    lia.z = ret_value[2]
    return lia
  
  def get_axis_grv(self, Axis = 4):
    grv = RET()
    value = self.get_axis_raw(Axis)
    factor = 1.0
    if Axis == self.axis_acc: 
      factor = 1.0
    elif Axis == self.axis_lia:
      factor = 1.0
    elif Axis == self.axis_grv:
      factor = 1.0
    elif Axis == self.axis_mag:
      factor = 16.0
    elif Axis == self.axis_gyr:
      factor = 16.0
    ret_value = [0,0,0]
    ret_value[0] = value[0] / factor
    ret_value[1] = value[1] / factor
    ret_value[2] = value[2] / factor
    grv.x = ret_value[0]
    grv.y = ret_value[1]
    grv.z = ret_value[2]
    return grv
  
  def get_axis_raw(self, Axis):
    offset = self.get_offset_of_data(Axis)
    self.set_to_page(0)
    if offset == 0:
      return False
    else:
      val = self.read_reg(offset, 6)
      if val[0]==0 and val[1]==0 and val[2]==0 and val[3]==0 and val[4]==0 and val[5]==0:
        self.begin() 
      data = [0, 0, 0]
      data[0] = self.bin2dec_auto(val[0],val[1])
      data[1] = self.bin2dec_auto(val[2],val[3])
      data[2] = self.bin2dec_auto(val[4],val[5])
    return data
    
  def get_offset_of_data(self, Axis):
    if Axis == self.axis_acc:
      return self.ACC_DATA
    elif Axis == self.axis_mag:
      return self.MAG_DATA
    elif Axis == self.axis_gyr:
      return self.GRV_DATA
    elif Axis == self.axis_lia:
      return self.LIA_DATA
    elif Axis == self.axis_grv:
      return self.GRV_DATA
    else:
      return 0
  
  def set_power_mode(self, power_mode):
    reg_field = 0x3
    reg_val = power_mode
    self.write_reg_bits_helper(0, self.PWR_MODE, reg_field, reg_val)
  
  def set_gyr_range(self, gyr_range):
    reg_field = 0x7
    reg_val = gyr_range
    self.write_reg_bits_helper(1, self.GYR_CONFIG0, reg_field, reg_val)
    self.__gyr_range = gyr_range
    
  def set_acc_range(self, Acc_range):
    reg_field = 0x3
    reg_val = Acc_range
    self.write_reg_bits_helper(1, self.ACC_CONFIG, reg_field, reg_val)
    self._acc_range = Acc_range
  
  def set_unit(self):
    reg = 0x1
    self.set_to_page(0)
    self.write_reg(self.UNIT_SEL, [reg])
    
  def set_power_mode(self, opr_mode):
    reg_field = 0xf
    reg_val = opr_mode
    self.write_reg_bits_helper(0, self.OPR_MODE, reg_field, reg_val)
    time.sleep(0.05)
  
  def get_reg(self, reg, pageId):
    self.set_to_page(pageId)
    data = self.read_reg(reg, 1)
    return data
  
  def reset(self):
    reg_field = 0x20
    reg_val   = 0x20
    self.write_reg_bits_helper(0, self.SYS_TRIGGER, reg_field, reg_val)
    
  def write_reg_bits_helper(self, pageId, reg, field, val):
    self.set_to_page(pageId)
    self.write_reg_bits(reg, field, val);

  def set_to_page(self, pageId):
    if self._currentPage != pageId:
      self.write_reg(0x07, [pageId])
      self._currentPage = pageId

  def write_reg_bits(self, reg, field, val):
    data = self.read_reg(reg, 1)
    reg_val = data[0] & (~field)
    reg_val = reg_val | val
    self.write_reg(reg, [reg_val])

  def set_acc_power_mode(self, acc_power_mode):
    reg_field = 0xe0
    if acc_power_mode == self.acc_power_mode_normal or acc_power_mode == self.acc_power_mode_suspend or acc_power_mode == self.acc_power_mode_low_power1 or acc_power_mode == self.acc_power_mode_standby or acc_power_mode == self.acc_power_mode_low_power2 or acc_power_mode == self.acc_power_mode_deep_suspend:
      reg_val = 32 * acc_power_mode
    else:
      reg_val = 0x0
    self.write_reg_bits_helper(1, self.ACC_CONFIG, reg_field, reg_val)
    
  def set_gyr_power_mode(self, gyr_power_mode):
    reg_field = 0x7
    reg_val = gyr_power_mode
    self.write_reg_bits_helper(1, self.GYR_CONFIG1, reg_field, reg_val)
    
  def set_mag_power_mode(self, mag_power_mode):
    reg_field = 0x60
    if mag_power_mode==self.mag_power_mode_normal or mag_power_mode == self.mag_power_mode_sleep or mag_power_mode == self.mag_power_mode_suspend or mag_power_mode == self.mag_power_mode_force:
      reg_val = 32 * mag_power_mode
    else:
      reg_val = 0x0
    self.write_reg_bits_helper(1, self.MAG_CONFIG, reg_field, reg_val)

  def set_acc_band_width(self, acc_bandwidth):
    reg_field = 0x1c
    if acc_bandwidth==self.acc_bandwidth_7_81 or acc_bandwidth==self.acc_bandwidth_15_63 or acc_bandwidth==self.acc_bandwidth_31_25 or acc_bandwidth==self.acc_bandwidth_62_5 or acc_bandwidth==self.acc_bandwidth_125 or acc_bandwidth==self.acc_bandwidth_500 or acc_bandwidth==self.acc_bandwidth_1000:
      reg_val = 4 * acc_bandwidth
    self.write_reg_bits_helper(1, self.ACC_CONFIG, reg_field, reg_val)
    
  def set_mag_data_rate(self, mag_data_rate):
    reg_field = 0x7
    reg_val = mag_data_rate
    self.write_reg_bits_helper(1, self.MAG_CONFIG, reg_field, reg_val)

  def set_mag_opr_mode(self, mag_opr_mode):
    reg_field = 0x18
    if mag_opr_mode==self.mag_opr_mode_low_power or mag_opr_mode==self.mag_opr_mode_regular or mag_opr_mode==self.mag_opr_mode_enhanced_regular or mag_opr_mode==self.mag_opr_mode_high_accuracy:
      reg_val = 8 * mag_opr_mode
    else:
      reg_val = 0x0
    self.write_reg_bits_helper(1, self.MAG_CONFIG, reg_field, reg_val)
  
  def set_gyr_band_width(self, gyr_bandwidth):
    reg_field = 0x38
    if gyr_bandwidth==self.gyr_bandwidth_523 or gyr_bandwidth==self.gyr_bandwidth_230 or gyr_bandwidth==self.gyr_bandwidth_116 or gyr_bandwidth==self.gyr_bandwidth_47 or gyr_bandwidth == self.gyr_bandwidth_23 or gyr_bandwidth == self.gyr_bandwidth_12 or gyr_bandwidth == self.gyr_bandwidth_64 or gyr_bandwidth == self.gyr_bandwidth_32:
      reg_val = 8 * gyr_bandwidth
    else:
      reg_val = 0x0
    self.write_reg_bits_helper(1, self.GYR_CONFIG0, reg_field, reg_val)

  def set_axis_offset(self, Axis, offset_val):
    offset = self.get_off_set_of_offset(Axis)
    factor = 0
    max_value = 0
    if Axis == self.axis_acc:
      factor = 1
      if self._acc_range == self.acc_range_2G:
        max_value = 2000
      elif self._acc_range == self.acc_range_4G:
        max_value = 4000
      elif self._acc_range == self.acc_range_8G:
        max_value = 8000
      elif self._acc_range == self.acc_range_16G:
        max_value = 16000
    elif Axis == self.axis_mag:
      factor = 16
      max_value = 1300
    elif Axis == self.axis_gyr:
      factor = 16
      if self.__gyr_range == self.gyr_range_2000:
        max_value = 2000
      elif self.__gyr_range == self.gyr_range_1000:
        max_value = 1000
      elif self.__gyr_range == self.gyr_range_500:
        max_value = 500
      elif self.__gyr_range == self.gyr_range_250:
        max_value = 250
      elif self.__gyr_range == self.gyr_range_125:
        max_value = 125
    
    if Axis == self.axis_gyr:
      if (offset == 0) or ((abs(offset_val[0]))>max_value) or ((abs(offset_val[1]))>max_value) or (abs(offset_val[2])>2500):
        return False
    elif (offset == 0) or (abs(offset_val[0])>max_value) or (abs(offset_val[1])>max_value) or (abs(offset_val[2])>max_value):
      return False
    Axis_Data = [0,0,0]
    Axis_Data[0] = offset_val[0] * factor
    Axis_Data[1] = offset_val[1] * factor
    Axis_Data[2] = offset_val[2] * factor
    
    self.set_to_page(0)
    self.write_reg(offset, Axis_Data)

  def get_off_set_of_offset(self, Axis):
    if Axis == self.axis_acc:
      return 0x55
    elif Axis == self.axis_mag:
      return 0x5b
    elif Axis == self.axis_gyr:
      return 0x61

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

  def read_reg(self, reg, lens):
    return self.i2c.readfrom_mem(self.i2c_addr, reg, lens)
 
  def write_reg(self, reg, data):
    self.i2c.writeto_mem(self.i2c_addr, reg, data)

class RET():
  def __init__(self):
    self.w = 0
    self.x = 0
    self.y = 0
    self.z = 0
    self.head = 0
    self.roll = 0
    self.pitch = 0