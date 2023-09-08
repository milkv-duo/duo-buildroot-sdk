# -*- coding: utf-8 -*-

#Nezha
#实验效果：BMP388传感器测量当前的气压和海拔

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bmp3xx import BMP3XX_I2C,ULTRA_PRECISION


Board("nezha").begin()
# iic_addr = 0x76: pin SDO is low
# iic_addr = 0x77: pin SDO is high
bmp388 = BMP3XX_I2C(iic_addr = 0x77)

def setup():
  while (bmp388.begin() == False):
    print ('Please check that the device is properly connected')
    time.sleep(3)
  print("bmp388 begin successfully!!!")

  '''
    # 让用户方便配置常用的6种采样模式，mode:
    #      ULTRA_LOW_PRECISION，超低精度，适合天气监控（最低功耗），电源模式为强制模式
    #      LOW_PRECISION，低精度，适合随意的检测，电源模式为正常模式
    #      NORMAL_PRECISION1，标准精度1，适合在手持式设备上动态检测（例如在手机上），电源模式为正常模式
    #      NORMAL_PRECISION2，标准精度2，适合无人机，电源模式为正常模式
    #      HIGH_PRECISION，高精度，适合在低功耗手持式设备上（例如在手机上），电源模式为正常模式
    #      ULTRA_PRECISION，超高精度，适合室内的导航，采集速率会极低，采集周期1000ms，电源模式为正常模式
  '''
  while(bmp388.set_common_sampling_mode(ULTRA_PRECISION) == False):
    print ('Set samping mode fail, retrying...')
    time.sleep(3)

  '''
    # 根据所给当前位置海拔，校准传感器
    # 此示例使用的是成都市温江区（中国）海拔540米，请使用者实际使用中换成本地海拔校准
    # 如果没有调用此接口，测量数据将不消除绝对误差
    # 注意：此接口只有第一次调用有效
    # 如果不需要消除测量绝对差，请注释下面两行
  '''
  if( bmp388.calibrated_absolute_difference(540.0) == True ):
    print("Absolute difference base value set successfully!")

setup()
while True:
  print("---------------------------------")
  ''' 直接读取当前测量的温度数据，单位℃ '''
  print(f"temperature : {bmp388.get_temperature} C")

  ''' 直接读取当前测量的压力数据，单位pa '''
  print(f"Pressure : {bmp388.get_pressure} Pa")

  ''' 读取海拔高度，单位m '''
  print(f"Altitude : {bmp388.get_altitude} m\n\n")

  time.sleep(0.5)
