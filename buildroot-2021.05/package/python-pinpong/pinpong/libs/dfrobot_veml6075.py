# -*- coding: utf-8 -*-
import time
from pinpong.board import gboard,I2C

class VEML6075:
  ID_DEFAULT = 0x26
  CONF = 0x00
  CONF_DEFAULT = 0x00
  UVA = 0x07
  UVB = 0x09
  UV_COMP1 = 0x0a
  UV_COMP2 = 0x0b
  ID = 0x0c

  UVA_RESPONSIVITY_100MS = 0.001111
  UVB_RESPONSIVITY_100MS = 0.00125

  UvaResponsivityList = [
    UVA_RESPONSIVITY_100MS / 0.5016286645,
    UVA_RESPONSIVITY_100MS,               
    UVA_RESPONSIVITY_100MS / 2.039087948, 
    UVA_RESPONSIVITY_100MS / 3.781758958, 
    UVA_RESPONSIVITY_100MS / 7.371335505  
  ]

  UvbResponsivityList = [
    UVB_RESPONSIVITY_100MS / 0.5016286645,
    UVB_RESPONSIVITY_100MS,               
    UVB_RESPONSIVITY_100MS / 2.039087948, 
    UVB_RESPONSIVITY_100MS / 3.781758958, 
    UVB_RESPONSIVITY_100MS / 7.371335505  
  ]

  UV_IT_50 = 0x00
  UV_IT_100 = 0x10
  UV_IT_200 = 0x20
  UV_IT_400 = 0x30
  UV_IT_800 = 0x40

  POWER_ON = 0x00
  POWER_OFF = 0x01

  ACTIVE_FORCE_ENABLE = 0x02
  ACTIVE_FORCE_DISABLE = 0x00

  DYNAMIC_NORMAL = 0x00
  DYNAMIC_HIGH = 0x08

  def __init__(self, board=None, bus_num = 1):
    if board is None:
      board = gboard

    self.board = board
    self.i2c = I2C(bus_num)
    self._addr = 0x10
    self._isActiveForceMode = False
    self._UV_IT = self.UV_IT_100 >> 4

  def begin(self):
    _id = self._readReg(self.ID, 2)
    if _id[0] == self.ID_DEFAULT:
      self._writeReg(self.CONF, [self.CONF_DEFAULT & 0xff, self.CONF_DEFAULT >> 8])
      self.setActiveForceMode(self.ACTIVE_FORCE_DISABLE)
      self.setPower(self.POWER_ON)
      self.setIntegrationTime(self.UV_IT_100)
      return True
    return False

  def _writeReg(self, reg, val):
    return self.i2c.writeto_mem(self._addr, reg, val)

  def _readReg(self, reg, read_byte):
    return self.i2c.readfrom_mem(self._addr, reg, read_byte)

  def _writeRegBits(self, reg, field, val):
    buf = self._readReg(reg, 2)
    buf[0] &= field
    buf[0] |= val
    self._writeReg(reg, buf)

  def setPower(self, power):
    """
    @brief Set power
    
    @param power Power status to set
    """
    self._writeRegBits(self.CONF, 0xfe, power)

  def Uvi2mwpcm2(self, Uvi):
    return (Uvi * 2500.0)

  def getPower(self):
    buf = self._readReg(self.CONF, 2)
    return buf[0] & 0x01

  def setActiveForceMode(self, isEnable):
    self._writeRegBits(self.CONF, 0xfd, isEnable)
    if isEnable == self.ACTIVE_FORCE_ENABLE:
      self._isActiveForceMode = True

  def getActiveForceMode(self):
    buf = self._readReg(self.CONF, 2)
    return buf[0] & 0x02

  def trigOneMeasurement(self):
    if self._isActiveForceMode:
      self._writeRegBits(self.CONF, 0xfb, 0x04)

  def setDynamic(self, dynamic):
    self._writeRegBits(self.CONF, 0xf7, dynamic)

  def getDynamic(self):
    buf = self._readReg(self.CONF, 2)
    return buf[0] & 0x08

  def setIntegrationTime(self, t):
    self._writeRegBits(self.CONF, 0x8f, t)
    self._UV_IT = t >> 4

  def getIntegrationTime(self):
    buf = self._readReg(self.CONF, 2)
    return buf[0] & 0x8f

  def readUvaRaw(self):
    buf = self._readReg(self.UVA, 2)
    return buf[0] | (buf[1] << 8)

  def readUvbRaw(self):
    buf = self._readReg(self.UVB, 2)
    return buf[0] | (buf[1] << 8)

  def readUvComp1Raw(self):
    buf = self._readReg(self.UV_COMP1, 2)
    return buf[0] | (buf[1] << 8)
  
  def readUvComp2Raw(self):
    buf = self._readReg(self.UV_COMP2, 2)
    return buf[0] | (buf[1] << 8)

  def getUva(self):
    return (self.readUvaRaw() - ((2.22 * 1.0 * self.readUvComp1Raw()) / 1.0) - ((1.33 * 1.0 * self.readUvComp2Raw()) / 1.0))

  def getUvb(self):
    return (self.readUvbRaw() - ((2.95 * 1.0 * self.readUvComp1Raw()) / 1.0) - ((1.74 * 1.0 * self.readUvComp2Raw()) / 1.0))

  def getUvi(self, Uva, Uvb):
    Uva = ((Uva * (1.0 / 1.0)) * self.UvaResponsivityList[self._UV_IT])
    Uvb = ((Uvb * (1.0 / 1.0)) * self.UvaResponsivityList[self._UV_IT])
    return (Uva + Uvb) / 2
