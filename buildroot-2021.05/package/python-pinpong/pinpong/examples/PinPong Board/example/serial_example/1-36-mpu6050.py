# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_mpu6050 import MPU6050

Board("PinPong Board").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong Board","COM36").begin()   #windows下指定端口初始化
#Board("PinPong Board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong Board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

sensor = MPU6050()

while True:
    print(sensor.acc(sensor.X))
    print(sensor.acc(sensor.Y))
    print(sensor.acc(sensor.Z))
    print(sensor.gyro(sensor.X))
    print(sensor.gyro(sensor.Y))
    print(sensor.gyro(sensor.Z))
    print("--------------")
    time.sleep(0.5)