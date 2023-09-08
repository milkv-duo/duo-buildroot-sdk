# -*- coding: utf-8 -*
""" 
lis2dw12三轴加速度计库
"""
import time
import numpy as np
from pinpong.board import gboard, Pin, I2C


class LIS2DW12(object):
  REG_CARD_ID      = 0x0F     #The chip id
  REG_CTRL_REG1    = 0x20     #Control register 1
  REG_CTRL_REG4    = 0x23     #Control register 2
  REG_CTRL_REG2    = 0x21     #Control register 3
  REG_CTRL_REG3    = 0x22     #Control register 4
  REG_CTRL_REG5    = 0x24     #Control register 5
  REG_CTRL_REG6    = 0x25     #Control register 6
  REG_CTRL_REG7    = 0x3F     #Control register 7
  REG_STATUS_REG   = 0x27     #Status register
  REG_OUT_X_L      = 0x28     #The low order of the X-axis acceleration register
  REG_OUT_X_H      = 0x29     #The high point of the X-axis acceleration register
  REG_OUT_Y_L      = 0x2A     #The low order of the Y-axis acceleration register
  REG_OUT_Y_H      = 0x2B     #The high point of the Y-axis acceleration register
  REG_OUT_Z_L      = 0x2C     #The low order of the Z-axis acceleration register
  REG_OUT_Z_H      = 0x2D     #The high point of the Z-axis acceleration register
  REG_WAKE_UP_DUR  = 0x35     #Wakeup and sleep duration configuration register (r/w).
  REG_FREE_FALL    = 0x36     #Free fall event register
  REG_STATUS_DUP   = 0x37     #Interrupt event status register
  REG_STATUS_DUP   = 0x37     #Interrupt event status register
  REG_WAKE_UP_SRC  = 0x38     #Wakeup source register
  REG_TAP_SRC      = 0x39     #Tap source register
  REG_SIXD_SRC     = 0x3A     #6D source register
  REG_ALL_INT_SRC  = 0x3B     #Reading this register, all related interrupt function flags routed to the INT pads are reset simultaneously
  REG_TAP_THS_X  =  0x30      #4D configuration enable and TAP threshold configuration .
  REG_TAP_THS_Y  =  0x31      #Threshold for tap recognition @ FS = ±2 g on Y direction
  REG_TAP_THS_Z  =  0x32      #Threshold for tap recognition @ FS = ±2 g on Z direction
  REG_INT_DUR    =  0x33      #Interrupt duration register
  REG_WAKE_UP_THS = 0x34      #Wakeup threshold register
  SPI_READ_BIT   =  0X80      # bit 0: RW bit. When 0, the data DI(7:0) is written into the device. When 1, the data DO(7:0) from the device is read.
  ID = 0X44
  __range =  0.061
  __range_d = 0
  
  '''
   Power mode
  '''
  HIGH_PERFORMANCE_14BIT                   = 0X04#High-Performance Mode
  CONT_LOWPWR4_14BIT                       = 0X03#Continuous measurement,Low-Power Mode 4(14-bit resolution)
  CONT_LOWPWR3_14BIT                       = 0X02#Continuous measurement,Low-Power Mode 3(14-bit resolution)
  CONT_LOWPWR2_14BIT                       = 0X01#Continuous measurement,Low-Power Mode 2(14-bit resolution)
  CONT_LOWPWR1_12BIT                       = 0X00#Continuous measurement,Low-Power Mode 1(12-bit resolution)
  SING_LELOWPWR4_14BIT                     = 0X0B#Single data conversion on demand mode,Low-Power Mode 4(14-bit resolution)
  SING_LELOWPWR3_14BIT                     = 0X0A#Single data conversion on demand mode,Low-Power Mode 3(14-bit resolution
  SING_LELOWPWR2_14BIT                     = 0X09#Single data conversion on demand mode,Low-Power Mode 2(14-bit resolution)
  SING_LELOWPWR1_12BIT                     = 0X08#Single data conversion on demand mode,Low-Power Mode 1(12-bit resolution)
  HIGHP_ERFORMANCELOW_NOISE_14BIT          = 0X14#High-Performance Mode,Low-noise enabled
  CONT_LOWPWRLOWNOISE4_14BIT               = 0X13#Continuous measurement,Low-Power Mode 4(14-bit resolution,Low-noise enabled)
  CONT_LOWPWRLOWNOISE3_14BIT               = 0X12#Continuous measurement,Low-Power Mode 3(14-bit resolution,Low-noise enabled)
  CONT_LOWPWRLOWNOISE2_14BIT               = 0X11#Continuous measurement,Low-Power Mode 2(14-bit resolution,Low-noise enabled)
  CONT_LOWPWRLOWNOISE1_12BIT               = 0X10#Continuous measurement,Low-Power Mode 1(14-bit resolution,Low-noise enabled)
  SINGLE_LOWPWRLOWNOISE4_14BIT             = 0X1B#Single data conversion on demand mode,Low-Power Mode 4(14-bit resolution),Low-noise enabled
  SINGLE_LOWPWRLOWNOISE3_14BIT             = 0X1A#Single data conversion on demand mode,Low-Power Mode 3(14-bit resolution),Low-noise enabled
  SINGLE_LOWPWRLOWNOISE2_14BIT             = 0X19#Single data conversion on demand mode,Low-Power Mode 2(14-bit resolution),Low-noise enabled
  SINGLE_LOWPWRLOWNOISE1_12BIT             = 0X18#Single data conversion on demand mode,Low-Power Mode 1(12-bit resolution),Low-noise enabled

  '''
    Sensor range
  '''
  RANGE_2G     = 2   #±2g
  RANGE_4G     = 4   #±4g
  RANGE_8G     = 8   #±8g
  RANGE_16G    = 16  #±16g
  
  '''
    Filtering mode
  '''
  LPF         = 0x00 #Low pass filter
  HPF         = 0x10 #High pass filter

  '''
     bandwidth of collected data
  '''
  RATE_DIV_2     = 0  #RATE/2 (up to RATE = 800 Hz, 400 Hz when RATE = 1600 Hz)>*/
  RATE_DIV_4     = 1  #RATE/4 (High Power/Low power)
  RATE_DIV_10    = 2  #RATE/10 (HP/LP)
  RATE_DIV_20    = 3  #RATE/20 (HP/LP)

  '''
  Data collection rate
  '''
  RATE_OFF            = 0X00  #测量关闭
  RATE_1HZ6           = 0X01  #1.6hz,仅在低功耗模式下使用
  RATE_12HZ5          = 0X02  #12.5hz
  RATE_25HZ           = 0X03
  RATE_50HZ           = 0X04
  RATE_100HZ          = 0X05
  RATE_200HZ          = 0X06
  RATE_400HZ          = 0X07  #仅在High-Performance mode下使用
  RATE_800HZ          = 0X08  #仅在High-Performance mode下使用
  RATE_1600HZ         = 0X09  #仅在High-Performance mode下使用
  SETSWTRIG           = 0X12  #软件触发单次测量
  
  '''
    Motion detection mode
  '''
  NO_DETECTION        = 0 #No detection
  DETECT_ACT          = 1 #Detect movement
  DETECT_STATMOTION   = 3 #Detect Motion


  '''
  Interrupt source 1 trigger event setting
  '''
  DOUBLE_TAP = 0x08     #双击事件
  FREEFALL   = 0x10     #自由落体事件
  WAKEUP     = 0x20     #唤醒事件
  SINGLE_TAP = 0x40     #单击事件
  IA6D       = 0x80     #在正面朝上/朝下/朝左/朝右/朝前/朝后 的状态发生改变的事件

  '''
  Interrupt source 2 trigger event setting
  '''
  SLEEP_STATE  = 0x40 #Sleep change status routed to INT2 pad
  SLEEP_CHANGE = 0x80 #Enable routing of SLEEP_STATE on INT2 pad

  
  '''
  tap or double tap
  '''
  S_TAP  = 0  #single tap
  D_TAP  = 1  #double tap
  NO_TAP      = 2  #no tap
  
  #which direction is tap event detected
  DIR_X_UP   = 0 #在X 正方向发生的点击事件
  DIR_X_DOWN = 1 #在X 负方向发生的点击事件
  DIR_Y_UP   = 2 #在Y 正方向发生的点击事件
  DIR_Y_DOWN = 3 #在Y 负方向发生的点击事件
  DIR_Z_UP   = 4 #在Z 正方向发生的点击事件
  DIR_Z_DOWN = 5 #在Z 负方向发生的点击事件
  
  #which direction is wake up event detected
  DIR_X = 0 #X方向的运动唤醒芯片
  DIR_Y = 1 #Y方向的运动唤醒芯片
  DIR_Z = 2 #Z方向的运动唤醒芯片
  
  ERROR = 0XFF
  
  #tap detection mode
  ONLY_SINGLE          = 0 #Only detect tap events.
  BOTH_SINGLE_DOUBLE   = 1 #Both single-tap and double-tap events are detected.

  '''
  位置检测
  '''
  DEGREES_80          = 0 #80 degrees.
  DEGREES_70          = 1 #70 degrees.
  DEGREES_60          = 2 #60 degrees.
  DEGREES_50          = 3 #50 degrees.
  
  #orientation
  X_DOWN = 0  #X is now down
  X_UP   = 1  #X is now up
  Y_DOWN = 2  #Y is now down
  Y_UP   = 3  #Y is now up
  Z_DOWN = 4  #Z is now down
  Z_UP   = 5  #Z is now up

  def __init__(self):
    __reset = 1
  '''
    @brief Initialize the function
    @return True(初始化成功)/Fasle(初始化失败)
  '''
  def begin(self):
    identifier = self.read_reg(self.REG_CARD_ID , 1)
    if identifier == self.ID:
      return True
    else:
      return False
      
  '''
    @brief Get chip id
    @return 8 bit serial number
  '''
  def get_id(self):
    identifier = self.read_reg(self.REG_CARD_ID,1)
    return identifier

  '''
    @brief Software reset to restore the value of all registers to the default value
  '''
  def soft_reset(self):
    value = self.read_reg(self.REG_CTRL_REG2,1)
    value = value | (1<<6)
    #print(value)
    self.write_reg(self.REG_CTRL_REG2,value)
    
  '''
    @brief Set the measurement range
    @param range Range
                 RANGE_2G     #±2g
                 RANGE_4G     #±4g
                 RANGE_8G     #±8g
                 RANGE_16G    #±16g
  '''
  def set_range(self,range_r):
    value = self.read_reg(self.REG_CTRL_REG6,1)
    self.__range_d = range_r
    value = value & (~(3<<4))
    if range_r == self.RANGE_2G:
      self.__range = 0.061
    elif range_r == self.RANGE_4G:
      self._range = 0.122
      value = value | (1<<4)
    elif range_r == self.RANGE_8G:
      self._range = 0.244
      value = value | (2<<4)
    elif range_r == self.RANGE_16G:
      self._range = 0.488
      value = value | (3<<4)
    self.write_reg(self.REG_CTRL_REG6,value)
    
  '''
    @brief Choose whether to continuously let the chip collect data
    @param enable  true(continuous update)/false( output registers not updated until MSB and LSB read)
  '''
  def contin_refresh(self,enable):
    value = self.read_reg(self.REG_CTRL_REG2,1)
    if enable == True:
       value = value | (1<<3)
    else:
       value = value &(~(1<<3))
    self.write_reg(self.REG_CTRL_REG2,value)
  
  '''
    @brief Set the filter processing mode
    @param path path of filtering
                LPF          #Low pass filter
                HPF          #High pass filter
  '''
  def set_filter_path(self,path):
    value = self.read_reg(self.REG_CTRL_REG6,1)
    enable = path & 0x10
    if(enable > 0):
       enable = 1
    value = value & (~(3<<2))
    value = value | (enable << 3)
    #print(value)
    self.write_reg(self.REG_CTRL_REG6,value)
    value = self.read_reg(self.REG_CTRL_REG7,1)
    
    value = value &(~(1<<4))
    value = value | enable << 4
    #print(value)
    self.write_reg(self.REG_CTRL_REG7,value)

  '''
    @brief Set the  bandwidth of the data
    @param bw bandwidth
                RATE_DIV_2   #RATE/2 (up to RATE = 800 Hz, 400 Hz when RATE = 1600 Hz)
                RATE_DIV_4   #RATE/4 (High Power/Low power)
                RATE_DIV_10  #RATE/10 (HP/LP)
                RATE_DIV_20  #RATE/20 (HP/LP)
  '''
  def set_filter_bandwidth(self,bw):
    value = self.read_reg(self.REG_CTRL_REG6,1)
    value = value & (~(3 << 6))
    value = value | (bw << 6)
    #print(value)
    self.write_reg(self.REG_CTRL_REG6,value)
    
  '''
    @brief Set power mode
    @param mode 16 power modes to choose from
               HIGH_PERFORMANCE_14BIT          #High-Performance Mode
               CONT_LOWPWR4_14BIT              #Continuous measurement,Low-Power Mode 4(14-bit resolution)
               CONT_LOWPWR3_14BIT              #Continuous measurement,Low-Power Mode 3(14-bit resolution)
               CONT_LOWPWR2_14BIT              #Continuous measurement,Low-Power Mode 2(14-bit resolution)
               CONT_LOWPWR1_12BIT              #Continuous measurement,Low-Power Mode 1(12-bit resolution)
               SING_LELOWPWR4_14BIT            #Single data conversion on demand mode,Low-Power Mode 4(14-bit resolution)
               SING_LELOWPWR3_14BIT            #Single data conversion on demand mode,Low-Power Mode 3(14-bit resolution
               SING_LELOWPWR2_14BIT            #Single data conversion on demand mode,Low-Power Mode 2(14-bit resolution)
               SING_LELOWPWR1_12BIT            #Single data conversion on demand mode,Low-Power Mode 1(12-bit resolution)
               HIGHP_ERFORMANCELOW_NOISE_14BIT #High-Performance Mode,Low-noise enabled
               CONT_LOWPWRLOWNOISE4_14BIT      #Continuous measurement,Low-Power Mode 4(14-bit resolution,Low-noise enabled)
               CONT_LOWPWRLOWNOISE3_14BIT      #Continuous measurement,Low-Power Mode 3(14-bit resolution,Low-noise enabled)
               CONT_LOWPWRLOWNOISE2_14BIT      #Continuous measurement,Low-Power Mode 2(14-bit resolution,Low-noise enabled)
               CONT_LOWPWRLOWNOISE1_12BIT      #Continuous measurement,Low-Power Mode 1(14-bit resolution,Low-noise enabled)
               SINGLE_LOWPWRLOWNOISE4_14BIT    #Single data conversion on demand mode,Low-Power Mode 4(14-bit resolution),Low-noise enabled
               SINGLE_LOWPWRLOWNOISE3_14BIT    #Single data conversion on demand mode,Low-Power Mode 3(14-bit resolution),Low-noise enabled
               SINGLE_LOWPWRLOWNOISE2_14BIT    #Single data conversion on demand mode,Low-Power Mode 2(14-bit resolution),Low-noise enabled
               SINGLE_LOWPWRLOWNOISE1_12BIT    #Single data conversion on demand mode,Low-Power Mode 1(12-bit resolution),Low-noise enabled
  '''                                          
  def set_power_mode(self,mode):
    value = self.read_reg(self.REG_CTRL_REG1,1)
    value = value & (~0x0f)
    value = value | (mode & 0xf)
    self.write_reg(self.REG_CTRL_REG1,value)
    value = self.read_reg(self.REG_CTRL_REG6,1)
    enable = mode >> 4
    value = value & (~(1 << 2))
    value = value | (enable << 2)
    self.write_reg(self.REG_CTRL_REG6,value)
    
  '''
    @brief Set data measurement rate
    @param rate rate
                 RATE_OFF          #测量关闭
                 RATE_1HZ6         #1.6hz,仅在低功耗模式下使用
                 RATE_12HZ5        #12.5hz
                 RATE_25HZ         
                 RATE_50HZ         
                 RATE_100HZ        
                 RATE_200HZ        
                 RATE_400HZ        #仅在High-Performance mode下使用
                 RATE_800HZ        #仅在High-Performance mode下使用
                 RATE_1600HZ       #仅在High-Performance mode下使用
                 SETSWTRIG         #软件触发单次测量
  '''
  def set_data_rate(self, rate):
    value = self.read_reg(self.REG_CTRL_REG1,1)
    value = value & (~(0xf << 4))
    value = value | (rate << 4)
    self.write_reg(self.REG_CTRL_REG1,value)
    value = self.read_reg(self.REG_CTRL_REG3,1)
    enable = (rate&0x30) >> 4
    
    value = value & (~3)
    value = value | enable
    self.write_reg(self.REG_CTRL_REG3,value)
    
  '''
     @brief 设置自由落体时间,也可以称作自由落体样本个数，只有产生足够多的自由落体样本，才会产生自由落体事件
     @param dur duration,范围:0~31
     @n time = dur * (1/rate)(unit:s)
     |                                  参数与时间之间的线性关系的示例                                                        |
     |------------------------------------------------------------------------------------------------------------------------|
     |                |                     |                          |                          |                           |
     |  Data rate     |       25 Hz         |         100 Hz           |          400 Hz          |         = 800 Hz          |
     |------------------------------------------------------------------------------------------------------------------------|
     |   time         |dur*(1s/25)= dur*40ms|  dur*(1s/100)= dur*10ms  |  dur*(1s/400)= dur*2.5ms |  dur*(1s/800)= dur*1.25ms |
     |------------------------------------------------------------------------------------------------------------------------|
  '''
  def set_free_fall_Dur(self,dur):
    value1 = self.read_reg(self.REG_WAKE_UP_DUR,1)
    value2 = self.read_reg(self.REG_FREE_FALL,1)
    
    value1 = value1 & (~0x80)
    value2 = value2 & (~0xf8)
    value2 = value2 | (dur << 3)
    #print(value1)
    self.write_reg(self.REG_WAKE_UP_DUR,value1)
    #print(value2)
    self.write_reg(self.REG_FREE_FALL,value2)
    self.__set_ff_threshold(3)
    
  '''
    @brief Set Free-fall threshold
    @param th threshold
  '''
  def __set_ff_threshold(self,th):
    value = self.read_reg(self.REG_FREE_FALL,1)
    value = value & (~0x07)
    value = value | (th & 0x07)
    #print(value)
    self.write_reg(self.REG_FREE_FALL,value)
  
  '''
    @brief Set the interrupt source of the int1 pin
    @param event  Several interrupt events, after setting, when an event is generated, a level transition will be generated on the int1 pin
              DOUBLE_TAP    #双击事件
              FREEFALL      #自由落体事件
              WAKEUP        #唤醒事件
              SINGLE_TAP    #单击事件
              IA6D          #在正面朝上/朝下/朝左/朝右/朝前/朝后 的状态发生改变的事件
    
  '''
  def set_int1_event(self,event):
    value1 = self.read_reg(self.REG_CTRL_REG4,1)
    value2 = self.read_reg(self.REG_CTRL_REG5,1)
    value3 = self.read_reg(self.REG_CTRL_REG7,1)
    value3 = value3 & (~0x20)
    value3 = value3 | 0x20
    value1 = value1 | event
    self.write_reg(self.REG_CTRL_REG4,value1)
    self.write_reg(self.REG_CTRL_REG7,value3)
    if event == self.FREEFALL:
      self.__lock_interrupt(True)
  '''
     @brief 设置唤醒持续时间,在setActMode()函数使用eDetectAct的检测模式时,芯片在被唤醒后,会持续一段时间以正常速率采集数据
     @n 然后便会继续休眠,以12.5hz的频率采集数据
     @param dur  duration,范围:0~3
     @n time = dur * (1/rate)(unit:s)
     |                                  参数与时间之间的线性关系的示例                                                        |
     |------------------------------------------------------------------------------------------------------------------------|
     |                |                     |                          |                          |                           |
     |  Data rate     |       25 Hz         |         100 Hz           |          400 Hz          |         = 800 Hz          |
     |------------------------------------------------------------------------------------------------------------------------|
     |   time         |dur*(1s/25)= dur*40ms|  dur*(1s/100)= dur*10ms  |  dur*(1s/400)= dur*2.5ms |  dur*(1s/800)= dur*1.25ms |
     |------------------------------------------------------------------------------------------------------------------------|
  '''
  def set_wakeup_dur(self,dur):
    value = self.read_reg(self.REG_WAKE_UP_DUR,1)
    value = value & (~0x60)
    value = value | ((dur << 5) & 0x60)
    #print(value)
    self.write_reg(self.REG_WAKE_UP_DUR,value)

  '''
    @brief 设置运动检测的模式,第一种模式不会去检测模块是否在运动，第二种模式在设置后芯片会以较低的频率测量数据,以降低功耗
    @n 在检测到运动后会恢复到正常频率,第三种只会检测模块是否处于睡眠状态
    @param mode 运动检测模式
                NO_DETECTION         #No detection
                DETECT_ACT           #Detect movement,the chip automatically goes to 12.5 Hz rate in the low-power mode
                DETECT_STATMOTION    #Detect Motion, the chip detects acceleration below a fixed threshold but does not change either rate or operating mode
  '''
  def set_act_mode(self,mode):
    value1 = self.read_reg(self.REG_WAKE_UP_THS,1)
    value2 = self.read_reg(self.REG_WAKE_UP_DUR,1)
    value1 = value1 & (~(1<<6))
    value2 = value2 & (~(1<<4))
    value1 = value1 | (mode & 0x01)<<6
    value2 = value2 | ((mode & 0x02)>>1)<<4
    #print(value1)
    #print(value2)
    self.write_reg(self.REG_WAKE_UP_THS,value1)
    self.write_reg(self.REG_WAKE_UP_DUR,value2)

  '''
    @brief Set the wake-up threshold,某个方向的加速度大于此值时,会触发wake-up事件
    @param th threshold ,unit:mg,数值是在量程之内
  '''
  def set_wakeup_threshold(self,th):
    th1 = (float(th)/self.__range_d) * 64
    value = self.read_reg(self.REG_WAKE_UP_THS,1)
    value = value &(~0x3f)
    value = value | (int(th1) & 0x3f)
    #print(value)
    self.write_reg(self.REG_WAKE_UP_THS,value)
    
  '''
    @brief lock interrupt Switches between latched ('1'-logic) and pulsed ('0'-logic) mode for 
    @n function source signals and interrupts routed to pins (wakeup, single/double-tap).
    @param enable  true lock interrupt.
                    false pulsed interrupt
  '''
  def __lock_interrupt(self,enable):
    value = self.read_reg(self.REG_CTRL_REG3,1)
    value = value & (~0x10)
    value = value | (enable << 4)
    self.write_reg(self.REG_CTRL_REG3,value)
    
  '''
    @brief Set to detect tap events in the Z direction
    @param enable Ture(使能点击检测\False(禁用点击检测)
  '''
  def enable_tap_detection_on_z(self, enable):
    value = self.read_reg(self.REG_TAP_THS_Z,1)
    value = value & (~(1<<5))
    value = value | (enable << 5)
    #print("enable_tap_detection_on_z")
    #print(value)
    self.write_reg(self.REG_TAP_THS_Z,value)
  
  '''
    @brief Set to detect tap events in the Y direction
    @param enable Ture(使能点击检测\False(禁用点击检测)
  '''
  def enable_tap_detection_on_y(self, enable):
    value = self.read_reg(self.REG_TAP_THS_Z,1)
    value = value & (~(1<<6))
    value = value | (enable << 6)
    #print("enable_tap_detection_on_y")
    #print(value)
    self.write_reg(self.REG_TAP_THS_Z,value)
    
  '''
    @brief Set to detect tap events in the X direction
    @param enable Ture(使能点击检)\False(禁用点击检)
  '''
  def enable_tap_detection_on_x(self, enable):
    value = self.read_reg(self.REG_TAP_THS_Z,1)
    value = value & (~(1<<7))
    value = value | (enable << 7)
    #print("enable_tap_detection_on_x")
    #print(value)
    self.write_reg(self.REG_TAP_THS_Z,value)

  '''
    @brief Set the tap threshold in the X direction
    @param th Threshold(g),Can only be used in the range of ±2g
  '''
  def set_tap_threshold_on_x(self,th):
    
    th1 = (float(th)/self.__range_d) * 32
    value = self.read_reg(self.REG_TAP_THS_X,1)
    value = value & (~0x1f)
    value = value | (int(th1) & 0x1f)
    #print("set_tap_threshold_on_x")
    #print(value)
    self.write_reg(self.REG_TAP_THS_X,value)
  
  '''
    @brief Set the tap threshold in the Y direction
    @param th Threshold(g),Can only be used in the range of ±2g
  '''
  def set_tap_threshold_on_y(self,th):
    th1 = (float(th)/self.__range_d) * 32
    value = self.read_reg(self.REG_TAP_THS_Y,1)
    value = value & (~0x1f)
    value = value | (int(th1) & 0x1f)
    #print("set_tap_threshold_on_y")
    #print(value)
    self.write_reg(self.REG_TAP_THS_Y,value)
    
  '''
    @brief Set the tap threshold in the Z direction
    @param th Threshold(g),Can only be used in the range of ±2g
  '''
  def set_tap_threshold_on_z(self,th):
    th1 = (float(th)/self.__range_d) * 32
    value = self.read_reg(self.REG_TAP_THS_Z,1)
    value = value & (~0x1f)
    value = value | (int(th1) & 0x1f)
    #print("set_tap_threshold_on_z")
    #print(value)
    self.write_reg(self.REG_TAP_THS_Z,value)
    
  '''
   @brief Duration of maximum time gap for double-tap recognition. When double-tap 
   @n recognition is enabled, this register expresses the maximum time between two 
   @n successive detected taps to determine a double-tap event.
   @param dur  duration,范围:0~15
   @n time = dur * (1/rate)(unit:s)
    |                                  参数与时间之间的线性关系的示例                                                        |
    |------------------------------------------------------------------------------------------------------------------------|
    |                |                     |                          |                          |                           |
    |  Data rate     |       25 Hz         |         100 Hz           |          400 Hz          |         = 800 Hz          |
    |------------------------------------------------------------------------------------------------------------------------|
    |   time         |dur*(1s/25)= dur*40ms|  dur*(1s/100)= dur*10ms  |  dur*(1s/400)= dur*2.5ms |  dur*(1s/800)= dur*1.25ms |
    |------------------------------------------------------------------------------------------------------------------------|
  '''
  def set_tap_dur(self,dur):
    value = self.read_reg(self.REG_INT_DUR,1)
    value = value & (~0xf0)
    value = value | (dur << 4)
    #print("set_tap_dur")
    #print(value)
    self.write_reg(self.REG_INT_DUR,value)
    self.__set_tap_quiet(2);
    self.__set_tap_shock(2);
    
  '''
    @brief set quiet time after a tap detection: this register represents
    @n the time after the first detected tap in which there must not be any overthreshold event.
    @param quiet quiet time
  '''
  def __set_tap_quiet(self,quiet):
    value = self.read_reg(self.REG_INT_DUR,1)
    value = value & (~0x0C)
    quiet = quiet & 0x03
    value = value | (quiet<<2)
    #print("set_tap_quiet")
    #print(value)
    self.write_reg(self.REG_INT_DUR,value)
  
  '''
    @brief Set the maximum time of an over-threshold signal detection to be recognized as a tap event.
    @param shock  shock time
  '''
  def __set_tap_shock(self,shock):
    value = self.read_reg(self.REG_INT_DUR,1)
    value = value & (~0x03)
    shock = shock & 0x03
    value = value | (shock)
    #print("set_tap_shock")
    #print(value)
    self.write_reg(self.REG_INT_DUR,value)
  
  '''
    @brief Set the tap detection mode,检测单击和单击,双击都检测
    @param mode  点击检测模式
                     ONLY_SINGLE        #检测单击
                     BOTH_SINGLE_DOUBLE #检测单击和双击
  '''
  def set_tap_mode(self,mode):
    value = self.read_reg(self.REG_WAKE_UP_THS,1)
    value = value & (~0x80)
    value = value | (mode << 7)
    #print("set_tap_mode")
    #print(value)
    self.write_reg(self.REG_WAKE_UP_THS,value)

  '''
    @brief Set Thresholds for 4D/6D，当转动的阈值大于指定角度时,就发生方向转变的事件
    @param degree   DEGREES_80   #80°
                    DEGREES_70   #70°
                    DEGREES_60   #60°
                    DEGREES_50   #50°
  '''
  def set_6d_threshold(self,degree):
    value = self.read_reg(self.REG_TAP_THS_X,1)
    value = value & (~0x60)
    value = value | (degree << 5)
    #print("set_6d_threshold")
    #print(value)
    self.write_reg(self.REG_TAP_THS_X,value)
    self.__set_6d_feed_data(1)
    
  '''
    @brief 选择在中断2引脚产生的中断事件
    @param event  Several interrupt events, after setting, when an event is generated, a level transition will be generated on the int2 pin
                  SLEEP_CHANGE  #Enable routing of SLEEP_STATE on INT2 pad
                  SLEEP_STATE   #0x80 Sleep change status routed to INT2 pad
  '''
  def set_int2_event(self,event):
    value1 = self.read_reg(self.REG_CTRL_REG4,1)
    value2 = self.read_reg(self.REG_CTRL_REG5,1)
    value3 = self.read_reg(self.REG_CTRL_REG7,1)
    value3 = value3 & (~0x20)
    value3 = value3 | 0x20
    value2 = value2 | event
    #print(value2)
    #print(value3)
    self.write_reg(self.REG_CTRL_REG5,value2)
    self.write_reg(self.REG_CTRL_REG7,value3)

  '''
    @brief Set 6d filtered data source
    @param data 0: RATE/2 low pass filtered data sent to 6D interrupt function (default)
                1: LPF2 output data sent to 6D interrupt function)
  '''
  def __set_6d_feed_data(self,data):
    value = self.read_reg(self.REG_CTRL_REG7,1)
    value = value & (~1)
    value = value | data
    #print(value)
    self.write_reg(self.REG_CTRL_REG7,value)
  '''
    @brief Read the acceleration in the x direction
    @return Acceleration data from x(mg),测量的量程为±2g,±4g,±8g或±16g,通过set_range()函数设置
  '''
  def read_acc_x_mg(self):
    value1 = self.read_reg(self.REG_OUT_X_L,1)
    value2 = self.read_reg(self.REG_OUT_X_L+1,1)
    acc_x = np.int8(value2)*256 + np.int8(value1)
    acc_x = acc_x * self.__range
    return acc_x

  '''
    @brief Read the acceleration in the y direction
    @return  Acceleration data from y(mg),测量的量程为±2g,±4g,±8g或±16g,通过set_range()函数设置
  '''
  def read_acc_y_mg(self):
    value1 = self.read_reg(self.REG_OUT_Y_L,1)
    value2 = self.read_reg(self.REG_OUT_Y_L+1,1)
    acc_y = np.int8(value2)*256 + np.int8(value1)
    acc_y = acc_y * self.__range
    return acc_y

  '''
    @brief Read the acceleration in the z direction
    @return Acceleration data from z(mg),测量的量程为±2g,±4g,±8g或±16g,通过set_range()函数设置
  '''
  def read_acc_z_mg(self):
    value1 = self.read_reg(self.REG_OUT_Z_L,1)
    value2 = self.read_reg(self.REG_OUT_Z_L+1,1)
    acc_z = np.int8(value2)*256 + np.int8(value1)
    acc_z = acc_z * self.__range
    return acc_z
  
  '''
    @brief 检测是否有运动产生
    @return True(产生运动)/False(传感器未运动)
  '''
  def act_detected(self):
    value = self.read_reg(self.REG_WAKE_UP_SRC,1)
    if(value & 0x08) > 0:
      return True
    else:
      return False
      
  '''
    @brief 自由落体运动检测
    @return True(检测到自由落体运动)/False(未检测到自由落体运动)
  '''
  def free_fall_detected(self):
    value = self.read_reg(self.REG_WAKE_UP_SRC,1)
    if(value & 0x20) > 0:
      return True
    return False
    
  '''
    @brief 芯片在正面朝上/朝下/朝左/朝右/朝前/朝后的状态发生改变.
    @return True(a change in position is detected)/False(no event detected)
  '''
  def ori_change_detected(self):
    value = self.read_reg(self.REG_SIXD_SRC,1)
    if(value & 0x40) > 0:
      return True
    else:
      return False
      
  '''
  @brief 获取传感器现在的位置
  @return      X_DOWN   #X is now down
               X_UP     #X is now up
               Y_DOWN   #Y is now down
               Y_UP     #Y is now up
               Z_DOWN   #Z is now down
               Z_UP     #Z is now up
  '''
  def get_oriention(self):
   value = self.read_reg(self.REG_SIXD_SRC,1)
   orient = self.ERROR
   if(value & 0x1) > 0:
     orient =  self.X_DOWN
   elif(value & 0x2) > 0:
     orient =  self.X_UP
   elif(value & 0x4) > 0:
     orient =  self.Y_DOWN
   elif(value & 0x8) > 0:
     orient =  self.Y_UP
   elif(value & 0x10) > 0:
     orient =  self.Z_DOWN
   elif(value & 0x20) > 0:
     orient =  self.Z_UP
   return orient
     
  '''
    @brief 点击检测,能检测是发生的双击,还是单击
    @return   S_TAP       #single tap
              D_TAP       #double tap
              NO_TAP,     #没有点击产生
  '''
  def tap_detect(self):
   value = self.read_reg(self.REG_TAP_SRC,1)
   #print(value)
   tap = self.NO_TAP
   if(value & 0x20) > 0:
     tap = self.S_TAP
   elif(value & 0x10) > 0:
     tap = self.D_TAP
   return tap
   #Wakeup event detection status on X-axis

  '''
    @brief 点击方向的源头检测
    @return     DIR_X_UP   #在X 正方向发生的点击事件
                DIR_X_DOWN #在X 负方向发生的点击事件
                DIR_Y_UP   #在Y 正方向发生的点击事件
                DIR_Y_DOWN #在Y 负方向发生的点击事件
                DIR_Z_UP   #在Z 正方向发生的点击事件
                DIR_Z_DOWN #在Z 负方向发生的点击事件
  '''
  def get_tap_direction(self):
   value = self.read_reg(self.REG_TAP_SRC,1)
   #print(value)
   direction = self.ERROR
   positive = value & 0x08
   if(value & 0x4)>0 and positive > 0:
     direction = self.DIR_X_UP
   elif(value & 0x4)>0 and positive == 0:
     direction = self.DIR_X_DOWN
   elif(value & 0x2)>0 and positive > 0:
     direction = self.DIR_Y_UP
   elif(value & 0x2)>0 and positive == 0:
     direction = self.DIR_Y_DOWN
   elif(value & 0x1)>0 and positive > 0:
     direction = self.DIR_Z_UP
   elif(value & 0x1)>0 and positive == 0:
     direction = self.DIR_Z_DOWN
   return direction
  '''
    @brief 唤醒的运动方向检测
    @return    DIR_X  #X方向的运动唤醒芯片
               DIR_Y  #Y方向的运动唤醒芯片
               DIR_Z  #Z方向的运动唤醒芯片
               eDirError,
  '''
  def get_wake_up_dir(self):
    value = self.read_reg(self.REG_WAKE_UP_SRC,1)
    direction = self.ERROR
    if(value & 0x01) > 0:
      direction = self.DIR_Z
    elif(value & 0x02) > 0:
      direction = self.DIR_Y
    elif(value & 0x04) > 0:
      direction = self.DIR_X
    return direction

  '''
    @brief In Single data conversion on demand mode,请求测量一次数据
  '''
  def demand_data(self):
    value = self.read_reg(self.REG_CTRL_REG3)
    value = value | 1
    self.write_reg(self.REG_CTRL_REG3,value)


  def read_acc(self):
      acc = ACCVAL()
      acc.x = self.read_acc_x_mg()
      acc.y = self.read_acc_y_mg()
      acc.z = self.read_acc_z_mg()
      return acc

class ACCVAL():
  def __init__(self):
    self.x = 0
    self.y = 0
    self.z = 0  
  
class LIS2DW12_I2C(LIS2DW12): 
  def __init__(self , iic_addr = 0x18, bus_num  = 0x00):
    self.iic_addr = iic_addr
    self.i2c = I2C(bus_num)
    super(LIS2DW12_I2C, self).__init__()


  '''
    @brief writes data to a register
    @param reg register address
    @param value written data
  '''
  def write_reg(self, reg, data):
        self.i2c.writeto_mem(self.iic_addr ,reg,[data])

  '''
    @brief read the data from the register
    @param reg register address
    @return read data
  '''
  def read_reg(self, reg, length):
    rslt = self.i2c.readfrom_mem(self.iic_addr, reg, length)
    return rslt[0]
    
