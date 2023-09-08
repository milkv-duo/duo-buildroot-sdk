# -*- coding: utf-8 -*
import time
from pinpong.board import gboard, I2C

class DualuseShield(object):
    M1                = 0
    M2                = 1
    ALL               = 2
    CW                = 0
    CCW               = 1
    def __init__(self):
        pass

    def run(self, index, direction, speed):
        _speed = abs(speed)
        if _speed > 255:
            _speed = 255
        buf = [0, direction, _speed]
        if index > 3 or index < 0:
            return
        if index == self.M1:
            buf[0] = 0x00
            self.write_cmd(buf)
        elif index == self.M2:
            buf[0] = 0x02
            self.write_cmd(buf)
        elif index == self.ALL:
            buf[0] = 0x00
            self.write_cmd(buf)
            buf[0] = 0x02
            self.write_cmd(buf)
    
    def stop(self, index):
        if index == self.M1:
            buf = [0, 0, 0]
            self.write_cmd(buf)
        elif index == self.M2:
            buf = [0x02, 0, 0]
            self.write_cmd(buf)
        elif index == self.ALL:
            buf = [0, 0, 0]
            self.write_cmd(buf)
            buf = [0x02, 0, 0]
            self.write_cmd(buf)
        

class DFRobot_DualuseShield(DualuseShield):
    def __init__(self, board=None, i2c_addr=0x10, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.__addr = i2c_addr
        self._i2c = I2C(bus_num)
        super(DFRobot_DualuseShield, self).__init__()

    def write_cmd(self, data):
        self._i2c.writeto(self.__addr, data)

    def read_cmd(self, len):
        rslt = self._i2c.readfrom(self.__addr, len)
        return rslt

