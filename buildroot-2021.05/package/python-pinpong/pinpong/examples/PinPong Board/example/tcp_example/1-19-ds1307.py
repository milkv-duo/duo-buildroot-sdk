# -*- coding: utf-8 -*-

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_ds1307 import DS1307

ip = "192.168.0.15"
port = 8081

Board(ip, port)

rtc = DS1307()

TIME_YR = 20                       #TIME_YR表示年，默认+2000
TIME_MTH = 12                       #TIME_MTH表示月
TIME_DATE = 4                     #TIME_DATE表示日
TIME_DOW = 5                       #TIME_DOW表示星期几
TIME_HR = 13                       #TIME_HR表示时
TIME_MIN = 56                      #TIME_MIN表示分
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

