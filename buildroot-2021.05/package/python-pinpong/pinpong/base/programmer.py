# -*- coding: utf-8 -*-
import serial
import logging

logger = logging.getLogger()
baudrate=9600
programmer = ""
verbose=0
sys_config=""
partdesc="" #part id
port=""
upd={}

class Programmer:
  def __init__(self,port,baudrate):
    self.port = port
    self.baudrate = baudrate
    #print("Programmer init")

  def rdy_led(self):
    pass

  def  err_led(self):
    pass

  def pgm_led(self):
    pass

  def vfy_led(self):
    pass

  def initialize(self):
    pass

  def drain(self):
    self.ser.read(self.ser.in_waiting)

  def display(self):
    print("open port:%s baudrate:%d"%(self.port, self.baudrate))
    try:
      self.ser=serial.Serial(self.port, self.baudrate, timeout=0.5)
      return True;
    except Exception as e:
      print(e)
      return False

  def enable(self):
    pass

  def disable(self):
    pass

  def powerup(self):
    pass

  def powerdown(self):
    pass

  def program_enable(self):
    pass

  def chip_erase(self):
    pass

  def cmd(self):
    pass

  def cmd_tpi(self):
    pass

  def open(self):
    pass

  def close(self):
    pass

  def paged_write(self):
    pass

  def paged_load(self):
    pass

  def write_setup(self):
    pass

  def write_byte(self):
    pass 

  def read_byte(self):
    pass

  def read_sig_bytes(self):
    pass

  def print_parms(self):
    pass

  def set_vtarget(self):
    pass

  def set_varef(self):
    pass

  def set_fosc(self):
    pass

  def set_sck_period(self):
    pass

  def setpin(self):
    pass

  def getpin(self):
    pass

  def highpulsepin(self):
    pass

  def parseexitspecs(self):
    pass

  def perform_osccal(self):
    pass

  def parseextparams(self):
    pass

  def setup(self):
    print("Programmer setup at %s"%self.port)

  def teardown(self):
    pass

  def ihex2b(self,filename):
    global logger
    ihex={}
    chksum = 0
    logger.info(filename)
    file = open(filename, 'rb')
    logger.info(file)
    self.buffer=bytearray()
    while True:
      line = file.readline()
      if(line):
        if(line[0] != 58):
          continue
        else:
          line=line[1:-2]
          sline=line.decode()
          b = bytearray.fromhex(sline)
          ihex["reclen"]=b[0]
          ihex["loadofs"]=b[1]*255+b[2]
          ihex["rectyp"]=b[3]
          ihex["data"] = b[4:-1]
          ihex["cksum"] = b[-1]
          chksum = (-sum(bytes(b[:-1])))&0x0ff
          self.buffer = self.buffer + ihex["data"]
      else:
        break

