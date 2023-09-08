# -*- coding: utf-8 -*-

import os
import sys, getopt
import json
import time
import ctypes
import serial
import signal
import platform
#import serial.tools.list_ports
import subprocess
import threading

from pinpong.base.avrdude import *
from pinpong.base import pymata4
from pinpong.base.comm import *
from pinpong.base.i2c import *
from pinpong.base.config import *

from pinpong.extension.globalvar import *
from pinpong.extension.firmata_extension import *

#不同系统支持的方法，尝试导入避免报错
try:
  import spidev
except Exception:
  pass

try:
  import modbus_tk
  from modbus_tk import modbus_tcp
  from modbus_tk import modbus_rtu
  import modbus_tk.defines as cst
except Exception:
  pass

try:
  import RPi.GPIO as GPIO
except Exception:
  pass

gboard = None
gthreads = []
globalvar_init()

#注册并导入板子资源信息
import pinpong.extension.uno
import pinpong.extension.leonardo
import pinpong.extension.rpi
import pinpong.extension.micro
import pinpong.extension.hand
import pinpong.extension.leonardo
import pinpong.extension.unihi
import pinpong.extension.nezha
import pinpong.extension.mega
import pinpong.extension.milkvDuo

class DuinoPin:
  def __init__(self, board=None, pin=None, mode=None):
    self.mode = mode
    self.board = board
    self.pin, self.apin = board.res["get_pin"](board,pin)  #通过板子信息获取对应引脚
    if(mode == Pin.OUT):
      self.board.board.set_pin_mode_digital_output(self.pin)
    elif(mode == Pin.IN):
      self.board.board.set_pin_mode_digital_input(self.pin, callback=None)
    elif(mode == Pin.PWM):#为了支持面向过程的4个API而设计的此选项，尽量避免使用,使用PWM类代替
      self.board.board.set_pin_mode_pwm_output(self.pin)
    elif(mode == Pin.ANALOG):#为了支持面向过程的4个API而设计的此选项，尽量避免使用，使用ADC类代替
      if board.res["pin"]["write_analog"] == "dfrobot_firmata":     #判断使用哪条pymata命令
        self.board.board.set_pin_analog_input(self.apin, None)
      else:
        self.board.board.set_pin_mode_analog_input(self.apin, None)

  def value(self, v = None):
    if v == None:  #Read
      if self.mode == Pin.OUT:
        return self.val
      else:
        if self.pin == None:
          return
        self.val = self.board.board.digital_read(self.pin)
        return self.val
    else:  #Write
      self.val = v
      if(self.pin == None):
        return
      self.board.board.digital_pin_write(self.pin, v)
      time.sleep(0.001)

  def on(self):
    self.val = 1
    if self.pin == None:
      return
    self.board.board.digital_pin_write(self.pin, 1)

  def off(self):
    self.val = 0
    if(self.pin == None):
      return
    self.board.board.digital_pin_write(self.pin, 0)

  def irq(self, trigger, handler):
    self.board.board.set_pin_mode_digital_input(self.pin, None)
    self.board.board.set_digital_pin_params(self.pin, trigger, handler)
  
  #这5个函数将打破原有的面向对象规则，请慎用
  #建议使用value方法 PWM和ADC类来替代这5个函数 
  def write_analog(self, duty):
    self.duty=duty
    self.freq=100
    if self.board.res["pin"]["write_analog"] == "firmata":  #区分不同的firmata方法
      real_duty = int(self.duty / 255 * 100)
      real_duty = 255 if real_duty>255 else real_duty
      self.board.board.pwm_write(self.pin, self.freq, real_duty)
    else:
      self.board.board.dfrobot_pwm_write(self.pin, self.freq, self.duty)

  def write_digital(self, value):
    self.val = value
    if(self.pin == None):
      return
    self.board.board.digital_pin_write(self.pin, value)

  def read_digital(self):
    if(self.pin == None):
      return
    self.val = self.board.board.digital_read(self.pin)
    return self.val

  def read_analog(self):
    return self.board.board.analog_read(self.apin)

  def pin_mode(self, mode):
    if(mode == Pin.OUT):
      self.board.board.set_pin_mode_digital_output(self.pin)
    elif(mode == Pin.IN):
      self.board.board.set_pin_mode_digital_input(self.pin, callback=None)

class RPiPin:
  def __init__(self, board=None, pin=None, mode=None):
    self.board = board
    if(pin == None):
      self.pin = None
      return
    self.pin = pin
    self.mode = mode
    if(mode == Pin.OUT):
      GPIO.setup(self.pin, GPIO.OUT)
    elif(mode == Pin.IN):
      GPIO.setup(self.pin, GPIO.IN)
    elif(mode == Pin.PWM):#为了支持面向过程的4个API而设计的此选项，尽量避免使用,使用PWM类代替
      GPIO.setup(self.pin, GPIO.OUT)
      self.pwm=GPIO.PWM(self.pin, 1000)

  def value(self, v = None):
    if v == None:  #Read
      if self.mode == Pin.OUT:
        return self.val
      else:
        if self.pin == None:
          return
        self.val = GPIO.input(self.pin)
        return self.val
    else:  #Write
      self.val = v
      if(self.pin == None):
        return
      GPIO.output(self.pin,v)

  def on(self):
    self.val = 1
    if self.pin == None:
      return
    GPIO.output(self.pin, GPIO.HIGH)

  def off(self):
    self.val = 0
    if(self.pin == None):
      return
    GPIO.output(self.pin, GPIO.LOW)

  def irq(self, trigger, handler):
    OFFSET = 30                                   #30是偏移量
    GPIO.add_event_detect(self.pin, OFFSET+trigger)
    GPIO.add_event_callback(self.pin, handler)
    #GPIO.add_event_detect(self.pin, 30+trigger, callback=handler)
  
  #这5个函数将打破原有的面向对象规则，请慎用
  #建议使用value方法 PWM和ADC类来替代这5个函数 
  def write_analog(self, duty):
    self.duty=duty
    self.pwm.start(duty)

  def write_digital(self, value):
    self.val = value
    if(self.pin == None):
      return
    GPIO.output(self.pin, value)

  def read_digital(self):
    if(self.pin == None):
      return
    self.val = GPIO.input(self.pin)
    return self.val

  def pin_mode(self, mode):
    if(mode == Pin.OUT):
      GPIO.setup(self.pin, GPIO.OUT)
    elif(mode == Pin.IN):
      GPIO.setup(self.pin, GPIO.IN)

class SYSFSPin:
  def __init__(self, board=None, pin=None, mode=None):
    self.board = board
    if(pin == None):
      self.pin = None
      return       
    self.pin = pin
    self.mode = mode
    
    self.pin = board.res["getPinmap"](pin, 1)    #获取GPIO引脚映射
    board.res["pinReuse"](self.pin, STATE_GPIO)  #设置引脚复用模式
    self.export_path = "/sys/class/gpio/export"
    self.value_path = "/sys/class/gpio/gpio"+str(self.pin)+'/value'
    self.direction_path = "/sys/class/gpio/gpio"+str(self.pin)+'/direction'
 
    if not os.path.exists("/sys/class/gpio/gpio"+str(self.pin)):
      os.system('echo '+str(self.pin)+' > '+self.export_path)
    
    if(mode == Pin.OUT):
      os.system('echo out > ' + self.direction_path)
    elif(mode == Pin.IN):
      os.system('echo in > ' + self.direction_path)

  def value(self, v = None):
    if v is None:  #Read
      if self.mode == Pin.OUT:
        return self.val
      else:
        if self.pin is None:
          return
        with open(self.value_path) as f:
          self.val = int(f.read())  
        return self.val
    else:  #Write
      self.val = v
      if(self.pin == None):
        return
      os.system("echo "+str(v)+" > " + self.value_path)

  def on(self):
    self.val = 1
    if self.pin is None:
      return
    os.system("echo 1 > " + self.value_path)

  def off(self):
    self.val = 0
    if(self.pin == None):
      return
    os.system("echo 0 > " + self.value_path)

  def irq(self, trigger, handler):
    self.board.res["irq"](self,trigger,handler)
    
  #这5个函数将打破原有的面向对象规则，请慎用
  #建议使用value方法 PWM和ADC类来替代这5个函数 
  def write_analog(self, duty):
    self.duty=duty
    self.pwm.start(duty)

  def write_digital(self, val):
    self.val = val
    if(self.pin == None):
      return
    self.value(val)

  def read_digital(self):
    if(self.pin == None):
      return
    if self.mode == Pin.OUT:
        return self.val
    else:
      if self.pin is None:
        return
      with open(self.value_path) as f:
        self.val = int(f.read())  
      return self.val

  def pin_mode(self, mode):
    if(mode == Pin.OUT):
      os.system('echo out > ' + self.direction_path)
    elif(mode == Pin.IN):
      os.system('echo in > ' + self.direction_path)

#继承自包含引脚信息定义的类
class Pin(PinInformation):
  def __init__(self, board=None, pin=None, mode=None):
    if isinstance(board, int):#兼容面向过程的4个api
      mode = pin
      pin = board
      board = gboard

    if board == None:
      board = gboard

    self.board = board

    self.last_duty = None #用于记录行空板上次PWM的占空比
    if(pin == None):
      return
    self.pin = pin
    self.mode = mode
    self.pin, self.apin = board.res["get_pin"](self,pin)
    self.obj = eval(board.res["pin"]["class"]+"(board, self.pin, mode)") #根据板子对象直接调用对应的pin方法
   
  def value(self, v = None):
    if v == None:  #Read
      return self.obj.value(v)
    else:  #Write
      self.obj.value(v)
      time.sleep(0.001)

  def on(self):
    self.val = 1
    if self.pin == None:
      return
    self.obj.on()

  def off(self):
    self.val = 0
    if self.pin == None:
      return
    self.obj.off()

  def irq(self, trigger, handler):
    self.obj.irq(trigger, handler)

  #这5个函数将打破原有的面向对象规则，请慎用
  #建议使用value方法 PWM和ADC类来替代这5个函数
  def write_analog(self, duty):
    if self.board.boardname == "UNIHIKER":
      if self.last_duty == duty:
        return None
      self.last_duty = duty
      self.obj.write_analog(duty)
    else:
      self.obj.write_analog(duty)

  def write_digital(self, value):
    self.obj.write_digital(value)

  def read_digital(self):
    return self.obj.read_digital()

  def read_analog(self):
    return self.obj.read_analog()

  def pin_mode(self, mode):
    self.obj.pin_mode(mode)

class DuinoADC:
  def __init__(self, board=None, pin_obj=None):
    if isinstance(board, Pin):
      pin_obj = board
      board = gboard
    elif board == None:
      board = gboard
    
    self.board = board
    self.pin_obj = pin_obj
    #MICROBIT HANDPY 使用协议 F0 xx(PIN) 02 DF私有Firamata协议
    #UNO使用标准的Firmata协议
    if board.res["adc"]["type"] == "dfrobot_firmata":     
      self.board.board.set_pin_analog_input(self.pin_obj.apin, None)
    else:
      self.board.board.set_pin_mode_analog_input(self.pin_obj.apin, None)

  def read(self):
    return self.board.board.analog_read(self.pin_obj.apin)

class LinuxADC:
  def __init__(self, board=None, pin_obj=None):
    if(isinstance(board, Pin)):
      pin_obj = board
      board = gboard
    elif board == None:
      board = gboard
    self.board = board
    self.pin_obj = pin_obj
    self.pin_obj.apin = board.res["getPinmap"](pin_obj.apin, 3)
    self.value_path = board.res["adc"]["value_path"]
    board.res["ADC"](self)
    
  def read(self):
    fd = os.open(self.value_path, os.O_RDONLY)
    data = os.read(fd, 12).decode('utf-8')
    os.close(fd)
    data = int(data)
    return data

class ADC:
  def __init__(self, board=None, pin_obj=None):
    if isinstance(board, Pin):
      pin_obj = board
      board = gboard
    elif board == None:
      board = gboard
    if pin_obj.apin not in board.res["adc"]["pinadc"]:
      raise ValueError("%d pin is not supported by adc"%pin_obj.apin, "List of supported pins", board.res["adc"]["pinadc"])
    self.board = board
    self.pin_obj = pin_obj

    self.obj = eval(self.board.res["adc"]["class"]+"(board, pin_obj)")
    
  def read(self):
    return self.obj.read()

class DuinoPWM:
  def __init__(self, board, pin_obj):
    self.board = board
    self.pin_obj = pin_obj
    self.freq_value = 100
    self.duty_value = 50
    self.board.board.set_pin_mode_pwm_output(self.pin_obj.pin)

  def freq(self, v=None):
    if v == None:
      return self.freq_value
    else:
      self.freq_value = v
    #f4（设置引脚模式） 04（引脚） 03（设置引脚模式为PWM模式）
    #e4（设置4号引脚的信息） 10（占空比：16%） 01（频率7~9位） 7f（频率0~6位） 
    if self.board.res["pwm"]["type"] == "firmata":   
      real_duty = int(self.duty_value / 255 * 100)
      real_duty = 255 if real_duty>255 else real_duty
      self.board.board.pwm_write(self.pin_obj.pin, self.freq_value, real_duty)
    else:
      self.board.board.dfrobot_pwm_write(self.pin_obj.pin, self.freq_value, self.duty_value)

  def duty(self, v=None):
    if v == None:
      return self.duty_value
    else:
      self.duty_value = v
    if self.board.res["pwm"]["type"] == "firmata":
      real_duty = int(self.duty_value / 255 * 100)
      real_duty = 255 if real_duty>255 else real_duty
      self.board.board.pwm_write(self.pin_obj.pin, self.freq_value, real_duty)
    else:
      self.board.board.dfrobot_pwm_write(self.pin_obj.pin, self.freq_value, self.duty_value)

  def deinit(self):
    self.board.pin_obj.pin_mode(Pin.IN)

class RPiPWM:
  def __init__(self, board, pin_obj):
    self.pin_obj = pin_obj
    self.freq_value = 100
    self.duty_value = 50

    GPIO.setup(self.pin_obj.pin, GPIO.OUT)
    self.pwm = GPIO.PWM(self.pin_obj.pin, self.freq_value)
    #self.pwm.start(duty)
    self.isStart = False

  def freq(self, v=None):
    if v == None:
      return self.freq_value
    else:
      self.freq_value = v
      if v == 0:
        self.pwm.stop()
        self.isStart = False
      else:
        self.pwm.start(self.duty_value)
        self.pwm.ChangeFrequency(self.freq_value)
        self.isStart = True

  def duty(self, v=None):
    if v == None:
      return self.duty_value
    else:
      self.duty_value = v
    self.pwm.ChangeDutyCycle(self.duty_value)

  def deinit(self):
    self.pwm.stop()

class SYSFSPWM:
  def __init__(self, board, pin_obj):
    self.pin_obj = pin_obj   
    self.freq_value = 100
    self.duty_value = 50
    self.period_ns = 10000000
    self.fixed_value = 1000000000
    self.io = 0
    self.board = board
    #if self.pin_obj.pin not in board.res["pwm"]["pinpwm"]:#PWM1-GPIO22-PB6(P35,IO38) PWM7--GPIO3-PD22(P7, 118)
      #raise ValueError("%d pin is not supported by PWM"%self.pin_obj.pin, "The supported pins are", board.res["pwm"]["pinpwm"])
    
    self.pin = board.res["getPinmap"](pin_obj.pin, 2)
    board.res["pinReuse"](self.pin,STATE_PWM)
    self.channel = str(board.res["pwm"]["pwmconfig"][self.pin]["channel"])
    print(self.channel, self.pin)
    self.export_path = board.res["pwm"]["export_path"]
    self.pwm_path = board.res["pwm"]["comm_path"]
    board.res["PWM"](self)
    self.period_path = self.pwm_path+self.channel+'/period'
    self.duty_path = self.pwm_path+self.channel+'/duty_cycle'
    self.enable_path = self.pwm_path+self.channel+'/enable'
    print(self.export_path,self.pwm_path)

  def freq(self, v=None):
    if v == None:
      return self.freq_value
    else:
      if v == 0:
        #print('echo 0 > /sys/class/pwm/pwmchip0/pwm'+self.channel+'/enable')
        os.system('echo 0 > '+self.pwm_path+self.channel+'/enable')
        self.isStart = False
        return
      self.freq_value = v
      
      self.period_ns = int(self.fixed_value/self.freq_value)
     
      #self.period_ns = int(self.fixed_value/self.freq_value)
      #print('echo '+str(self.period_ns)+' > '+self.pwm_path+self.channel+'/period')
      print("###############")
      os.system('echo '+str(self.period_ns)+' > '+self.pwm_path+self.channel+'/period')
      print('echo '+str(self.period_ns)+' > '+self.pwm_path+self.channel+'/period');
      if self.isStart == False:
        os.system('echo 0 > '+self.pwm_path+self.channel+'/enable')
        
        #print('echo 1 > /sys/class/pwm/pwmchip0/pwm'+self.channel+'/enable')
        os.system('echo 1 > '+self.pwm_path+self.channel+'/enable')
        self.isStart = True

  def duty(self, v=None):
    if v == None:
      return self.duty_value
    else:
      self.duty_value = v
      
      if(self.board.res["pwm"]["polarity"] == "normal"):
        duty_ns = int(self.period_ns*(self.duty_value)/100.0)  ###milkv-duo极性narmol无法设置，只能单独设置占空比
      else:
        duty_ns = int(self.period_ns*(100-self.duty_value)/100.0)
      #os.system('echo normal > '+self.pwm_path+self.channel+'/polarity')  #UNIHIKER此逻辑必须设置PWM极性相反
      
      #print('echo 0 > '+self.pwm_path+self.channel+'/enable')
      #print('echo '+str(duty_ns)+' > '+self.duty_path)
      
      os.system('echo '+str(duty_ns)+' > '+self.duty_path)
      
      print('echo '+str(duty_ns)+' > '+self.duty_path)
      #os.system('echo 1 > '+self.pwm_path+self.channel+'/enable')
      if self.isStart == False:
        #print('echo 1 > /sys/class/pwm/pwmchip0/pwm'+self.channel+'/enable')
        
        #os.system('echo 1 > '+self.pwm_path+self.channel+'/enable')
        #print('echo 1 > '+self.pwm_path+self.channel+'/enable')
        self.isStart = True

  def deinit(self):
    os.system('echo 0 > '+self.pwm_path+self.channel+'/enable')
    self.isStart = False

class PWM:
  def __init__(self, board=None, pin_obj=None):
    if isinstance(board, Pin):
      pin_obj = board
      board = gboard
    elif board == None:
      board = gboard

    #if(board.boardname == "UNIHIKER"):
     # pin_obj.pin = pin_obj.pin-200
      
    self.board = board
    self.pin_obj = pin_obj
    
    if pin_obj.pin not in board.res["pwm"]["pinpwm"]:
      raise ValueError("pin is not supported by pwm%d"%pin_obj.pin,"The supported pins are",board.res["pwm"]["pinpwm"])
    self.freq_value = 100
    self.duty_value = 50
    self.obj = eval(board.res["pwm"]["class"]+"(board, pin_obj)")

  def freq(self, v=None):
    if v == None:
      return self.freq_value
    else:
      self.freq_value = v
      
    if v == None:
      return self.obj.freq(v)
    else:
      self.obj.freq(v)

  def duty(self, v=None):
    if v == None:
      return self.duty_value
    else:
      self.duty_value = v
    
    if v == None:                                       
      return self.obj.duty(v)
    else:
      self.obj.duty(v)

  def deinit(self):
    self.obj.deinit()

class DuinoSPI:
  def __init__(self, board, device_num, cs, bus_num = 0, baudrate=10000):
    self.board = board
    self.device_num = device_num
    self.board.board.set_pin_mode_spi(device_num, cs)

  def read(self, length, default_value=0xff):
    ret = self.board.board.spi_read(self.device_num, length)
    return ret

  def readinto(self, buf):
    length = len(buf)
    buf=self.board.spi_read(self.device_num, length)

  def write(self, buf): #write some bytes on MOSI
    if isinstance(buf, list):
      self.board.board.spi_write(self.device_num, buf)
    else:
      self.board.board.spi_write(self.device_num, [buf])

  def write_readinto(self, wbuf, rbuf): # write to MOSI and read from MISO into the buffer
    rbuf=self.board.board.xfer3(wbuf)

class SoftSPI:
  def __init__(self, board, sck, mosi, miso, baudrate=100000, polarity=0, phase=0, bits=8):
    self.board = board
    self.mosi = mosi
    self.miso = miso
    self.sck = sck
    self.phase = phase
    self.mosi.value(0)
    self.sck.value(polarity)

  def read(self, num, default_value=0xff):
    ret = bytearray(num)
    for i in range (num):
      ret[i] = self._transfer(default_value)
    return ret

  def readinto(self, buf):
    num = len(buf)
    buf=self.read(num)

  def write(self, buf): #write some bytes on MOSI
    num = len(buf)
    for i in range (num):
      self._transfer(buf[i])

  def write_readinto(self, wbuf, rbuf): # write to MOSI and read from MISO into the buffer
    num = len(wbuf)
    for i in range (num):
      rbuf[i] = self._transfer(wbuf[i])

  def _transfer(self,data):
    ret = 0
    for i in range(8):
      self.mosi.value(1 if data&0x80 else 0)
      self.sck.value(0 if self.sck.value() else 1) #这样书写兼容了MODE0 和 MODE3
      self.sck.value(0 if self.sck.value() else 1)
      if self.miso:
        ret= ret<<1 + self.miso.value()
      data <<= 1
    return ret

class LinuxSPI:
  def __init__(self, bus_num=0, device_num=0, baudrate=31200000):
    self.spi = spidev.SpiDev(bus_num, device_num)
    self.spi.open(bus_num, device_num)
    self.spi.max_speed_hz = baudrate

  def read(self, num, default_value=0xff):
    ret = self.spi.readbytes(num)
    return ret

  def readinto(self, buf):
    num = len(buf)
    buf=self.read(num)

  def write(self, buf): #write some bytes on MOSI
    self.spi.xfer3(buf)

  def write_readinto(self, wbuf, rbuf): # write to MOSI and read from MISO into the buffer
    num = len(wbuf)

class SPI:
  # spi四种模式SPI的相位(CPHA)和极性(CPOL)分别可以为0或1，对应的4种组合构成了SPI的4种模式(mode)
  # Mode 0 CPOL=0, CPHA=0  ->  第一个跳变，即上升沿采样
  # Mode 1 CPOL=0, CPHA=1  ->  第二个跳变，即下降沿采样
  # Mode 2 CPOL=1, CPHA=0  ->  第一个跳变，即下降沿采样
  # Mode 3 CPOL=1, CPHA=1  ->  第二个跳变，即上升沿采样
  # 时钟极性CPOL: 即SPI空闲时，时钟信号SCLK的电平 (1:空闲时高电平; 0:空闲时低电平)
  # 时钟相位CPHA: 即SPI在SCLK第几个边沿开始采样 (0:第一个边沿开始; 1:第二个边沿开始)
  # 默认设置为MODE 0 因为大部分的外设使用的是MODE 0
  def __init__(self, board=None, bus_num=0, device_num=0, sck=None, mosi=None, miso=None, cs=None, baudrate=100000, polarity=0, phase=0, bits=8):
    if isinstance(board, int):
      device_num = board
      bus_num = device_num
      board = gboard
    elif board == None:
      board = gboard
    self.board = board
    spi_num = (bus_num, device_num)
    if spi_num not in board.res["spi"]["busnum"]:
      raise ValueError("spi does not support this device", spi_num, "List of supported spi devices",self.board.res["spi"]["busnum"])
    self.board.spi[bus_num][device_num] = eval(self.board.res["spi"]["class"]+"(bus_num, device_num)")
    
    self.obj = self.board.spi[bus_num][device_num]
    #print("-----",self.obj)
  def read(self, num, default_value=0xff):
    return self.obj.read(num, default_value)

  def readinto(self, buf):
    self.obj.readinto(buf)

  def write(self, buf): #write some bytes on MOSI
    self.obj.write(buf)

  def write_readinto(self, wbuf, rbuf): # write to MOSI and read from MISO into the buffer
    self.obj.writeinto(wbuf, rbuf)

class DuinoI2C:
  def __init__(self, board, bus_num):
    self.board = board
    self.board.board.set_pin_mode_i2c()

  def scan(self):
    plist = self.board.board.i2c_scan()
    return plist

  def writeto(self, i2c_addr, value):
    self.board.board.i2c_write(i2c_addr, value)

  def readfrom(self, i2c_addr, read_byte):
    return self.board.board.i2c_addr_read(i2c_addr, read_byte)

  def readfrom_mem(self, i2c_addr, reg, read_byte):
    return self.board.board.i2c_read(i2c_addr, reg, read_byte, None)

  def readfrom_mem_restart_transmission(self, i2c_addr, reg, read_byte):
    return self.board.board.i2c_read_restart_transmission(i2c_addr, reg, read_byte, None)

  def writeto_mem(self, i2c_addr, reg, value):
    if isinstance(reg,(list)):
        self.board.board.i2c_write(i2c_addr, reg+list(value))
    else:
        self.board.board.i2c_write(i2c_addr, [reg]+list(value))

#使用ioctl的方法实现
class LinuxI2C:
  def __init__(self, board, bus_num=1):
    self.bus_num = bus_num
    self.i2c = I2CTrans(bus_num)
    
  def scan(self):
    plist=[]
    ack=[1]
    for i in range(1,127):
      try:
        ack = self.i2c.transfer(writing(i, ack))
        plist.append(i)
      except:
        pass
    return plist

  def writeto(self, i2c_addr, value): 
    self.i2c.transfer(writing(i2c_addr, value))

  def readfrom(self, i2c_addr, read_byte):
    return self.i2c.read(reading(i2c_addr, read_byte), read_byte)
     
  def readfrom_mem(self, i2c_addr, reg, read_byte):
    buf = []
    msg = []
    buf.append(reg)  
    msg.append(writing(i2c_addr, buf))
    msg.append(reading(i2c_addr, read_byte))  
    return self.i2c.read_mem(msg, read_byte)
  
  def readfrom_mem_restart_transmission(self, i2c_addr, reg, read_byte):
    return self.readfrom_mem(i2c_addr, reg, read_byte)
  
  def writeto_mem(self, i2c_addr, reg, value):
     buf = []
     buf.append(reg)
     buf += value
     self.i2c.transfer(writing(i2c_addr, buf))

class I2C:
  def __init__(self, board=None, bus_num=0):
    if isinstance(board, int):
      bus_num = board
      board = gboard
    elif board == None:
      board = gboard
    self.board = board
    if self.board.i2c[bus_num] == None:
      self.bus_num = bus_num
    
    if bus_num not in board.res["i2c"]["busnum"]:
      raise ValueError("i2c does not support this device%d"%bus_num, "Supports the i2c device list",board.res["i2c"]["busnum"])
    self.board.i2c[bus_num] = eval(self.board.res["i2c"]["class"] + "(board, bus_num)")
    self.obj = self.board.i2c[bus_num]

  def scan(self):
    return self.obj.scan()

  def writeto(self, i2c_addr, value):
    self.obj.writeto(i2c_addr, value)

  def readfrom(self, i2c_addr, read_byte):
    return self.obj.readfrom(i2c_addr, read_byte)

  def readfrom_mem(self, i2c_addr, reg, read_byte):
    return self.obj.readfrom_mem(i2c_addr, reg, read_byte)

  def readfrom_mem_restart_transmission(self, i2c_addr, reg, read_byte):
    return self.obj.readfrom_mem_restart_transmission(i2c_addr, reg, read_byte)

  def writeto_mem(self, i2c_addr, reg, value):
    return self.obj.writeto_mem(i2c_addr, reg, value)

class TTYUART:
  def __init__(self, board, tty_name, baud_rate):
    self.board = board
    self.uart = serial.Serial(tty_name, baud_rate, timeout=0.1)

  def init(self, baud_rate = 9600, bits = 0, parity = 0, stop = 1):
    pass

  def deinit(self):
    pass

  def writechar(self,data):
    if isinstance(data, str):
      data = ord(data[0])
    self.uart.write(data)

  def write(self, data):
    if isinstance(data, str):
      data = data.encode('utf-8')
    self.uart.write(data)

  def readchar(self):
    return ord(self.uart.read(1))

  def readall(self):
    return self.uart.read(self.uart.inWaiting())

  def readline(self):
    return self.uart.read_until()

  def read(self, n):
    return self.uart.read(n)

  def readinto(self, buf, num = -1):
    buf = self.uart.read(len(buf))

  def any(self):
    return self.uart.inWaiting()

class DuinoUART:
  def __init__(self, board, bus_num, baud_rate):
    self.bus_num = bus_num
    self.board = board
    self.board.board.set_pin_mode_uart(bus_num, baud_rate)

  def init(self, baud_rate = 9600, bits = 0, parity = 0, stop = 1):
    self.board.board.set_pin_mode_uart(self.bus_num, baud_rate, bits, parity, stop)

  def deinit(self):
    self.board.board.uart_deinit(self.bus_num)

  def writechar(self,data):
    return self.board.board.uart_write(self.bus_num,[data])

  def write(self, data):
    self.board.board.uart_write(self.bus_num, data)

  def readchar(self):
    return self.board.board.uart_readchar(self.bus_num)

  def readall(self):
    return self.board.board.uart_readall(self.bus_num)

  def readline(self):
    return self.board.board.uart_readline(self.bus_num)

  def read(self, n):
    return self.board.board.uart_read(self.bus_num, n)

  def readinto(self, buf, num = -1):
    self.board.board.uart_readinto(self.bus_num, buf, num)

  def any(self):
    return self.board.board.uart_any(self.bus_num)

class UART:
  def __init__(self, board=None, tty_name="", bus_num=1, baud_rate=9600):
    if isinstance(board, str):
      baud_rate = bus_num
      bus_num = tty_name
      tty_name = board
      board = gboard
    elif isinstance(board, int):
      bus_num = board
      board = gboard
    elif board == None:
      board = gboard
    
    self.board = board
    self.uart_num = bus_num if tty_name == "" else tty_name

    if self.board.uart[bus_num] == None:
      self.bus_num = bus_num
    
    if self.uart_num not in board.res["uart"]["busnum"]:
      raise ValueError("The uart does not support this device%s"%(str(self.uart_num)), "Supports the uart device list",board.res["uart"]["busnum"])
    self.board.uart[bus_num] = eval(self.board.res["uart"]["class"]+"(board, self.uart_num, baud_rate)")
    
    self.obj = self.board.uart[bus_num]

  def deinit(self):
    self.obj.deinit()

  def init(self, baud_rate = 9600, bits = 0, parity = 0, stop = 1):
    self.obj.init(baud_rate, bits, parity, stop)

  def writechar(self,data):
    return self.obj.writechar(data)

  def write(self, data):
    self.obj.write(data)

  def readchar(self):
    return self.obj.readchar()

  def readall(self):
    return self.obj.readall()

  def readline(self):
    return self.obj.readline()

  def read(self, n):
    return self.obj.read(n)

  def readinto(self, buf, num = -1):
    self.obj.readinto(buf, num)

  def any(self):
    return self.obj.any()

class ModBus:
  def __init__(self, board=None, port=None, baudrate=None, host=None, timeout=5):
    self.board = gboard
    if port:
      if port.upper() in list(self.board.modbus.keys()) and self.board.modbus[port]:
        self.master = modbus[port.upper()]
      else:
        ser = serial.Serial(port=port,baudrate=9600, bytesize=8, parity='N', stopbits=1)
        #self.master.set_timeout(1.0)
        self.master = modbus_rtu.RtuMaster(ser)
        self.ser = ser
        self.board.modbus[port.upper()] = self.master
    elif host:
      host = host.upper()
      if host in list(self.board.modbus.keys()) and self.board.modbus[host]:
        self.master = modbus[host]
      else:
        self.master = modbus_tcp(host=host)
        self.host = host
        self.board.modbus[host] = self.master

    self.master.set_timeout(timeout)
    self.master.set_verbose(True)

  def config_serial(self, baudrate=9600, bytesize=8, parity='N', stopbits=1):
    self.ser.baudrate = baudrate

  def read_holding_reg(self, slave, starting_address, quantity_of_x):
    return self.master.execute(slave, cst.READ_HOLDING_REGISTERS, starting_address, quantity_of_x)

  def read_input_reg(self, slave, starting_address, quantity_of_x):
    return self.master.execute(slave, cst.READ_INPUT_REGISTERS, starting_address, quantity_of_x)

  def read_coils(self, slave, starting_address, quantity_of_x):
    return self.master.execute(slave, cst.READ_COILS, starting_address, quantity_of_x)

  # 读离散输入寄存器
  #logger.info(master.execute(1, cst.READ_DISCRETE_INPUTS, 0, 16))
  def write_single_reg(self, slave, starting_address, output_value):
    return self.master.execute(slave, cst.WRITE_SINGLE_REGISTER, starting_address, output_value=output_value)
        
  def write_multiple_reg(self, slave, starting_address, output_value):
    return self.master.execute(slave, cst.WRITE_MULTIPLE_REGISTERS, starting_address, output_value=output_value)

  def write_multiple_coils(self, slave, starting_address, output_value):
    return self.master.execute(slave, cst.WRITE_MULTIPLE_COILS, starting_address, output_value=output_value)

  #位操作
  def write_single_coil(self, slave, starting_address, output_value):
    return self.master.execute(slave, cst.WRITE_SINGLE_COIL, starting_address, output_value=output_value)

class EVENTIRRecv:
  def __init__(self, board, pin_obj, callback):
    global gthreads
    self.pin_obj = pin_obj
    if not callback:
      raise ValueError("no callback function")
    if not pin_obj:
      raise ValueError("invalid Pin")
    
    node= board.res["irrecv"]["event"][self.pin_obj.pin]
    #启动一个线程，执行select，发生事件后调用callback函数
    t = threading.Thread(target=self.work,args=(node, callback))
    t.start()
    gthreads.insert(0,t)

  def work(self, node, callback):
    from evdev import InputDevice
    from select import select
    dev = InputDevice(node)
    while True:
      r,w,x = select([dev],[],[],0.2)
      if len(r) == 0:
        continue
      if r[0] == dev:
        for event in dev.read():
          if event.type != 0:
            callback(event.value)

  def read(self):
    return None

class DuinoIRRecv:
  def __init__(self, board, pin_obj, callback):
    self.board = board
    self.pin_obj = pin_obj
    self.board.board.set_pin_mode_ir_recv(self.pin_obj.pin, callback)

  def read(self):
    return self.board.board.ir_read(self.pin_obj.pin)

class IRRecv:
  def __init__(self, board=None, pin_obj=None, callback=None):
    if isinstance(board, Pin):
      callback = pin_obj
      pin_obj = board
    elif callable(board):
      pin_obj = Pin(20)
      callback = board
    self.board = gboard  
    if pin_obj.pin in self.board.res["irrecv"]["pininvalid"]:
      raise ValueError("irrecv does not support %d pin"%pin_obj.pin, "List of unsupported pins",board.res["irrecv"]["pininvalid"])
       
    self.obj = eval(self.board.res["irrecv"]["class"]+"(self.board, pin_obj, callback)") 
   
  def read(self):
    return self.obj.read()


class RPiIRRemote:
  def __init__(self, board, pin_obj):
    self.pin_obj = pin_obj

  def send(self, value):
    return None

class DuinoIRRemote:
  def __init__(self, board, pin_obj,callback=None):
    self.board = board
    self.pin_obj = pin_obj

  def send(self, value):
    value &= 0xFFFFFFFF      #最多只支持32位
    return self.board.board.ir_send(self.pin_obj.pin, value)

class IRRemote:
  def __init__(self, board=None, pin_obj=None):
    if isinstance(board, Pin):
      pin_obj = board
      board = gboard
    elif board == None:
      board = gboard
    if pin_obj.pin in board.res["irremote"]["pininvalid"]:
      raise ValueError("irremote does not support %d pin"%pin_obj.pin, "Pin lists are not supported",board.res["irremote"]["pininvalid"])
    self.board = board
    self.obj = eval(self.board.res["irremote"]["class"]+"(self.board, pin_obj)")

  def send(self, value):
    return self.obj.send(value)

class LinuxTone:
  def __init__(self, board, pin_obj):
    self.board = board
    self.pwm = PWM(pin_obj = pin_obj)
    self.freq_value = 0

  def on(self):
    self.pwm.freq(self.freq_value)

  def off(self):
    self.pwm.freq(0)

  def freq(self, v=None):
    if v == None:
      return self.freq_value
    else:
      self.freq_value = v
      self.pwm.freq(v)

  def tone(self, freq, duration):
#    self.pwm.play_tone(self.pin_obj.pin, freq, duration) 
     pass

class DuinoTone:
  def __init__(self, board, pin_obj):
    self.board = board
    self.pin_obj = pin_obj
    self.board.board.set_pin_mode_tone(self.pin_obj.pin)
    self.freq_value = 100
    
  def on(self):
   
    if self.board.res["tone"]["type"] == "dfrobot_firmata": 
      self.board.board.dfrobot_play_tone(self.pin_obj.pin, self.freq_value, 0)
    self.board.board.play_tone(self.pin_obj.pin, self.freq_value, 0)

  def off(self):
    
    if self.board.res["tone"]["type"] == "dfrobot_firmata":
      self.board.board.dfrobot_play_tone(self.pin_obj.pin, 0, 0)
    self.board.board.play_tone(self.pin_obj.pin, 0, 0)

  def freq(self, v=None):
    if v == None:
      return self.freq_value
    else:
      if isinstance(v, float):
        self.off()
        raise TypeError("decimals are not supported")
      elif v < 0:
        self.off()
        raise TypeError("negative numbers are not supported")
      else:
        self.freq_value = v

  def tone(self, freq, duration):
    
    if self.board.res["tone"]["type"] == "dfrobot_firmata":
      self.board.board.dfrobot_play_tone(self.pin_obj.pin, freq, duration)
      self.board.board.play_tone(self.pin_obj.pin, freq, duration)
      self.freq_value = freq
    else:
      if duration > 0:
        self.freq_value = freq
        self.on()
        start = time.time()
        while time.time() - start < duration / 1000:
          pass
        self.off()
      else:
        self.board.board.play_tone(self.pin_obj.pin, freq, duration)

class Tone:
  def __init__(self, board=None, pin_obj=None):
    if isinstance(board, Pin):
      pin_obj = board
      board = gboard
    elif board == None:
      board = gboard
    self.board = board  
    if pin_obj.pin in board.res["tone"]["pininvalid"]:
      raise ValueError("tone does not support %d pin"%pin_obj.pin, "Pin lists are not supported",board.res["tone"]["pininvalid"])
    self.obj = eval(board.res["tone"]["class"]+"(board, pin_obj)")

  def on(self):
    self.obj.on()

  def off(self):
    self.obj.off()

  def freq(self, v=None):
    if v == None:
      return self.obj.freq(v)
    else:
     self.obj.freq(v)

  def tone(self, freq, duration):
    self.obj.tone(freq=freq,duration=duration)

class RPiServo:
  def __init__(self, board, pin_obj):
    self.board = board
    self.pin_obj = pin_obj
    GPIO.setup(self.pin_obj.pin, GPIO.OUT)
    self.pwm=GPIO.PWM(self.pin_obj.pin, 50)
    self.pwm.start(0)

  def write_angle(self, value):
    self.angle(value)
    
  def angle(self, _angle):
    duty = int(_angle*(10.0/180.0)+2.5)
    self.pwm.ChangeDutyCycle(duty)
  
  def detach(self):
    self.pwm.stop()
    self.pwm = None
    GPIO.setup(self.pin_obj.pin, GPIO.IN)

class SYSFSServo:
  def __init__(self, board, pin_obj):
    self.board = board
    self.pin_obj = pin_obj
    self.pwm=SYSFSPWM(board, pin_obj)
    self.pwm.freq(50)

  def write_angle(self, value):
    self.angle(value)
    
  def angle(self, _angle):
    #duty_value = int(_angle*(10.0/180.0)+2.5)
    duty_value = _angle*(10.0/180.0)+2.5
    self.pwm.duty(duty_value)
  
  def detach(self):
    self.pwm.deinit()

class DuinoServo:
  def __init__(self, board, pin_obj):
    self.board = board
    self.pin_obj = pin_obj
    self.board.board.set_pin_mode_servo(self.pin_obj.pin)
    self.board.board.set_mode_servo(self.pin_obj.pin)

  def write_angle(self, value):
    self.angle(value)
  
  def angle(self, _angle):
  
    if self.board.res["servo"]["type"] == "dfrobot_firmata":
      if self.pin_obj.pin < 16:
        self.board.board.servo_write(self.pin_obj.pin, _angle)
      else:
        self.board.board.dfrobot_servo_write(self.pin_obj.pin, _angle)
    else:
      self.board.board.servo_write(self.pin_obj.pin, _angle)
  
  def detach(self):
    self.board.set_pin_mode_digital_input(self.pin_obj.pin, callback=None)

class Servo:
  def __init__(self, board=None, pin_obj=None):
    if isinstance(board, Pin):
      pin_obj = board
      board = gboard
    elif board == None:
      board = gboard
    
    self.board = board
    self.pin_obj = pin_obj
    if pin_obj.pin in board.res["servo"]["pininvalid"]:
      raise ValueError("%d pin is not supported by servo"%pin_obj.pin, "List of unsupported pins",board.res["servo"]["pininvalid"])
    self.obj = eval(board.res["servo"]["class"]+"(board, pin_obj)")

  def write_angle(self, value):
    self.obj.write_angle(value)
  
  def angle(self, _angle):
    self.obj.angle(_angle)
  
  def detach(self):
    self.obj.detach()

##firmata协议支持的模块，采用继承的方法实现
class NeoPixel(NeoPixelExtension):
  def __init__(self, board=None, pin_obj=None, num=None):
    if isinstance(board, Pin):
      num = pin_obj
      pin_obj = board
      board = gboard
    elif board == None:
      board = gboard
    NeoPixelExtension.__init__(self, board, pin_obj, num)

class DHT11(DHTExtension):
  def __init__(self,board=None, pin_obj=None, num=11): 
    if isinstance(board, Pin):
      pin_obj = board
      board = gboard
    elif board == None:
      board = gboard
    DHTExtension.__init__(self, board, pin_obj, num)

class DHT22(DHTExtension):
  def __init__(self,board=None, pin_obj=None, num=22): 
    if isinstance(board, Pin):
      pin_obj = board
      board = gboard
    elif board == None:
      board = gboard
    DHTExtension.__init__(self, board, pin_obj, num)

class SR04_URM10(SR04_URM10Extension):
  def __init__(self,board=None, trigger_pin_obj=None, echo_pin_obj=None):
    if isinstance(board, Pin):
      echo_pin_obj = trigger_pin_obj
      trigger_pin_obj = board
      board = gboard
    elif board == None:
      board = gboard
    SR04_URM10Extension.__init__(self, board, trigger_pin_obj, echo_pin_obj)

class DS18B20(DS18B20Extension):
  def __init__(self, board=None, pin_obj=None):
    if isinstance(board, Pin):
      pin_obj = board
      board = gboard
    elif board == None:
      board = gboard
    DS18B20Extension.__init__(self, board, pin_obj)

class GP2Y1010AU0F(GP2Y1010AU0FExtension): #空气质量粉尘传感器 仅unihiker
  def __init__(self, board=None, anapin=None, digpin=None):
    if isinstance(board, int):
      digpin = anapin
      anapin = board
      board = gboard
    elif board == None:
      board = gboard
    GP2Y1010AU0FExtension.__init__(self, board, anapin, digpin)

class AudioAnalyzer(AudioAnalyzerExtension):
   def __init__(self, board=None, strobe=None, RST=None, DC=None):
    if isinstance(board, int):
      strobe_pin = board
      RST_pin = strobe
      DC_pin = RST
      board = gboard
    elif board == None:
      board = gboard
    AudioAnalyzerExtension.__init__(self, board, strobe_pin, RST_pin, DC_pin)
#firmata 协议支持的模块
class HX711(HX711Extension):
   def __init__(self, board, dout_pin, sck_pin = 2121, scale = None):
    if isinstance(board, int):
      scale = sck_pin
      sck_pin = dout_pin
      dout_pin = board
      board = gboard
    elif board == None:
      board = gboard
    HX711Extension.__init__(self,  board, dout_pin, sck_pin, scale)

class Board:
  def __init__(self, boardname="", port=None):
    global gboard
    gboard = None
   
    self.boardname = boardname.upper()
    self.port = port
    self.connected = False
    self.timeout = 0
    self.res = {}
    self.i2c = [None, None, None, None, None]
    self.uart = [None, None, None, None, None]
    self.modbus = {}
    self.spi = [[None, None], [None, None]]
  
    signal.signal(signal.SIGINT, self._exit_handler)

    find_board(self)   #自动识别板子
    self.res = get_globalvar_value(self.boardname) #获取板子信息资源
    if self.res == None:
      raise ValueError("No development board information found. Check to see if you entered the development board name")
      
    if gboard == None:
      gboard = self
      self.res[self.boardname] = gboard

    self.res["init"](self) #不同板子初始化

  def begin(self, firmware = 0):

    self.res["begin"](self) #firmata烧录准备
    
    if self.connected: #Linux or Win SBC
      return self
    major,minor = self.detect_firmata()
    print("[32] Firmata ID: %d.%d"%(major,minor))
    if (major,minor) != firmware_version[self.boardname] or firmware == 1:#burn new firmware
      FIRMATA_MAJOR = firmware_version[self.boardname][0]
      FIRMATA_MINOR = firmware_version[self.boardname][1]
      self.serial.close()
      self.serial = self.port
      print("[35] Burning firmware...")
      cwdpath,_ = os.path.split(os.path.realpath(__file__))
      self.pgm = Burner(self.boardname,self.port)
      self.res["find_port"](self)
      
      self.pgm.burn(cwdpath + self.res["firmware"][0]+str(FIRMATA_MAJOR)+"."+str(FIRMATA_MINOR)+self.res["firmware"][1])

      time.sleep(2)
      print("[37] Burn done")
      self.detect_firmata()
    self.board = pymata4.Pymata4(com_port=self.serial, arduino_wait=2, baud_rate=115200, name = self.boardname)
    
    self.connected = True
    return self
  
  def detect_firmata(self):
    self.duration={  #传入打开串口函数的时间
    "UNO": 0.1,
    "LEONARDO": 0.1,
    "MEGA2560":0.5,
    "MICROBIT": 0.5,
    "HANDPY": 0.5,
    "UNIHIKER": 0.1
    }
    
    print("selected -> board: %s serial: %s"%(self.boardname, self.port))
    if self.port == None:
      return 0,0
    print("[10] Opening "+self.port)
    
    self.res["reset"]() #复位

    self.res["open_serial"](self) #打开串口
  
    self.serial.read(self.serial.in_waiting)
    buf=bytearray(b"\xf0\x79\xf7")
    self.serial.write(buf)
    res = self.serial.read(5)
    if self.boardname == "MEGA2560":
      time.sleep(1)
      self.serial.write(buf)
      res = self.serial.read(5)
    major,minor=0,0
    
    if len(res) >=3 and res[0] == 0xF0 and res[1] == 0x79:
      major,minor = res[2],res[3]
      
    if (major,minor) == (FIRMATA_MAJOR,FIRMATA_MINOR):
      self.res["soft_reset"](self)
    # else: # 解决行空板P9引脚 begin的时候电平会跳动一下, 当固件版本不同时，才复位
    #   self.res["reset"]()

    return major,minor
  
  def disconnect(self):
    self.board.disconnect()

  def get_i2c_master(self, bus_num=0):
    if bus_num == -1:#如果用户填写-1，自动分配device_num
      for i in range(len(self.i2c)):
        if self.i2c[i] == None:
          bus_num = i
          break

    if bus_num == -1: #分配满了，不再分配
      return None

    if self.i2c[bus_num] == None:
      self._i2c_init[bus_num] = True
      self.i2c[bus_num] = IIC(self,bus_num)
    return self.i2c[bus_num]

  def distinguish_bit(self):
    if sys.platform == 'win32':
      try:
        disks = subprocess.Popen("wmic logicaldisk get deviceid, description", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read().decode('utf-8').split("\n")
      except Exception:
        disks = subprocess.Popen("wmic logicaldisk get deviceid, description", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read().decode('gbk').split("\n")
      for disk in disks:
        if 'Removable' in disk or '可移动磁盘' in disk:
          d=re.search(r'\w:', disk).group()
          diskname = subprocess.Popen("wmic logicaldisk where name='%s' get volumename"%(d), shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read().decode('utf-8').split("\n")
          if "MICROBIT" in diskname[1]:
            file_path = d+ "/" + "DETAILS.TXT"
            bit_info = open(file_path, mode='r').readlines()
            result=list(filter(lambda x: 'Unique ID:' in x, bit_info))[0]
            if int(result[11:15]) >= 9904:
              return True
      return False
    elif sys.platform == 'linux':
      message=""
      with open('/proc/mounts', 'r') as f:
        while True:
          l = f.readline()
          if l == "":
            break
          elif "MICROBIT" in l:
            message=l
        if message != "":
          mount_point = message.split(" ")[1]+"/" + "DETAILS.TXT"
          with open(mount_point, 'r') as file:
            bit_info = file.readlines()
            result=list(filter(lambda x: 'Unique ID:' in x, bit_info))[0]
            if int(result[11:15]) >= 9904:
              return True
        return False
    elif sys.platform == 'darwin':
      result = subprocess.Popen(
      "ls /Volumes", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read().decode('utf-8').split()
      if 'MICROBIT' in result:
        mount_point = "/Volumes/MICROBIT/"+ "DETAILS.TXT"
        with open(mount_point, 'r') as file:
          bit_info = file.readlines()
          result=list(filter(lambda x: 'Unique ID:' in x, bit_info))[0]
          if int(result[11:15]) >= 9904:
            return True
      else:
        mount_point = None
        return False

  def get_spi_master(self, device_num=0, sck=None, mosi=None, miso=None, cs=None, baudrate=100000, polarity=0, phase=0, bits=8):
    if device_num == -1:#如果用户填写-1，自动分配device_num
      for i in range(len(self.spi)):
        if self.spi[i] == None:
          device_num = i
          break

    if device_num == -1: #分配满了，不再分配
      return None

    if self.spi[device_num] == None:
      self._spi_init[device_num] = True
      self.spi[device_num] = SPI(self, device_num, sck, mosi, miso, cs, baudrate, polarity, phase, bits)
    return self.spi[device_num]
  
  @staticmethod
  def set_default_board(board):
    global gboard
    gboard = board

  def _exit_handler(self, signum, frame):
    global gthreads
    print("exit_handler")
    for thread in gthreads:
        _async_raise(thread.ident, SystemExit)
    sys.exit(0)

def _async_raise(tid, exctype):
    """raises the exception, performs cleanup if needed"""
    try:
        tid = ctypes.c_long(tid)
        if not inspect.isclass(exctype):
            exctype = type(exctype)
        res = ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, ctypes.py_object(exctype))
        if res == 0:
            # pass
            raise ValueError("invalid thread id")
        elif res != 1:
            # """if it returns a number greater than one, you're in trouble,
            # and you should call it again with exc=NULL to revert the effect"""
            ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, None)
            raise SystemError("PyThreadState_SetAsyncExc failed")
    except Exception as err:
        print(err)