# -*- coding: utf-8 -*-

#实验效果：在哈士奇的屏幕上显示文字，清屏
#接线：使用windows或linux电脑连接一块arduino主控板，哈士奇接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens

Board("xugu").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别

husky = Huskylens()

husky.command_request_custom_text("hiiii",25,52) #20字符限制

time.sleep(2)


husky.command_request_clear_text() #清屏
time.sleep(2)

