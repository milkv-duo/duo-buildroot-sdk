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
    print("Please press the finger to remove the fingerprint")
    if fingerprint.collection_fingerprint(10) != fingerprint.error:
        ret = fingerprint.search()
        if ret != 0:
            fingerprint.del_fingerprint(ret)   #删除要删除得ID
#            fingerprint.del_fingerprint(fingerprint.DELALL)   #删除全部ID
            print("Delete successfully,ID = {}".format(ret))
        else:
            print("Fingerprints not registered")
    else:
        print("Acquisition failure")
        fingerprint.ctrl_led(fingerprint.keeps_on, fingerprint.red, 0)
    print("Please loosen your fingers")
    while(fingerprint.detect_finger()):pass
    print("-------------------------")
    time.sleep(1)