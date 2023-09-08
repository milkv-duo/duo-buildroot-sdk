# -*- coding: utf-8 -*-

import time
from pinpong.extension.globalvar import *
try:
    from PIL import Image        #ESP32有显示图片功能
except Exception:
    pass
    
class HPScreen:
  def __init__(self, board=None):
    self.first_flag = True
    self.buffer = []

  def get_board(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      
  def DispChar(self, data, x=None, y=None):
    self.get_board()
    self.buffer.append((0,data,x,y))

  def show(self):
    self.get_board()
    for i in self.buffer:
      if i[0] == 0:
        self._play(i[1], i[2], i[3])
      elif i[0] == 1:
        self._Bitmap(i[1], i[2], i[3], i[4], i[5])
      elif i[0] == 2:
        self._clean(i[1])
      elif i[0] == 3:
        self._fill(i[1])
      elif i[0] == 4:
        self._rotation(i[1])
      elif i[0] == 5:
        self._pixel(i[1], i[2])
      elif i[0] == 6:
        self._line(i[1],i[2],i[3],i[4])
      elif i[0] == 7:
        self._circle(i[1], i[2], i[3])
      elif i[0] == 8:
        self._fill_circle(i[1], i[2], i[3])
      elif i[0] == 9:
        self._rect(i[1], i[2], i[3], i[4])
      elif i[0] == 10:
        self._fill_rect(i[1], i[2], i[3], i[4])
    self.buffer = []

  def _play(self, data, x = None, y = None):  #兼容屏幕显示在第几行和显示在x, y处
    self.get_board()
    if y== None:
      try:
        if x < 1 or x > 4:
          raise ValueError("line must be a number greater than 0 and less than or equal to 4")
      except ValueError as e:
        print("Throw an exception:",repr(e))
      else:
        buf = []
        text = str(data)
        text = text.encode(encoding='UTF-8',errors='strict')
        for i in text:
          buf.append(i)
        length = len(buf)
        try:
          if length > 500:
            raise ValueError("Please enter less than 500")
        except ValueError as e:
          print("Throw an exception:",repr(e))
        else:
          self.board.board.handpy_display_in_line(buf, x, length)
    else:
      try:
        if x < 0 or x > 127 or y < 0 or y > 63:
          raise ValueError("Please input x:0-127 y:0-63")
      except ValueError as e:
        print("Throw an exception:",repr(e))
      else:
        buf = []
        text = str(data)
        text = text.encode(encoding='UTF-8',errors='strict')
        for i in text:
          buf.append(i)
        length = len(buf)
        try:
          if length > 500:
            raise ValueError("Please input less than 500")
        except ValueError as e:
          print("Throw an exception:",repr(e))
        else:
          self.board.board.handpy_display_in_XY(x, y, buf, length)

  def clean(self, line):
    self.get_board()
    self.buffer.append((2, line))
  
  def _clean(self, line):
    self.board.board.handpy_display_clear_in_line(line)
  
  def fill(self, color = 0):                 #名字兼容micropython 模式
    self.get_board()
    self.buffer.append((3, color))

  def _fill(self, color = 0):                 #名字兼容micropython 模式
    try:
      if color != 0 and color != 1:
        raise ValueError("Please input 0 or 1")
    except ValueError as e:
      print("Throw an exception:",repr(e))
    else:
      self.board.board.handpy_fill_screen(color)
   
  def rotation(self, angle = 0):
    self.get_board()
    self.buffer.append((4, angle))
  
  def _rotation(self, angle = 0):
    try:
      if angle != 0 and angle != 180:
        raise ValueError("Please input 0 or 180")
    except ValueError as e:
      print("Throw an exception:",repr(e))
    else:
      angle = 2 if angle == 180 else 0
      self.board.board.handpy_screen_rotation(angle)
  
  def pixel(self, x, y):
    self.get_board()
    self.buffer.append((5, x, y))

  def _pixel(self, x, y):
    try:
      if x < 0 or x > 127 or y < 0 or y > 63:
        raise ValueError("Please input x:0-127 y:0-63")
    except ValueError as e:
      print("Throw an exception:",repr(e))
    else:
      self.board.board.handpy_point_drawing(x, y)

  def set_line_width(self, lineW):
    self.get_board()
    try:
      if lineW < 1 or lineW > 128:
        raise ValueError("Please input x:0-127 y:0-63")
    except ValueError as e:
      print("Throw an exception:",repr(e))
    else:
      self.board.board.handpy_set_line_width(lineW)
  
  def line(self, x1, y1, x2, y2, c=None):
    self.get_board()
    self.buffer.append((6, x1, y1, x2, y2))

  def _line(self, x1, y1, x2, y2, c=None):
    try:
      if x1 < 0 or x1 > 127 or y1 < 0 or y1 > 63 or x2 < 0 or x2 > 127 or y2 < 0 or y2 > 63:
        raise ValueError("Please input x:0-127 y:0-63")
    except ValueError as e:
      print("Throw an exception:",repr(e))
    else:
      self.board.board.handpy_line_drawing(x1, y1, x2, y2)
  
  def circle(self, x, y, r):
    self.get_board()
    self.buffer.append((7, x, y, r))

  def _circle(self, x, y, r):
    self.board.board.handpy_circle_drawing(x, y, 0 ,r)

  def fill_circle(self, x, y, r):
    self.get_board()
    self.buffer.append((8, x, y, r))
  
  def _fill_circle(self, x, y, r):
    self.board.board.handpy_circle_drawing_fill(x, y, 1 ,r)

  def rect(self, x, y, width, height, c=None):
    self.get_board()
    self.buffer.append((9, x, y, width, height, c))
  
  def _rect(self, x, y, width, height, c=None):
    self.board.board.handpy_rectangle_drawing(x, y, width, height, 0)

  def fill_rect(self, x, y, width, height, c=None):
    self.get_board()
    self.buffer.append((10, x, y, width, height, c))
  
  def _fill_rect(self, x, y, width, height, c=None):
    self.board.board.handpy_rectangle_drawing_fill(x, y, width, height, 1)
  
  def Bitmap(self, x, y, width, height, path):
    self.get_board()
    self.buffer.append((1, x, y, width, height, path))
  
  def _Bitmap(self, x, y, width, height, path):
    try:
      if width < 0 or height < 0  or x < 0 or y < 0 or height > 64 or y >= 64 or width > 128 or x >= 128:
        raise ValueError("Please input width, x:0-127 height, y:0-63")
    except ValueError as e:
        print("Throw an exception:",repr(e))
    else:
      img_src = Image.open(path)
      im = img_src.resize((width,height))
      img_src = im.convert('L')
      aa = img_src.getdata()
      sequ0 = list(aa)
      data = []
      threshold = 130
      for i in range(height):
        j = 0; k = 0; l=0
        for j in range(width//8):
          byte = 0
          for k in range(8):
            byte <<= 1
            bit = sequ0[i*width+j*8+k]
            byte |= (1 if bit>threshold else 0)
          data .append(byte)
        byte = 0
        for l in range(width%8):
          byte <<= 1
          bit = sequ0[i*width+j*8+k + l]
          byte |= (1 if bit>threshold else 0)
        for m in range(7-l):
          byte <<= 1
        if width%8:
          data.append(byte)
      width_bytes = len(data) // height
      max_height = 128 // width_bytes
      for ii in range(height // max_height):
        time.sleep(0.1)
        self.board.board.handpy_display_image(x, y+max_height*ii, width, max_height, data[max_height * width_bytes * ii : max_height * width_bytes*(ii+1)], max_height * width_bytes)
      time.sleep(0.1)
      if height % max_height:
        self.board.board.handpy_display_image(x, y+max_height * (height // max_height), width, height % max_height, data[len(data) -((height % max_height) * width_bytes) : len(data)], (height % max_height) * width_bytes)
      time.sleep(0.1)
      self.board.board.handpy_oled12864_show(x, y, width, height)

class RGB:
  def __init__(self, board=None):
    self.first_flag = True
    self.__data = [(0,0,0), (0,0,0), (0,0,0)]

  def get_board(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]

  def __getitem__(self, i):
    self.get_board()
    return self.__data[i]  # 返回data绑定列表中的第i个元素
 
  def __setitem__(self, i, v):
    self.get_board()
    self.__data[i]=v

  def write(self):
    self.get_board()
    for i in range(3):
      index = i
      r = self.__data[i]
      if isinstance(r,tuple):
        b=r[2]
        g=r[1]
        r=r[0]
      try:
        if index < -1 or index > 2 or r < 0 or r > 255 or g < 0 or g > 255 or b < 0 or b > 255:
          raise ValueError("Please input rgb(0-255)")
      except ValueError as e:
        print("Throw an exception:",repr(e))
      else:
        color = r << 16 | g << 8 | b
        self.board.board.handpy_set_lights_color(index, color)

  def fill(self, v):
    self.get_board()
    self.linght = 9
    for i in range(3):
      self.__data[i] = v

  def disable(self, index):
    self.get_board()
    try:
      if index < -1 or index > 2:
        raise ValueError("Please input (-1,0,1,2)")
    except ValueError as e:
      print("Throw an exception:",repr(e))
    else:
      if index == -1:
        index = 3
      self.board.board.handpy_rgb_disable(index, 0)
  
  def brightness(self, value = None):
    self.get_board()
    if value != None:
      try:
        if value < 0 or value > 9:
          raise ValueError("Please input 0-9")
      except ValueError as e:
        print("Throw an exception:",repr(e))
      else:
        self.linght = value
        self.board.board.handpy_set_brightness(value)
    else:
      return self.linght
    
class Music:
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
    "C3":131,
    "D3":147,
    "E3":165,
    "F3":175,
    "G3":196,
    "A3":220,
    "B3":247,
    "C4":262,
    "D4":294,
    "E4":330,
    "F4":349,
    "G4":392,
    "A4":440,
    "B4":494,
    "C5":523,
    "D5":587,
    "E5":659,
    "F5":698,
    "G5":784,
    "A5":880,
    "B5":988,
    "C#3":139,
    "D#3":156,
    "F#3":185,
    "G#3":208,
    "A#3":233,
    "C#4":277,
    "D#4":311,
    "F#4":370,
    "G#4":415,
    "A#4":466,
    "C#5":554,
    "D#5":622,
    "F#5":740,
    "G#5":831,
    "A#5":932
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
  def __init__(self, board=None):
    self.first_flag = True
    self.speed = 120
  
  def set_tempo(self, ticks, tempo):
    self.get_board()
    self.board.board.handpy_music_set_tempo(0x7f, ticks, tempo)
  
  def play(self, note, beat = 1):
    self.get_board()
    if isinstance(note, list):
      for i in note:
        note = i.upper()
        info = note.split(':')
        _note = self.music_map[info[0]]
        _beat = int(info[1])
        self.board.board.handpy_set_buzzer_freq(0x7f, _note, int(_beat/2))
    else:    
      note = note.upper()
      info = note.split(':')
      _note = self.music_map[info[0]]
      _beat = int(info[1])
      self.board.board.handpy_set_buzzer_freq(0x7f, _note, 0)
  
  def stop(self):
    self.get_board()
    self.board.board.handpy_set_buzzer_freq(0x7f, 0, 0)

  def stop_background_play(self):
    self.get_board()
    self.board.board.handpy_stop_background_buzzer_freq()

  def get_board(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board.handpy_set_buzzer_freq(0x7f, 0, 2)
      time.sleep(1)

class MBSensor_buttonA:
  def __init__(self, board=None):
    self.first_flag = True
  
  def value(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_buttonA_is_pressed()
    
class MBSensor_buttonB:
  def __init__(self, board=None):
    self.first_flag = True
  
  def value(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_buttonB_is_pressed()

class MBSensor_buttonAB:
  def __init__(self, board=None):
    self.first_flag = True
  
  def value(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_buttonAB_is_pressed()

class MBSensor_touchPad_P:
  def __init__(self, board=None):
    self.first_flag = True
  
  def is_touched(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_touchP()
  
  def read(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_read_touch_P()
    
class MBSensor_touchPad_Y:
  def __init__(self, board=None):
    self.first_flag = True
  
  def is_touched(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_touchY()
    
  def read(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_read_touch_Y()

class MBSensor_touchPad_T:
  def __init__(self, board=None):
    self.first_flag = True
  
  def is_touched(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_touchT()
    
  def read(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_read_touch_T()

class MBSensor_touchPad_H:
  def __init__(self, board=None):
    self.first_flag = True
  
  def is_touched(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_touchH()
  
  def read(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_read_touch_H()
  
class MBSensor_touchPad_O:
  def __init__(self, board=None):
    self.first_flag = True
  
  def is_touched(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_touchO()
    
  def read(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_read_touch_O()
    
class MBSensor_touchPad_N:
  def __init__(self, board=None):
    self.first_flag = True
  
  def is_touched(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_touchN()
  
  def read(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_read_touch_N()
    
class MBSensor_sound:
  def __init__(self, board=None):
    self.first_flag = True
  
  def read(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_read_sound()
    
class MBSensor_light:
  def __init__(self, board=None):
    self.first_flag = True
  
  def read(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_read_light()
    
class MBSensor_acc:
  def __init__(self, board=None):
    self.first_flag = True
  
  def get_x(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_get_accelerometer_X()
  
  def get_y(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_get_accelerometer_Y()
  
  def get_z(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return self.board.board.handpy_get_accelerometer_Z()
  
  def get_strength(self):
    if self.first_flag:
      self.first_flag = False
      self.board = get_globalvar_value("HANDPY")["HANDPY"]
      self.board.board._report_sensor()
    return round(self.board.board.handpy_get_accelerometer_strength(),2)

def touch_threshold(obj="", value=30):
  board = get_globalvar_value("HANDPY")["HANDPY"]
  if value > 80:
    value = 80
  elif value < 0:
    value = 0
  data = obj.upper()
  board.board.handpy_set_touch_threshold(data, value)

def sleep_ms(tim):
  time.sleep(tim / 1000)

button_a = MBSensor_buttonA()  #兼容micropython方法
button_b = MBSensor_buttonB()  #兼容micropython方法
button_ab = MBSensor_buttonAB()
touchPad_P = MBSensor_touchPad_P()
touchPad_Y = MBSensor_touchPad_Y()
touchPad_T = MBSensor_touchPad_T()
touchPad_H = MBSensor_touchPad_H()
touchPad_O = MBSensor_touchPad_O()
touchPad_N = MBSensor_touchPad_N()
sound = MBSensor_sound()
light = MBSensor_light()
accelerometer = MBSensor_acc()
rgb = RGB()
oled = HPScreen()
music = Music()
