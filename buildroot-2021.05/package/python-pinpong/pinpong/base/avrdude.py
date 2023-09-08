#!/usr/bin/python
# -*- coding: UTF-8 -*- 

import time
import serial
import logging
import sys, getopt
from pinpong.base.stk500 import *
from pinpong.base.stk500v2 import *
from pinpong.base.butterfly import *
from pinpong.base.microbit import *
from pinpong.base.EspTool import *
from pinpong.base.DfuUtil import *

def parse_op(arg):
  x={}
  l=arg.split(":")
  x["filename"] = l[2]
  x["op"] = 0
  x["format"] = 2
  return x

def read_config():
  print(sys_config)

def parse_args(argv):
  global baudrate, programmer,verbose,sys_config,partdesc,port,upd
  opts,args = getopt.getopt(argv,"?b:B:c:C:DeE:Fi:np:OP:qstU:uvVx:yY:")
  print(opts)
  for opt,arg in opts:
    if(opt == "-b"):
      baudrate = int(arg)
      logger.info("baudrate=%d"%baudrate)

    if(opt == "-c"):
      programmer = arg
      logger.info("programmer=%s"%(programmer))

    if(opt == "-C"):
      sys_config = arg
      logger.info("sys_config=%s"%(sys_config))

    if(opt == "-p"):
      partdesc = arg
      logger.info("partdesc=%s"%(partdesc))

    if(opt == "-P"):
      port = arg
      logger.info("port=%s"%(port))

    if(opt == "-v"):
      verbose = verbose + 1

    if(opt == "-U"):
      upd = parse_op(arg)
      logger.info(upd)

def producePGM(name,port,baudrate):
  pgms={
    "arduino":STK500,
    "avr109":Butterfly,
    "wiring":STK500V2,
    "microbit":Microbit,
    "handpy":EspTool,
    "dfu-util":DfuUtil,
  }
  return pgms[name](port,baudrate)


def main(argv):
  global programmer,port,baudrate
  parse_args(argv)
  pgm = producePGM(programmer, port, baudrate)
  if not pgm.display():
    return False

  pgm.initialize()
  print(upd)
  pgm.read_sig_bytes()
  pgm.enable()
  pgm.ihex2b(upd["filename"])
  pgm.burn()
  pgm.disable()

def set_logger():
  global logger
  #logger.setLevel(logging.INFO)
  logger.setLevel(logging.FATAL)
  ph = logging.StreamHandler()
  formatter = logging.Formatter("%(asctime)s - [%(filename)s %(funcName)s]:%(lineno)d - %(levelname)s: %(message)s")
  ph.setFormatter(formatter) 
  logger.addHandler(ph)
  
if __name__ == "__main__":
  set_logger()
  main(sys.argv[1:])

class Burner:
  def __init__(self,board,port):
    global programmer,baudrate
    set_logger()
    if board.upper() == 'UNO':
      programmer="arduino"
      baudrate = 115200
    elif board.upper() == 'LEONARDO':
      programmer="avr109"
      baudrate = 57600
    elif board.upper() == 'MEGA2560':
      programmer = "wiring"
      baudrate = 115200
    elif board.upper() == 'MICROBIT':
      programmer = 'microbit'
      baudrate = 115200
    elif board.upper() == 'HANDPY':
      programmer = 'handpy'
      baudrate = 921600
    elif board.upper() == 'UNIHIKER':
      programmer = 'dfu-util'
      baudrate = 115200
    self.pgm = producePGM(programmer, port, baudrate)

  def burn(self,filename):
    if not self.pgm.display():
      return False
    self.pgm.initialize()
    self.pgm.read_sig_bytes()
    self.pgm.enable()
    self.pgm.filename = filename
    self.pgm.ihex2b(filename)
    self.pgm.burn()
    self.pgm.disable()
