# -*- coding: utf-8 -*-

#实验效果：读取超声波
#接线：使用windows或linux电脑连接一块arduino主控板，使用SR04或URM10超声波，Trig接D7，Echo接D8
import time
from pinpong.board import Board,Pin,SR04_URM10

TRIGER_PIN = Pin.D6
ECHO_PIN = Pin.D7

Board("PinPong Board").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong Board","COM36").begin()   #windows下指定端口初始化
#Board("PinPong Board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong Board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

sonar = SR04_URM10(Pin(TRIGER_PIN), Pin(ECHO_PIN))

while True:
  dis = sonar.distance_cm() #获取距离，单位厘米(cm)
  print("distance = %d cm"%dis)
  time.sleep(0.1)
