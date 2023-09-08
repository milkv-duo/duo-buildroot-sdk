# -*- coding: utf-8 -*-

#实验效果：控制蜂鸣器发声
#接线：使用windows或linux电脑连接一块arduino主控板，主控板D12接一个蜂鸣器模块
import time
from pinpong.board import Board,Pin,Tone

Board("xugu").begin()

tone = Tone(Pin(Pin.D12)) #将Pin传入Tone中实现模拟输出
tone.freq(200) #按照设置的频率播放


while True:
  tone.tone(200,500)  #按照设置的频率和时间播放直到完成
  time.sleep(1)


while True:
  print("freq=",tone.freq()) #读取频率并打印
  tone.on()  #打开蜂鸣器
  time.sleep(1) 
  tone.off() #关闭蜂鸣器
  time.sleep(1)
  tone.freq(tone.freq()+100) #按照设置的频率播放
