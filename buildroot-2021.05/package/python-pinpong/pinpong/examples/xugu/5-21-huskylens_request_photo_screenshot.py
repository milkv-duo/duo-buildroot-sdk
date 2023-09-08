# -*- coding: utf-8 -*-

#实验效果：触发拍照/截图保存至SD卡
#接线：使用windows或linux电脑连接一块arduino主控板，哈士奇接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens

Board("xugu").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别

husky = Huskylens()

husky.command_request_photo() #保存照片
time.sleep(2)


husky.command_request_screenshot() #保存截图
time.sleep(2)




