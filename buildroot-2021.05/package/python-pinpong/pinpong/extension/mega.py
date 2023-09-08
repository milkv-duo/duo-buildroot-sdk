# -*- coding: utf-8 -*- 

import serial
import platform
import sys
import time

from pinpong.base import pymata4
from pinpong.base.comm import *
from pinpong.extension.globalvar import *


meg_res = {
    "i2c" : {
        "busnum" : [0],
        "class" : "DuinoI2C"
        },
    "spi" : {
        "busnum" : [(0,0)],
        "class" : "DuinoSPI"
        },
    "uart" : {
        "busnum" : [],
        "class" : "DuinoUART",           
        },
    "pin" : {
        "pinnum" : [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
                    21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,
                    38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53],
        "dpin" : [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
                    21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,
                  38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53],
        "apin" : [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15],
        "type" : "firmata",
        "class" : "DuinoPin",
        "write_analog" : "firmata"        
        },
    "adc" : {
        "pinadc" : [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15],
        "class" : "DuinoADC",
        "type" : "firmata"
        },
    "pwm" : {
        "pinpwm" : [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15],  
        "class" : "DuinoPWM",
        "type" : "dfrobot_firmata"
        },
    "tone" : {
        "type" : "firmata",
        "class" : "DuinoTone" ,
        "pininvalid" : []
        },
    "dht11" : {
        "type" : "firmata",
        "pininvalid" : []
        },
    "dht22" : {
        "type" : "firmata", 
        "pininvalid" : []
        },
    "servo" : {
        "type" : "firmata",
        "class" : "DuinoServo",
        "pininvalid" : []
        },
    "irrecv" : {
        "class" : "DuinoIRRecv",
        "pininvalid" : []
        },
    "irremote" : {
        "class" : "DuinoIRRemote",
        "pininvalid" : []
        },
    "sr04" : {  #两个引脚不做判断
        "type" : "firmata",
        "pininvalid" : []
        }        
    }

def begin(board): 
  printlogo_big()
  version = sys.version.split(' ')[0]
  name = platform.platform()
  print("[01] Python"+version+" "+name+(" " if board.boardname == "" else " Board: "+ board.boardname))
  meg_res["firmware"] = ["/base/FirmataExpress.MEGA2560.", ".hex"]

def init(board):
  board.connected = False 

def soft_reset(board):
  pass
  
def reset():
  #self.serial = serial.Serial(self.port, 115200, timeout=times[self.boardname])
  pass
def reset_delay():
  time.sleep(2)

def open_serial(board):
  board.serial = serial.Serial(board.port, 115200, timeout=board.duration[board.boardname])
  time.sleep(2)

def find_port(board):
  pass

def get_pin(board,vpin):

  dpin = vpin if vpin<54 else (vpin-100+14) if vpin >= 100 else -1
  apin = vpin-100 if vpin >= 100 else -1
  if vpin < 100:
    if dpin not in meg_res["pin"]["dpin"]:
      raise ValueError("MEGA2560 does not support this digital pin D%d"%vpin, "Digital pin lists are supported",meg_res["pin"]["dpin"])
     
  else:
    if apin not in meg_res["pin"]["apin"]:
      raise ValueError("MEGA2560 does not support this analog pin A%d"%vpin, "Support for simulation pin list",meg_res["pin"]["apin"])

  return dpin,apin
  
meg_res["init"] = init
meg_res["begin"] = begin
meg_res["reset"] = reset
meg_res["soft_reset"] = soft_reset
meg_res["open_serial"] = open_serial
meg_res["find_port"] = find_port
meg_res["get_pin"] = get_pin

set_globalvar_value("MEGA2560", meg_res)
