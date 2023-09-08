# -*- coding: utf-8 -*-

#实验效果：心率传感器读取心率
import time
from pinpong.board import Board,Pin#,HEARTRATE
from pinpong.libs.dfrobot_heartrate import HEARTRATE

Board("uno").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

DIGITAL_MODE = 1
ANALOG_MODE = 0
heart_rate = HEARTRATE(ANALOG_MODE, Pin.A1)    # ANALOG_MODE or DIGITAL_MODE 方式
# heart_rate = HEARTRATE(DIGITAL_MODE, Pin.A1)    # ANALOG_MODE or DIGITAL_MODE 方式

while True:
  heart_rate.get_value()
  rate_value = heart_rate.get_rate()
  if rate_value:
    print(rate_value)
  time.sleep(0.02)


















