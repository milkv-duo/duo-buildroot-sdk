# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_id809 import ID809

Board("xugu").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("xugu","COM36").begin()  #windows下指定端口初始化
#Board("xugu","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("xugu","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

fingerprint = ID809()

while fingerprint.connected() == False:
    print("Communication with device failed, please check connection")
    time.sleep(1)

while True:
    print("-------------------------")
#    fingerprint.set_deviceID(1)         #设置模块ID, 不设置默认为1, 可设置0-255
    print("The module ID is: {}".format(fingerprint.get_deviceID()))          #获取模块ID
#    fingerprint.set_security_level(3)     #设置安全等级, 默认为3, 可设置1-5
    print("The module security level is: {}".format(fingerprint.get_security_level()))    #获取安全等级
##    9600bps    19200bps   38400bps   57600bps   115200bps
##      1           2          3          4          5
    print("The baud rate of the module is: {}".format(fingerprint.get_baudrate()))    #获取模块波特,IIC波特率只能为115200
#    fingerprint.set_self_learn(1)        #设置自学功能, 1(ON) or 0(OFF)
    print("Module self-learning function: {}".format("ON" if fingerprint.get_self_learn() else "OFF"))    #获取模块自学功能知否开启
#    fingerprint.set_moduleSN("DFRobot")            #设置模块序列号
    print("The module sequence number is: {}".format(fingerprint.get_moduleSN()))   #读取模块序列号
    print("The number of registered fingerprints inside the module: {}".format(fingerprint.get_enroll_count()))          #读取模块内已注册的指纹数量
    info = fingerprint.get_enrolledID_list()
    print("A list of registered user ids:", info)
    print("Number of fingerprint damage: {}".format(fingerprint.get_broken_quantity()))           #获取指纹损坏数量
#    fingerprint.get_brokenID()                                                    #获取第一个损坏的ID