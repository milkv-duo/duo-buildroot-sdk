# -*- coding: utf-8 -*-

# 实验效果：驱动两用扩展板电机
# 接线：使用两用扩展板连接一块MICROBIT或者掌控主控板
import time
from pinpong.board import Board, Pin
from pinpong.libs.dfrobot_dualuse_shield import DFRobot_DualuseShield

Board("microbit").begin()  # 初始化，选择板型和端口号，不输入端口号则进行自动识别
# Board("microbit","COM36").begin()  #windows下指定端口初始化
# Board("microbit","/dev/ttyACM0").begin()   #linux下指定端口初始化
# Board("microbit","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

ds = DFRobot_DualuseShield()
# 电机M1,M2,ALL, CW代表正转，CCW代表反转，200是速度，范围0-255
ds.run(ds.M1, ds.CW, 255)
time.sleep(2)
# 停止电机M1,M2,ALL
ds.stop(ds.M1)
while True:
    pass
