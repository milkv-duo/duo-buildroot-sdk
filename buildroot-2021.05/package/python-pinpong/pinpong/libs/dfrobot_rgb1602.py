# -*- coding: utf-8 -*-
import time
from pinpong.board import gboard,I2C

class RGB1602:
  LCD_FUNCTIONSET         = 0x20
  LCD_CLEARDISPLAY        = 0x01
  LCD_RETURNHOME          = 0x02
  LCD_CURSORSHIFT         = 0x10
  LCD_2LINE               = 0x08
  LCD_DISPLAYON           = 0x04
  LCD_DISPLAYCONTROL      = 0x08
  LCD_CURSOROFF           = 0x00
  LCD_BLINKOFF            = 0x00
  LCD_ENTRYMODESET        = 0x04
  LCD_ENTRYLEFT           = 0x02
  LCD_ENTRYSHIFTDECREMENT = 0x00
  
  LCD_DISPLAYMOVE         = 0x08
  LCD_CURSORMOVE          = 0x00
  LCD_MOVERIGHT           = 0x04
  LCD_MOVELEFT            = 0x00
  
  REG_MODE1               = 0x00
  REG_MODE2               = 0x01
  REG_OUTPUT              = 0x08
  REG_RED                 = 0x04
  REG_GREEN               = 0x03
  REG_BLUE                = 0x02

  def __init__(self, board=None, col=16, row=2,bus_num=0):
    if board is None:
      board = gboard
      
    self.board = board
    self.i2c = I2C(bus_num)
    for cmd in(
    self.LCD_FUNCTIONSET | self.LCD_2LINE,
    self.LCD_DISPLAYCONTROL | self.LCD_DISPLAYON | self.LCD_CURSOROFF | self.LCD_BLINKOFF,
    self.LCD_CLEARDISPLAY,
    self.LCD_ENTRYMODESET | self.LCD_ENTRYLEFT | self.LCD_ENTRYSHIFTDECREMENT
    ):
      self.command(cmd)
      time.sleep(0.1)
    self.set_reg(self.REG_MODE1, 0)
    self.set_reg(self.REG_OUTPUT, 0xFF)
    self.set_reg(self.REG_MODE2, 0x20)
        
  def command(self,cmd):
    b=bytearray(2)
    b[0]=0x80
    b[1]=cmd
    self.i2c.writeto(62,b)

  def write(self,data):
    b=bytearray(2)
    b[0]=0x40
    b[1]=data
    self.i2c.writeto(62,b)

  def set_reg(self,reg,data):
    b=bytearray(1)
    b[0]=data
    self.i2c.writeto_mem(96,reg,b)

  def set_rgb(self,r,g,b):
    self.set_reg(self.REG_RED,  r)
    self.set_reg(self.REG_GREEN,  g)
    self.set_reg(self.REG_BLUE,  b)

  def set_cursor(self,col,row):
    if(row == 0):
      col|=0x80
    else:
      col|=0xc0;
    self.command(col)

  def print(self,arg):
    if(isinstance(arg,int)):
      arg=str(arg)

    for x in bytearray(arg,"utf-8"):
      self.write(x)

  def clear(self):
    self.command(self.LCD_CLEARDISPLAY)
    time.sleep_ms(2)

  def scroll_left(self):
    self.command(self.LCD_CURSORSHIFT | self.LCD_DISPLAYMOVE | self.LCD_MOVELEFT)

  def scroll_right(self):
    self.command(self.LCD_CURSORSHIFT | self.LCD_DISPLAYMOVE | self.LCD_MOVERIGHT)
