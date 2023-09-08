import time
import datetime
from pinpong.board import gboard, I2C
import math


class TM1650():
    TM1650_DISPLAY_BASE = 0x34
    TM1650_DCTRL_BASE = 0x24
    TM1650_NUM_DIGITS = 16
    TM1650_MAX_STRING = 128
    TM1650_BIT_ONOFF = 0b00000001
    TM1650_MSK_ONOFF = 0b11111110
    TM1650_BIT_DOT = 0b00000001
    TM1650_MSK_DOT = 0b11110111
    TM1650_BRIGHT_SHIFT = 4
    TM1650_MSK_BRIGHT = 0b10001111
    TM1650_MIN_BRIGHT = 0
    TM1650_MAX_BRIGHT = 7

    TM1650_CDigits = [
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x82, 0x21, 0x00, 0x00, 0x00, 0x00, 0x02, 0x39, 0x0F, 0x00, 0x00, 0x00, 0x40, 0x80, 0x00,
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7f, 0x6f, 0x00, 0x00, 0x00, 0x48, 0x00, 0x53,
        0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x6F, 0x76, 0x06, 0x1E, 0x00, 0x38, 0x00, 0x54, 0x3F,
        0x73, 0x67, 0x50, 0x6D, 0x78, 0x3E, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x00, 0x0F, 0x00, 0x08,
        0x63, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x6F, 0x74, 0x02, 0x1E, 0x00, 0x06, 0x00, 0x54, 0x5C,
        0x73, 0x67, 0x50, 0x6D, 0x78, 0x1C, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x30, 0x0F, 0x00, 0x00
    ]

    def __init__(self, board=None, i2c_addr=0x32, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.i2c_addr = i2c_addr
        self._i2c = I2C(bus_num)
        aNumDigits = 4
        self.iNumDigits = self.TM1650_NUM_DIGITS if aNumDigits > self.TM1650_NUM_DIGITS else aNumDigits
        self.iBuffer = [0 for i in range(self.TM1650_NUM_DIGITS + 1)]
        self.iCtrl = [0 for i in range(self.TM1650_NUM_DIGITS + 1)]
        self.init()

    def display_string(self, astring):
        astring = str(astring)
        astring = list(astring)
        buf = [100, 100, 100, 100, 100]
        k = 0
        slen = 49 if len(astring) > 49 else len(astring)
        cstring = astring
        for i in range(slen):
            if ord(cstring[i]) == 46 and i != 0:
                if i < 5:
                    buf[k] = i
                    k += 1
                for j in range(i, slen - 1):
                    cstring[j] = cstring[j+1]
                slen -= 1
        slen = 4 if slen > 4 else slen
        bstring = cstring
        for i in range(4-slen):
            bstring.insert(0, " ")
        for i in range(self.iNumDigits):
            a = (ord(bstring[i])) & 0b01111111
            dot = (ord(bstring[i])) & 0b10000000
            self.iBuffer[i] = self.TM1650_CDigits[a]
            if a:
                self.write_to_addr(self.TM1650_DISPLAY_BASE + i, self.iBuffer[i] | dot)
            else:
                break
        for i in range(5):
            if buf[i] != 100:
                self.set_dot(buf[i] + 4 - slen, True)


    def set_dot(self, aPos, aState):
        aPos -= 1
        self.iBuffer[aPos] = (self.iBuffer[aPos] & 0x7f) | (0b10000000 if aState else 0)
        self.set_position(aPos, self.iBuffer[aPos])

    def set_position(self, aPos, aValue):
        if aPos < self.iNumDigits:
            self.iBuffer[aPos] = aValue
            self.write_to_addr(self.TM1650_DISPLAY_BASE + aPos, aValue)

    def init(self):
        self.clear()
        self.display_on()

    def clear(self):
        for i in range(self.iNumDigits):
            self.iBuffer[i] = 0
            self.write_to_addr(self.TM1650_DISPLAY_BASE + i, 0)
            time.sleep(0.01)

    def display_on(self):
        for i in range(self.iNumDigits):
            self.iCtrl[i] = (
                self.iCtrl[i] & self.TM1650_MSK_ONOFF) | self.TM1650_BIT_DOT
            self.write_to_addr(self.TM1650_DCTRL_BASE + i, self.iCtrl[i])    #debug wsq
            time.sleep(0.1)

    def write_to_addr(self, addr, value):
        if not isinstance(value, list):
            value = [value]
        self._i2c.writeto(addr, value)

    def read_to_addr(self, lens):
        return self._i2c.readfrom(self.i2c_addr, lens)
