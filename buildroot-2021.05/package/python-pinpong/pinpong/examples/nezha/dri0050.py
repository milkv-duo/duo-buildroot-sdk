# -*- coding: utf-8 -*-

#Nezha
#驱动Modbus PWM Driver 驱动板，输出不同频率和占空比的PWM波形

'''
PWM频率高于2K，频率和设定值可能存在较大偏差
频率高于2K请参考使用如下的频率值
46875HZ, 23437HZ, 15625HZ, 11718HZ,
9375HZ, 7812HZ, 6696HZ, 5859HZ, 5208HZ, 4687HZ, 4261HZ,
3906HZ, 3605HZ, 3348HZ, 3125HZ,
2929HZ, 2757HZ, 2604HZ, 2467HZ, 2343HZ, 2232HZ, 2130HZ, 2038HZ,
'''

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_dri0050 import DRI0050 #从libs中导入 DFI0050 库

Board("nezha").begin()

pwmd = DRI0050(uart="/dev/ttyUSB0")

print("version=0x{:x}, addr=0x{:x}".format(pwmd.get_version(), pwmd.get_addr()))
print("pid=0x{:x}, vid=0x{:x}".format(pwmd.get_vid(), pwmd.get_pid()))

while True:
  print("\n--------初始值------") 
  print("freq={}, duty={:.2f} enable={}".format(pwmd.get_freq(), pwmd.get_duty(), pwmd.get_enable()))  

  print("--------设置新值------")  
  #pwmd.pwm(freq=860,duty=0.82) # freq(183HZ-46875HZ) duty(0%-100%)
  pwmd.set_freq(860) #(183HZ-46875HZ)
  pwmd.set_duty(0.82)#(0%-100%)
  pwmd.set_enable(1)
  print("freq={}, duty={:.2f} enable={}".format(pwmd.get_freq(), pwmd.get_duty(), pwmd.get_enable()))

  print("--------恢复出厂设置(366HZ, 占空比50%, 禁止输出)-------\n")
  pwmd.pwm(freq=366, duty=0.5) # freq(183HZ-46875HZ) duty(0%-100%)
  pwmd.set_enable(0)
  time.sleep(5)
