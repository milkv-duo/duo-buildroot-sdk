# -*- coding: utf-8 -*-

#RPi and PythonBoard
#实验效果：舵机控制

import time
from pinpong.board import Board,Pin,Servo

Board("RPi").begin()
s1 = Servo(Pin(21)) #将Pin传入Servo中初始化舵机引脚

while True:
  s1.write_angle(0) #控制舵机转到0度位置
  print("0")
  time.sleep(1)

  s1.write_angle(90) #控制舵机转到90度位置
  print("90")
  time.sleep(1)

  s1.write_angle(180) #控制舵机转到180度位置
  print("180")
  time.sleep(1)

  s1.write_angle(90) #控制舵机转到90度位置
  print("90")
  time.sleep(1)