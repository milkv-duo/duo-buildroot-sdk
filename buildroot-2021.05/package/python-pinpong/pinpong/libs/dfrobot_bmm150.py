# -*- coding: utf-8 -*-

import time
import math
from pinpong.board import gboard,I2C

class trim_register:
  def __init__(self):
    self.dig_x1   = 0;
    self.dig_y1   = 0;
    self.dig_x2   = 0;
    self.dig_y2   = 0;
    self.dig_z1   = 0;
    self.dig_z2   = 0;
    self.dig_z3   = 0;
    self.dig_z4   = 0;
    self.dig_xy1  = 0;
    self.dig_xy2  = 0;
    self.dig_xyz1 = 0;
_trim_data = trim_register()

class geomagnetic_data:
  def __init__(self):
    self.x   = 0;
    self.y   = 0;
    self.z   = 0;
    self.r   = 0;
_geomagnetic = geomagnetic_data()

class BMM150(object):
  PI                             = 3.141592653
  I2C_MODE                       = 1
  SPI_MODE                       = 2
  ENABLE_POWER                   = 1
  DISABLE_POWER                  = 0
  POLARITY_HIGH                  = 1
  POLARITY_LOW                   = 0
  ERROR                          = -1
  SELF_TEST_XYZ_FALL             = 0
  SELF_TEST_YZ_FAIL              = 1
  SELF_TEST_XZ_FAIL              = 2
  SELF_TEST_Z_FAIL               = 3
  SELF_TEST_XY_FAIL              = 4
  SELF_TEST_Y_FAIL               = 5
  SELF_TEST_X_FAIL               = 6
  SELF_TEST_XYZ_OK               = 7
  DRDY_ENABLE                    = 1
  DRDY_DISABLE                   = 0
  INTERRUPUT_LATCH_ENABLE        = 1
  INTERRUPUT_LATCH_DISABLE       = 0
  MEASUREMENT_X_ENABLE           = 0
  MEASUREMENT_Y_ENABLE           = 0
  MEASUREMENT_Z_ENABLE           = 0
  MEASUREMENT_X_DISABLE          = 1
  MEASUREMENT_Y_DISABLE          = 1
  MEASUREMENT_Z_DISABLE          = 1
  DATA_OVERRUN_ENABLE            = 1
  DATA_OVERRUN_DISABLE           = 0
  OVERFLOW_INT_ENABLE            = 1
  OVERFLOW_INT_DISABLE           = 0
  INTERRUPT_X_ENABLE             = 0
  INTERRUPT_Y_ENABLE             = 0
  INTERRUPT_Z_ENABLE             = 0
  INTERRUPT_X_DISABLE            = 1
  INTERRUPT_Y_DISABLE            = 1
  INTERRUPT_Z_DISABLE            = 1
  
  CHANNEL_X                      = 1
  CHANNEL_Y                      = 2
  CHANNEL_Z                      = 3
  ENABLE_INTERRUPT_PIN           = 1
  DISABLE_INTERRUPT_PIN          = 0
  POWERMODE_NORMAL               = 0x00
  POWERMODE_FORCED               = 0x01
  POWERMODE_SLEEP                = 0x03
  POWERMODE_SUSPEND              = 0x04
  PRESETMODE_LOWPOWER            = 0x01
  PRESETMODE_REGULAR             = 0x02
  PRESETMODE_HIGHACCURACY        = 0x03
  PRESETMODE_ENHANCED            = 0x04
  REPXY_LOWPOWER                 = 0x01
  REPXY_REGULAR                  = 0x04
  REPXY_ENHANCED                 = 0x07
  REPXY_HIGHACCURACY             = 0x17
  REPZ_LOWPOWER                  = 0x01
  REPZ_REGULAR                   = 0x07
  REPZ_ENHANCED                  = 0x0D
  REPZ_HIGHACCURACY              = 0x29
  CHIP_ID_VALUE                  = 0x32
  CHIP_ID_REGISTER               = 0x40
  REG_DATA_X_LSB                 = 0x42
  REG_DATA_READY_STATUS          = 0x48
  REG_INTERRUPT_STATUS           = 0x4a
  CTRL_POWER_REGISTER            = 0x4b
  MODE_RATE_REGISTER             = 0x4c
  REG_INT_CONFIG                 = 0x4D
  REG_AXES_ENABLE                = 0x4E
  REG_LOW_THRESHOLD              = 0x4F
  REG_HIGH_THRESHOLD             = 0x50
  REG_REP_XY                     = 0x51
  REG_REP_Z                      = 0x52
  RATE_10HZ                      = 0x00        #(default rate)
  RATE_02HZ                      = 0x01
  RATE_06HZ                      = 0x02
  RATE_08HZ                      = 0x03
  RATE_15HZ                      = 0x04
  RATE_20HZ                      = 0x05
  RATE_25HZ                      = 0x06
  RATE_30HZ                      = 0x07
  DIG_X1                         = 0x5D
  DIG_Y1                         = 0x5E
  DIG_Z4_LSB                     = 0x62
  DIG_Z4_MSB                     = 0x63
  DIG_X2                         = 0x64
  DIG_Y2                         = 0x65
  DIG_Z2_LSB                     = 0x68
  DIG_Z2_MSB                     = 0x69
  DIG_Z1_LSB                     = 0x6A
  DIG_Z1_MSB                     = 0x6B
  DIG_XYZ1_LSB                   = 0x6C
  DIG_XYZ1_MSB                   = 0x6D
  DIG_Z3_LSB                     = 0x6E
  DIG_Z3_MSB                     = 0x6F
  DIG_XY2                        = 0x70
  DIG_XY1                        = 0x71
  LOW_THRESHOLD_INTERRUPT        = 0x00
  HIGH_THRESHOLD_INTERRUPT       = 0x01
  NO_DATA                        = -32768
  __txbuf          = [0]          # i2c send buffer
  __threshold_mode = 2

  def __init__(self):
    pass

  '''
    @brief 初始化bmm150 判断芯片id是否正确
    @return 0  is init success
            -1 is init failed
  '''
  def sensor_init(self):
    self.set_power_bit(self.ENABLE_POWER)
    chip_id = self.get_chip_id()
    if chip_id == self.CHIP_ID_VALUE:
      self.get_trim_data()
      return 0
    else:
      return -1

  '''
    @brief get bmm150 chip id
    @return chip id
  '''
  def get_chip_id(self):
    rslt = self.read_reg(self.CHIP_ID_REGISTER, 1)
    return rslt[0]

  '''
    @brief 软件复位，软件复位后先恢复为挂起模式，而后恢复为睡眠模式,suspend mode下不能软件复位
  '''
  def soft_reset(self):
    rslt = self.read_reg(self.CTRL_POWER_REGISTER, 1)
    self.__txbuf[0] = rslt[0] | 0x82
    self.write_reg(self.CTRL_POWER_REGISTER, self.__txbuf)

  '''
    @brief 传感器自测，返回字符串表明自检结果
    @return 测试结果的字符串
  '''
  def self_test(self):
    str1 = ""
    self.set_operation_mode(self.POWERMODE_SLEEP)
    rslt = self.read_reg(self.MODE_RATE_REGISTER, 1)
    self.__txbuf[0] == rslt[0] | 0x01
    self.write_reg(self.MODE_RATE_REGISTER, self.__txbuf)
    time.sleep(1)
    rslt = self.read_reg(self.REG_DATA_X_LSB, 5)
    number = (rslt[0]&0x01) | (rslt[2]&0x01)<<1 | (rslt[4]&0x01)<<2
    if (number>>0)&0x01:
      str1 += "x "
    if (number>>1)&0x01:
      str1 += "y "
    if (number>>2)&0x01:
      str1 += "z "
    if number == 0:
      str1 = "xyz aix self test fail"
    else:
      str1 += "aix test success"
    return str1

  '''
    @brief 使能或禁止电源
    @param ctrl is enable/disable power
      DISABLE_POWER is disable power
      ENABLE_POWER  is enable power
  '''
  def set_power_bit(self, ctrl):
    rslt = self.read_reg(self.CTRL_POWER_REGISTER, 1)
    if ctrl == self.DISABLE_POWER:
      self.__txbuf[0] = rslt[0] & 0xFE
      self.write_reg(self.CTRL_POWER_REGISTER, self.__txbuf)
    else:
      self.__txbuf[0] = rslt[0] | 0x01
      self.write_reg(self.CTRL_POWER_REGISTER, self.__txbuf)

  '''
    @brief 获取电源状态
    @return power state
      DISABLE_POWER is disable power
      ENABLE_POWER  is enable power
  '''
  def get_power_bit(self):
    rslt = self.read_reg(self.CTRL_POWER_REGISTER, 1)
    return rslt[0]&0x01

  '''
    @brief 设置传感器的执行模式
    @param modes
      POWERMODE_NORMAL       normal mode  正常的获得地磁数据的模式
      POWERMODE_FORCED       forced mode  单次测量，测量完成后，传感器恢复sleep mode
      POWERMODE_SLEEP        sleep mode   用户可以访问所有寄存器，不能测量地磁数据
      POWERMODE_SUSPEND      suspend mode 用户只能访问控制寄存器 BMM150_REG_POWER_CONTROL的内容
  '''
  def set_operation_mode(self, modes):
    rslt = self.read_reg(self.MODE_RATE_REGISTER, 1)
    if modes == self.POWERMODE_NORMAL:
      self.set_power_bit(self.ENABLE_POWER)
      rslt[0] = rslt[0] & 0xf9
      self.write_reg(self.MODE_RATE_REGISTER, rslt)
    elif modes == self.POWERMODE_FORCED:
      rslt[0] = (rslt[0] & 0xf9) | 0x02
      self.set_power_bit(self.ENABLE_POWER)
      self.write_reg(self.MODE_RATE_REGISTER, rslt)
    elif modes == self.POWERMODE_SLEEP:
      self.set_power_bit(self.ENABLE_POWER)
      rslt[0] = (rslt[0] & 0xf9) | 0x04
      self.write_reg(self.MODE_RATE_REGISTER, rslt)
    else:
      self.set_power_bit(self.DISABLE_POWER)

  '''
    @brief 获取传感器的执行模式
    @return 返回模式的字符串
  '''
  def get_operation_mode(self):
    str1 = ""
    if self.get_power_bit() == 0:
      mode = self.POWERMODE_SUSPEND
    else:
      rslt = self.read_reg(self.MODE_RATE_REGISTER, 1)
      mode = (rslt[0]&0x06)>>1
    if mode == self.POWERMODE_NORMAL:
      str1 = "normal mode"
    elif mode == self.POWERMODE_SLEEP:
      str1 = "sleep mode"
    elif mode == self.POWERMODE_SUSPEND:
      str1 = "suspend mode"
    else:
      str1 = "forced mode"
    return str1

  '''
    @brief 设置地磁数据获取的速率，速率越大获取越快(不加延时函数)
    @param rate
      RATE_02HZ
      RATE_06HZ
      RATE_08HZ
      RATE_10HZ        #(default rate)
      RATE_15HZ
      RATE_20HZ
      RATE_25HZ
      RATE_30HZ
  '''
  def set_rate(self, rates):
    rslt = self.read_reg(self.MODE_RATE_REGISTER, 1)
    if rates == self.RATE_10HZ:
      rslt[0] = rslt[0]&0xc7
      self.write_reg(self.MODE_RATE_REGISTER, rslt)
    elif rates == self.RATE_02HZ:
      rslt[0] = (rslt[0]&0xc7) | 0x08
      self.write_reg(MODE_RATE_REGISTER, rslt)
    elif rates == self.RATE_06HZ:
      rslt[0] = (rslt[0]&0xc7) | 0x10
      self.write_reg(MODE_RATE_REGISTER, rslt)
    elif rates == self.RATE_08HZ:
      rslt[0] = (rslt[0]&0xc7) | 0x18
      self.write_reg(self.MODE_RATE_REGISTER, rslt)
    elif rates == self.RATE_15HZ:
      rslt[0] = (rslt[0]&0xc7) | 0x20
      self.write_reg(self.MODE_RATE_REGISTER, rslt)
    elif rates == RATE_20HZ:
      rslt[0] = (rslt[0]&0xc7) | 0x28
      self.write_reg(self.MODE_RATE_REGISTER, rslt)
    elif rates == self.RATE_25HZ:
      rslt[0] = (rslt[0]&0xc7) | 0x30
      self.write_reg(MODE_RATE_REGISTER, rslt)
    elif rates == self.RATE_30HZ:
      rslt[0] = (rslt[0]&0xc7) | 0x38
      self.write_reg(self.MODE_RATE_REGISTER, rslt)
    else:
      rslt[0] = rslt[0]&0xc7
      self.write_reg(self.MODE_RATE_REGISTER, rslt)

  '''
    @brief 获取配置的数据速率 单位：HZ
    @return rate
  '''
  def get_rate(self):
    rslt = self.read_reg(self.MODE_RATE_REGISTER, 1)
    rate = (rslt[0]&0x38)>>3
    if rate == self.RATE_02HZ:
      return 2
    elif rate == self.RATE_06HZ:
      return 6
    elif rate == self.RATE_08HZ:
      return 8
    elif rate == self.RATE_10HZ:
      return 10
    elif rate == self.RATE_15HZ:
      return 15
    elif rate == self.RATE_20HZ:
      return 20
    elif rate == self.RATE_25HZ:
      return 25
    else:
      return 30

  '''
    @brief 设置预置模式，使用户更简单的配置传感器来获取地磁数据
    @param modes 
       PRESETMODE_LOWPOWER       低功率模式,获取少量的数据 取均值
       PRESETMODE_REGULAR        普通模式,获取中量数据 取均值
       PRESETMODE_ENHANCED       增强模式,获取大量数据 取均值
       PRESETMODE_HIGHACCURACY   高精度模式,获取超大量数据 取均值
  '''
  def set_preset_mode(self, modes):
    if modes == self.PRESETMODE_LOWPOWER:
      self.set_xy_rep(self.REPXY_LOWPOWER)
      self.set_z_rep(self.REPZ_LOWPOWER)
    elif modes == self.PRESETMODE_REGULAR:
      self.set_xy_rep(self.REPXY_REGULAR)
      self.set_z_rep(self.REPZ_REGULAR)
    elif modes == self.PRESETMODE_HIGHACCURACY:
      self.set_xy_rep(self.REPXY_HIGHACCURACY)
      self.set_z_rep(self.REPZ_HIGHACCURACY)
    elif modes == self.PRESETMODE_ENHANCED:
      self.set_xy_rep(self.REPXY_ENHANCED)
      self.set_z_rep(self.REPZ_ENHANCED)
    else:
      self.set_xy_rep(self.REPXY_LOWPOWER)
      self.set_z_rep(self.REPZ_LOWPOWER)

  '''
    @brief the number of repetitions for x/y-axis
    @param modes
      PRESETMODE_LOWPOWER      低功率模式，功耗降低的获取数据模式
      PRESETMODE_REGULAR       普通模式，正常获取数据的模式
      PRESETMODE_HIGHACCURACY  高精度模式，数据获取更加精准
      PRESETMODE_ENHANCED      增强模式，比高精度模式获取数据更精准
  '''
  def set_xy_rep(self, modes):
    self.__txbuf[0] = modes
    if modes == self.REPXY_LOWPOWER:
      self.write_reg(self.REG_REP_XY, self.__txbuf)
    elif modes == self.REPXY_REGULAR:
      self.write_reg(self.REG_REP_XY, self.__txbuf)
    elif modes == self.REPXY_ENHANCED:
      self.write_reg(self.REG_REP_XY, self.__txbuf)
    elif modes == self.REPXY_HIGHACCURACY:
      self.write_reg(self.REG_REP_XY, self.__txbuf)
    else:
      __txbuf[0] = self.REPXY_LOWPOWER
      self.write_reg(self.REG_REP_XY, self.__txbuf)

  '''
    @brief the number of repetitions for z-axis
    @param modes
      PRESETMODE_LOWPOWER      低功率模式，功耗降低的获取数据模式
      PRESETMODE_REGULAR       普通模式，正常获取数据的模式
      PRESETMODE_HIGHACCURACY  高精度模式，数据获取更加精准
      PRESETMODE_ENHANCED      增强模式，比高精度模式获取数据更精准
  '''
  def set_z_rep(self, modes):
    self.__txbuf[0] = modes
    if modes == self.REPZ_LOWPOWER:  
      self.write_reg(self.REG_REP_Z, self.__txbuf)
    elif modes == self.REPZ_REGULAR:
      self.write_reg(self.REG_REP_Z, self.__txbuf)
    elif modes == self.REPZ_ENHANCED:
      self.write_reg(self.REG_REP_Z, self.__txbuf)
    elif modes == self.REPZ_HIGHACCURACY:
      self.write_reg(self.REG_REP_Z, self.__txbuf)
    else:
      __txbuf[0] = self.REPZ_LOWPOWER
      self.write_reg(self.REG_REP_Z, self.__txbuf)

  '''
    @brief 获取bmm150预留的数据信息，用来进行数据补偿的操作
  '''
  def get_trim_data(self):
    trim_x1_y1    = self.read_reg(self.DIG_X1, 2)
    trim_xyz_data = self.read_reg(self.DIG_Z4_LSB, 4)
    trim_xy1_xy2  = self.read_reg(self.DIG_Z2_LSB, 10)
    _trim_data.dig_x1 = self.uint8_to_int8(trim_x1_y1[0])
    _trim_data.dig_y1 = self.uint8_to_int8(trim_x1_y1[1])
    _trim_data.dig_x2 = self.uint8_to_int8(trim_xyz_data[2])
    _trim_data.dig_y2 = self.uint8_to_int8(trim_xyz_data[3])
    temp_msb = int(trim_xy1_xy2[3]) << 8
    _trim_data.dig_z1 = int(temp_msb | trim_xy1_xy2[2])
    temp_msb = int(trim_xy1_xy2[1] << 8)
    _trim_data.dig_z2 = int(temp_msb | trim_xy1_xy2[0])
    temp_msb = int(trim_xy1_xy2[7] << 8)
    _trim_data.dig_z3 = temp_msb | trim_xy1_xy2[6]
    temp_msb = int(trim_xyz_data[1] << 8)
    _trim_data.dig_z4 = int(temp_msb | trim_xyz_data[0])
    _trim_data.dig_xy1 = trim_xy1_xy2[9]
    _trim_data.dig_xy2 = self.uint8_to_int8(trim_xy1_xy2[8])
    temp_msb = int((trim_xy1_xy2[5] & 0x7F) << 8)
    _trim_data.dig_xyz1 = int(temp_msb | trim_xy1_xy2[4])

  '''
    @brief 获取x y z 三轴的地磁数据
    @return x y z 三轴的地磁数据的列表 单位：微特斯拉（uT）
            [0] x 轴地磁的数据
            [1] y 轴地磁的数据
            [2] z 轴地磁的数据
  '''
  def get_geomagnetic(self):
    rslt = self.read_reg(self.REG_DATA_X_LSB, 8)
    rslt[1] = self.uint8_to_int8(rslt[1])
    rslt[3] = self.uint8_to_int8(rslt[3])
    rslt[5] = self.uint8_to_int8(rslt[5])
    _geomagnetic.x = ((rslt[0]&0xF8) >> 3)  | int(rslt[1]*32)
    _geomagnetic.y = ((rslt[2]&0xF8) >> 3)  | int(rslt[3]*32)
    _geomagnetic.z = ((rslt[4]&0xFE) >> 1)  | int(rslt[5]*128)
    _geomagnetic.r = ((rslt[6]&0xFC) >> 2)  | int(rslt[7]*64)
    rslt[0] = self.compenstate_x(_geomagnetic.x, _geomagnetic.r)
    rslt[1] = self.compenstate_y(_geomagnetic.y, _geomagnetic.r)
    rslt[2] = self.compenstate_z(_geomagnetic.z, _geomagnetic.r)
    return rslt

  '''
    @brief 获取罗盘方向
    @return 罗盘方向 (0° - 360°)  0° = North, 90° = East, 180° = South, 270° = West.
  '''
  def get_compass_degree(self):
    geomagnetic = self.get_geomagnetic()
    compass = math.atan2(geomagnetic[0], geomagnetic[1])
    if compass < 0:
      compass += 2 * self.PI
    if compass > 2 * self.PI:
     compass -= 2 * self.PI
    return compass * 180 / self.PI

  '''
    @brief uint8_t to int8_t
    @param number    需要转换的uint8_t数据
    @return number   转换后的数据
  '''
  def uint8_to_int8(self, number):
    if number <= 127:
      return number
    else:
      return (256-number)*-1

  '''
    @berif 补偿x轴的地磁数据
    @param  data_x       原始的地磁数据
    @param  data_r       补偿的数据
    @return retval       校准后的地磁数据
  '''
  def compenstate_x(self, data_x, data_r):
    if data_x != -4096:
      if data_r != 0:
        process_comp_x0 = data_r
      elif _trim_data.dig_xyz1 != 0:
        process_comp_x0 = _trim_data.dig_xyz1
      else:
        process_comp_x0 = 0
      if process_comp_x0 != 0:
        process_comp_x1 = int(_trim_data.dig_xyz1*16384)
        process_comp_x2 = int(process_comp_x1/process_comp_x0 - 0x4000)
        retval = process_comp_x2
        process_comp_x3 = retval*retval
        process_comp_x4 = _trim_data.dig_xy2*(process_comp_x3/128)
        process_comp_x5 = _trim_data.dig_xy1*128
        process_comp_x6 = retval*process_comp_x5
        process_comp_x7 = (process_comp_x4+process_comp_x6)/512 + 0x100000
        process_comp_x8 = _trim_data.dig_x2 + 0xA0
        process_comp_x9 = (process_comp_x8*process_comp_x7)/4096
        process_comp_x10= data_x*process_comp_x9
        retval = process_comp_x10/8192
        retval = (retval + _trim_data.dig_x1*8)/16
      else:
        retval = -32368
    else:
      retval = -32768
    return retval

  '''
    @berif 补偿y轴的地磁数据
    @param  data_y       原始的地磁数据
    @param  data_r       补偿的数据
    @return retval       校准后的地磁数据
  '''
  def compenstate_y(self, data_y, data_r):
    if data_y != -4096:
      if data_r != 0:
        process_comp_y0 = data_r
      elif _trim_data.dig_xyz1 != 0:
        process_comp_y0 = _trim_data.dig_xyz1
      else:
        process_comp_y0 = 0
      if process_comp_y0 != 0:
        process_comp_y1 = int(_trim_data.dig_xyz1*16384/process_comp_y0)
        process_comp_y2 = int(process_comp_y1 - 0x4000)
        retval = process_comp_y2
        process_comp_y3 = retval*retval
        process_comp_y4 = _trim_data.dig_xy2*(process_comp_y3/128)
        process_comp_y5 = _trim_data.dig_xy1*128
        process_comp_y6 = (process_comp_y4+process_comp_y5*retval)/512
        process_comp_y7 = _trim_data.dig_y2 + 0xA0
        process_comp_y8 = ((process_comp_y6 + 0x100000)*process_comp_y7)/4096
        process_comp_y9 = data_y*process_comp_y8
        retval = process_comp_y9/8192
        retval = (retval + _trim_data.dig_y1*8)/16
      else:
        retval = -32368
    else:
      retval = -32768
    return retval

  '''
    @berif 补偿z轴的地磁数据
    @param  data_z       原始的地磁数据
    @param  data_r       补偿的数据
    @return retval       校准后的地磁数据
  '''
  def compenstate_z(self, data_z, data_r):
    if data_z != -16348:
      if _trim_data.dig_z2 != 0 and _trim_data.dig_z1 != 0 and _trim_data.dig_xyz1 != 0 and data_r != 0:
        process_comp_z0 = data_r - _trim_data.dig_xyz1
        process_comp_z1 = (_trim_data.dig_z3*process_comp_z0)/4
        process_comp_z2 = (data_z - _trim_data.dig_z4)*32768
        process_comp_z3 = _trim_data.dig_z1 * data_r*2
        process_comp_z4 = (process_comp_z3+32768)/65536
        retval = (process_comp_z2 - process_comp_z1)/(_trim_data.dig_z2+process_comp_z4)
        if retval > 32767:
          retval = 32367
        elif retval < -32367:
          retval = -32367
        retval = retval/16
      else:
        retval = -32768
    else:
      retval = -32768
    return retval

  '''
    @brief 使能或者禁止数据准备中断引脚
           使能后有数据来临DRDY引脚跳变
           禁止后有数据来临DRDY不进行跳变
           高极性：高电平为活动电平，默认为低电平，触发中断时电平变为高
           低极性：低电平为活动电平，默认为高电平，触发中断时电平变为低
    @param modes
        DRDY_ENABLE      使能DRDY
        DRDY_DISABLE     禁止DRDY
    @param polarity
        POLARITY_HIGH    高极性
        POLARITY_LOW     低极性
  '''
  def set_data_ready_pin(self, modes, polarity):
    rslt = self.read_reg(self.REG_AXES_ENABLE, 1)
    if modes == self.DRDY_DISABLE:
      self.__txbuf[0] = rslt[0] & 0x7F
    else:
      self.__txbuf[0] = rslt[0] | 0x80
    if polarity == self.POLARITY_LOW:
      self.__txbuf[0] = self.__txbuf[0] & 0xFB
    else:
      self.__txbuf[0] = self.__txbuf[0] | 0x04
    self.write_reg(self.REG_AXES_ENABLE, self.__txbuf)

  '''
    @brief 获取数据准备的状态，用来判断数据是否准备好
    @return status
      1 is   data is ready
      0 is   data is not ready
  '''
  def get_data_ready_state(self):
    rslt = self.read_reg(self.REG_DATA_READY_STATUS, 1)
    if (rslt[0]&0x01) != 0:
      return 1
    else:
      return 0

  '''
    @brief 使能x y z 轴的测量，默认设置为使能不需要配置，禁止后xyz轴的地磁数据不准确
    @param channel_x
      MEASUREMENT_X_ENABLE     使能 x 轴的测量
      MEASUREMENT_X_DISABLE    禁止 x 轴的测量
    @param channel_y
      MEASUREMENT_Y_ENABLE     使能 y 轴的测量
      MEASUREMENT_Y_DISABLE    禁止 y 轴的测量
    @param channel_z
      MEASUREMENT_Z_ENABLE     使能 z 轴的测量
      MEASUREMENT_Z_DISABLE    禁止 z 轴的测量
  '''
  def set_measurement_xyz(self, channel_x = MEASUREMENT_X_ENABLE, channel_y = MEASUREMENT_Y_ENABLE, channel_z = MEASUREMENT_Z_ENABLE):
    rslt = self.read_reg(self.REG_AXES_ENABLE, 1)
    if channel_x == self.MEASUREMENT_X_DISABLE:
      self.__txbuf[0] = rslt[0] | 0x08
    else:
      self.__txbuf[0] = rslt[0] & 0xF7

    if channel_y == self.MEASUREMENT_Y_DISABLE:
      self.__txbuf[0] = self.__txbuf[0] | 0x10
    else:
      self.__txbuf[0] = self.__txbuf[0] & 0xEF

    if channel_z == self.MEASUREMENT_Z_DISABLE:
      self.__txbuf[0] = self.__txbuf[0] | 0x20
    else:
      self.__txbuf[0] = self.__txbuf[0] & 0xDF
    self.write_reg(self.REG_AXES_ENABLE, self.__txbuf)

  '''
    @brief 获取 x y z 轴的使能状态
    @return 返回xyz 轴的使能状态的字符串
  '''
  def get_measurement_xyz_state(self):
    str1 = ""
    rslt = self.read_reg(self.REG_AXES_ENABLE, 1)
    if (rslt[0]&0x08) == 0:
      str1 += "x "
    if (rslt[0]&0x10) == 0:
      str1 += "y "
    if (rslt[0]&0x20) == 0:
      str1 += "z "
    if str1 == "":
      str1 = "xyz aix not enable"
    else:
      str1 += "aix enable"
    return str1

  '''
    @brief 使能或者禁止 INT 中断引脚
           使能引脚后会触发中断引脚 INT 的电平跳变
           禁止引脚后 INT 中断引脚不会发生电平的跳变
           高极性：高电平为活动电平，默认为低电平，触发中断时电平变为高
           低极性：低电平为活动电平，默认为高电平，触发中断时电平变为低
    @param modes
      ENABLE_INTERRUPT_PIN     使能中断引脚
      DISABLE_INTERRUPT_PIN    禁止中断引脚
    @param polarity
      POLARITY_HIGH            高极性
      POLARITY_LOW             低极性
  '''
  def set_interrupt_pin(self, modes, polarity):
    rslt = self.read_reg(self.REG_AXES_ENABLE, 1)
    if modes == self.DISABLE_INTERRUPT_PIN:
      self.__txbuf[0] = rslt[0] & 0xBF
    else:
      self.__txbuf[0] = rslt[0] | 0x40
    if polarity == self.POLARITY_LOW:
      self.__txbuf[0] = self.__txbuf[0] & 0xFE
    else:
      self.__txbuf[0] = self.__txbuf[0] | 0x01
    self.write_reg(self.REG_AXES_ENABLE, self.__txbuf)

  '''
    @brief 设置中断锁定模式，中断锁定后只有读取 BMM150_REG_INTERRUPT_STATUS 中断状态寄存器后数据才会刷新
                             中断不锁定，数据实时刷新
    @param modes
      INTERRUPUT_LATCH_ENABLE         中断锁定
      INTERRUPUT_LATCH_DISABLE        不锁定中断
  '''
  def set_interruput_latch(self, modes):
    rslt = self.read_reg(self.REG_AXES_ENABLE, 1)
    if modes == self.INTERRUPUT_LATCH_DISABLE:
      self.__txbuf[0] = rslt[0] & 0xFD
    else:
      self.__txbuf[0] = rslt[0] | 0x02
    self.write_reg(self.REG_AXES_ENABLE, self.__txbuf)

  '''
    @brief 设置阈值中断，当某个通道的地磁值高/低于阈值时触发中断
           高极性：高电平为活动电平，默认为低电平，触发中断时电平变为高
           低极性：低电平为活动电平，默认为高电平，触发中断时电平变为低
    @param modes
      LOW_THRESHOLD_INTERRUPT     低阈值中断模式
      HIGH_THRESHOLD_INTERRUPT    高阈值中断模式
    @param threshold
      阈值，默认扩大16倍，例如：低阈值模式下传入阈值1，实际低于16的地磁数据都会触发中断
    @param polarity
      POLARITY_HIGH               高极性
      POLARITY_LOW                低极性
    @param channelX
      INTERRUPT_X_ENABLE          使能 x 轴低阈值中断
      INTERRUPT_X_DISABLE         禁止 x 轴低阈值中断
    @param channelY
      INTERRUPT_Y_ENABLE          使能 y 轴低阈值中断
      INTERRUPT_Y_DISABLE         禁止 y 轴低阈值中断
    @param channelZ
      INTERRUPT_Z_ENABLE          使能 z 轴低阈值中断
      INTERRUPT_Z_DISABLE         禁止 z 轴低阈值中断
  '''
  def set_threshold_interrupt(self, mode, threshold, polarity, channel_x = INTERRUPT_X_ENABLE, channel_y = INTERRUPT_Y_ENABLE, channel_z = INTERRUPT_Z_ENABLE):
    if mode == self.LOW_THRESHOLD_INTERRUPT:
      self.__threshold_mode = self.LOW_THRESHOLD_INTERRUPT
      self.set_low_threshold_interrupt(channel_x, channel_y, channel_z, threshold, polarity)
    else:
      self.__threshold_mode = self.HIGH_THRESHOLD_INTERRUPT
      self.set_high_threshold_interrupt(channel_x, channel_y, channel_z, threshold, polarity)

  '''
    @brief 获取发生阈值中断的数据
    @return 返回存放地磁数据的列表，列表三轴当数据和中断状态，
            [0] x 轴触发阈值的数据 ，当数据为NO_DATA时为触发中断
            [1] y 轴触发阈值的数据 ，当数据为NO_DATA时为触发中断
            [2] z 轴触发阈值的数据 ，当数据为NO_DATA时为触发中断
            [3] 存放触发阈值中断状态的字符串
            [4] 存放阈值中断的状态的二进制数据格式，如下
               bit0 is 1 代表x轴触发了阈值中断
               bit1 is 1 代表y轴触发了阈值中断
               bit2 is 1 代表z轴触发了阈值中断
               ------------------------------------
               | bit7 ~ bit3 | bit2 | bit1 | bit0 |
               ------------------------------------
               |  reserved   |  0   |  0   |  0   |
               ------------------------------------
  '''
  def get_threshold_interrupt_data(self):
    data = [0]*10
    str1 = ""
    if self.__threshold_mode == self.LOW_THRESHOLD_INTERRUPT:
      state = self.get_low_threshold_interrupt_state()
    else:
      state = self.get_high_threshold_interrupt_state()
    rslt = self.get_geomagnetic()
    if (state>>0)&0x01:
      data[0] = rslt[0]
      str1 += "X "
    else:
      data[0] = self.NO_DATA
    if (state>>1)&0x01:
      data[1] = rslt[1]
      str1 += "Y "
    else:
      data[1] = self.NO_DATA
    if (state>>2)&0x01:
      data[2] = rslt[2]
      str1 += "Z "
    else:
      data[2] = self.NO_DATA
    if state != 0:
      str1 += " threshold interrupt"
    data[3] = str1
    data[4] = state&0x07
    
    return data
  '''
    @brief 设置低阈值中断，当某个通道的地磁值低于低阈值时触发中断
           高极性：高电平为活动电平，默认为低电平，触发中断时电平变为高
           低极性：低电平为活动电平，默认为高电平，触发中断时电平变为低
    @param channelX
      INTERRUPT_X_ENABLE          使能 x 轴低阈值中断
      INTERRUPT_X_DISABLE         禁止 x 轴低阈值中断
    @param channelY
      INTERRUPT_Y_ENABLE          使能 y 轴低阈值中断
      INTERRUPT_Y_DISABLE         禁止 y 轴低阈值中断
    @param channelZ
      INTERRUPT_Z_ENABLE          使能 z 轴低阈值中断
      INTERRUPT_Z_DISABLE         禁止 z 轴低阈值中断
    @param low_threshold              低阈值，默认扩大16倍，例如：传入 1 的阈值，实际低于16的地磁数据都会触发中断
    @param polarity
      POLARITY_HIGH                   高极性
      POLARITY_LOW                    低极性
  '''
  def set_low_threshold_interrupt(self, channel_x, channel_y, channel_z, low_threshold, polarity):
    if low_threshold < 0:
      self.__txbuf[0] = (low_threshold*-1) | 0x80
    else:
      self.__txbuf[0] = low_threshold
    self.write_reg(REG_LOW_THRESHOLD ,self.__txbuf)
    rslt = self.read_reg(REG_INT_CONFIG, 1)
    if channel_x == self.INTERRUPT_X_DISABLE:
      self.__txbuf[0] = rslt[0] | 0x01
    else:
      self.__txbuf[0] = rslt[0] & 0xFE
    if channel_y == self.INTERRUPT_Y_DISABLE:
      self.__txbuf[0] = self.__txbuf[0] | 0x02
    else:
      self.__txbuf[0] = self.__txbuf[0] & 0xFC
    if channel_x == self.INTERRUPT_X_DISABLE:
      self.__txbuf[0] = self.__txbuf[0] | 0x04
    else:
      self.__txbuf[0] = self.__txbuf[0] & 0xFB
    self.write_reg(self.REG_INT_CONFIG ,self.__txbuf)
    self.set_interrupt_pin(self.ENABLE_INTERRUPT_PIN, polarity)

  '''
    @brief 获取低阈值中断的状态，什么轴触发了低阈值中断
    @return status 返回的数字代表什么轴发生了低阈值中断
        bit0 is 1 代表x轴发生了中断
        bit1 is 1 代表y轴发生了中断
        bit2 is 1 代表z轴发生了中断
          ------------------------------------
          | bit7 ~ bit3 | bit2 | bit1 | bit0 |
          ------------------------------------
          |  reserved   |  0   |  0   |  0   |
          ------------------------------------
  '''
  def get_low_threshold_interrupt_state(self):
    rslt = self.read_reg(self.REG_INTERRUPT_STATUS, 1)
    return rslt[0]&0x07

  '''
    @brief设置高阈值中断，当某个通道的地磁值高于高阈值时触发中断，阈值默认扩大16倍
         INT 引脚产生中断后发生电平跳变
         引脚极性为高：高电平为活动电平，默认为低电平，触发临界值时电平进行跳变
         引脚极性为低：低电平为活动电平，默认为高电平，触发临界值时电平进行跳变
    @param channelX
      INTERRUPT_X_ENABLE          使能 x 轴高阈值中断
      INTERRUPT_X_DISABLE         禁止 x 轴高阈值中断
    @param channelY
      INTERRUPT_Y_ENABLE          使能 y 轴高阈值中断
      INTERRUPT_Y_DISABLE         禁止 y 轴高阈值中断
    @param channelZ
      INTERRUPT_Z_ENABLE          使能 z 轴高阈值中断
      INTERRUPT_Z_DISABLE         禁止 z 轴高阈值中断
    @param high_threshold              高阈值，默认扩大16倍，例如：传入 1 的阈值，实际高于16的地磁数据都会触发中断
    @param polarity
      POLARITY_HIGH                   高极性
      POLARITY_LOW                    低极性
  '''
  def set_high_threshold_interrupt(self, channel_x, channel_y, channel_z, high_threshold, polarity):
    if high_threshold < 0:
      self.__txbuf[0] = (high_threshold*-1) | 0x80
    else:
      self.__txbuf[0] = high_threshold
    self.write_reg(self.REG_HIGH_THRESHOLD, self.__txbuf)
    rslt = self.read_reg(self.REG_INT_CONFIG, 1)
    if channel_x == self.HIGH_INTERRUPT_X_DISABLE:
      self.__txbuf[0] = rslt[0] | 0x08
    else:
      self.__txbuf[0] = rslt[0] & 0xF7
    if channel_y == self.HIGH_INTERRUPT_Y_DISABLE:
      self.__txbuf[0] = self.__txbuf[0] | 0x10
    else:
      self.__txbuf[0] = self.__txbuf[0] & 0xEF
    if channel_x == self.HIGH_INTERRUPT_X_DISABLE:
      self.__txbuf[0] = self.__txbuf[0] | 0x20
    else:
      self.__txbuf[0] = self.__txbuf[0] & 0xDf    
    
    self.write_reg(REG_INT_CONFIG ,self.__txbuf)
    self.set_interrupt_pin(ENABLE_INTERRUPT_PIN, polarity)

  '''
    @brief 获取高阈值中断的状态，什么轴触发了高阈值中断
    @return status  返回的数字代表什么轴发生了高阈值中断
      bit0 is 1 代表x轴发生了中断
      bit1 is 1 代表y轴发生了中断
      bit2 is 1 代表z轴发生了中断
        ------------------------------------
        | bit7 ~ bit3 | bit2 | bit1 | bit0 |
        ------------------------------------
        |  reserved   |  0   |  0   |  0   |
        ------------------------------------
  '''
  def get_high_threshold_interrupt_state(self):
    rslt = self.read_reg(self.REG_INTERRUPT_STATUS, 1)
    return (rslt[0]&0x38)>>3

'''
  @brief An example of an i2c interface module
'''
class BMM150_I2C(BMM150):
  def __init__(self, i2c_addr = 0x13, bus_num = 1 ):
    self.__addr = i2c_addr
    self.i2c = I2C(bus_num)
    super(BMM150_I2C, self).__init__()

  '''
    @brief writes data to a register
    @param reg register address
    @param value written data
  '''
  def write_reg(self, reg, data):
    while 1:
      try:
        self.i2c.writeto_mem(self.__addr, reg, data)
        return
      except Exception:
        print("please check connect!")
        #os.system('i2cdetect -y 1')
        time.sleep(1)
        return
  '''
    @brief read the data from the register
    @param reg register address
    @param value read data
  '''
  def read_reg(self, reg ,len):
    while 1:
      try:
        rslt = self.i2c.readfrom_mem(self.__addr, reg, len)
        #print rslt
        return rslt
      except Exception:
        time.sleep(1)
        print("please check connect!")