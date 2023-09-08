# -*- coding: utf-8 -*- 
import serial
import platform
import sys
import time
import os

from pinpong.extension.globalvar import *
from pinpong.base.comm import *
from pinpong.base.config import *

uni_res = {
    "i2c" : {
        "busnum" : [4],
        "class" : "LinuxI2C",
        },
    "spi" : {
        "busnum" : [(0,0)],
        "class" : "LinuxSPI"
        },
    "uart" : {
        "busnum" : [0],
        "class" : "TTYUART",           
        },
    "pin" : {
        "pinnum" : [0,1,2,3,4,8,9,10,11,12,13,14,15,16,19,20,21,22,23,24],
        "pinmap" : [203,205,207,202,204,221,220,206,214,213,229,230,231,225,226,227,217,216,211,212],
        "class" : "SYSFSPin",
        "export_path" : "/sys/class/gpio/export",
        "value_path" : "/sys/class/gpio/gpio"       
        }, 
    "adc" : {
        "pinadc" : [0,1,2,3,4,10,21,22],
        "pinmap" : [3,5,7,2,4,6,17,16],
        "adcconfig" : {3:{"channel":"3"}, 5:{"channel":"5"}, 7:{"channel":"7"}, 2:{"channel":"2"}, 4:{"channel":"4"}, 6:{"channel":"6"}, 17:{"channel":"9"}, 16:{"channel":"8"}},
        "class" : "LinuxADC",
        "value_path" : "/sys/bus/iio/devices/iio:device1/",
        },
    "pwm" : {
        "pinpwm" : [0,2,3,8,9,10,16,20,21,22,23],
        "pinmap" : [203,207,202,204,221,220,225,227,217,216,211],
        "class" : "SYSFSPWM",
        "pwmconfig" : {203:{"channel":"3"}, 202:{"channel":2}, 207:{"channel":"7"}, 227:{"channel":"27"}, 217:{"channel":"17"}, 216:{"channel":"16"}, 211:{"channel":"11"},
        220:{"channel":"20"}, 221:{"channel":"21"}, 225:{"channel":"25"}},
        "export_path" : "/sys/class/pwm/pwmchip2/export",
        "comm_path" : "/sys/class/pwm/pwmchip2/pwm"
        },
    "dht11" : {
       
        },
    "dht22" : {
       
        },
    "servo" : {
        "type" : "firmata",
        "class" : "SYSFSServo",
        "pininvalid" : []
        },
    "irrecv" : {
       
        },
    "irremote" : {
        
        },        
    "tone" : {
        "class" : "LinuxTone" ,
        "pininvalid" : []
        },
  }

def init(board):
  fd = os.open("/dev/pinpong_config", os.O_RDWR)
  if fd == -1:
    print("open error")
    return -1
  params = IoctlParams()
  params.mode = 0x1F
  params.num = 1
  
  ret = fcntl.ioctl(fd, MY_IOCTL_RESET, params)
  if ret == -1:
    print("reset fail")

  os.close(fd)  
  time.sleep(1)

  board.connected = True

  
def begin(board):
  printlogo_big()
 
  board.port = "/dev/ttyS3"

def pinReuse(pin, mode):
  
  if(pin >= 200):
    pin_default_state[pin-200] = mode
  else:
    pin_default_state[pin] = mode
  pinConfig()

def reset():
  pass
 
def open_serial(board):
  board.serial = serial.Serial(board.port, 115200, timeout=board.duration[board.boardname])

def find_port(board):
  pass

def PWM(board):
  board.fixed_value = 1000000
  if os.path.exists("/sys/class/pwm/pwmchip2/pwm"+board.channel):
    os.system('echo '+board.channel+ ' > /sys/class/pwm/pwmchip2/unexport')
    #print('echo '+board.channel+ '> /sys/class/pwm/pwmchip2/unexport')
  os.system('echo '+board.channel+' > '+board.export_path)
  #print('echo '+board.channel+' > '+board.export_path)
  board.isStart = False

def ADC(board):
  adcname = "in_voltage%s_raw"%board.res["adc"]["adcconfig"][self.pin_obj.apin]["channel"]
  board.value_path = board.res["adc"]["value_path"]+adcname
  
def irq(board, trigger, handler):
  IRQ_FALLING = 2
  IRQ_RISING = 1
  index = uni_res["pin"]["pinmap"].index(board.pin) 
  pin = uni_res["pin"]["pinnum"][index]
  time.sleep(1)
  if(trigger == IRQ_FALLING):
    lastlevel = '1'
  else:
    lastlevel = '0'
  value = lastlevel
  while True:
    
    fd = os.open(board.value_path, os.O_RDONLY)
    level = os.read(fd,1).decode("utf-8")
    os.close(fd)
    time.sleep(0.05)
    if(value == level):
      value = level
      continue
    if(value != level and trigger == IRQ_FALLING+IRQ_RISING):
      value = level
      handler(pin)
      continue
    value = level
    if(level != lastlevel):
      if(trigger == IRQ_RISING):
        if(lastlevel == '0'):
          handler(pin)
      if(trigger == IRQ_FALLING):
        if(lastlevel == '1'):
          handler(pin)

def getPinmap(pin, mode):
 
  if(mode == 1):
    index = uni_res["pin"]["pinnum"].index(pin) 
    dpin = uni_res["pin"]["pinmap"][index]
  elif(mode == 2):
    index = uni_res["pwm"]["pinpwm"].index(pin)
    dpin = uni_res["pwm"]["pinmap"][index]  
  else:
    index = uni_res["adc"]["pinadc"].index(pin)
    dpin = uni_res["adc"]["pinmap"][index]

  return dpin  

def get_pin(board,vpin):
  apin = 0
  if(vpin >= 100):
    vpin = vpin - 100
    apin = vpin
  else:
    apin = vpin
  if vpin not in uni_res["pin"]["pinnum"]:
    raise ValueError("%d pin is not supported"%vpin, "Support pin", uni_res["pin"]["pinnum"])
  dpin = vpin
  return dpin,apin

def soft_reset(board):
  pass
uni_res["init"] = init
uni_res["begin"] = begin
uni_res["reset"] = reset
uni_res["open_serial"] = open_serial 
uni_res["find_port"] = find_port 
uni_res["get_pin"] = get_pin
uni_res["soft_reset"] = soft_reset 
uni_res["PWM"] = PWM
uni_res["pinReuse"] = pinReuse
uni_res["getPinmap"] = getPinmap
uni_res["irq"] = irq
uni_res["ADC"] = ADC
set_globalvar_value("UNIHIKER", uni_res)