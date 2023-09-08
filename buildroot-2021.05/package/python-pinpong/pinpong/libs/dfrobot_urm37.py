# -*- coding: utf-8 -*-
import time
from pinpong.board import gboard, UART

class URM37:
  def __init__(self, board=None, tty_name="/dev/ttyS0", baud_rate=9600):
    if isinstance(board, str):
      tty_name = board
      board = gboard
    elif board is None:
      board = gboard

    self.board = board
    self.uart = UART(tty_name, baud_rate)

  def temp_c(self):
    cmd = [0x11,0x00,0x00,0x11]
    self.uart.write(cmd)
    time.sleep(0.1)
    rslt = self.uart.read(4)
    if (rslt[0] == 0x11) and (rslt[0]+rslt[1]+rslt[2] == rslt[3]):
      return ((rslt[1]&0x0F)*256 + rslt[2])/10
    else:
      return 0

  def distance_cm(self):
    cmd=[0x22,0x00,0x00,0x22]
    self.uart.write(cmd)
    time.sleep(0.1)
    if rslt[0] == 0x22:
      return (rslt[1]*256 + rslt[2])/10

  def servo_angle(self,angle):
    cmd=[0x22,0x00,0x00,0x22]
    self.uart.write(cmd)
    time.sleep(0.1)
    if rslt[0] == 0x22:
      return rslt[1]*256 + rslt[2]
