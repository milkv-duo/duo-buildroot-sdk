import time
import datetime
from pinpong.board import gboard, I2C
import math


class DS0469():

    def __init__(self, board=None, i2c_addr=0x32, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.i2c_addr = i2c_addr
        self._i2c = I2C(bus_num)
        self.data = [0 for i in range(7)]
    
    def adjust_aotu(self):
        year = datetime.datetime.now().year
        month = datetime.datetime.now().month
        day = datetime.datetime.now().day
        hour = datetime.datetime.now().hour
        minute = datetime.datetime.now().minute
        second = datetime.datetime.now().second
        week = datetime.datetime.now().isoweekday()
        self.adjust_rtc(year, month, day, week, hour, minute, second)
    
    def adjust_rtc(self, year, month, day, week, hour, minute, second):
        if not (2000 <= year <= 2099):
            raise ValueError("The year is incorrect. Please enter a value between 2000 and 2099")
        if not (1 <= month <= 12):
            raise ValueError("The month is incorrect. Please enter a value between 1 and 12")
        if not (1 <= day <= 31):
            raise ValueError("The date is incorrect. Please enter a value between 1 and 31")
        if not (1 <= week <= 7):
            raise ValueError("Week is incorrect. Please enter a value between 1 and 7")
        if not (0 <= hour <= 23):
            raise ValueError("Hour is incorrect. Please enter a value between 0 and 23")
        if not (0 <= minute <= 59):
            raise ValueError("Minute is incorrect. Please enter a value between 0 and 59")
        if not (0 <= second <= 59):
            raise ValueError("Seconds is incorrect. Please enter a value between 0 and 59")
        if month == 2:
            if year%400 == 0 or (year%4 == 0 and year%100 != 0):
                if day > 29:
                    raise ValueError("There is no %dth in February %d"%(day, year))
            else:        
                if day > 28:
                    raise ValueError("There is no %dth in February %d"%(day, year)) 
        if year >= 2000:
            year -= 2000
        self.write_time_On()
        time.sleep(0.01)
        buf = []
        buf.append(0)
        buf.append(self.decTobcd(second))
        buf.append(self.decTobcd(minute))
        buf.append(self.decTobcd(hour + 80))
        buf.append(self.decTobcd(week))
        buf.append(self.decTobcd(day))
        buf.append(self.decTobcd(month))
        buf.append(self.decTobcd(year))
        self.write_to_addr(buf)
        time.sleep(0.01)
        self.write_to_addr([0x12, 0])
        time.sleep(0.01)
        self.write_time_Off()
    
    def read(self):
        self.readRtc()
        self.processRtc()
    
    def processRtc(self):
        for i in range(7):
            if i != 2:
                self.data[i] = (((self.data[i] & 0xf0) >> 4) * 10) + (self.data[i] & 0x0f)
            else:
                self.data[2] = (self.data[2] & 0x7f)
                self.data[2] = (((self.data[2] & 0xf0) >> 4) * 10) + (self.data[2] & 0x0f)
        self.year = self.data[6] + 2000
        self.month = self.data[5]
        self.day = self.data[4]
        self.week = self.data[3]
        self.hour = self.data[2]
        self.minute = self.data[1]
        self.second = self.data[0]
    
    def readRtc(self):
        val = self.read_to_addr(7)
        if val == None:
            pass
        else:
            self.data = val
    
    def decTobcd(self, num):
        return ((num // 10 * 16) + (num % 10)) 
    
    def write_time_Off(self):
        self.write_to_addr([0x0F, 0, 0])
    
    def write_time_On(self):
        self.write_to_addr([0x10, 0x80])
        time.sleep(0.01)
        self.write_to_addr([0x0F, 0X84])
        time.sleep(0.01)

    def write_to_addr(self, value):
        self._i2c.writeto(self.i2c_addr, value)
    
    def read_to_addr(self, lens):
        return self._i2c.readfrom(self.i2c_addr, lens)
    