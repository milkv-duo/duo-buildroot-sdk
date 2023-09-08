# -*- coding: utf-8 -*-

# 实验效果：驱动maqueen plus
# 接线：使用maqueen plus连接一块MICROBIT或者掌控主控板
import time
from pinpong.board import Board, IRRecv, Pin
from pinpong.libs.dfrobot_maqueen_plus import DFRobot_MqueenPlus

Board("handpy").begin()  # 初始化，选择板型和端口号，不输入端口号则进行自动识别
# Board("handpy","COM36").begin()  #windows下指定端口初始化
# Board("handpy","/dev/ttyACM0").begin()   #linux下指定端口初始化
# Board("handpy","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

maqueen = DFRobot_MqueenPlus()
# 打开/关闭PID, ON/OFF
# maqueen.write_pid(maqueen.ON)
# 电机LEFT, RIGHT, ALL, CW代表前进，CCW代表后退，200是速度，范围0-255
# maqueen.run(maqueen.ALL, maqueen.CCW, 255)
# 停止电机LEFT,RIGHT,ALL
# maqueen.stop(maqueen.ALL)
# 读取电机速度 LEFT/RIGHT/ALL
# print(maqueen.read_speed(maqueen.RIGHT))
# 读取电机速度 (0:停止 1:前进 2:后退)
# print(maqueen.read_direction(maqueen.RIGHT))
# 舵机S1/S2/S3/S1_3, 转动角度(0-180)
# maqueen.servo(maqueen.S1_3, 90)
# 设置RGB灯LEFT/RIGHT/ALL, 颜色WHITH/RED/GREEN/BLUE/YELLOW/CYAN/PINK/PUT
# maqueen.set_rgb(maqueen.ALL, maqueen.RED)
# 读取巡线传感器L1/L2/L3/R1/R2/R3
# print(maqueen.read_patrol(maqueen.L2))
# 读取巡线传感器灰度值L1/L2/L3/R1/R2/R3
# print(maqueen.read_patrol_voltage(maqueen.L2))
# 读取红外数据
# def ir_recv3(data):
#     print("------P16--------")
#     print(hex(data))

# ir3 = IRRecv(Pin(16), ir_recv3)
# 读取版本号
# print(maqueen.read_version())
# 读取车轮圈数 LEFT,RIGHT
# print(maqueen.read_distance(maqueen.RIGHT))
# 清除车轮圈数 LEFT,RIGHT
# maqueen.clear_distance(maqueen.RIGHT)
while True:
    time.sleep(1)
