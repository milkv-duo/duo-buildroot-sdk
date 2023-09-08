# -*- coding: utf-8 -*-

#RPi and PythonBoard
#实验效果：控制蜂鸣器发声

import time
from pinpong.board import Board,Pin,Tone

Board("RPi").begin()
tone = Tone(Pin(21)) #将Pin传入Tone中实现模拟输出
tone.freq(0) #按照设置的频率播放

while True:
  print("freq=",tone.freq()) #读取频率并打印
  tone.on()  #打开蜂鸣器
  time.sleep(1) 
  tone.off() #关闭蜂鸣器
  time.sleep(1)
  tone.freq(tone.freq()+100) #按照设置的频率播放

