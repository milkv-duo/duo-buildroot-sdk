# -*- coding: utf-8 -*-
import time
import math
from pinpong.board import Pin,gboard,RPiI2C

#I2C address
PCA9685_ADDRESS    = 0x40
# Registers
MODE1              = 0x00
MODE2              = 0x01
SUBADR1            = 0x02
SUBADR2            = 0x03
SUBADR3            = 0x04
PRESCALE           = 0xFE
LED0_ON_L          = 0x06
#LED1__ON_L        = 0x0A (0x06+4*1)
#LED2__ON_L        = 0x0E (0x06+4*2)
#LED3__ON_L        = 0x12 (0x06+4*3)
#LED4__ON_L        = 0x16 (0x06+4*4)
#...
LED0_ON_H          = 0x07
LED0_OFF_L         = 0x08
LED0_OFF_H         = 0x09
ALL_LED_ON_L       = 0xFA
ALL_LED_ON_H       = 0xFB
ALL_LED_OFF_L      = 0xFC
ALL_LED_OFF_H      = 0xFD


RESTART            = [0x80]
SLEEP              = [0x10]
ALLCALL            = [0x01]
INVRT              = [0x10]
OUTDRV             = [0x04]

frequency          = 50
MIN_PULSE_WIDTH     =  650
MAX_PULSE_WIDTH     =  2350

class PCA9685:

  def __init__(self, board,channel, i2c_addr=PCA9685_ADDRESS,bus_num=1):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board == None:
      board = gboard

    self.i2c_addr = i2c_addr
    self.i2c = RPiI2C(board,bus_num)
    self.channel = channel

    self.set_all_pwm(0,0)
    #self.i2c.i2c.write_byte_data(self.i2c_addr, MODE2, OUTDRV)  
    self.i2c.writeto_mem(self.i2c_addr, MODE2, OUTDRV)  
    #self.i2c.i2c.write_byte_data(self.i2c_addr, MODE1, ALLCALL)   
    self.i2c.writeto_mem(self.i2c_addr, MODE1, ALLCALL) 
    time.sleep(0.005)
    mode1 =self.i2c.readfrom_mem_restart_transmission(self.i2c_addr, MODE1, 1)[0] 
    mode1 = mode1 & ~SLEEP[0]
    #self.i2c.i2c.write_byte_data(self.i2c_addr, MODE1, mode1)  
    self.i2c.writeto_mem(self.i2c_addr, MODE1, [mode1])  
    time.sleep(0.005) 
    self._set_pwm_freq(frequency)

  def _set_pwm_freq(self, freq_hz):
    prescaleval = 25000000.0    # 25MHz
    prescaleval /= 4096.0       # 12-bit
    prescaleval /= float(freq_hz)
    prescaleval -= 1.0
    prescale = int(math.floor(prescaleval + 0.5))
    oldmode =self.i2c.readfrom_mem_restart_transmission(self.i2c_addr, MODE1, 1)[0] 
    newmode = (oldmode & 0x7F) | 0x10   
    #self.i2c.i2c.write_byte_data(self.i2c_addr, MODE1, newmode)  
    self.i2c.writeto_mem(self.i2c_addr,MODE1, [newmode])
    #self.i2c.i2c.write_byte_data(self.i2c_addr, PRESCALE, prescale)
    self.i2c.writeto_mem(self.i2c_addr,PRESCALE, [prescale])
    #self.i2c.i2c.write_byte_data(self.i2c_addr, MODE1, oldmode)
    self.i2c.writeto_mem(self.i2c_addr,MODE1, [oldmode])
    time.sleep(0.005)
    #self.i2c.i2c.write_byte_data(self.i2c_addr, MODE1, oldmode | 0x80)
    self.i2c.writeto_mem(self.i2c_addr, MODE1, [oldmode | 0x80])

  def set_all_pwm(self, on, off):
    #self.i2c.i2c.write_byte_data(self.i2c_addr, ALL_LED_ON_L, on & 0xFF)
    self.i2c.writeto_mem(self.i2c_addr,ALL_LED_ON_L, [on & 0xFF])
    #self.i2c.i2c.write_byte_data(self.i2c_addr, ALL_LED_ON_H, on >> 8)
    self.i2c.writeto_mem(self.i2c_addr,ALL_LED_ON_H, [on >> 8])
    #self.i2c.i2c.write_byte_data(self.i2c_addr, ALL_LED_OFF_L, off & 0xFF)
    self.i2c.writeto_mem(self.i2c_addr,ALL_LED_OFF_L, [off & 0xFF])
    #self.i2c.i2c.write_byte_data(self.i2c_addr, ALL_LED_OFF_H, off >> 8)
    self.i2c.writeto_mem(self.i2c_addr,ALL_LED_OFF_H, [off >> 8])


  def set_pwm(self, on, off):
    channel = self.channel
    #self.i2c.i2c.write_byte_data(self.i2c_addr, LED0_ON_L+4*channel, on & 0xFF)
    self.i2c.writeto_mem(self.i2c_addr,LED0_ON_L+4*channel, [on & 0xFF])
    #self.i2c.i2c.write_byte_data(self.i2c_addr, LED0_ON_H+4*channel, on >> 8)
    self.i2c.writeto_mem(self.i2c_addr,LED0_ON_H+4*channel, [on >> 8])
    #self.i2c.i2c.write_byte_data(self.i2c_addr, LED0_OFF_L+4*channel, off & 0xFF)
    self.i2c.writeto_mem(self.i2c_addr,LED0_OFF_L+4*channel, [off & 0xFF])
    #self.i2c.i2c.write_byte_data(self.i2c_addr, LED0_OFF_H+4*channel, off >> 8)
    self.i2c.writeto_mem(self.i2c_addr,LED0_OFF_H+4*channel, [off >> 8])


  def angle(self, degree):
    channel = self.channel
    pulse_wide = (degree/180)*(MAX_PULSE_WIDTH-MIN_PULSE_WIDTH)+MIN_PULSE_WIDTH
    pulse_width = int(float(pulse_wide) / 1000000 * frequency * 4096)
    self.set_pwm(0, pulse_width)
   

  def bright(self, brightness):#0-255
    channel = self.channel
    brightness = (brightness/255)*4096
    self.set_pwm(0, int(brightness))

