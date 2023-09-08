# -*- coding: utf-8 -*-

#实验效果：读取I2C DS1307实时时钟
#接线：使用windows或linux电脑连接一块arduino主控板，实时时钟接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_ds1307 import DS1307

Board("PinPong Board").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong Board","COM36").begin()   #windows下指定端口初始化
#Board("PinPong Board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong Board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

rtc = DS1307()

TIME_YR = 20                       #TIME_YR表示年，默认+2000
TIME_MTH = 12                       #TIME_MTH表示月
TIME_DATE = 7                     #TIME_DATE表示日
TIME_DOW = 1                       #TIME_DOW表示星期几
TIME_HR = 15                       #TIME_HR表示时
TIME_MIN = 24                      #TIME_MIN表示分
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

