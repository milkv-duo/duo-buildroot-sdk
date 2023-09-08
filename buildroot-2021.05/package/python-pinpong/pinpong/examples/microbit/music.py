# -*- coding: utf-8 -*-

import time
from pinpong.board import Board,Pin
from pinpong.extension.microbit import *

Board("microbit").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("microbit","COM36").begin()   #windows下指定端口初始化
#Board("microbit","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("microbit","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

#接口播放音乐：DADADADUM、ENTERTAINER、PRELUDE、ODE、NYAN、RINGTONE、FUNK、BLUES、BIRTHDAY、WEDDING、FUNERAL、PUNCHLINE、BADDY
#CHASE、BA_DING、WAWAWAWAA、JUMP_UP、JUMP_DOWN、POWER_UP、POWER_DOWN
music.play(music.ENTERTAINER, Pin.P0, wait = False)               #后台播放音乐
#music.play(music.DADADADUM, Pin.P0, wait = True)                 #播放音乐直到结束
#music.play(music.ENTERTAINER, pin = pin_speaker, wait = False)   #microbitV2板载蜂鸣器播放音乐
#music.set_tempo(220)                                             #设置声音速度为x
#print(music.get_tempo())                                         #获取声音速度
#for freq in range(880, 1760, 16):
#    music.pitch(freq, 6)                                         #设置频率响多少时间(ms)
