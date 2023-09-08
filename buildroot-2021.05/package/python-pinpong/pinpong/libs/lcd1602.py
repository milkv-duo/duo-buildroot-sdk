# -*- coding: utf-8 -*-
import time
from pinpong.board import gboard,I2C

class LCD1602_I2C:
  def __init__(self, board=None, i2c_addr=0x20,bus_num=0):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard

    self.board = board
    self.i2c_addr = i2c_addr
    self.i2c = I2C(bus_num)
    self.buf = bytearray(1)
    self.BK = 0x08
    self.RS = 0x00
    self.E = 0x04
    self.set_cmd(0x33)
    time.sleep(0.005)
    self.send(0x30)
    time.sleep(0.005)
    self.send(0x20)
    time.sleep(0.005)
    self.set_cmd(0x28)
    self.set_cmd(0x0C)
    self.set_cmd(0x06)
    self.set_cmd(0x01)
    self.version='1.0'
    self.x=0
    self.y=0

  def set_reg(self, dat):
      self.buf[0] = dat
      self.i2c.writeto(self.i2c_addr, self.buf)
      time.sleep(0.001)

  def send(self, dat):
    d=dat&0xF0
    d|=self.BK
    d|=self.RS
    self.set_reg(d)
    self.set_reg(d|0x04)
    self.set_reg(d)

  def set_cmd(self, cmd):
    self.RS=0
    self.send(cmd)
    self.send(cmd<<4)

  def set_data(self, dat):
    self.RS=1
    self.send(dat)
    self.send(dat<<4)

  def clear(self):
    self.set_cmd(1)

  def backlight(self, on):
    if on:
      self.BK=0x08
    else:
      self.BK=0
    self.set_cmd(0)

  def display(self, on):
    if on:
      self.set_cmd(0x0C)
    else:
      self.set_cmd(0x08)

  def scroll_left(self):
    self.set_cmd(0x18)

  def scroll_right(self):
    self.set_cmd(0x1C)

  def set_cursor(self, x, y):
    if x >= 16:
      x=16
    if y >= 2:
      y=1

    self.x = x
    self.y = y

  def char(self, ch):
    if ch == 10 or ch == 13:
      self.y = 1-self.y
      self.x = 0
    elif self.x>=0:
      a=0x80
      if self.y>0:
        a=0xC0
      a+=self.x
      self.set_cmd(a)
      self.x += 1
      if self.x == 16:
        self.x = 0
        self.y = 1-self.y
      self.set_data(ch)

  def print(self, s):
    if(isinstance(s,int)):
      s=str(s)
    if len(s)>0:
      self.char(ord(s[0]))
      for i in range(1, len(s)):
        self.char(ord(s[i]))
