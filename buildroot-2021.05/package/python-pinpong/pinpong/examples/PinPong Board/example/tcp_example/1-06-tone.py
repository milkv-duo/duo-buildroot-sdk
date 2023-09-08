# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin,Tone

ip = "192.168.1.116"
port = 8081

Board(ip, port)

tone = Tone(Pin(Pin.D5))

tone.freq(200) #按照设置的频率播放

while True:
  print("freq=",tone.freq()) #读取频率并打印
  tone.on()  #打开蜂鸣器
  time.sleep(1) 
  tone.off() #关闭蜂鸣器
  time.sleep(1)
  tone.freq(tone.freq()+100) #按照设置的频率播放

#while True:
#  tone.tone(200,500)  #按照设置的频率和时间播放直到完成
#  time.sleep(1)