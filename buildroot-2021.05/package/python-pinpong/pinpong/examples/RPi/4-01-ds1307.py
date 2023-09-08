# -*- coding: utf-8 -*-

#实验效果：读取I2C DS1307实时时钟

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_ds1307 import DS1307

Board("RPI").begin()#初始化

rtc = DS1307(bus_num=1) #根据支持的i2c设备填写

TIME_YR = 22                       #TIME_YR表示年，默认+2000
TIME_MTH = 12                     #TIME_MTH表示月
TIME_DATE = 8                    #TIME_DATE表示日
TIME_DOW = 4                       #TIME_DOW表示星期几
TIME_HR = 17                       #TIME_HR表示时
TIME_MIN = 25                      #TIME_MIN表示分
TIME_SEC = 10                      #TIME_SEC表示秒

def update():
  data = [TIME_SEC,TIME_MIN,TIME_HR,TIME_DOW,TIME_DATE,TIME_MTH,TIME_YR]
  rtc.stop()
  rtc.set_time(data)
  rtc.start()

def set_square_signal():
  rtc.set_output(rtc.HIGH)

update()                          #更新时间函数
#set_square_signal()               #SQW/OUT引脚设置高电平、低电平、方波输出功能

while True:
  now = rtc.get_time()
  print("%s-%s-%s,%s:%s:%s"%(now.year,now.month,now.day,now.hours,now.minute,now.second))
  print(now.week)
  print("*****************")
  time.sleep(1)

