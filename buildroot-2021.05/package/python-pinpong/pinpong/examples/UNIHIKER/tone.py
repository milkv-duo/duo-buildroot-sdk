# -*- coding: utf-8 -*-

#实验效果：控制蜂鸣器发声
#接线：使用电脑连接一块UNIHIKER主控板，主控板P0接一个蜂鸣器模块
import time
from pinpong.board import Board,Pin,Tone

Board("UNIHIKER").begin()  #初始化，选择板型，不输入板型则进行自动识别

tone = Tone(Pin(Pin.P2)) #将Pin传入Tone中实现模拟输出

tone.freq(200) #按照设置的频率播放

while True:
  print("freq=",tone.freq()) #读取频率并打印
  tone.on()  #打开蜂鸣器
  time.sleep(1) 
  tone.off() #关闭蜂鸣器
  time.sleep(1)
  tone.freq(tone.freq()+100) #按照设置的频率播放

'''
while True:
  tone.tone(200,500)  #按照设置的频率和时间播放直到完成
  time.sleep(1)
'''
