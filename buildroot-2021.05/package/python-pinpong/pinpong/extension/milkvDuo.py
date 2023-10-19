# -*- coding: utf-8 -*- 
import serial
import platform
import sys
import time
import os
import threading
import select

from pinpong.extension.globalvar import *
from pinpong.base.comm import *
from pinpong.base.config import *

duo_res = {
    "i2c" : {
        "busnum" : [0,1,3],
        "class" : "LinuxI2C",
        },
    "spi" : {
        "busnum" : [(0,0), (1,0)],
        "class" : "LinuxSPI"
        },
    "uart" : {
        "busnum" : ["/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3", "/dev/ttyS4"],
        "class" : "TTYUART",           
        },
    "pin" : {
        "pinnum" : [  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 25, 26, 27],
        "pinmap" : [508,509,378,377,371,372,375,374,373,370,425,426,496,497,494,495,503,504,502,505,507,506,356,440,451,454],
        "class" : "SYSFSPin",     
        }, 
    "adc" : {
        "pinadc" : [0,1],  ##和板子不一致需要沟通
        "pinmap" : [1,2],
        "adcconfig" : {3:{"channel":"3"}, 5:{"channel":"5"}, 7:{"channel":"7"}, 2:{"channel":"2"},
        4:{"channel":"4"}, 6:{"channel":"6"}, 17:{"channel":"9"}, 16:{"channel":"8"}},
        "class" : "LinuxADC",
        "value_path" : "/sys/class/cvi-saradc/cvi-saradc0/device/cv_saradc", #咨询后期会不会改成iio
        },
    "pwm" : {
        "pinpwm" : [3,4,5,6,7,8,9,10,13,14],
        "pinmap" : [10,11,5,6,9,8,7,4,4,5],  #一对多问题怎么解决
        "class" : "SYSFSPWM",
        "pwmconfig" : {10:{"channel":"2"}, 11:{"channel":"3"}, 5:{"channel":"1"}, 
        6:{"channel":"2"}, 9:{"channel":"1"}, 8:{"channel":"0"}, 7:{"channel":"3"},
        4:{"channel":"0"}, 4:{"channel":"0"}, 5:{"channel":"1"}},   
        "export_path" : "/sys/class/pwm/pwmchip2/export",
        "comm_path" : "/sys/class/pwm/pwmchip2/pwm",
        "polarity" : "normal"
        },
    "dht11" : {
       
        },
    "dht22" : {
       
        },
    "servo" : {
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
  board.connected = True

def begin(board):
  printlogo_big()
  board.port = "/dev/ttyS3"

def pinReuse(pin, mode):
  pass

def reset():
  pass
 
def open_serial(board):
  board.serial = serial.Serial(board.port, 115200, timeout=board.duration[board.boardname])

def find_port(board):
  pass

def PWM(board):
  board.fixed_value = 1000000000
  pwmchip = str(int(int(board.pin/4)*4))
  
  print("pwmchip:",pwmchip,"pwm:",board.pin/4)
  board.export_path = duo_res["pwm"]["export_path"].replace("2", pwmchip)  #milkv-duo的pwm组号是动态变化的
  board.pwm_path = duo_res["pwm"]["comm_path"].replace("2", pwmchip)
  if os.path.exists(board.pwm_path+board.channel):
    os.system('echo '+board.channel+ ' > '+board.export_path.replace("export","")+"unexport")
    #print('echo '+board.channel+ '> /sys/class/pwm/pwmchip2/unexport')
  
  os.system('echo '+board.channel+' > '+board.export_path)
  os.system('echo 10000000 > '+board.pwm_path+board.channel+"/period")
  print('echo 10000000 > '+board.pwm_path+board.channel+"/period")
  #os.system('echo normal > '+board.pwm_path+board.channel+'/polarity')
  os.system('echo 1 > '+board.pwm_path+board.channel+'/enable')
  #print('echo '+board.channel+' > '+board.export_path)
  board.isStart = False

def ADC(board):
  board.value_path = duo_res["adc"]["value_path"]
  os.system('echo '+str(board.pin_obj.apin)+' > '+board.value_path)
  print('echo '+str(board.pin_obj.apin)+' > '+board.value_path)
  
def getInterurput(gpio_fd, pin, handler, exit_event):
  if gpio_fd == -1:
    print("gpio open failed")
  poller = select.poll()
  poller.register(gpio_fd, select.POLLPRI)    #将事件关联到文件描述符，监听文件描述符上状态改变
  while not exit_event.is_set():
    events = poller.poll()
    if events:
      for fd, event in events:
        if event & select.POLLPRI:
          os.lseek(gpio_fd, 0, os.SEEK_SET)
          buff = os.read(gpio_fd, 10)
          if buff == -1:
            print("read failed")
          handler(pin)
  os.close(gpio_fd)
  
def irq(board, trigger, handler):   ##开线程阻塞读取
  IRQ_FALLING = 2
  IRQ_RISING = 1
  index = duo_res["pin"]["pinmap"].index(board.pin) 
  pin = duo_res["pin"]["pinnum"][index]
  if(trigger == IRQ_RISING):
    os.system('echo rising > /sys/class/gpio/gpio'+str(board.pin)+'/edge')
  elif(trigger == IRQ_FALLING):
    os.system('echo falling > /sys/class/gpio/gpio'+str(board.pin)+'/edge')
  else:
    os.system('echo both > /sys/class/gpio/gpio'+str(board.pin)+'/edge')
  gpio_fd = os.open("/sys/class/gpio/gpio508/value", os.O_RDONLY)
  exit_event = threading.Event()
  block_thread = threading.Thread(target=getInterurput, args=(gpio_fd, pin, handler, exit_event))  
  block_thread.daemon = True
  block_thread.start()

  
def getPinmap(pin, mode):
 
  if(mode == 1):
    index = duo_res["pin"]["pinnum"].index(pin) 
    dpin = duo_res["pin"]["pinmap"][index]
  elif(mode == 2):
    index = duo_res["pwm"]["pinpwm"].index(pin)
    dpin = duo_res["pwm"]["pinmap"][index]  
    
  else:
    index = duo_res["adc"]["pinadc"].index(pin)
    dpin = duo_res["adc"]["pinmap"][index]

  return dpin  

def get_pin(board,vpin):
  apin = 0
  if(vpin >= 100):
    vpin = vpin - 100
    apin = vpin
  else:
    apin = vpin
  if vpin not in duo_res["pin"]["pinnum"]:
    raise ValueError("%d pin is not supported"%vpin, "Support pin", duo_res["pin"]["pinnum"])
  dpin = vpin
  return dpin,apin

def soft_reset(board):
  pass
duo_res["init"] = init
duo_res["begin"] = begin
duo_res["reset"] = reset
duo_res["open_serial"] = open_serial 
duo_res["find_port"] = find_port 
duo_res["get_pin"] = get_pin
duo_res["soft_reset"] = soft_reset 
duo_res["PWM"] = PWM
duo_res["pinReuse"] = pinReuse
duo_res["getPinmap"] = getPinmap
duo_res["irq"] = irq
duo_res["ADC"] = ADC
set_globalvar_value("MILKV-DUO", duo_res)
