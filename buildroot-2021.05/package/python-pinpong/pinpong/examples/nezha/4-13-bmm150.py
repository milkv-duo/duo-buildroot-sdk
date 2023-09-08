# -*- coding: utf-8 -*-
#Nezha
#实验效果：使用bmm150传感器读取地磁信息

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bmm150 import BMM150_I2C

Board("nezha").begin()

bmm150 =  BMM150_I2C(i2c_addr = 0x13)

while bmm150.ERROR == bmm150.sensor_init():
  print("sensor init error, please check connect")
  time.sleep(1)

'''
  设置传感器的执行模式
    opMode:
      POWERMODE_NORMAL  正常的获得地磁数据的模式
      POWERMODE_FORCED  单次测量，测量完成后，传感器恢复sleep mode
      POWERMODE_SLEEP   用户可以访问所有寄存器，不能测量地磁数据
      POWERMODE_SUSPEND 此时传感器cpu不工作，无法执行任何操作,用户只能访问控制寄存器 BMM150_REG_POWER_CONTROL的内容
'''
bmm150.set_operation_mode(bmm150.POWERMODE_NORMAL)

'''
  设置预置模式，使用户更简单的配置传感器来获取地磁数据
    presetMode:
      PRESETMODE_LOWPOWER       低功率模式,获取少量的数据 取均值
      PRESETMODE_REGULAR        普通模式,获取中量数据 取均值
      PRESETMODE_ENHANCED       增强模式,获取大量数据 取均值
      PRESETMODE_HIGHACCURACY   高精度模式,获取超大量数据 取均值
'''
bmm150.set_preset_mode(bmm150.PRESETMODE_HIGHACCURACY)

'''
  设置获取地磁数据的速率，速率越大获取越快(不加延时函数)
    rate:
      RATE_02HZ
      RATE_06HZ
      RATE_08HZ
      RATE_10HZ        #(default rate)
      RATE_15HZ
      RATE_20HZ
      RATE_25HZ
      RATE_30HZ
'''
bmm150.set_rate(bmm150.RATE_10HZ)

'''
  使能x y z 轴的测量，默认设置为使能不需要配置，禁止后xyz轴的地磁数据不准确
  如果想配置更多参数请参考readme文件
'''
bmm150.set_measurement_xyz()

while True:
  geomagnetic = bmm150.get_geomagnetic()
  print("mag x = %d ut"%geomagnetic[0])
  print("mag y = %d ut"%geomagnetic[1])
  print("mag z = %d ut"%geomagnetic[2])
  degree = bmm150.get_compass_degree()
  print("the angle between the pointing direction and north (counterclockwise) is: %.2f \n"%degree)
  time.sleep(0.1)

