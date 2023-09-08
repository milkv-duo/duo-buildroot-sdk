# -*- coding: utf-8 -*-

#实验效果：读取音频分析
import time
from pinpong.board import Board,Pin,AUDIO

strobe_Pin = Pin.A0
rst_Pin = Pin.D5
analog_Pin = Pin.A5

Board("xugu").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别


audio = AUDIO(strobe_Pin, rst_Pin, analog_Pin)

while True:
  freq_val = audio.read_freq()   #Frequency(Hz):63  160  400  1K  2.5K  6.25K  16K
  for i in range(7):             #FreqVal[]:      0    1    2    3    4    5    6  
    if i < 6:
      print(freq_val[i], end = ", ")
    else:
      print(freq_val[i])
  time.sleep(1)








































