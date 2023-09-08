import time
from pinpong.board import gboard,I2C
import math

class DHT20():
    def __init__(self, board = None, i2c_addr = 0x38, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.i2c_addr = i2c_addr
        self._i2c = I2C(bus_num)
        self.wait_flag =False
        self.soft_reset_dth20()
        state = self.get_statebit_dth20()
        if (state & 0x08) == 1:
            pass
        elif (state == 255):
            pass
        else:
            pass    
    
    def humidity(self):
        if not self.wait_flag:
            self.measure_template_humidity()
        buf = self.get_template_humidity()
        if buf == -1:
            return 0
        else:
            return buf[0]
        
    def temp_c(self):
        if not self.wait_flag:
            self.measure_template_humidity()
        buf = self.get_template_humidity()
        if buf == -1:
            return 0
        else:
            return buf[1]
    
    def get_statebit_dth20(self):
        val = self.read_to_addr(1)
        return val[0]
    
    def soft_reset_dth20(self):
        data = [0x71]
        self.write_to_addr(data)
        time.sleep(0.1)
    
    def get_template_humidity(self):
        data = [0,0]
        rxbuf = self.read_to_addr(6)
        if (rxbuf[0] & 0x80) == 0:
            self.wait_flag = 0
        else:
            self.wait_flag = 1
            time.sleep(0.08)
            return -1
        data[0] = round(((rxbuf[1] << 12) + (rxbuf[2] << 4) + (rxbuf[3] >> 4)) / 1048576 * 100, 2)
        data[1] = round((((rxbuf[3] & 0x0f) << 16) + (rxbuf[4] << 8) + (rxbuf[5])) / 1048576 * 200 - 50, 2)
        return data
    
    def measure_template_humidity(self):
        data = [0xAC, 0x33, 0x00]
        self.write_to_addr(data)
        time.sleep(0.08)
    
    def read_to_addr(self, lens):
        return self._i2c.readfrom(self.i2c_addr, lens)
    
    def write_to_addr(self, value):
        self._i2c.writeto(self.i2c_addr, value)
    
