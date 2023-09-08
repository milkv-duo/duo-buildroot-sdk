# -*- coding: utf-8 -*-

#NeZha
#实验效果：自定义ID名字

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens

Board("nezha").begin()

husky = Huskylens()



#husky.command_request_knock()
#       Header	Header 2	Address	  Data Length	Command	 Checksum
#return 0x55	0xAA	    0x11	      0x00	    0x2E	 0x3E

husky.command_request_customnames(1,"TEST") 

time.sleep(2)


