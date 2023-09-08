# -*- coding: utf-8 -*-
import os
import re
import sys
import time
import subprocess

from pinpong.base.programmer import *

class DfuUtil(Programmer):
  def __init__(self, port="", baudrate=115200):
    super().__init__(port, baudrate)

  def setup(self):
    print("DfuUtil setup")
    
  def initialize(self):
    global logger
    print("initialize")

  def display(self):
    return True

  def read_sig_bytes(self):
    pass

  def enable(self):
    pass

  def program_enable(self):
    global logger
    pass

  def burn(self):
    print(self.filename)
    print("stm32flash -w %s -v -g 0x08000000 /dev/ttyS3"%(self.filename))
    if not os.path.exists("/sys/class/gpio/gpio80"):
       os.system("echo 80 > /sys/class/gpio/export")#RST
    if not os.path.exists("/sys/class/gpio/gpio69"):
      os.system("echo 69 > /sys/class/gpio/export")#BOOT0
    os.system("echo out > /sys/class/gpio/gpio69/direction")
    os.system("echo out > /sys/class/gpio/gpio80/direction")
    os.system("echo 0 > /sys/class/gpio/gpio69/value")
    os.system("echo 1 > /sys/class/gpio/gpio80/value")
    time.sleep(0.1)
    os.system("echo 1 > /sys/class/gpio/gpio69/value")#BOOT0=HIGH
    time.sleep(0.1)
    os.system("echo 0 > /sys/class/gpio/gpio80/value")#RST=LOW
    time.sleep(0.1)
    os.system("echo 1 > /sys/class/gpio/gpio80/value")#RST = HIGH
    time.sleep(0.1)
    os.system("echo 0 > /sys/class/gpio/gpio69/value")#BOOT0=LOW
    time.sleep(1)
    os.system("stm32flash -w %s -v -g 0x08000000 /dev/ttyS3"%(self.filename))
    os.system("echo 0 > /sys/class/gpio/gpio80/value")#RST=LOW
    time.sleep(0.2)
    os.system("echo 1 > /sys/class/gpio/gpio80/value")#RST = HIGH
    time.sleep(2)
  def disable(self):
    pass
