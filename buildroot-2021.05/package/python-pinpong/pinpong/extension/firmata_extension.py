# -*- coding: utf-8 -*-
import time

from pinpong.base import pymata4
from pinpong.extension.globalvar import *


class NeoPixelExtension(object):
  def __init__(self, board=None, pin_obj=None, num=None):
    
    self.pin_obj  = pin_obj
    self.board = board
    self.num = num
    self.__data = [(0,0,0) for i in range(num)]
    #self.board.board.set_pin_mode_neo(self.pin_obj.pin)
    self.board.board.neopixel_config(self.pin_obj.pin,self.num)
    time.sleep(0.1)

  def __repr__(self):
    return 'pixel data (%s)' % self.__data
 
  def __getitem__(self, i):
    return self.__data[i]  # 返回data绑定列表中的第i个元素
 
  def __setitem__(self, i, v):
    #print(i,v)
    self.__data[i]=v
    self.write(i,v)
    if self.board.boardname == "UNO":
      time.sleep(0.15)
    else:
      pass

  def write(self , index, r, g=None, b=None):
    if isinstance(r,tuple):
      b=r[2]
      g=r[1]
      r=r[0]
    color = (r<<16) + (g<<8) + b
    self.board.board.neopixel_write(self.pin_obj.pin, index, color)

  def brightness(self, brightness):
    self.board.board.neopixel_set_brightness(self.pin_obj.pin, brightness)

  def rainbow(self , start, end, hsv_start, hsv_end):
    self.board.board.neopixel_set_rainbow(self.pin_obj.pin, start, end, hsv_start, hsv_end)

  def shift(self , n):
    self.board.board.neopixel_shift(self.pin_obj.pin, n)

  def rotate(self , n):
    self.board.board.neopixel_rotate(self.pin_obj.pin, n)

  def range_color(self, start, end, color):
    if isinstance(color, int):
      self.board.board.neopixel_set_range_color(self.pin_obj.pin, start, end, color)
    elif isinstance(color, tuple):
      self.board.board.neopixel_set_range_color(self.pin_obj.pin, start, end, (color[0]<<16 | color[1]<<8 | color[2]))


    
#  def bar_graph(self, start, end, numerator, denominator):
#    self.board.board.set_bar_graph(self.pin_obj.pin, start, end, numerator, denominator)

  def clear(self):
    self.board.board.neopixel_set_range_color(self.pin_obj.pin, 0, self.num-1, 0)

class DHTExtension:
  def __init__(self,board=None, pin_obj=None, num=0):
    
    self.board = board
    self.pin_obj = pin_obj
    self.key = "dht%d"%num 
    if pin_obj.pin in board.res[self.key]["pininvalid"]:
      raise ValueError(self.key+"is not supported %d pin"%pin_obj.pin, "Pin lists are not supported",board.res[self.key]["pininvalid"])
    self.type = num
    self.board.board.set_pin_mode_dht(self.pin_obj.pin, self.type, differential=.01)
    time.sleep(1.2) #防止用户层读出数据为0
    
  def measure(self):
    
    if self.board.res[self.key]["type"] == "dfrobot_firmata":
      self.board.board.dfrobot_dht_read(self.pin_obj.pin, self.type)
    self.value = self.board.board.dht_read(self.pin_obj.pin)

  def temp_c(self):
    if self.board.res[self.key]["type"] == "dfrobot_firmata":
      self.board.board.dfrobot_dht_read(self.pin_obj.pin, self.type)
    return self.board.board.dht_read(self.pin_obj.pin)[1]

  def humidity(self):
    if self.board.res[self.key]["type"] == "dfrobot_firmata":
      self.board.board.dfrobot_dht_read(self.pin_obj.pin, self.type)
    return self.board.board.dht_read(self.pin_obj.pin)[0]


class SR04_URM10Extension:
  def __init__(self,board=None, trigger_pin_obj=None, echo_pin_obj=None):
                          
    self.board  = board
    self.trigger_pin_obj = trigger_pin_obj
    self.echo_pin_obj = echo_pin_obj
    if self.board.res["sr04"]["type"] == "dfrobot_firmata":
      self.board.board.dfrobot_set_pin_mode_sonar(self.trigger_pin_obj.pin, self.echo_pin_obj.pin)
    else:
      self.board.board.set_pin_mode_sonar(self.trigger_pin_obj.pin, self.echo_pin_obj.pin)

  def distance_cm(self):
    if self.board.res["sr04"]["type"] == "dfrobot_firmata":
      self.board.board.dfrobot_sonar_read(self.trigger_pin_obj.pin, self.echo_pin_obj.pin)
      time.sleep(0.01)
    return self.board.board.sonar_read(self.trigger_pin_obj.pin)[0]

class DS18B20Extension:
  def __init__(self, board=None, pin_obj=None):
    
    self.board = board
    self.pin_obj = pin_obj
    self.board.board.set_pin_mode_DS18B20(self.pin_obj.pin)
    
  def temp_c(self):
    return self.board.board.ds18b20_read(self.pin_obj.pin)

class GP2Y1010AU0FExtension: #空气质量粉尘传感器 仅unihiker
  def __init__(self, board=None, anapin=None, digpin=None):
   
    self.board = board
    self.anapin = anapin
    self.digpin = digpin
    self.dust_value = 0
  
  def dust_density(self):
    self.__calc_value()
    return round(self.dust_value,2)
  
  def __calc_value(self):
    raw_value = self.board.board.gp2y1010au0f_read(self.anapin, self.digpin)
    calc_value = raw_value * (6.0 / 4095.0)
    self.dust_value = 0.17 * calc_value - 0.1

class AudioAnalyzerExtension:
  def __init__(self, board=None, strobe_pin=None, RST_pin=None, DC_pin=None):
    
    self.board = board
    self.strobe_pin = strobe_pin
    self.RST_pin = RST_pin
    self.DC_pin = DC_pin
    
    self.DC_pin,self.DC_apin = board.res["get_pin"](board,DC_pin)
    self.RST_pin,self.RST_apin = board.res["get_pin"](board,RST_pin)
    self.strobe_pin,self.strobe_apin = board.res["get_pin"](board,strobe_pin)
    
    self.board.board.set_audio_init(self.strobe_pin, self.RST_pin, self.DC_pin)
    
  def read_freq(self):
    return self.board.board.audio_analyzer_read_freq()

class HX711Extension:
  def __init__(self, board, dout_pin, sck_pin, scale = None):
    
    self.board = board
    self.dout_pin = dout_pin
    self.sck_pin = sck_pin
    self.dout_pin,self.dout_apin = board.res["get_pin"](board,dout_pin)
    self.sck_pin,self.sck_apin = board.res["get_pin"](board,sck_pin)
    self.scale = scale
    self.board.board.set_hx711_init(self.dout_pin, self.sck_pin, self.scale)
  
  def read_weight(self):
    return self.board.board.hx711_read_weight(self.dout_pin)
    

