# -*- coding: utf-8 -*
import time
from pinpong.board import gboard,I2C
import math

class MAX17043():
    MAX17043_ADDRESS       = 0x36
    MAX17043_VCELL         = 0x02
    MAX17043_SOC           = 0x04
    MAX17043_MODE          = 0x06
    MAX17043_VERSION       = 0x08
    MAX17043_CONFIG        = 0x0c
    MAX17043_COMMAND       = 0xfe 

    def __init__(self, board = None, i2c_addr = MAX17043_ADDRESS, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.i2c_addr = i2c_addr
        self._i2c = I2C(bus_num)      

    def begin(self):
        self.write16_reg(self.MAX17043_COMMAND, 0x5400)
        time.sleep(0.01)
        if self.read16_reg(self.MAX17043_CONFIG) == 0x971c:
            self.write16_reg(self.MAX17043_MODE, 0x4000)
            self.write16_reg(self.MAX17043_CONFIG, 0x9700)
            time.sleep(0.01)
            return True
        return False

    def read_voltage(self):
        return (1.25 * (self.read16_reg(self.MAX17043_VCELL) >> 4))
    
    def read_percentage(self):
        per = self.read16_reg(self.MAX17043_SOC)
        return round(((per >> 8) + 0.003906 * (per & 0x00ff)), 2)

    def write16_reg(self, reg, value):
        data = [value >> 8, value&0xff]
        self._i2c.writeto_mem(self.i2c_addr, reg, data)
         
    def read16_reg(self, reg, len = 2):
        data = self._i2c.readfrom_mem(self.i2c_addr, reg, len)
        return data[0] << 8 | data[1]
