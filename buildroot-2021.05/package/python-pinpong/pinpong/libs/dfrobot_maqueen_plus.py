# -*- coding: utf-8 -*
from textwrap import indent
import time
import math
from pinpong.board import gboard, I2C
from pytest import param


class MqueenPlus(object):
    LEFT = 1
    RIGHT = 2
    ALL = 3

    CW = 1
    CCW = 2

    OFF = 0
    ON = 1

    S1 = 1
    S2 = 2
    S3 = 3
    S1_3 = 4

    RED = 1
    GREEN = 2
    YELLOW = 3
    BLUE = 4
    PINK = 5
    CYAN = 6
    WHITH = 7
    PUT = 8

    L1 = 1
    L2 = 2
    R1 = 3
    R2 = 4
    L3 = 5
    R3 = 6

    def __init__(self):
        self.first = True

    def hardware_reset(self):
        if self.first:
            return
        buf = [0x00, 0, 0, 0, 0]
        self.write_cmd(buf)
        buf = [0x0B, 8, 8]
        self.write_cmd(buf)
        self.first = False

    def run(self, index, direction, speed):
        self.hardware_reset()
        _speed = abs(speed)
        if _speed > 255:
            _speed = 255
        if index > 3 or index < 1:
            return
        if index == 1:
            buf = [0x00, direction, _speed]
            self.write_cmd(buf)
        elif index == 2:
            buf = [0x02, direction, _speed]
            self.write_cmd(buf)
        elif index == 3:
            buf = [0x00, direction, _speed, direction, _speed]
            self.write_cmd(buf)

    def stop(self, index):
        self.hardware_reset()
        self.run(index, self.CW, 0)

    def write_pid(self, pid):
        self.hardware_reset()
        if not isinstance(pid, int):
            return
        buf = [0x0A, pid]
        self.write_cmd(buf)

    def read_speed(self, index):
        self.hardware_reset()
        recbuf = self.read_cmd(0x00, 4)
        if index == 1:
            if recbuf[1] < 20 and recbuf[1] != 0:
                return recbuf[1] + 255
            else:
                return recbuf[1]
        elif index == 2:
            if recbuf[3] < 20 and recbuf[3] != 0:
                return recbuf[3] + 255
            else:
                return recbuf[3]

    def read_direction(self, index):
        self.hardware_reset()
        recbuf = self.read_cmd(0x00, 4)
        if index == 1:
            return recbuf[0]
        elif index == 2:
            return recbuf[2]

    def servo(self, index, angle):
        self.hardware_reset()
        buf = [0, 0]
        if not isinstance(angle, int):
            return
        if angle > 180:
            angle = 180
        if angle < 0:
            angle = 0
        buf[1] = angle
        if index == 1:
            buf[0] = 0x14
        elif index == 2:
            buf[0] = 0x15
        elif index == 3:
            buf[0] = 0x16
        elif index == 4:
            buf[0] = 0x14
            self.write_cmd(buf)
            buf[0] = 0x15
            self.write_cmd(buf)
            buf[0] = 0x16
        else:
            return
        self.write_cmd(buf)

    def set_rgb(self, rgb, color):
        self.hardware_reset()
        if rgb == 1:
            buf = [0, 0]
            buf[0] = 0x0B
            buf[1] = color
            self.write_cmd(buf)
        elif rgb == 2:
            buf = [0, 0]
            buf[0] = 0x0C
            buf[1] = color
            self.write_cmd(buf)
        elif rgb == 3:
            buf = [0, 0, 0]
            buf[0] = 0x0B
            buf[1] = color
            buf[2] = color
            self.write_cmd(buf)

    def read_patrol(self, patrol):
        self.hardware_reset()
        y = self.read_cmd(0x1D, 1)
        if patrol == 1:
            mark = 1 if (y[0] & 0x04) == 0x04 else 0
        elif patrol == 2:
            mark = 1 if (y[0] & 0x02) == 0x02 else 0
        elif patrol == 3:
            mark = 1 if (y[0] & 0x08) == 0x08 else 0
        elif patrol == 4:
            mark = 1 if (y[0] & 0x10) == 0x10 else 0
        elif patrol == 5:
            mark = 1 if (y[0] & 0x01) == 0x01 else 0
        elif patrol == 5:
            mark = 1 if (y[0] & 0x20) == 0x20 else 0
        return mark

    def read_patrol_voltage(self, patrol):
        self.hardware_reset()
        y = self.read_cmd(0x1E, 12)
        if patrol == 1:
            return y[5] | y[4] << 8
        elif patrol == 2:
            return y[3] | y[2] << 8
        elif patrol == 3:
            return y[7] | y[6] << 8
        elif patrol == 4:
            return y[9] | y[8] << 8
        elif patrol == 5:
            return y[1] | y[0] << 8
        else:
            return y[11] | y[10] << 8
    
    def read_version(self):
        self.hardware_reset()
        len = self.read_cmd(0x32, 1)
        name = self.read_cmd(0x33, len[0])
        return "".join(map(chr, name))
    
    def read_distance(self, index):
        self.hardware_reset()
        y = self.read_cmd(0x04, 4)
        if index == 1:
            return round(((y[0] << 8 | y[1])*10/900), 1)
        else:
            return round(((y[2] << 8 | y[3])*10/900), 1)
    
    def clear_distance(self, index):
        self.hardware_reset()
        if index == 1:
            buf = [0x04, 0]
            self.write_cmd(buf)
        elif index == 2:
            buf = [0x06, 0]
            self.write_cmd(buf)
        elif index == 3:
            buf = [0x04, 0, 0, 0]
            self.write_cmd(buf)

class DFRobot_MqueenPlus(MqueenPlus):
    def __init__(self, board=None, i2c_addr=0x10, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.__addr = i2c_addr
        self._i2c = I2C(bus_num)
        super(DFRobot_MqueenPlus, self).__init__()

    def write_cmd(self, data):
        self._i2c.writeto(self.__addr, data)

    def read_cmd(self, reg, len):
        rslt = self._i2c.readfrom_mem(self.__addr, reg, len)
        return rslt
