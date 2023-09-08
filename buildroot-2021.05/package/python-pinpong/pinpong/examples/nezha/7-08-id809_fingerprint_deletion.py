# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_id809 import ID809

Board("nezha").begin()#初始化


fingerprint = ID809(bus_num=2)

while fingerprint.connected() == False:
    print("Communication with device failed, please check connection")
    time.sleep(1)

while True:
#   ctrl_led参数1:<LEDMode>
#   breathing   fast_blink   keeps_on    normal_close
#   fade_in      fade_out     slow_blink   
#   ctrl_led参数2:<LEDColor>
#   green  red      yellow   blue
#   cyan   magenta  white
#   ctrl_led参数3:<呼吸、闪烁次数> 0表示一直呼吸、闪烁，
#   该参数仅breathing、fast_blink、slow_blink模式下有效
    fingerprint.ctrl_led(fingerprint.breathing, fingerprint.blue, 0)
    print("请按下需要删除指纹的手指")
    if fingerprint.collection_fingerprint(10) != fingerprint.error:
        ret = fingerprint.search()
        if ret != 0:
            fingerprint.del_fingerprint(ret)   #删除要删除得ID
#            fingerprint.del_fingerprint(fingerprint.DELALL)   #删除全部ID
            print("删除成功,ID = {}".format(ret))
        else:
            print("指纹未注册")
    else:
        print("采集失败")
        fingerprint.ctrl_led(fingerprint.keeps_on, fingerprint.red, 0)
    print("请松开手指")
    while(fingerprint.detect_finger()):pass
    print("-------------------------")
    time.sleep(1)