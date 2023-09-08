# -*- coding: utf-8 -*- 

import platform
import serial
import sys
import subprocess
import re

from pinpong.extension.globalvar import *
from pinpong.base.comm import *

mic_res = {
    "i2c" : {
        "busnum" : [0,1],
        "class" : "DuinoI2C",
        },
    "spi" : {
        "busnum" : [(0,0)],
        "class" : "DuinoSPI"
        },
    "uart" : {
        "busnum" : [0],
        "class" : "DuinoUART",           
        },
    "pin" : {
        "pinnum" : [0,1,2,3,4,5,6,7,8,9,10,11,13,14,15,16,19,20],
        "dpin" :[0,1,2,5,6,7,8,9,13,14,15,16,19,20],
        "apin" : [0,1,2,3,4,10],
        "type" : "",
        "write_analog" : "dfrobot_firmata",
        "class" : "DuinoPin"        
        }, 
    "pwm" : {
        "pinpwm" : [1,2,4,5,6,7,8,11,13,14,15,16],
        "type" : "dfrobot_firmata",
        "class" : "DuinoPWM"       
        },
    "adc" : {
        "pinadc" : [0,1,2,3,4,10],
        "type" : "dfrobot_firmata",
        "class" : "DuinoADC",
        }, 
    "tone" : {
        "type" : "dfrobot_firmata",
        "class" : "DuinoTone",
        "pininvalid" : [12,19,20]
        },
    "servo" : {
        "type" : "firmata",
        "class" : "DuinoServo",
        "pininvalid" : [3,4,6,7,9,10]
        },
    "dht11" : {
        "type" : "dfrobot_firmata",
        "pininvalid" : [3,4,6,7,9,10,17,18,19,20]
        },
    "dht22" : {
        "type" : "dfrobot_firmata", 
        "pininvalid" : [17,18,19,20]
        },
    "irrecv" : {
        "class" : "DuinoIRRecv",
        "pininvalid" : [2,3,4,5]
        },
    "irremote" : {
        "class" : "DuinoIRRemote",
        "pininvalid" : [1,2,3,4,5,6,7,8,9,10,11,13,14,15,16,19,20]
        },
    "sr04" : {
        "type" : "dfrobot_firmata",
        "pininvalid" : []
        }       
    }
def init(board):
    board.connected = False

    
def differ_microbit():       #区分microbit V1 V2

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
        
def begin(board):
    printlogo_big()
    version = sys.version.split(' ')[0]
    name = platform.platform()
    print("[01] Python"+version+" "+name+(" " if board.boardname == "" else " Board: "+ board.boardname))
    val = differ_microbit()
    if val:
      mic_res["firmware"] = ["/base/FirmataExpress.MICROBITV2.", ".hex"]
    else:
      mic_res["firmware"] = ["/base/FirmataExpress.MICROBIT.", ".hex"]        
        
        
        
def open_serial(board):
  board.serial = serial.Serial(board.port, 115200, timeout=board.duration[board.boardname])
  
  
def soft_reset(board):
  board.serial.read(board.serial.in_waiting)
  reset_buf=bytearray(b"\xf0\x0d\x55\xf7")
  board.serial.write(reset_buf)
  reset = board.serial.read(1024)
  
def reset():
  pass  
  
def find_port(board):
  pass
 
def get_pin(board,vpin):
  if vpin not in mic_res["pin"]["pinnum"]:
    raise ValueError("microbit does not support this pin %d"%vpin, "The supported pins are",mic_res["pin"]["pinnum"])

  dpin = apin = vpin  
  return dpin,apin
  
mic_res["init"] = init
mic_res["begin"] = begin
mic_res["open_serial"] = open_serial
mic_res["soft_reset"] = soft_reset  
mic_res["reset"] = reset
mic_res["find_port"] = find_port  
mic_res["get_pin"] = get_pin 

set_globalvar_value("MICROBIT", mic_res)