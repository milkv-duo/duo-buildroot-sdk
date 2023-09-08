# -*- coding: utf-8 -*-
import serial
from pinpong.base.stk500_param import *
from pinpong.base.programmer import *

class STK500(Programmer):
  def __init__(self, port="/dev/ttyS1", baudrate=115200):
    super().__init__(port, baudrate)

  def setup(self):
    print("STK500 setup  %s"%self.port)
    
  def initialize(self):
    global logger
    print("initialize")
    stk500_devcode = 0x86
    sw_major = self.stk500_getparm(Parm_STK_SW_MAJOR) #  0x41 0x81 0x20  return 0x14 0x04 0x10
    sw_minor = self.stk500_getparm(Parm_STK_SW_MINOR) #  0x41 0x82 0x20  return 0x14 0x04 0x10
    buf = bytearray(32)
    #Cmnd_STK_SET_DEVICE     #  42 86 00 00  01 01 01 01  03 ff ff ff  ff 00 80 04  00 00 00 80  00 20
    buf[0] = Cmnd_STK_SET_DEVICE
    buf[1] = stk500_devcode
    buf[2] = 0
    buf[3] = 0
    
    buf[4] = 1
    buf[5] = 1
    buf[6] = 1
    buf[7] = 1
    
    buf[8]  = 0x03 #fuse size
    buf[9]  = 0xFF #
    buf[10] = 0xFF #
    buf[11] = 0xFF #
    
    buf[12] = 0xFF #
    buf[13] = 0x00 #
    buf[14] = 0x80 #
    buf[15] = 0x04 #
    
    buf[16] = 0x00 #
    buf[17] = 0x00 #  
    buf[18] = 0x00 #
    buf[19] = 0x80 #
    buf[20] = 0x00 #
    buf[21] = Sync_CRC_EOP;
    self.ser.write(buf[:22])
    res = self.ser.read(1)
    logger.info(res)
    res = self.ser.read(1)
    logger.info(res)
    
    buf[0] = Cmnd_STK_SET_DEVICE_EXT  #45 05 04 d7  c2 00 20
    buf[1] = 0x05 #n_extparms+1
    buf[2] = 0x04 #page_size;
    buf[3] = 0xD7 #pagel;
        
    buf[4] = 0xC2 #bs2;
    buf[5] = 0x00 #reset_disposition == RESET_DEDICATED
    buf[6] = Sync_CRC_EOP  
    self.ser.write(buf[:7])
    res = self.ser.read(1)
    logger.info(res)
    res = self.ser.read(1)
    logger.info(res)
    self.program_enable()

  def stk500_cmd(self,cmd):
    global logger
    buf = bytearray(16)
    buf[0] = Cmnd_STK_UNIVERSAL
    buf[1] = cmd[0]
    buf[2] = cmd[1]
    buf[3] = cmd[2]
    buf[4] = cmd[3]
    buf[5] = Sync_CRC_EOP
    self.ser.write(buf[:6])
    res = self.ser.read(1)
    logger.info(res)
    res = self.ser.read(1)
    logger.info(res)
    res = self.ser.read(1)
    logger.info(res)

  def stk500_getparm(self,param):
    global logger
    buf = bytearray(16)
    buf[0] = Cmnd_STK_GET_PARAMETER;
    buf[1] = param;
    buf[2] = Sync_CRC_EOP;
    self.ser.write(buf[:3])
    res = self.ser.read(1)
    logger.info(res)
    ret = self.ser.read(1)
    logger.info(ret)
    res = self.ser.read(1)
    logger.info(res)
    return ret

  def display(self):
    global logger,baudrate
    self.ser=serial.Serial(self.port, self.baudrate, timeout=0.5)
    retry = 5
  
    while retry != 0:
      retry = retry - 1
      self.ser.read(self.ser.in_waiting)
      b = bytearray()
      b.append(Cmnd_STK_GET_SYNC)
      b.append(Sync_CRC_EOP)
      self.ser.write(bytes(b))
      res = self.ser.read(1)
  
      if(len(res) == 0):
        continue
  
      logger.info(res)
      if(res[0] != Resp_STK_INSYNC):
        continue
      res = self.ser.read(1)
      if(res[0] != Resp_STK_OK):
        continue
      logger.info(res)
      break
    logger.info("retry=%d"%retry)
    
    if retry == 0:
      return False
    
    hwv      = self.stk500_getparm(Parm_STK_HW_VER)
    sw_major = self.stk500_getparm(Parm_STK_SW_MAJOR)
    sw_minor = self.stk500_getparm(Parm_STK_SW_MINOR)
    topcard  = self.stk500_getparm(Param_STK500_TOPCARD_DETECT)
    vtarget  = self.stk500_getparm(Parm_STK_VTARGET)
    vadjust  = self.stk500_getparm(Parm_STK_VADJUST)
    pscale   = self.stk500_getparm(Parm_STK_OSC_PSCALE)
    cmatch   = self.stk500_getparm(Parm_STK_OSC_CMATCH)
    duration = self.stk500_getparm(Parm_STK_SCK_DURATION)
  
    logger.info(hwv)
    logger.info(sw_major)
    logger.info(sw_minor)
    logger.info(topcard)
    
    return True

  def read_sig_bytes(self):
    global logger
    buf = bytearray(16)
    buf[0] = Cmnd_STK_READ_SIGN
    buf[1] = Sync_CRC_EOP
    
    self.ser.write(buf[:2])
    
    res = self.ser.read(1)
    logger.info(res)
    res = self.ser.read(1)
    logger.info(res)
    res = self.ser.read(1)
    logger.info(res)
    res = self.ser.read(1)
    logger.info(res)
    res = self.ser.read(1)
    logger.info(res)

  def enable(self):
    self.stk500_cmd([0x58, 0x08, 0x00, 0x00])
    self.stk500_cmd([0x50, 0x08, 0x00, 0x00])
    self.stk500_cmd([0xa0, 0x03, 0xfc, 0x00])
    self.stk500_cmd([0xa0, 0x03, 0xfd, 0x00])
    self.stk500_cmd([0xa0, 0x03, 0xfe, 0x00])
    self.stk500_cmd([0xa0, 0x03, 0xff, 0x00])
  
  def program_enable(self):
    global logger
    buf = bytearray(16)
    buf[0] = Cmnd_STK_ENTER_PROGMODE
    buf[1] = Sync_CRC_EOP
    
    self.ser.write(buf[:2])
    res = self.ser.read(1)
    logger.info(res)
    res = self.ser.read(1)
    logger.info(res)

  def burn(self):
    global logger
    page_size = 128
    addr = 0
    total_len = len(self.buffer)
    logger.info("total_len=%d"%(total_len))
    while(addr < total_len):
      block_size = page_size if addr+page_size <= total_len else total_len%page_size
      b=bytearray()
      b.append(Cmnd_STK_PROG_PAGE)
      b.append((block_size>>8) & 0xff)
      b.append((block_size) & 0xff)
      b.append(0x46) ######TODO
      b = b + self.buffer[addr:addr+block_size]
      b.append(Sync_CRC_EOP)

      buf = bytearray(4)
      buf[0] = Cmnd_STK_LOAD_ADDRESS
      buf[1] = (addr>>1) & 0xff
      buf[2] = ((addr>>1) >> 8) & 0xff
      buf[3] = Sync_CRC_EOP
      self.ser.write(buf)
      res = self.ser.read(1)
      logger.info(res)
      res = self.ser.read(1)
      logger.info(res)
  
      self.ser.write(bytes(b))
      res = self.ser.read(1)
      logger.info(res)
      res = self.ser.read(1)
      logger.info(res)
  
      addr = addr + block_size

  def disable(self):
    buf = bytearray(2)
    buf[0] = Cmnd_STK_LEAVE_PROGMODE;
    buf[1] = Sync_CRC_EOP;
    self.ser.write(buf)
    res = self.ser.read(1)
    logger.info(res)
    res = self.ser.read(1)
    logger.info(res)
    self.ser.close()