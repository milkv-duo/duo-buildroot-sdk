# -*- coding: utf-8 -*-

from threading import current_thread
import time
from pinpong.extension.globalvar import *

pin_speaker = 100                    #V2板载蜂鸣器引脚，兼容micropython

class MBMusic:
  DADADADUM = "DADADADUM"
  ENTERTAINER = "ENTERTAINER"
  PRELUDE = "PRELUDE"
  ODE = "ODE"
  NYAN = "NYAN"
  RINGTONE = "RINGTONE"
  FUNK = "FUNK"
  BLUES = "BLUES"
  BIRTHDAY = "BIRTHDAY"
  WEDDING = "WEDDING"
  FUNERAL = "FUNERAL"
  PUNCHLINE = "PUNCHLINE"
  BADDY = "BADDY"
  CHASE = "CHASE"
  BA_DING = "BA_DING"
  WAWAWAWAA = "WAWAWAWAA"
  JUMP_UP = "JUMP_UP"
  JUMP_DOWN = "JUMP_DOWN"
  POWER_UP = "POWER_UP"
  POWER_DOWN = "POWER_DOWN"

  sound = {
    "DADADADUM": 0,
    "ENTERTAINER": 1,
    "PRELUDE": 2,
    "ODE": 3,
    "NYAN": 4,
    "RINGTONE": 5,
    "FUNK": 6,
    "BLUES": 7,
    "BIRTHDAY": 8,
    "WEDDING": 9,
    "FUNERAL": 10,
    "PUNCHLINE": 11,
    "BADDY": 12,
    "CHASE": 13,
    "BA_DING": 14,
    "WAWAWAWAA": 15,
    "JUMP_UP": 16,
    "JUMP_DOWN": 17,
    "POWER_UP": 18,
    "POWER_DOWN": 19
  }
  music_map = {
    "C/C3":131,
    "D/D3":147,
    "E/E3":165,
    "F/F3":175,
    "G/G3":196,
    "A/A3":220,
    "B/B3":247,
    "C/C4":262,
    "D/D4":294,
    "E/E4":330,
    "F/F4":349,
    "G/G4":392,
    "A/A4":440,
    "B/B4":494,
    "C/C5":523,
    "D/D5":587,
    "E/E5":659,
    "F/F5":698,
    "G/G5":784,
    "A/A5":880,
    "B/B5":988,
    "C#/C#3":139,
    "D#/D#3":156,
    "F#/F#3":185,
    "G#/G#3":208,
    "A#/A#3":233,
    "C#/C#4":277,
    "D#/D#4":311,
    "F#/F#4":370,
    "G#/G#4":415,
    "A#/A#4":466,
    "C#/C#5":554,
    "D#/D#5":622,
    "F#/F#5":740,
    "G#/G#5":831,
    "A#/A#5":932
  }
  beats_map = {
    1/4: 1,
    1/2: 2,
    3/4: 3,
    1: 4,
    1.5: 6,
    2: 8,
    3: 12,
    4: 16
  }
  microbit_map = {
    1: 1,
    1/2: 2,
    1/4: 4,
    1/8: 8,
    1/16: 16,
    2: 32,
    4: 64
  }
  def __init__(self, board=None):
    self.first_flag = True
    self.speed = 120
    self.pin = 0
  
  def get_board(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]

  def play(self, music, pin, wait = False):
    self.get_board()
    self.pin = pin
    try:
      val = self.sound[music]
    except Exception:
      print("please input correct music")
    else:
      self.board.board.set_pin_mode_digital_output(pin)
      time.sleep(0.1)
      if not wait:
        self.board.board.microbit_play_music_background(pin, val, 0)
      else:
        self.board.board.microbit_play_music_background(pin, val, 1)
  
  def play_buzzer(self, pin, note, beat):
    self.get_board()
    self.pin = pin
    try:
      note = note.upper()
      _note = self.music_map[note]
      if self.microbit_map[beat]:
        _beat = self.microbit_map[beat]
      else:
        if (num > 0 and num <= 0.0625): _beat = 16
        elif (num > 0.0625 and num <= 0.125): _beat = 8
        elif (num > 0.125 and num <= 0.25): _beat = 4
        elif (num > 0.25 and num <= 0.5): _beat = 2
        elif (num > 0.5 and num <= 1): _beat = 1
        elif (num > 1 and num <= 2): _beat = 32
        elif (num > 2 and num <= 4): _beat = 64
        else: _beat = 1
    except Exception:
      print("please input correct param")
    else:
      self.board.board.microbit_play_buzzer_freq(pin, _note, _beat)
  
  def add_tempo(self, val):
    self.get_board()
    self.speed += val
    if self.speed < 4:
      self.speed = 4
    if self.speed > 400:
      self.speed = 400
    self.board.board.microbit_set_speed(self.speed)

  def set_tempo(self, val):
    self.get_board()
    self.speed = val
    if self.speed < 4:
      self.speed = 4
    if self.speed > 400:
      self.speed = 400
    self.board.board.microbit_set_speed(self.speed)
  
  def get_tempo(self):
    return self.speed

  def pitch(self, freq, duration):
    self.get_board()
    self.board.board.dfrobot_play_tone(self.pin, freq, duration)

class Image:
  HAPPY          = "0000001010000001000101110"
  SAD            = "0000001010000000111010001"
  ANGRY          = "1000101010000001111110101"
  ARROW_E        = "0010000010111110001000100"
  ARROW_N        = "0010001110101010010000100"
  ARROW_NE       = "0011100011001010100010000"
  ARROW_NW       = "1110011000101000001000001"
  ARROW_S        = "0010000100101010111000100"
  ARROW_SE       = "1000001000001010001100111"
  ARROW_SW       = "0000100010101001100011100"
  ARROW_W        = "0010001000111110100000100"
  ASLEEP         = "0000011011000000111000000"
  BUTTERFLY      = "1101111111001001111111011"
  CHESSBOARD     = "0101010101010101010101010"
  CLOCK1         = "0001000010001000000000000"
  CLOCK10        = "0000011000001000000000000"
  CLOCK11        = "0100001000001000000000000"
  CLOCK12        = "0010000100001000000000000"
  CLOCK2         = "0000000011001000000000000"
  CLOCK3         = "0000000000001110000000000"
  CLOCK4         = "0000000000001000001100000"
  CLOCK5         = "0000000000001000001000010"
  CLOCK6         = "0000000000001000010000100"
  CLOCK7         = "0000000000001000100001000"
  CLOCK8         = "0000000000001001100000000"
  CLOCK9         = "0000000000111000000000000"
  CONFUSED       = "0000001010000000101010101"
  COW            = "1000110001111110111000100"
  DIAMOND        = "0010001010100010101000100"
  DIAMOND_SMALL  = "0000000100010100010000000"
  DUCK           = "0110011100011110111000000"
  FABULOUS       = "1111111011000000101001110"
  GHOST          = "1111110101111111111110101"
  GIRAFFE        = "1100001000010000111001010"
  HEART          = "0101010101100010101000100"
  HEART_SMALL    = "0000001010011100010000000"
  HOUSE          = "0010001110111110111001010"
  MEH            = "0101000000000100010001000"
  MUSIC_CROTCHET = "0010000100001001110011100"
  MUSIC_QUAVER   = "0010000111001011110011100"
  MUSIC_QUAVERS  = "0111101001010011101111011"
  NO             = "1000101010001000101010001"
  PACMAN         = "0111111010111001111001111"
  PITCHFORK      = "1010110101111110010000100"
  RABBIT         = "1010010100111101101011110"
  ROLLERSKATE    = "0001100011111111111101010"
  SILLY          = "1000100000111110010100111"
  SKULL          = "0111010101111110111001110"
  SMILE          = "0000000000000001000101110"
  SNAKE          = "1100011011010100111000000"
  SQUARE         = "1111110001100011000111111"
  SQUARE_SMALL   = "0000001110010100111000000"
  STICKFIGURE    = "0010011111001000101010001"
  SURPRISED      = "0101000000001000101000100"
  SWORD          = "0010000100001000111000100"
  TARGET         = "0010001110110110111000100"
  TORTOISE       = "0000001110111110101000000"
  TRIANGLE       = "0000000100010101111100000"
  TRIANGLE_LEFT  = "1000011000101001001011111"
  TSHIRT         = "1101111111011100111001110"
  UMBRELLA       = "0111011111001001010001100"
  XMAS           = "0010001110001000111011111"
  YES            = "0000000001000101010001000"

  ALL_CLOCKS = [CLOCK12, CLOCK1, CLOCK2, CLOCK3, CLOCK4, CLOCK5, CLOCK6, CLOCK7, CLOCK8, CLOCK9, CLOCK10, CLOCK11, CLOCK12]

  def __init__(self, image):
    self.image = image

class MBScreen:
  def __init__(self, board=None):
    self.first_flag = True
    self.toclear = False
    self.mark = ""
    self.flag = False
  
  def get_board(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]

  def show(self, shape, loop=False, delay=1000):
    self.get_board()
    if isinstance(shape, str):
      if len(shape) == 1:
        if shape == self.mark and not self.toclear:
          return
        else:
          self.mark = shape
        self.scroll(shape)
      else:
        data = shape
        if data == self.mark and not self.toclear:
          return
        else:
          self.mark = data
        buf = []
        for i in range(0, 21, 5):
          buf.append(data[i:i+5])
        data = []
        for i in buf:
          data.append(int(i,2))
        self.toclear = False
        if self.flag:
          self.clear()
        self.board.board.microbit_show_shape(data)
    elif isinstance(shape, list):
      while loop:
        for i in shape:
          self.show(i)
          sleep(delay)
    else:
      mlist = shape.image.split(':')
      if len(shape.image) != 29 and len(mlist) != 5:
        print("Incorrect filling format")
        return
      else:
        if mlist == self.mark and not self.toclear:
          return
        else:
          self.mark = mlist
        val = ""
        for i in mlist:
          val += i
        self.toclear = False
        self.flag = True
        self.board.board.microbit_set_pixel_brightness(val)
  
  def scroll(self, data):
    self.get_board()
    data = str(data)
    if len(data) > 20:
      data = data[0:20]
    buf = []
    for i in data:
      buf.append(ord(i))
    buf.append(0)
    self.board.board.microbit_show_font(buf)
  
  def set_pixel(self, x, y):
    self.get_board()
    try:
      if x < 0 or y < 0 or x > 4 or y > 4:
        raise ValueError("Please input 0-4")
    except ValueError as e:
      print("Throw an exception:",repr(e))
    else:
      self.board.board.microbit_control_light_on(x, y, 1)
  
  def off_pixel(self, x, y):
    self.get_board()
    try:
      if x < 0 or y < 0 or x > 4 or y > 4:
        raise ValueError("Please input 0-4")
    except ValueError as e:
      print("Throw an exception:",repr(e))
    else:
      self.board.board.microbit_control_light_on(x, y, 0)

  def set_brightness(self, brightness):
    self.get_board()
    try:
      if brightness < 0 or brightness > 9 :
        raise ValueError("Please input 0-9")
    except ValueError as e:
      print("Throw an exception:",repr(e))
    else:
      if brightness == 0:
        val = 0
      elif brightness == 9:
        val = 255
      else:
        val = brightness * 28
      self.board.board.microbit_set_light_brightness(val)
  
  def clear(self):
    self.get_board()
    self.toclear = True
    self.board.board.microbit_hide_all_lights()

class MBSensor_buttonA:
  def __init__(self, board=None):
    self.first_flag = True
  
  def is_pressed(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    return self.board.board.microbit_buttonA_is_pressed()


class MBSensor_buttonB:
  def __init__(self, board=None):
    self.first_flag = True
  
  def is_pressed(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    return self.board.board.microbit_buttonB_is_pressed()

class MBSensor_buttonAB:
  def __init__(self, board=None):
    self.first_flag = True
  
  def is_pressed(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    return self.board.board.microbit_buttonAB_is_pressed()

class MBSensor_calibrate:
  def __init__(self, board=None):
    self.first_flag = True
  
  def calibrate(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    return self.board.board.microbit_cal_compass()
  
  def heading(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    return self.board.board.microbit_get_compass()

class MBSensor_pin0:
  def __init__(self, board=None):
    self.first_flag = True
  
  def is_touched(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    return self.board.board.microbit_touch0()

class MBSensor_pin1:
  def __init__(self, board=None):
    self.first_flag = True
  
  def is_touched(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    return self.board.board.microbit_touch1()

class MBSensor_pin2:
  def __init__(self, board=None):
    self.first_flag = True
  
  def is_touched(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    return self.board.board.microbit_touch2()

class MBSensor_acc:
  _gesture = {
    1 :'up',
    2 :'down',
    3 :'left',
    4 :'right',
    5 :'face up',
    6 :'face down',
    7 :'freefall',
    8 :'3g',
    9 :'6g',
    10:'8g',
    11:'shake' 
  }
  def __init__(self, board=None):
    self.first_flag = True
  
  def current_gesture(self):                   #兼容micropython的获取姿态current_gesture
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    val = self.board.board.microbit_get_gesture()
    if val:
      return self._gesture[val]
    else:
      return "Failure to recognize gesture"
  
  def is_gesture(self, gesture):
    val = self.current_gesture()
    if val == gesture:
      return True
    else:
      return False
  
  def get_x(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    return self.board.board.microbit_get_accelerometer_X()
  
  def get_y(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    return self.board.board.microbit_get_accelerometer_Y()
  
  def get_z(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    return self.board.board.microbit_get_accelerometer_Z()
  
  def get_strength(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    return self.board.board.microbit_get_accelerometer_strength()

class MBSensor_brightness:
  def __init__(self, board=None):
    self.first_flag = True
  
  def get_brightness(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    return self.board.board.microbit_get_brightness()

class MBSensor_microphone:
  def __init__(self, board=None):
    self.first_flag = True

  def sound_level(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("MICROBIT")["MICROBIT"]
      self.board.board.microbit_report_sensor()
    return self.board.board.microbit_get_microphone()

class MBWireless:
  def __init__(self, board=None):
    if board == None:
      board = get_globalvar_value("MICROBIT")["MICROBIT"]
    self.board = board
  
  def config(self, channel):
    self.board.board.microbit_set_wireless_channel(channel)
  
  def on(self):
    self.board.board.microbit_open_wireless(1)
  
  def off(self):
    self.board.board.microbit_open_wireless(0)
  
  def send(self, data):
    data = str(data)
    val = []
    for i in data:
      val.append(ord(i))
    self.board.board.microbit_send_wireless(val)
  
  def receive(self, callback = None):
    self.board.board.microbbit_recv_data(callback)

def temperature():
  board = get_globalvar_value("MICROBIT")["MICROBIT"]
  board.board.microbit_report_sensor()
  return board.board.microbit_get_temp()

def sleep(tim):
  time.sleep(tim / 1000)

button_a = MBSensor_buttonA()  #兼容micropython方法
button_b = MBSensor_buttonB()
button_ab = MBSensor_buttonAB()
compass = MBSensor_calibrate()
pin0 = MBSensor_pin0()
pin1 = MBSensor_pin1()
pin2 = MBSensor_pin2()
accelerometer = MBSensor_acc()
brightness = MBSensor_brightness()
microphone = MBSensor_microphone()
display = MBScreen()
music = MBMusic()
