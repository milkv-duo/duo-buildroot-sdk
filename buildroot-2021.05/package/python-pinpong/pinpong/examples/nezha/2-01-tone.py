# -*- coding: utf-8 -*-

#实验效果：控制蜂鸣器发声

import time
from pinpong.board import Board,Pin,Tone

Board("nezha").begin()  #初始化


tone = Tone(Pin(3)) #将Pin传入Tone中实现模拟输出



while True:
  tone.freq(200) #按照设置的频率播放
  while True:
    print("freq=",tone.freq()) #读取频率并打印
    tone.on()  #打开蜂鸣器
    time.sleep(1) 
    tone.off() #关闭蜂鸣器
    time.sleep(1)
    if tone.freq() == 1200:
      break
    tone.freq(tone.freq()+100) #按照设置的频率播放
    
'''
while True:
  tone.tone(200,500)  #按照设置的频率和时间播放直到完成
  time.sleep(1)
'''