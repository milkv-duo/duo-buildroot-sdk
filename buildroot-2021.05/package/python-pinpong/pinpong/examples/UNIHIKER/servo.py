# -*- coding: utf-8 -*-

#实验效果：舵机控制
#接线：使用电脑连接一块UNIHIKER主控板，P10连接一个舵机
import time
from pinpong.board import Board,Pin,Servo

Board("UNIHIKER").begin()  #初始化，选择板型，不输入板型则进行自动识别

s1 = Servo(Pin(Pin.P10)) #将Pin传入Servo中初始化舵机引脚,支持P0 P2 P3 P8 P9 P10 P16  P21 P22 P23

while True:
  s1.angle(0) #控制舵机转到0度位置
  print("0")
  time.sleep(1)

  s1.angle(90) #控制舵机转到90度位置
  print("90")
  time.sleep(1)

  s1.angle(180) #控制舵机转到180度位置
  print("180")
  time.sleep(1)

  s1.angle(90) #控制舵机转到90度位置
  print("90")
  time.sleep(1)
