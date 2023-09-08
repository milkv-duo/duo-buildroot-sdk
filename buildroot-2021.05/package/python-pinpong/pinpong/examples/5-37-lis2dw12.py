# -*- coding:utf-8 -*-
"""
   @file get_acceleration.py
   @brief Get the acceleration in x, y, z directions,测量的量程为±2g,±4g,±8g或±16g,通过set_range()函数设置
   @n 本示例默认地选用连续测量模式测量数据，加速度数据会根据测量速率不停地测量，
   @n 还可使用单次按需求转换模式  1.需要在set_power_mode()函数中选用适合的转换模式
   @n                             2.然后在set_data_rate()函数填入SETSWTRIG参数
   @n                             3.使用demandData()函数请求测量一次数据
"""

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_lis2dw12 import LIS2DW12_I2C

Board("nezha").begin()


acce = LIS2DW12_I2C()

#Chip initialization
acce.begin()
#Get chip id
print('chip id :%x'%acce.get_id())
#Software reset to restore the value of all registers
acce.soft_reset()
'''
  Set the sensor measurement range:
              RANGE_2G     #±2g
              RANGE_4G     #±4g
              RANGE_8G     #±8g
              RANGE_16G    #±16g
'''
acce.set_range(acce.RANGE_2G)
#Choose whether to continuously let the chip collect data
acce.contin_refresh(True)

'''
    Set the sensor data collection rate:
        RATE_OFF            #测量关闭
        RATE_1HZ6           #1.6hz,仅在低功耗模式下使用
        RATE_12HZ5          #12.5hz
        RATE_25HZ           
        RATE_50HZ           
        RATE_100HZ          
        RATE_200HZ          
        RATE_400HZ          #仅在High-Performance mode下使用
        RATE_800HZ          #仅在High-Performance mode下使用
        RATE_1600HZ         #仅在High-Performance mode下使用
        SETSWTRIG           #软件触发单次测量
'''
acce.set_data_rate(acce.RATE_200HZ)

'''
  Filter settings:
      LPF       #Low pass filter
      HPF       #High pass filter
'''
acce.set_filter_path(acce.LPF)

'''
   Set bandwidth
        RATE_DIV_2   #RATE/2 (up to RATE = 800 Hz, 400 Hz when RATE = 1600 Hz)
        RATE_DIV_4   #RATE/4 (High Power/Low power)
        RATE_DIV_10  #RATE/10 (HP/LP)
        RATE_DIV_20  # RATE/20 (HP/LP)
'''
acce.set_filter_bandwidth(acce.RATE_DIV_4)

'''
   Set power mode:
     HIGH_PERFORMANCE_14BIT           #High-Performance Mode
     CONT_LOWPWR4_14BIT               #Continuous measurement,Low-Power Mode 4(14-bit resolution)
     CONT_LOWPWR3_14BIT               #Continuous measurement,Low-Power Mode 3(14-bit resolution)
     CONT_LOWPWR2_14BIT               #Continuous measurement,Low-Power Mode 2(14-bit resolution)
     CONT_LOWPWR1_12BIT               #Continuous measurement,Low-Power Mode 1(12-bit resolution)
     SING_LELOWPWR4_14BIT             #Single data conversion on demand mode,Low-Power Mode 4(14-bit resolution)
     SING_LELOWPWR3_14BIT             #Single data conversion on demand mode,Low-Power Mode 3(14-bit resolution
     SING_LELOWPWR2_14BIT             #Single data conversion on demand mode,Low-Power Mode 2(14-bit resolution)
     SING_LELOWPWR1_12BIT             #Single data conversion on demand mode,Low-Power Mode 1(12-bit resolution)
     HIGHP_ERFORMANCELOW_NOISE_14BIT  #High-Performance Mode,Low-noise enabled
     CONT_LOWPWRLOWNOISE4_14BIT       #Continuous measurement,Low-Power Mode 4(14-bit resolution,Low-noise enabled)
     CONT_LOWPWRLOWNOISE3_14BIT       #Continuous measurement,Low-Power Mode 3(14-bit resolution,Low-noise enabled)
     CONT_LOWPWRLOWNOISE2_14BIT       #Continuous measurement,Low-Power Mode 2(14-bit resolution,Low-noise enabled)
     CONT_LOWPWRLOWNOISE1_12BIT       #Continuous measurement,Low-Power Mode 1(14-bit resolution,Low-noise enabled)
     SINGLE_LOWPWRLOWNOISE4_14BIT     #Single data conversion on demand mode,Low-Power Mode 4(14-bit resolution),Low-noise enabled
     SINGLE_LOWPWRLOWNOISE3_14BIT     #Single data conversion on demand mode,Low-Power Mode 3(14-bit resolution),Low-noise enabled
     SINGLE_LOWPWRLOWNOISE2_14BIT     #Single data conversion on demand mode,Low-Power Mode 2(14-bit resolution),Low-noise enabled
     SINGLE_LOWPWRLOWNOISE1_12BIT     #Single data conversion on demand mode,Low-Power Mode 1(12-bit resolution),Low-noise enabled
'''
acce.set_power_mode(acce.CONT_LOWPWRLOWNOISE2_14BIT);
time.sleep(0.1)

while True:
    #在单次按需求转化模式下,请求测量一次数据
    #acce.demand_data()
    #Get the acceleration in the three directions of xyz
    #测量的量程为±2g,±4g,±8g或±16g,通过set_range()函数设置
    time.sleep(0.3)
    acc = acce.read_acc()
    #y = acce.read_acc()
    #z = acce.read_acc()
    print("Acceleration [X = %.2d mg,Y = %.2d mg,Z = %.2d mg]"%(acc.x,acc.y,acc.z))
