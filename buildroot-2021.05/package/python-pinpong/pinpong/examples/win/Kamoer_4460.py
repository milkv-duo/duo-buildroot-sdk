# -*- coding: utf_8 -*-

import time
from pinpong.board import Board
from pinpong.libs.kamoer_4460 import STEP_DRIVER_4460 #从libs中导入 STEP_DRIVER_4460 步进电机驱动库

Board("win").begin()

d = STEP_DRIVER_4460(port="com20")
v = 0
while True:
  for i in range(100):
    d.set_speed(i,d.CW)
    time.sleep(0.2)
    print("set_speed() = ", i)

  for i in range(100):
    d.set_speed(100-i,d.CW)
    time.sleep(0.2)
    print("set_speed() = ", 100-i)

  d.set_speed(0,d.CW)
  time.sleep(10)

    
  for i in range(100):
    d.set_speed(i,d.CCW)
    time.sleep(0.2)
    print("set_speed() = ", i)

  for i in range(100):
    d.set_speed(100-i,d.CCW)
    time.sleep(0.2)
    print("set_speed() = ", 100-i)

  d.set_speed(0,d.CCW)
  time.sleep(10)

while True:
  v = d.get_status()
  print("get_status() = ", v)
  
  v = d.get_error()
  print("get_error() = ", v)
  
  v = d.get_version()
  print("get_version() = ", v)
  
  v = d.get_max_speed()
  print("get_max_speed() = ", v)
  
  v = d.get_motor_dir()
  print("get_motor_dir() = ", v)
  
  v = d.get_motor_pos()
  print("get_motor_pos() = ", v)

  v = d.set_origin()
  print("set_origin() = ", v)

  v = d.set_speed(20,d.CW)
  print("set_speed() = ", v)
  
  v = d.get_speed()
  print("get_speed() = ", v)

  #v = d.set_steps(1000,d.CW)
  #print("set_steps() === ", v)
  
  time.sleep(2)
  v = d.set_speed(40,d.CW)
  v = d.get_speed()
  print("get_speed() = ", v)
  time.sleep(2)
  v = d.set_speed(0,d.CW)
  v = d.get_speed()
  print("get_speed() = ", v)

  time.sleep(2)
  v = d.set_speed(80,d.CW)
  v = d.get_speed()
  print("get_speed() = ", v)

  time.sleep(2)
  v = d.set_speed(100,d.CW)
  v = d.get_speed()
  print("get_speed() = ", v)

  print("set_speed() = ", v)
  time.sleep(2)
  v = d.get_speed()
  print("get_speed() = ", v)
  v = d.stop()
  print("stop() === ", v)
  
  time.sleep(3)