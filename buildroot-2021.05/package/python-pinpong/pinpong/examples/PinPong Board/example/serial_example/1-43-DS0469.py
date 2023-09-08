# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_ds0469 import DS0469

Board("PinPong board").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong board","COM36").begin()  #windows下指定端口初始化
#Board("PinPong board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

rtc = DS0469()

#rtc.adjust_aotu()                                  #自动调整时间
#rtc.adjust_rtc(2021,1,4,1,13,30,0)                 #设置时间，年月日星期时分秒

while True:
    rtc.read()
    print("{}-{}-{} {}:{}:{}".format(rtc.year,rtc.month,rtc.day,rtc.hour,rtc.minute,rtc.second))
    print(rtc.week)
    time.sleep(1)