# -*- coding: utf-8 -*-
import time
from pinpong.board import Pin
import RPi.GPIO as GPIO

'''
CS = 21
Clock = 26
Address = 16
DataOut = 20
Button = 19
'''

class TLC:

  def __init__(self, board, pin):
    GPIO.setmode(GPIO.BCM)
    #self.test = Pin(Pin.D17, Pin.OUT)
    self.CS = Pin(Pin.D21, Pin.OUT)
    #self.CS = 21
    #GPIO.setup(self.CS,GPIO.OUT)
    self.Clock = Pin(Pin.D26, Pin.OUT)
    #self.Clock = 26
    #GPIO.setup(self.Clock,GPIO.OUT)    
    self.Address = Pin(Pin.D16, Pin.OUT)
    #self.Address = 16
    #GPIO.setup(self.Address,GPIO.OUT)  
    self.DataOut = Pin(Pin.D20, Pin.IN)
    #self.DataOut = 20
    #GPIO.setup(self.DataOut,GPIO.IN,GPIO.PUD_UP)
    self.Button = Pin(Pin.D19, Pin.IN)
    #self.Button = 19
    #GPIO.setup(self.Button,GPIO.IN,GPIO.PUD_UP)    
    self.numSensors = 11
    self.pin = pin

  def analogRead(self):
    value = [0]*(self.numSensors+1)
    for j in range(0,self.numSensors+1):
      self.CS.value(0)
      #GPIO.output(self.CS, GPIO.LOW)
      for i in range(0,4):
        #sent 4-bit Address
        if(((j) >> (3 - i)) & 0x01):
          self.Address.value(1)
          #GPIO.output(self.Address, GPIO.HIGH)
        else:
          self.Address.value(0)
          #GPIO.output(self.Address, GPIO.LOW)
		#read MSB 4-bit data
        value[j] <<= 1
        if(self.DataOut.value()):
        #if(GPIO.input(self.DataOut)):
          value[j] |= 0x01
        self.Clock.value(1)
        self.Clock.value(0)
        #GPIO.output(self.Clock, GPIO.HIGH)
        #GPIO.output(self.Clock, GPIO.LOW)
      for i in range(0,6):
		#read LSB 8-bit data
        value[j] <<= 1
        if(self.DataOut.value()):
        #if(GPIO.input(self.DataOut)):
          value[j] |= 0x01
        self.Clock.value(1)
        self.Clock.value(0)
        #GPIO.output(self.Clock, GPIO.HIGH)
        #GPIO.output(self.Clock, GPIO.LOW)
      time.sleep(0.0001)
      self.CS.value(1)
      #GPIO.output(self.CS, GPIO.HIGH)
    #print (value[1:])
    return value[self.pin+1]



