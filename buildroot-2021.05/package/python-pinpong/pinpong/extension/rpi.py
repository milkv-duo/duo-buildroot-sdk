# -*- coding: utf-8 -*-

import sys
import platform

from pinpong.extension.globalvar import *
from pinpong.base.comm import *

try:
  import RPi.GPIO as GPIO
except Exception:
  pass
  
rpi_res = {
    "i2c" : {
        "busnum" : [0,1],
        "class" : " LinuxI2C"
        },
    "spi" : {
        "busnum" : [(0,0),(0,1)],
        "class" : "LinuxSPI"
        },
    "uart" : {
        "busnum" : ["/dev/ttyS0", "/dev/ttyAMA0", "/dev/ttyUSB0"],
        "class" : "TTYUART"         
        },
    "pin" : {
        "pinnum" : [4,5,6,12,13,16,17,18,19,20,21,22,23,24,25,26,27],    
        "class" : "RPiPin"        
        },
    "pwm" : {
        "pinpwm" : [4,5,6,12,13,16,17,18,19,20,21,22,23,24,25,26,27],
        "class" : "RPiPWM"
        },
    "tone" : {       
        "class" : "LinuxTone",
        "pininvalid" : []
        }, 
    "servo" : {        
        "class" : "RPiServo",
        "pininvalid" : []
        },
    "dht11" : {
       
        "pininvalid" : []
        },
    "dht22" : {
       
        "pininvalid" : []
        },
    "irrecv" : {
        "class" : "EVENTIRRecv",
        "pininvalid" : []
        }
    }   


def begin(board):
  printlogo_big()
  version = sys.version.split(' ')[0]
  name = platform.platform()
  print("[01] Python"+version+" "+name+(" " if board.boardname == "" else " Board: "+ board.boardname))
  
def init(board):
  GPIO.setmode(GPIO.BCM)
  GPIO.setwarnings(False)
  board.connected = True
  

def reset():
  pass
  
def sofr_reset(board):
  pass
  
def get_pin(board,vpin):
  if vpin not in rpi_res["pin"]["pinnum"]:
    raise ValueError("Raspberry PI does not support this pin %d"%vpin, "Support pins are",rpi_res["pin"]["pinnum"])
   
  dpin = apin = vpin
  return dpin,apin

def find_port(board):
  pass

def open_serial(board):
  pass

rpi_res["begin"] = begin 
rpi_res["init"] = init
rpi_res["find_port"] = find_port
rpi_res["reset"] = reset
rpi_res["sofr_reset"] = sofr_reset
rpi_res["get_pin"] = get_pin

set_globalvar_value("RPI", rpi_res)

  
  

