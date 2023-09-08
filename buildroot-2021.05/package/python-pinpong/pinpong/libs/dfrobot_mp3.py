import time
import datetime
from pinpong.board import gboard, UART
import math

class MP3():
    def __init__(self, board=None, bus_num=0, baud_rate=9600):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self._uart = UART(bus_num=bus_num, baud_rate=baud_rate)
        time.sleep(0.001)
        self.volume()
        
    def volume(self, value=50):
        time.sleep(0.001)
        vol = self.maps(value, 0, 100, 0, 30)
        play = [0xAA, 0x13, 0x01, vol, vol + 0xBe]
        self.cal_data(play)
        self.uart_write(play)
        time.sleep(0.001)
    
    def volume_down(self):
        time.sleep(0.001)
        data = [0,0,0,0]
        data[0] = 0xAA
        data[1] = 0x15
        data[2] = 0x00
        data[3] = data[0] + data[1] + data[2]
        self.uart_write(data)
        time.sleep(0.001)
    
    def volume_up(self):
        time.sleep(0.001)
        data = [0,0,0,0]
        data[0] = 0xAA
        data[1] = 0x14
        data[2] = 0x00
        data[3] = data[0] + data[1] + data[2]
        self.uart_write(data)
        time.sleep(0.001)
    
    def next(self):
        time.sleep(0.001)
        data = [0,0,0,0]
        data[0] = 0xAA
        data[1] = 0x06
        data[2] = 0x00
        data[3] = data[0] + data[1] + data[2]
        self.uart_write(data)
        time.sleep(0.001)
    
    def prev(self):
        time.sleep(0.001)
        data = [0,0,0,0]
        data[0] = 0xAA
        data[1] = 0x05
        data[2] = 0x00
        data[3] = data[0] + data[1] + data[2]
        self.uart_write(data)
        time.sleep(0.001)
    
    def end(self):
        time.sleep(0.001)
        data = [0,0,0,0]
        data[0] = 0xAA
        data[1] = 0x10
        data[2] = 0x00
        data[3] = data[0] + data[1] + data[2]
        self.uart_write(data)
        time.sleep(0.001)
    
    def pause(self):
        time.sleep(0.001)
        data = [0,0,0,0]
        data[0] = 0xAA
        data[1] = 0x03
        data[2] = 0x00
        data[3] = data[0] + data[1] + data[2]
        self.uart_write(data)
        time.sleep(0.001)
    
    def play(self):
        time.sleep(0.001)
        data = [0,0,0,0]
        data[0] = 0xAA
        data[1] = 0x02
        data[2] = 0x00
        data[3] = data[0] + data[1] + data[2]
        self.uart_write(data)
        time.sleep(0.001)
    
    
    def playList(self, value):
        time.sleep(0.001)
        data = [0,0,0,0,0,0]
        data[0] = 0xaa
        data[1] = 0x07
        data[2] = 0x02
        data[3] = (value >> 8) & 0xff
        data[4] = value & 0xff
        data[5] = data[0] + data[1] + data[2] + data[3] + data[4]
        self.cal_data(data)
        self.uart_write(data)
        time.sleep(0.001)
    
    def query_play_status(self):
        time.sleep(0.001)
        data = [0,0,0,0]
        data[0] = 0xAA
        data[1] = 0x01
        data[2] = 0x00
        data[3] = data[0] + data[1] + data[2]
        self.uart_write(data)
        time.sleep(0.1)
        dataIn = [0,0,0,0,0] 
        self.uart_read(dataIn)
        if dataIn[4] != dataIn[0] + dataIn[1] + dataIn[2] + dataIn[3]:
            return 0
        return dataIn[3]
        
    def maps(self, x, in_min, in_max, out_min, out_max):
        divisor = in_max - in_min
        if divisor == 0:
            return -1
        return (x - in_min) * (out_max - out_min) // divisor + out_min

    def cal_data(self, data):
        for i in range(len(data)):
            if data[i] > 255:
                data[i] = data[i] % 256
                
    def uart_write(self, value):
        if not isinstance(value, list):
            value = list(value)
        self._uart.write(value)
    
    def uart_read(self, num):
        return self._uart.readinto(num)
    