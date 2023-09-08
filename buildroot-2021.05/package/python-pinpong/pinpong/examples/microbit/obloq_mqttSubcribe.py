# -*- coding: utf-8 -*-

#实验效果：I2C obloq
#接线：使用windows或linux电脑连接一块microbit主控板，obloq接到I2C口SCL及SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_obloq import Obloq #导入Obloq库

Board("microbit").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("microbit","COM36").begin()  #windows下指定端口初始化
#Board("microbit","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("microbit","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

SERVER = "iot.dfrobot.com.cn"
PORT = "1883"
SSID = "dfrobotOffice"
PASSWORD = "dfrobot2011"
IOT_ID = "F2BvTZ8Mg"
IOT_PWD = "KhfDoWUMRz"
IOT_TOPIC1 = "Ae0xHqXMR"
IOT_TOPIC2 = "zYIfLxCGg"
IOT_TOPIC = [IOT_TOPIC1,  IOT_TOPIC2]

olq=Obloq()

def event(e,param):
    if e == IOT_TOPIC1:
        print("receive %s message: %s"%(e,param))
    elif e == IOT_TOPIC2:
        print("receive %s message: %s"%(e,param))

olq.mqtt_connect(SSID, PASSWORD, IOT_ID, IOT_PWD, IOT_TOPIC, SERVER, PORT)
olq.mqtt_handle(event)

while True:
    time.sleep(1)
