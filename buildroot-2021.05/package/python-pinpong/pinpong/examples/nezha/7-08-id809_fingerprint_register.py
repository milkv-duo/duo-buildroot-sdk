# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_id809 import ID809

Board("nezha").begin()#初始化


fingerprint = ID809(bus_num=2)

COLLECT_NUMBER = 3

while fingerprint.connected() == False:
    print("Communication with device failed, please check connection")
    time.sleep(1)

while True:
    print("-----------------------------")
    ID = fingerprint.get_emptyID()                                       #获取一个空ID
    print("未注册编号,ID={}".format(ID))

#   ctrl_led参数1:<LEDMode>
#   breathing   fast_blink   keeps_on    normal_close
#   fade_in      fade_out     slow_blink   
#   ctrl_led参数2:<LEDColor>
#   green  red      yellow   blue
#   cyan   magenta  white
#   ctrl_led参数3:<呼吸、闪烁次数> 0表示一直呼吸、闪烁，
#   该参数仅breathing、fast_blink、slow_blink模式下有效
    i = 0
    while i < COLLECT_NUMBER:
        fingerprint.ctrl_led(fingerprint.breathing, fingerprint.blue, 0)
        print("正在进行第{}次指纹采集".format(i+1))
        print("请按下手指")
#采集指纹图像，超过10S没按下手指则采集超时，如果timeout=0，关闭采集超时功能,如果获取成功返回0，否则返回ERR_ID809
        if fingerprint.collection_fingerprint(10) != fingerprint.error:      
            fingerprint.ctrl_led(fingerprint.fast_blink, fingerprint.yellow, 3) #设置指纹灯环为黄色快闪3次
            print("采集成功")
            i += 1
        else:
            print("采集失败")
        print("请松开手指")
        while fingerprint.detect_finger():pass
    if fingerprint.store_fingerprint(ID) != fingerprint.error:      #将指纹信息保存到一个未注册的编号中
        print("保存成功，ID={}".format(ID))
        fingerprint.ctrl_led(fingerprint.keeps_on, fingerprint.green, 0)  #设置指纹灯环为绿色常亮
        time.sleep(1)
        fingerprint.ctrl_led(fingerprint.normal_close, fingerprint.blue, 0)#关闭指纹灯环
        time.sleep(1)
    else:
        print("保存失败")
    
    