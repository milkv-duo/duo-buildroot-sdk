# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_asr import DFRobot_ASR

Board().begin()

asr = DFRobot_ASR()

# 选择模式 LOOP(循环模式)/PASSWORD(指令模式)/BUTTON(按钮模式), 选择MIC(默认麦克风)/MONO(外部音频)
asr.begin(asr.LOOP, asr.MIC)
# 添加词条
asr.add_command("kai deng", 1)              # 开灯
asr.add_command("guan deng", 2)             # 关灯
asr.add_command("bei jing", 3)              # 北京
asr.add_command("shang hai", 4)             # 上海
asr.add_command("xiang gang", 5)            # 香港
# 添加词条完毕，开始识别
asr.start()
while True:
    result = asr.read()                    # 识别一次，获取编号
    print(result)
    time.sleep(1)
