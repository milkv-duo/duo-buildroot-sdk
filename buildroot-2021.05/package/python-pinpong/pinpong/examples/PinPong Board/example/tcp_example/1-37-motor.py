# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_motor import MOTOR

ip = "192.168.1.116"
port = 8081

Board(ip, port)

motor = MOTOR()
#motor.motor_run(motor.ALL, motor.CW, 255)      #motor.ALL 两个电机,CW 正转, 转速(0-255)
#motor.motor_run(motor.M1, motor.CCW, 255)      #motor.M1 电机M1,CCW 反转, 转速(0-255)
#motor.motor_run(motor.M1, motor.CCW, 255)      #motor.M1 电机M2,CCW 反转, 转速(0-255)
motor.motor_run(motor.ALL, motor.CW, 255)
time.sleep(5)
motor.motor_stop(motor.M2)                      #停止电机M2
motor.motor_stop(motor.M1)                      #停止电机M1