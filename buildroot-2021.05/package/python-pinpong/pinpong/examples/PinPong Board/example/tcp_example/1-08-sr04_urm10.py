# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin,SR04_URM10

ip = "192.168.0.15"
port = 8081

Board(ip, port)

TRIGER_PIN = Pin.D6
ECHO_PIN = Pin.D7

sonar = SR04_URM10(Pin(TRIGER_PIN), Pin(ECHO_PIN))

while True:
  dis = sonar.distance_cm() #获取距离，单位厘米(cm)
  print("distance = %d cm"%dis)
  time.sleep(0.1)
