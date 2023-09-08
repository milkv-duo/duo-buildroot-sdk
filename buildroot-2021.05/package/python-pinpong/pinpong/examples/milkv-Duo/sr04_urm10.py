# -*- coding: utf-8 -*-

#实验效果：读取超声波
#接线：使用电脑连接一块UNIHIKER主控板，使用SR04或URM10超声波，Trig接P0，Echo接P1
import time
from pinpong.board import Board,Pin,SR04_URM10

TRIGER_PIN = Pin.P0
ECHO_PIN = Pin.P1

Board("UNIHIKER").begin()  #初始化，选择板型，不输入板型则进行自动识别

sonar = SR04_URM10(Pin(TRIGER_PIN), Pin(ECHO_PIN))

while True:
  dis = sonar.distance_cm() #获取距离，单位厘米(cm)
  print("distance = %d cm"%dis)
  time.sleep(0.1)
