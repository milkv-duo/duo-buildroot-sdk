# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.extension.microbit import *

Board("microbit").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("microbit","COM36").begin()   #windows下指定端口初始化
#Board("microbit","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("microbit","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

#compass.calibrate()                                     #校准电子罗盘

while True:
  print(button_a.is_pressed())                            #按键A是否按下
#  print(button_b.is_pressed())                            #按键B是否按下
#  print(button_ab.is_pressed())                           #按键AB是否按下
#  print(pin0.is_touched())                                #接口0是否被接通
#  print(pin1.is_touched())                                #接口1是否被接通
#  print(pin2.is_touched())                                #接口2是否被接通
#  print(accelerometer.current_gesture())                  #获取当前姿态,返回字符串
#  print(brightness.get_brightness())                      #读取环境光
#  print(compass.heading())                                #读取指南针
#  print(temperature())                                    #获取温度
#  print(accelerometer.get_x())                            #读取加速度X的值
#  print(accelerometer.get_y())                            #读取加速度Y的值
#  print(accelerometer.get_z())                            #读取加速度Z的值
#  print(accelerometer.get_strength())                     #读取加速度的强度
#  print(microphone.sound_level())                         #读取声音强度(V2)
  print("------------------")
  time.sleep(0.2)
