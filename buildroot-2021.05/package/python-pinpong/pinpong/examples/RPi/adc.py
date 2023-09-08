# -*- coding: utf-8 -*-

#RPi TLC_10位ADC 实现模拟输入
#实验效果：树莓派读取模拟量
import time
from pinpong.board import Board
from pinpong.libs.TLC_10bit_adc import TLC #从libs导入TLC库

pin = 1 #A1
rpi = Board("RPi").begin()
adc_A1 = TLC(rpi,pin)

while True:
  print(adc_A1.analogRead())
  time.sleep(1)
