# -*- coding: utf-8 -*-

# RPi and PythonBoard
#实验效果：引脚模拟中断功能测试

import time
from pinpong.board import Board,Pin  #控制RPi和PythonBoard本地资源不再需要导入Board

Board("RPi").begin()
btn = Pin(pin=Pin.D26, mode=Pin.IN)

def btn_rasing_handler(pin):#中断事件回调函数
  print("\n--rising---")
  print("pin = ", pin)
  
def btn_falling_handler(pin):#中断事件回调函数
  print("\n--falling---")
  print("pin = ", pin)

def btn_both_handler(pin):#中断事件回调函数
  print("\n--both---")
  print("pin = ", pin)

btn.irq(trigger=Pin.IRQ_FALLING, handler=btn_falling_handler) #设置中断模式为下降沿触发
#btn.irq(trigger=Pin.IRQ_RISING, handler=btn_rasing_handler) #设置中断模式为上升沿触发，及回调函数
#btn.irq(trigger=Pin.IRQ_RISING+Pin.IRQ_FALLING, handler=btn_both_handler) #设置中断模式为电平变化时触发

while True:
  time.sleep(1)