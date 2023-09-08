import time
from pinpong.board import gboard, I2C
import math


class RGBpanel():
    _RGBAddr = 0x10
    FUNC = 0x02
    COLOR = 0x03
    PIX_X = 0x04
    PIX_Y = 0x05
    BITMAP = 0x06
    STR = 0x07

    UNCLEAR = 0x0
    CLEAR = 0x1
    Left = (0x0 << 1)
    Right = (0x1 << 1)
    none = (0x11)
    UNSCROLL = (0x0 << 2)
    SCROLL = (0x1 << 2)
    PIX_ENABLE = (0x01 << 3)
    BITMAP_ENABLE = (0x10 << 3)
    STR_ENABLE = (0x11 << 3)

    QUENCH = 0
    RED = 1
    GREEN = 2
    YELLOW = 3
    BLUE = 4
    PURPLE = 5
    CYAN = 6
    WHITE = 7

    SIZE = 12

    def __init__(self, board=None, i2c_addr=0x10, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.i2c_addr = i2c_addr
        self._i2c = I2C(bus_num)
        self.buf = [0 for i in range(self.SIZE)]

    def scroll(self, dir):
        if dir != None and dir != self.Right and dir != self.Left:
            return
        if dir == None:
            self.buf[0] &= (~(0x01 << 2))
        elif dir == self.Right:
            self.buf[0] |= (0x01 << 2) | (0x01 << 1)
        elif dir == self.Left:
            self.buf[0] |= (0x01 << 2)
            self.buf[0] &= (~(0x01 << 1))
        else:
            return

    def fillScreen(self, color):
        self.buf[0] = 0x1
        self.buf[0] = self.buf[0] = (self.buf[0] & (0xe7)) | (0x01 << 3)
        self.buf[1] = color
        self.buf[2] = 0
        self.buf[3] = 0
        self.write_reg(0x02, self.buf)
        time.sleep(0.1)

    def print(self, val, color):
        val = str(val)
        lens = len(val)
        if lens > 7:
            lens = 7
        self.buf[0] = (self.buf[0] & (0xe6))| (0x03 << 3)
        self.buf[1] = color
        for i in range(lens):
            self.buf[5+i] = ord(val[i])
#        self.buf[5+lens] = 0
        self.write_reg(0x02, self.buf)
        time.sleep(0.1)
    
    def pixel(self, x,  y, color):
        self.buf[0] = (self.buf[0] & (0xe6)) | (0x01 << 3)
        self.buf[1] = color
        self.buf[2] = x
        self.buf[3] = y
        self.write_reg(0x02, self.buf)
        time.sleep(0.01)

    def display(self, picIndex, color):
        self.buf[0] = (self.buf[0] & (0xe6)) | (0x02 << 3)
        self.buf[1] = color
        self.buf[4] = picIndex
        self.write_reg(0x02, self.buf)
        time.sleep(0.1)

    def clear(self):
        self.buf[0] = 0x01
        for i in range(1,self.SIZE):
            self.buf[i] = 0
        self.write_reg(0x02, self.buf)
        time.sleep(0.1)
        
    def write_reg(self, reg, value):
        self._i2c.writeto_mem(self.i2c_addr, reg, value)
         
