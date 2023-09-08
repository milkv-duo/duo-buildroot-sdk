# -*- coding: utf-8 -*-

'''
激光测距仪,测量距离：4-400cm,测量精度：2cm
'''
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_sen0492 import Laser_Ranging

Board("win").begin()
laser = Laser_Ranging(port="/dev/ttyUSB0")

#输出状态字典
input_dir={
    0x00:"Sensor Range Valid",
    0x01:"Sensor Sigma Fail",
    0x02:"Sensor Signal Fail",
    0x03:"Sensor Min Range Fail",
    0x04:"Sensor Phase Fail",
    0x05:"Sensor Hardware Fail",
    0x07:"Sensor No Update"
}

'''
设置地址（默认地址为0x50）
可写入0x00~0xFE
调用此函数设置地址后需重新上电,并在构造函数重新写入地址
'''
#laser.set_addr(0x50)

'''
设置波特率
波特率可以设置为以下值（默认波特率115200）：
2400,  4800,  9600, 19200,
38400, 57600, 115200,
230400,  460800, 921600
调用此函数设置波特率后需重新上电,并在构造函数重新写入波特率
'''
#laser.set_baudrate(115200)

'''
设置测量模式，测量模式有三种短、中、长
也可以不用设置测量模式运行
'''
#laser.set_meature_mode(0x01)#短距离模式，最多1.3m，更好的环境免疫力）
#laser.set_meature_mode(0x02)#中距离 （最多3米）
#laser.set_meature_mode(0x03)#长距离模式 （最多4米）


#laser.recovery_sys()#将从机恢复默认值

'''
设置报警阈值,阈值设置范围40~4000mm,单位：毫米（mm）
'''
#laser.set_threshold_mm(1000) 

while True:
    if laser.get_input_status() in input_dir:
        print("Sensor Status:{}".format(input_dir[laser.get_input_status()]))
    print("distance={}mm".format(laser.get_distance_mm())) #读取距离数据，单位毫米（mm）
    time.sleep(1.0)
