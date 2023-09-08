# -*- coding: utf-8 -*-

#PCA_9685 16路PWM控制
#实验效果：pwm控制
import time
from pinpong.board import Board
from pinpong.libs.pca_9685 import PCA9685#从libs导入PCA9685库


channel = 0 #channel0 ~ channel 15
rpi = Board("RPi").begin()
pwm = PCA9685(rpi,channel,bus_num=1)

while True:
  #pwm.set_pwm(on, off)  
  #pwm.set_pwm(4096,0)#fully on 
  #time.sleep(1)
  #pwm.set_pwm(0,4096)#fully off
  
  #time.sleep(1)
  #pwm.angle(45)#45 degree
  #time.sleep(1)
  #pwm.angle(135)#135 degree
  
  time.sleep(1)
  for i in range (255):
    pwm.bright(i)
  time.sleep(1)
