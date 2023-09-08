# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.extension.handpy import *

Board("handpy").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("handpy","COM36").begin()   #windows下指定端口初始化
#Board("handpy","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("handpy","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

while True:
  print(button_a.value())                    #按键A是否按下
  print(button_b.value())                    #按键B是否按下
  print(button_ab.value())                   #按键AB是否按下
#  print(touchPad_P.is_touched())                               #是否触摸P
#  print(touchPad_Y.is_touched())                               #是否触摸Y
#  print(touchPad_T.is_touched())                               #是否触摸T
#  print(touchPad_H.is_touched())                               #是否触摸H
#  print(touchPad_O.is_touched())                               #是否触摸O
#  print(touchPad_N.is_touched())                               #是否触摸N
#  touch_threshold("all",60)                  #设置按键P/Y/T/H/O/N的触摸阈值，all代表全部
#  print(touchPad_P.read())                          #读取按键P的触摸值
#  print(touchPad_Y.read())                          #读取按键Y的触摸值
#  print(touchPad_T.read())                          #读取按键T的触摸值
#  print(touchPad_H.read())                          #读取按键H的触摸值
#  print(touchPad_O.read())                          #读取按键O的触摸值
#  print(touchPad_N.read())                          #读取按键N的触摸值
#  print(sound.read())                            #读取麦克风强度
#  print(light.read())                            #读取环境光强度
#  print(accelerometer.get_x())                    #读取加速度X的值
#  print(accelerometer.get_y())                    #读取加速度Y的值
#  print(accelerometer.get_z())                    #读取加速度Z的值
#  print(accelerometer.get_strength())             #读取加速度的强度
  print("------------------")
  time.sleep(0.4)
