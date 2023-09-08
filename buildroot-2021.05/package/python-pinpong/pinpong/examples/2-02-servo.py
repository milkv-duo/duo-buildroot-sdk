# -*- coding: utf-8 -*-

#实验效果：舵机控制
#接线：使用windows或linux电脑连接一块arduino主控板，D4连接一个舵机
import time
from pinpong.board import Board,Pin,Servo

Board("uno").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

s2 = Servo(Pin(Pin.D4)) #将Pin传入Servo中初始化舵机引脚
s1 = Servo(Pin(Pin.D5)) #将Pin传入Servo中初始化舵机引脚

while True:
  s1.angle(0) #控制舵机转到0度位置
  print("S1 0")
  time.sleep(1)

  s2.angle(0) #控制舵机转到90度位置
  print("S2 0")
  time.sleep(1)

  s1.angle(180) #控制舵机转到180度位置
  print("S1 180")
  time.sleep(1)

  s2.angle(180) #控制舵机转到90度位置
  print("S2 180")
  time.sleep(1)
