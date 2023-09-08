# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_mpu6050 import MPU6050

ip = "192.168.1.106"
port = 8081

Board(ip, port)

sensor = MPU6050()

while True:
    print(sensor.acc(sensor.X))
    print(sensor.acc(sensor.Y))
    print(sensor.acc(sensor.Z))
    print(sensor.gyro(sensor.X))
    print(sensor.gyro(sensor.Y))
    print(sensor.gyro(sensor.Z))
    print("--------------")
    time.sleep(0.3)