# -*- coding: utf-8 -*-

#实验效果：串口音乐模块（DFR0534）
#接线：使用UNIHIKER主控板,连接P0(RX)和P3(TX)
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_mp3 import MP3

Board("UNIHIKER").begin()  #初始化，选择板型，不输入板型则进行自动识别

mp3 = MP3()

mp3.playList(1)       #播放第几首歌曲
#mp3.pause()          #暂停播放
#mp3.play()           #播放
#mp3.next()           #播放下一首歌曲
#mp3.prev()           #播放上一首歌曲
#mp3.end()            #结束播放
#mp3.volume_up()      #音量加1
#mp3.volume_down()    #音量减1
#mp3.volume()    #音量设置
#time.sleep(6)
#mp3.query_play_status()    #查询播放状态

