# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_speech_synthesis import *

Board().begin()

sstts = DFRobot_SpeechSynthesis_I2C()

sstts.begin(sstts.V1)            #supports V1 and V2

# sstts.set_voice(5)             #Set volume(0-9)
# sstts.set_speed(5)             #Set playback speed (0-9)
# sstts.set_tone(5)              #Set tone(0-9)
# sstts.set_sound_type(sstts.FEMALE)   #Set voice type/FEMALE/MALE/DONALDDUCK(only V1)
# sstts.set_english_pron(sstts.WORD)   #Set word synthesis mode /WORD/ALPHABET
# sstts.set_digital_pron(sstts.NUMBER) #Set set read number NUMBER/NUMERIC/AUTOJUDGED

while True:
    sstts.speak("520")
    sstts.speak("i have one and three books")   #Too long sentences are not supported
    sstts.speak("Hello world")
    sstts.speak("我叫小明，高二三班的学生")
    time.sleep(2)