# -*- coding: utf-8 -*-
import os
import fcntl
import termios
import sys
import ctypes
import time
from ctypes import c_int

MY_IOCTL_RESET = 25345  # reset gd32V

STATE_NONE = 0
STATE_GPIO = 1
STATE_IIC = 2
STATE_SPI = 4
STATE_UART = 8
STATE_PWM = 16
STATE_ADC = 32

pin_default_state = [
    STATE_GPIO,         #> PA0   |        |  ADC_0  |  tim4_0 pwm  |
    STATE_GPIO,          #> PA1   |        |  ADC_1  |  tim4_1 pwm  |
    STATE_GPIO,         #> PA2   |  uart1 |  ADC_2  |  tim4_2 pwm  | 不测
    STATE_GPIO,         #> PA3   |  uart1 |  ADC_3  |  tim1_3 pwm  | 不测
    STATE_GPIO,          #> PA4   |        |  ADC_4  |              |
    STATE_GPIO,          #> PA5   |  spi0  |  ADC_5  |              |
    STATE_GPIO,          #> PA6   |  spi0  |  ADC_6  |  tim2_0 pwm  |
    STATE_GPIO,          #> PA7   |  spi0  |  ADC_7  |  tim2_1 pwm  |
    STATE_GPIO,         #> PA8   |        |         |  tim0_0 pwm  |
    STATE_UART,         #> PA9   |  uart0 |         |              |  通信接口不可更改
    STATE_UART,         #> PA10  |  uart0 |         |              |  通信接口不可更改
    STATE_GPIO,         #> PA11  |        |         |  tim0_3 pwm  |
    STATE_GPIO,         #> PA12  |        |         |              |
    STATE_GPIO,         #> PA13  |        |         |              |
    STATE_GPIO,         #> PA14  |        |         |              |
    STATE_GPIO,         #> PA15  |        |         |              |
    STATE_GPIO,          #> PB0   |        |  ADC_8  |  tim2_2 pwm  |
    STATE_GPIO,          #> PB1   |        |  ADC_9  |  tim2_3 pwm  |
    STATE_GPIO,         #> PB2   |        |         |              |
    STATE_GPIO,         #> PB3   |        |         |  tim1_1 pwm  |
    STATE_GPIO,         #> PB4   |        |         |  tim2_0 pwm  |
    STATE_GPIO,         #> PB5   |        |         |  tim2_1 pwm  |
    STATE_GPIO,         #> PB6   |        |         |  tim3_0 pwm  | 
    STATE_GPIO,         #> PB7   |        |         |  tim3_1 pwm  |
    STATE_GPIO,         #> PB8   |        |         |  tim3_2 pwm  |
    STATE_GPIO,         #> PB9   |        |         |  tim3_3 pwm  |
    STATE_IIC,          #> PB10  |  iic1  |         |  tim1_2 pwm  |
    STATE_IIC,          #> PB11  |  iic1  |         |  tim1_3 pwm  |
    STATE_GPIO,         #> PB12  |        |         |              |
    STATE_GPIO,         #> PB13  |  spi1  |         |              |
    STATE_GPIO,         #> PB14  |  spi1  |         |              |
    STATE_GPIO          #> PB15  |  spi1  |         |              |
]
class IoctlParams(ctypes.Structure):
    _fields_ = [("mode", c_int) ,
                ("num",  c_int)]  # 根据实际情况定义字段类型和名称



def pinConfig():
    fd = os.open("/dev/pinpong_config", os.O_RDWR)
    if fd == -1:
      print("open error")
      return -1
      '''
    params = IoctlParams()
    params.mode = 0x1F
    params.num = 1
  
    ret = fcntl.ioctl(fd, MY_IOCTL_RESET, params)
    time.sleep(1);
    if ret == -1:
      print("reset fail")
      '''
    temp = bytearray(200)
    
    '''
    # Configuring a single IO port
    temp[0] = 2
    temp[1] = STATE_UART
    temp[2] = STATE_UART
    ret = os.write(fd, temp[:3])
    if ret == -1:
        print("io error")
    else:
        print("one io success")
    '''
    # Configuring all IO ports
    temp[0] = 0
    temp[1:33] = bytearray(pin_default_state)
    ret = os.write(fd, temp[:33])
    if ret == -1:
        print("io error")

    
    '''
    temp = os.read(fd, 32)
    if len(temp) == 32:
        print("read success")
        for i in range(32):
            if i % 8 == 0:
                print()
            if temp[i] == STATE_NONE:
                print("none \t", end="")
            elif temp[i] == STATE_GPIO:
                print("gpio \t", end="")
            elif temp[i] == STATE_IIC:
                print("iic  \t", end="")
            elif temp[i] == STATE_SPI:
                print("spi  \t", end="")
            elif temp[i] == STATE_UART:
                print("uart \t", end="")
            elif temp[i] == STATE_PWM:
                print("pwm  \t", end="")
            elif temp[i] == STATE_ADC:
                print("ADC\t", end="")
    '''
    os.close(fd)


