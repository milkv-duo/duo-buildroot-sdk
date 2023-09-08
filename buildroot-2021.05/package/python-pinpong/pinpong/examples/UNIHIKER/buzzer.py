# -*- coding: utf-8 -*-

#实验效果：控制板载蜂鸣器播放音乐和控制L灯闪烁
#接线：使用UNIHIKER主板

import time
from pinpong.board import Board,Pin
from pinpong.extension.unihiker import *

Board().begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("UNIHIKER","COM36").begin()   #windows下指定端口初始化
#Board("UNIHIKER","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("UNIHIKER","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

led = Pin(Pin.P25, Pin.OUT) #引脚初始化为电平输出

#音乐 DADADADUM ENTERTAINER PRELUDE ODE NYAN RINGTONE FUNK BLUES BIRTHDAY WEDDING FUNERAL PUNCHLINE
#音乐 BADDY CHASE BA_DING WAWAWAWAA JUMP_UP JUMP_DOWN POWER_UP POWER_DOWN
#播放模式 Once(播放一次) Forever(一直播放) OnceInBackground(后台播放一次) ForeverInBackground(后台一直播放)
buzzer.play(buzzer.DADADADUM, buzzer.OnceInBackground) #后台播放音乐一次
# buzzer.set_tempo(4,60) #设置每一拍音符数，每分钟节拍数
# buzzer.pitch(494, 4) #播放音符
# buzzer.pitch(494) #后台播放音符
# time.sleep(10)
# buzzer.stop() #停止后台播放
# buzzer.redirect(Pin.P0) #引脚重定向，只支持PWM引脚 
# buzzer.play(buzzer.ENTERTAINER, buzzer.ForeverInBackground) #后台永久播放音乐
while True:
  led.value(1) #输出高电平
  print("1") #终端打印信息
  time.sleep(0.3) #等待1秒 保持状态

  led.value(0) #输出低电平
  print("0") #终端打印信息
  time.sleep(0.3) #等待1秒 保持状态