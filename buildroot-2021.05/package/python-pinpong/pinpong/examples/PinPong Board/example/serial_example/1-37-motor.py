# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_motor import MOTOR

Board("PinPong board").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong Board","COM36").begin()  #windows下指定端口初始化
#Board("PinPong Board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong Board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化


motor = MOTOR()
#motor.motor_run(motor.ALL, motor.CW, 255)      #motor.ALL 两个电机,CW 正转, 转速(0-255)
#motor.motor_run(motor.M1, motor.CCW, 255)      #motor.M1 电机M1,CCW 反转, 转速(0-255)
#motor.motor_run(motor.M1, motor.CCW, 255)      #motor.M1 电机M2,CCW 反转, 转速(0-255)
motor.motor_run(motor.ALL, motor.CW, 255)
time.sleep(5)
motor.motor_stop(motor.M2)                      #停止电机M2
motor.motor_stop(motor.M1)                      #停止电机M1