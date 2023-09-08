# -*- coding: utf-8 -*-

import time
import math
from numpy import mat
from pinpong.extension.globalvar import *
from pinpong.board import Pin


class GD32Sensor_buttonA:
    def __init__(self, board=None):
        self.first_flag = True

    def is_pressed(self):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
            self.board.board._report_sensor()
        return self.board.board.GD32V_buttonA_is_pressed()

    def irq(self, trigger=None, handler=None):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
            self.board.board._report_sensor()
        self.Pin = Pin(Pin.P27)
        self.board.board.GD32V_buttonA_irq(trigger, handler, self.Pin)

    def value(self):
        return 1 if self.is_pressed() else 0


class GD32Sensor_buttonB:
    def __init__(self, board=None):
        self.first_flag = True

    def is_pressed(self):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
            self.board.board._report_sensor()
        return self.board.board.GD32V_buttonB_is_pressed()

    def irq(self, trigger=None, handler=None):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
            self.board.board._report_sensor()
        self.Pin = Pin(Pin.P28)
        return self.board.board.GD32V_buttonB_irq(trigger, handler, self.Pin)

    def value(self):
        return 1 if self.is_pressed() else 0


class GD32Sensor_light:
    def __init__(self, board=None):
        self.first_flag = True

    def read(self):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
            self.board.board._report_sensor()
        return self.board.board.GD32V_read_light()


class GD32Sensor_acc:
    def __init__(self, board=None):
        self.first_flag = True

    def get_x(self):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
            self.board.board._report_sensor()
        return self.board.board.GD32V_get_accelerometer_X()

    def get_y(self):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
            self.board.board._report_sensor()
        return self.board.board.GD32V_get_accelerometer_Y()

    def get_z(self):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
            self.board.board._report_sensor()
        return self.board.board.GD32V_get_accelerometer_Z()

    def get_strength(self):
        return round(math.sqrt(math.pow(self.get_x() ,2) + math.pow(self.get_y() ,2) + math.pow(self.get_z() ,2)), 2 )

class GD32Sensor_gyro:
    def __init__(self, board=None):
        self.first_flag = True

    def get_x(self):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
            self.board.board._report_sensor()
        return self.board.board.GD32V_get_Macceleration_X()

    def get_y(self):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
            self.board.board._report_sensor()
        return self.board.board.GD32V_get_Macceleration_Y()

    def get_z(self):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
            self.board.board._report_sensor()
        return self.board.board.GD32V_get_Macceleration_Z()


class GD32_buzz:
    DADADADUM = 0
    ENTERTAINER = 1
    PRELUDE = 2
    ODE = 3
    NYAN = 4
    RINGTONE = 5
    FUNK = 6
    BLUES = 7
    BIRTHDAY = 8
    WEDDING = 9
    FUNERAL = 10
    PUNCHLINE = 11
    BADDY = 12
    CHASE = 13
    BA_DING = 14
    WAWAWAWAA = 15
    JUMP_UP = 16
    JUMP_DOWN = 17
    POWER_UP = 18
    POWER_DOWN = 19

    Once = 1
    Forever = 2
    OnceInBackground = 4
    ForeverInBackground = 8

    BEAT_1 = 4
    BEAT_1_2 = 2
    BEAT_1_4 = 1
    BEAT_3_4 = 3
    BEAT_3_2 = 6
    BEAT_2 = 8
    BEAT_3 = 12
    BEAT_4 = 16
    music_map = {
      "C3": 131,
      "D3": 147,
      "E3": 165,
      "F3": 175,
      "G3": 196,
      "A3": 220,
      "B3": 247,
      "C4": 262,
      "D4": 294,
      "E4": 330,
      "F4": 349,
      "G4": 392,
      "A4": 440,
      "B4": 494,
      "C5": 523,
      "D5": 587,
      "E5": 659,
      "F5": 698,
      "G5": 784,
      "A5": 880,
      "B5": 988,
      "C#3": 139,
      "D#3": 156,
      "F#3": 185,
      "G#3": 208,
      "A#3": 233,
      "C#4": 277,
      "D#4": 311,
      "F#4": 370,
      "G#4": 415,
      "A#4": 466,
      "C#5": 554,
      "D#5": 622,
      "F#5": 740,
      "G#5": 831,
      "A#5": 932
       }

    def __init__(self, board=None):
        self.first_flag = True

    def play(self, index, options):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
        self.board.board.GD32V_play_buzz(index, options)
        if options == self.Once or options == self.Forever:
            while not self.board.board.GD32V_get_state():
                pass

    def pitch(self, _freq, beat=None):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
        if not isinstance(_freq, str) and not isinstance(_freq, list) and beat == None:
            self.board.board.GD32V_play_note(_freq)
        else:
            if isinstance(_freq, list):
                for i in _freq:
                    note = i.upper()
                    info = note.split(':')
                    _note = self.music_map[info[0]]
                    _beat = int(info[1])
                    print(_note)
                    print(_beat)
                    self.board.board.GD32V_play_note(_note, _beat)
                    out = time.time()
                    while not self.board.board.GD32V_get_state():
                      if int(time.time()-out)>0.31:
                        break
                      pass
            elif isinstance(_freq, str):
                note = _freq.upper()
                info = note.split(':')
                _note = self.music_map[info[0]]
                _beat = int(info[1])
                self.board.board.GD32V_play_note(_note, _beat)
                while not self.board.board.GD32V_get_state():
                    pass
            else:
                self.board.board.GD32V_play_note(_freq, beat)
                while not self.board.board.GD32V_get_state():
                    pass

    def set_tempo(self, ticks, bpm):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
        self.board.board.GD32V_set_ticks_tempo(ticks, bpm)

    def stop(self):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
        self.board.board.GD32V_buzz_stop()

    def redirect(self, pin):
        if self.first_flag:
            self.first_flag = False
            self.board = get_globalvar_value("UNIHIKER")["UNIHIKER"]
        self.board.board.GD32V_buzz_redirect(pin)

button_a = GD32Sensor_buttonA()  # 兼容micropython方法
button_b = GD32Sensor_buttonB()
light = GD32Sensor_light()
accelerometer = GD32Sensor_acc()
gyroscope = GD32Sensor_gyro()
buzzer = GD32_buzz()
