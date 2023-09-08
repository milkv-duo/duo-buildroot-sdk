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

IP="192.168.0.189"
PORT="8080"
SSID="dfrobotOffice"
PASSWORD="dfrobot2011"

olq=Obloq()

olq.http_connect(SSID, PASSWORD, IP, PORT)

print(olq.ip_address())

while True:
    val = olq.get("input?id=1&val=30.2", 2000)
    print(val)
    #val = olq.post("input?name=admin", "{\"id\":\"1\",\"val\":\""+str(30.2)+"\"}", 2000)
    #print(val)
    time.sleep(1)