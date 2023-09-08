from pinpong.board import gboard,I2C
import time
import math

class COMMON:
  PCA9685_ADDRESS                                 = 0x40
  MODE1                                           = 0x00
  MODE2                                           = 0x01
  PRESCALE                                        = 0xFE
  LED0_ON_L                                       = 0x06
  LED0_ON_H                                       = 0x07
  LED0_OFF_L                                      = 0x08
  LED0_OFF_L                                      = 0x09

  M1                                              = 0x1
  M2                                              = 0x2
  M3                                              = 0x3
  M4                                              = 0x4
  ALL                                             = 0x5

  CW                                              = 1
  CCW                                             = -1

  STP_CHA_L                                       =2047
  STP_CHA_H                                       =4095
  
  STP_CHB_L                                       =1
  STP_CHB_H                                       =2047
  
  STP_CHC_L                                       =1023
  STP_CHC_H                                       =3071
  
  STP_CHD_L                                       =3071
  STP_CHD_H                                       =1023
  
  BYG_CHA_L                                       =3071
  BYG_CHA_H                                       =1023
  
  BYG_CHB_L                                       =1023
  BYG_CHB_H                                       =3071
  
  BYG_CHC_L                                       =4095
  BYG_CHC_H                                       =2047
  
  BYG_CHD_L                                       =2047
  BYG_CHD_H                                       =4095

  M1_M2                                           = 0x1
  M3_M4                                           = 0x2
  def __init__(self, board=None, i2c_addr = 0x40 ,bus_num=0):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard
      
    self.board = board
    self.i2c_addr = 0x40
    self.i2c = I2C(bus_num)

    self.init_PCA9685()

  def set_pwm(self, channel, on, off):
    if channel < 0 or channel > 15:
        return
    buf = [self.LED0_ON_L+4*channel, on & 0xff, (on >> 8)&0xff, off & 0xff, (off>>8) & 0xff]
    self.i2c_write_buf(self.PCA9685_ADDRESS, buf)

  def init_PCA9685(self):
    self.writeReg(self.PCA9685_ADDRESS, self.MODE1, 0x00)
    self.set_freq(50)

  def set_freq(self, freq):
    prescaleval = 25000000
    prescaleval //= 4096
    prescaleval //= freq
    prescaleval -= 1
    prescale = prescaleval
    oldmode = self.readReg(self.PCA9685_ADDRESS, self.MODE1, 1)
    if oldmode != []:
        newmode = (oldmode[0] & 0x7F) | 0x10
        self.writeReg(self.PCA9685_ADDRESS, self.MODE1, newmode)
        self.writeReg(self.PCA9685_ADDRESS, self.PRESCALE, 0x84)
        self.writeReg(self.PCA9685_ADDRESS, self.MODE1, oldmode[0])
        time.sleep(0.005)
        self.writeReg(self.PCA9685_ADDRESS, self.MODE1, oldmode[0] | 0xa1)
  
  def stepper_stop(self, M):
    for i in range(1 if M == 5 else M, (M -1 if M == 5 else M) + 1):
        self.set_pwm((4-i) * 2, 0, 0)
        self.set_pwm((4-i) * 2 + 1, 0, 0)

  def stop(self):
    for i in range(1 if self.M == 5 else self.M, (self.M -1 if self.M == 5 else self.M) + 1):
        self.set_pwm((4-i) * 2, 0, 0)
        self.set_pwm((4-i) * 2 + 1, 0, 0)

  def readReg(self, address, reg, size):
    time.sleep(0.02)
    if reg == 0x1E:
      data = [0, 0x06]
      self.i2c.writeto_mem(address, reg, data)
      result = self.i2c.readfrom(address,size)
    else:
      result = self.i2c.readfrom_mem(address, reg, size)
    return result

  def i2c_write_buf(self, addr, p):
    self.i2c.writeto(addr, p)

  def writeReg(self, address, reg, val):
    if not isinstance(val, list):
        data = [val]
    self.i2c.writeto_mem(address, reg, data)

class DFMotor(COMMON):
  PCA9685_ADDRESS                                 = 0x40
  MODE1                                           = 0x00
  MODE2                                           = 0x01
  PRESCALE                                        = 0xFE
  LED0_ON_L                                       = 0x06
  LED0_ON_H                                       = 0x07
  LED0_OFF_L                                      = 0x08
  LED0_OFF_L                                      = 0x09

  M1                                              = 0x1
  M2                                              = 0x2
  M3                                              = 0x3
  M4                                              = 0x4
  ALL                                             = 0x5

  CW                                              = 1
  CCW                                             = -1

  STP_CHA_L                                       =2047
  STP_CHA_H                                       =4095
  
  STP_CHB_L                                       =1
  STP_CHB_H                                       =2047
  
  STP_CHC_L                                       =1023
  STP_CHC_H                                       =3071
  
  STP_CHD_L                                       =3071
  STP_CHD_H                                       =1023
  
  BYG_CHA_L                                       =3071
  BYG_CHA_H                                       =1023
  
  BYG_CHB_L                                       =1023
  BYG_CHB_H                                       =3071
  
  BYG_CHC_L                                       =4095
  BYG_CHC_H                                       =2047
  
  BYG_CHD_L                                       =2047
  BYG_CHD_H                                       =4095

  M1_M2                                           = 0x1
  M3_M4                                           = 0x2

  def __init__(self, M):
    COMMON.__init__(self)
    self.speeds = 200
    self.M = M
  
  def speed(self, spe = 200):
    self.speeds = spe

  def run(self, direction):
    speed = self.speeds * 16 * direction
    if speed >= 4096:
        speed = 4096
    if speed <= -4096:
        speed = -4096
    if self.M > 5 or self.M <= 0:
        return 
    for i in range(1 if self.M == 5 else self.M, (self.M -1 if self.M == 5 else self.M) + 1):
        pn = (4 - i) * 2
        pp = (4 - i) * 2 + 1
        if speed >= 0:
            self.set_pwm(pp, 0, speed)
            self.set_pwm(pn, 0, 0)
        else:
            self.set_pwm(pp ,0, 0)
            self.set_pwm(pn, 0, -speed)

class DFServo(COMMON):
  
  map = [1, 8, 7, 6, 5, 4, 3, 2, 1]

  def __init__(self, servo):
    COMMON.__init__(self)
    self.servo = self.map[servo]

  def angle(self, degree):
    if degree >= 180:
      degree = 180
    if degree < 0:
      degree = 0
    v_us = degree * 10 + 600
    value = v_us * 4095 // (1000000 // 50)
    self.set_pwm(self.servo + 7, 0, value)

class DFdriver:
    def __init__(self, board=None, i2c_addr = 0x40 ,bus_num=0):
      if isinstance(board, int):
        i2c_addr = board
        board = gboard
      elif board is None:
        board = gboard
        
      self.board = board
      self.i2c_addr = 0x40
      self.I2C = I2C(bus_num)
      self.i2cW(0x00, 0x00)
      self.freq(100)

    def i2cW(self, reg, value):
        buf = bytearray(2)
        buf[0] = reg
        buf[1] = value
        self.I2C.writeto(0x40,buf)

    def i2cR(self, reg):
        buf = bytearray(1)
        buf[0] = reg
        self.I2C.writeto(0x40,buf)
        return self.I2C.readfrom(0x40,1)

    def freq(self, freq):
        pre = math.floor(((25000000/4096/(freq * 0.915))-1) + 0.5)
        oldmode = self.i2cR(0x00)
        self.i2cW(0x00, (oldmode[0] & 0x7F) | 0x10)
        self.i2cW(0xFE, pre)
        self.i2cW(0x00, oldmode[0])
        time.sleep(0.005)
        self.i2cW(0x00, oldmode[0] | 0xa1)

    def pwm(self, channel, on, off):
        if ((channel < 0) or (channel > 15)):
            return
        buf = bytearray(5)
        buf[0] = 0x06 + 4 * channel
        buf[1] = on & 0xff
        buf[2] = (on >> 8) & 0xff
        buf[3] = off & 0xff
        buf[4] = (off >> 8) & 0xff
        self.I2C.writeto(0x40,buf)

    def motorStop(self, Motors):
            self.pwm((4 - Motors) * 2, 0, 0);
            self.pwm((4 - Motors) * 2 + 1, 0, 0);

    def setStepper(self, number, dir):
        if(number == 1):
            if dir:
                buf = bytearray([7,6,5,4])
            else:
                buf = bytearray([5,4,7,6])
        else:
            if dir:
                buf = bytearray([3,2,1,0])
            else:
                buf = bytearray([1,0,3,2])
        self.pwm(buf[0], 3071, 1023)
        self.pwm(buf[1], 1023, 3071)
        self.pwm(buf[2], 4095, 2047)
        self.pwm(buf[3], 2047, 4095)

class DFStepper:
    def __init__(self, Ste):
        self.CW = 1
        self.CCW = -1
        self._ste = Ste
        self._dri = DFdriver()
        self._speed = 0
        self.freq = 100

    def angle(self, ang, dir):
        self._dri.setStepper(self._ste, 1 if dir>0 else 0)
        if(not ang):
            return
        a = math.floor(( 50000 * ang ) / (360 * self.freq))
        time.sleep(a / 1000)
        if (self._ste == 1):
            self._dri.motorStop(1)
            self._dri.motorStop(2)
        else:
            self._dri.motorStop(3)
            self._dri.motorStop(4)

    def circle(self, num, dir):
        _ang = num * 360
        self.angle(_ang, dir)

    def stop(self):
        if (self._ste == 1):
            self._dri.motorStop(1)
            self._dri.motorStop(2)
        else:
            self._dri.motorStop(3)
            self._dri.motorStop(4)