# -*- coding: utf-8 -*-
import serial
from pinpong.base.butterfly_param import *
from pinpong.base.programmer import *

class Butterfly(Programmer):
  def __init__(self, port="/dev/ttyS1", baudrate=115200):
    super().__init__(port, baudrate)

  def setup(self):
    print("Butterfly setup  %s"%self.port)
  
  def vfy_cmd_sent(self):
    res = self.ser.read(1)
    logger.info(res)
    return res[0] == 0x0d
    
  def enter_prog_mode(self):
    buf = bytearray(8)
    buf[0] = ord('P')
    self.ser.write(buf[:1])
    self.vfy_cmd_sent()
    
  def initialize(self):
    global logger
    print("butter initialize")
    id = bytearray(1)
    sw = bytearray(1)
    hw = bytearray(1)
    buf = bytearray(8)
    type=0
    has_auto_incr_addr=0
    buffersize=0
    
    buf[0] = 0x1b
    self.ser.write(buf[:1])
    self.drain()
    
    buf[0] = ord('S')
    self.ser.write(buf[:1])
    res = self.ser.read(1)
    logger.info(res)
    if(res[0] != '?'):
      id[0] = res[0]
      id = id + self.ser.read(6)
    
    buf[0] = ord('V')
    self.ser.write(buf[:1])
    res = self.ser.read(1)
    logger.info(res)
    if(res[0] != '?'):
      sw[0] = res[0]
      sw = sw + self.ser.read(1)
    
    buf[0] = ord('v')
    self.ser.write(buf[:1])
    res = self.ser.read(1)
    logger.info(res)
    if(res[0] != '?'):
      hw[0] = res[0]
      hw = hw + self.ser.read(1)

    buf[0] = ord('p')
    self.ser.write(buf[:1])
    res = self.ser.read(1)
    logger.info(res)
    type = res[0]
        
    buf[0] = ord('a')
    self.ser.write(buf[:1])
    res = self.ser.read(1)
    has_auto_incr_addr=res[0]
    logger.info(res)

    buf[0] = ord('b')
    self.ser.write(buf[:1])
    res = self.ser.read(1)
    if res[0] == ord('Y'):
      res = self.ser.read(2)
      buffersize = res[0]*256 + res[1]

    devtype_1st = 0
    buf[0] = ord('t')
    self.ser.write(buf[:1])
    while True:
      res = self.ser.read(1)
      if devtype_1st == 0:
        devtype_1st = res[0]
      if(res[0] == 0 or res==None):
        break;

    buf[0] = ord('T')
    buf[1] = devtype_1st
    self.ser.write(buf[:2])
    self.vfy_cmd_sent()

    self.enter_prog_mode()
    self.drain()
    
  def read_sig_bytes(self):
    global logger
    buf = bytearray(1)
    buf[0] = ord('s')
    self.ser.write(buf[:1])
    res = self.ser.read(3)
  
  def set_addr(self,addr):
    buf = bytearray(3)
    buf[0] = ord('A')
    buf[1] = (addr//2)>>8
    buf[2] = (addr//2) & 0xff
    self.ser.write(buf[:3])
    self.vfy_cmd_sent()
    
  def burn(self):
    addr = 0
    blocksize = 128
    total_len = len(self.buffer)
    self.set_addr(addr)    
    
    while True:
      buf = bytearray(4)
      buf[0] = ord('B')
      buf[3] = ord('F')
      if total_len - addr < blocksize:
        blocksize = total_len - addr
      buf[1] = (blocksize)>>8
      buf[2] = (blocksize)&0xff

      next_addr = addr + blocksize
      buf = buf+self.buffer[addr:addr+blocksize]
      self.ser.write(buf)
      self.vfy_cmd_sent()
      
      if next_addr - total_len == 0:
        break
      else:
        addr = next_addr

  def disable(self):
    buf = bytearray(1)
    buf[0] = ord('L')
    self.ser.write(buf)
    self.vfy_cmd_sent()
    
    buf[0] = ord('E')
    self.ser.write(buf)
    self.vfy_cmd_sent()
    self.ser.close()