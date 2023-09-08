# -*- coding: utf-8 -*-

#实验效果：自定义ID名字
#接线：使用windows或linux电脑连接一块arduino主控板，哈士奇接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens

ip = "192.168.0.90"
port = 8081

Board(ip, port)

husky = Huskylens()



#husky.command_request_knock()
#       Header	Header 2	Address	  Data Length	Command	 Checksum
#return 0x55	0xAA	    0x11	      0x00	    0x2E	 0x3E

husky.command_request_customnames(1,"TEST") 

time.sleep(2)


