# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_id809 import ID809

ip = "192.168.1.116"
port = 8081

Board(ip, port)

fingerprint = ID809()

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
#   该参数仅eBreathing、fast_blink、SlowBlink模式下有效
    fingerprint.ctrl_led(fingerprint.breathing, fingerprint.blue, 0)
    print("Please press your finger")
    if fingerprint.collection_fingerprint(10) != fingerprint.error:
        fingerprint.ctrl_led(fingerprint.fast_blink, fingerprint.yellow, 3)
        print("Successful acquisition")
        print("Please loosen your fingers")
        while fingerprint.detect_finger():pass
        ret = fingerprint.search()   #将采集到的指纹与指定编号指纹对比,成功返回指纹编号(1-80)，失败返回0
        if ret != 0:
        #设置指纹灯环为绿色常亮
            fingerprint.ctrl_led(fingerprint.keeps_on, fingerprint.green, 0)
            print("Successful match,ID = {}".format(ret))
        else:
            print("Failed match")
    else:
        print("Acquisition failure")
    print("-----------------------------")
    time.sleep(1)
        