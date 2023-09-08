# -*- coding: utf-8 -*-
import time
import serial
from pinpong.base.stk500v2_param import *
from pinpong.base.programmer import *

class STK500V2(Programmer):
  def __init__(self, port="/dev/ttyS1", baudrate=115200):
    super().__init__(port, baudrate)

  def setup(self):
    #print("STK500V2 setup  %s"%self.port)
    self.command_sequence = 1

  def stk500v2_send(self,_buf,length):
    buf = bytearray(275+6)
    buf[0] = MESSAGE_START
    buf[1] = self.command_sequence
    buf[2] = int(length/256)
    buf[3] = length%256
    buf[4] = TOKEN
    for i in range(length):
      buf[5+i] = _buf[i]
    buf[5+length] = 0
    for i in range(5+length):
      buf[5+length] ^= buf[i]
    self.ser.write(buf[:6+length])

  def stk500v2_recv(self,maxsize):
    sSTART = 0
    sSEQNUM = 1
    sSIZE1 = 2
    sSIZE2 = 3
    sTOKEN = 4
    sDATA = 5
    sCSUM = 6
    sDONE = 7
    state =sSTART
    checksum = 0
    msglen = 0
    curlen = 0
    msg = bytearray(0)
    timeout = False
    tstart = int(time.time())
    while state is not sDONE and not timeout:
      res = self.ser.read(1)
      if len(res):
        checksum ^= res[0]
        if state == sSTART:
          if res[0] == MESSAGE_START:
            checksum = MESSAGE_START
            state = sSEQNUM
        elif state == sSEQNUM:
          if res[0] == self.command_sequence:
            state = sSIZE1
          else:
            state = sSTART
        elif state == sSIZE1:
          msglen = res[0]*256
          state = sSIZE2
        elif state == sSIZE2:
          msglen += res[0]
          state = sTOKEN
        elif state == sTOKEN:
          if res[0] == TOKEN:
            state = sDATA
          else:
            state = sSTART
        elif state == sDATA:
          msg.append(res[0])
          curlen += 1
          if curlen == msglen:
            state = sCSUM
        elif state == sCSUM:
          if checksum == 0:
            state = sDONE
          else:
            state = sSTART
        else:
          state = sSTART
      else:
        timeout = True
      tnow = int(time.time())
      if tnow - tstart > 2:
        timeout = True
    self.command_sequence = self.command_sequence+1
    return msg

  def stk500v2_command(self,buf,len):
    self.stk500v2_send(buf,len)
    resp = self.stk500v2_recv(32)
    return resp
    
  def stk500v2_getsync(self):
    retry = 5
    buf = bytearray(1)
    buf[0] = CMD_SIGN_ON 

    while retry:
      self.setup()
      retry = retry -1
      self.stk500v2_send(buf,1)
      resp = self.stk500v2_recv(32)
      if(len(resp) > 0):
        break

  def initialize(self):
    global logger
    #print("initialize")
    #stk500_devcode = 0x86
    #sw_major = self.stk500_getparm(Parm_STK_SW_MAJOR) #  0x41 0x81 0x20  return 0x14 0x04 0x10
    #sw_minor = self.stk500_getparm(Parm_STK_SW_MINOR) #  0x41 0x82 0x20  return 0x14 0x04 0x10
    #buf = bytearray(32)
    #Cmnd_STK_SET_DEVICE     #  42 86 00 00  01 01 01 01  03 ff ff ff  ff 00 80 04  00 00 00 80  00 20
    
    self.program_enable()

  def stk500v2_cmd(self,cmd):
    global logger
    buf = bytearray(16)
    buf[0] = CMD_SPI_MULTI
    buf[1] = 4
    buf[2] = 4
    buf[3] = 0
    buf[4] = cmd[0]
    buf[5] = cmd[1]
    buf[6] = cmd[2]
    buf[7] = cmd[3]

    res = self.stk500v2_command(buf, 8)
    logger.info(res)

  def stk500v2_getparm(self,param):
    global logger
    buf = bytearray(2)
    buf[0] = CMD_GET_PARAMETER
    buf[1] = param
    ret = self.stk500v2_command(buf, 2)
    return ret[2]

  def display(self):
    global logger,baudrate
    self.ser=serial.Serial(self.port, self.baudrate, timeout=0.5)
    self.stk500v2_getsync()
    hwv      = self.stk500v2_getparm(PARAM_HW_VER)
    sw_major = self.stk500v2_getparm(PARAM_SW_MAJOR)
    sw_minor = self.stk500v2_getparm(PARAM_SW_MINOR)
    
    #topcard  = self.stk500v2_getparm(Param_STK500_TOPCARD_DETECT)
    vtarget  = self.stk500v2_getparm(PARAM_VTARGET);
    #vadjust  = self.stk500v2_getparm(Parm_STK_VADJUST);
    #pscale   = self.stk500v2_getparm(Parm_STK_OSC_PSCALE);
    #cmatch   = self.stk500v2_getparm(Parm_STK_OSC_CMATCH);
    #duration = self.stk500v2_getparm(Parm_STK_SCK_DURATION);
    logger.info(hwv)
    logger.info(sw_major)
    logger.info(sw_minor)
    logger.info(vtarget)
    #logger.info(topcard)
    return True

  def read_sig_bytes(self):
    pass
    #global logger
    #buf = bytearray(16)
    #buf[0] = CMD_SPI_MULTI;
    #buf[1] = Sync_CRC_EOP;
    #stk500_cmd();
    #logger.info(res)

  def enable(self):
    self.stk500v2_cmd([0x30, 0x00, 0x00, 0x00])
    self.stk500v2_cmd([0x30, 0x00, 0x01, 0x00])
    self.stk500v2_cmd([0x30, 0x00, 0x02, 0x00])

    self.stk500v2_cmd([0x50, 0x00, 0x00, 0x00])
    self.stk500v2_cmd([0x50, 0x00, 0x00, 0x00])
    self.stk500v2_cmd([0x50, 0x00, 0x00, 0x00])

    self.stk500v2_cmd([0x58, 0x08, 0x00, 0x00])
    self.stk500v2_cmd([0x58, 0x08, 0x00, 0x00])
    self.stk500v2_cmd([0x58, 0x08, 0x00, 0x00])

    self.stk500v2_cmd([0x50, 0x08, 0x00, 0x00])
    self.stk500v2_cmd([0x50, 0x08, 0x00, 0x00])
    self.stk500v2_cmd([0x50, 0x08, 0x00, 0x00])

    self.stk500v2_cmd([0xA0, 0x0F, 0xFC, 0x00])
    self.stk500v2_cmd([0xA0, 0x0F, 0xFD, 0x00])
    self.stk500v2_cmd([0xA0, 0x0F, 0xFE, 0x00])
    self.stk500v2_cmd([0xA0, 0x0F, 0xFF, 0x00])

  def program_enable(self):
    global logger
    buf = bytearray(16)
    buf[0] = CMD_ENTER_PROGMODE_ISP
    buf[1] = 0xc8
    buf[2] = 0x64
    buf[3] = 0x19
    buf[4] = 0x20
    buf[5] = 0x00
    buf[6] = 0x53
    buf[7] = 0x03
    buf[8] = 0xac
    buf[9] = 0x53
    buf[10] = 0x00
    buf[11] = 0x00
    ret = self.stk500v2_command(buf,12)
    #print(ret)

  def burn(self):
    global logger
    page_size = 256
    load_addr = 0x80000000
    total_len = len(self.buffer)
    logger.info("total_len=%d"%(total_len))
    left = total_len
    addr = 0
    if(total_len % page_size):
      for i in range(page_size- (total_len % page_size)):
        self.buffer.append(0xff)
    while(addr < total_len):
      b=bytearray()
      b.append(CMD_LOAD_ADDRESS)
      b.append((load_addr>>24) & 0xff)
      b.append((load_addr>>16) & 0xff)
      b.append((load_addr>>8) & 0xff)
      b.append((load_addr) & 0xff)
      self.stk500v2_command(b,5)
      
      b=bytearray()
      b.append(CMD_PROGRAM_FLASH_ISP)
      b = b + bytearray([0x01,0x00, 0xc1,0x0a,0x40,0x4c, 0x20,0x00,0x00])
      b = b + self.buffer[addr:addr+page_size]
      self.stk500v2_command(b, page_size+10)
      load_addr += 0x80
      addr = addr + page_size

  def disable(self):
    buf = bytearray(3)
    buf[0] = CMD_LEAVE_PROGMODE_ISP
    buf[1] = 0x01
    buf[2] = 0x01
    self.stk500v2_command(buf, 3)
    self.ser.close()