import time
from pinpong.board import gboard,I2C
import math

class SHTC3():
    def __init__(self, board = None, i2c_addr = 0x70, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.i2c_addr = i2c_addr
        self._i2c = I2C(bus_num)

        self.wake_up()
        self.software_reset()
        self.sleep()
        self.wake_up()
        id = self.get_ID()
        self.temperature = 0.0
        self.humi = 0.0


    def reset(self):
        self.software_reset()
        self.wake_up()

    def get_ID(self):
        cmd = 0xEFC8
        self.writeCommand(cmd)
        time.sleep(0.012)
        data = self.readValue(3)
        try:
            if(self.checkCrc(data[0], data[1], data[2])):
                id1 = data[0] << 8 | data[1]
                if (id1 & 0x807) == 0x807:
                    id = id1
                else:
                    id = 0
            else:
                id = 0
            return id
        except Exception:
            pass
    
    def sleep(self):
        cmd = 0xB098
        self.writeCommand(cmd)
        time.sleep(0.00023)

    def checkCrc(self, data1, data2, crcValue):
        crc = 0xFF
        crcData = [data1, data2]
        for i in range(2):
            crc ^= crcData[i]
            for i in range(8):
                if crc & 0x80:
                    crc = (crc << 1) ^ 0x31
                else:
                    crc = (crc << 1)
        if crc > 256:
            crc = crc % 256
        if crc != crcValue:
            return False
        return True

    def wake_up(self):
        cmd = 0x3517
        self.writeCommand(cmd)
        time.sleep(0.0005)


    def software_reset(self):
        cmd = 0x805D
        self.writeCommand(cmd)
        time.sleep(0.000173)

    def humidity(self): 
        tem = False
        hum = False
        tem ,hum = self.getTandRHRawData()
        if tem == False and hum == False:
            return round(self.humi, 2)
        self.humi = (hum * 100.0) / 65536
        return round(self.humi, 2)

    def temp_c(self):
        tem = False
        hum = False
        count = 5
        tem ,hum = self.getTandRHRawData()
        if tem == False and hum == False:
            return round(self.temperature, 2)
        self.temperature = -45 + 175*(tem / 65536)
        return round(self.temperature, 2)
    
    def getTandRHRawData(self):
        mode = 0x7CA2
        self.writeCommand(mode)
        time.sleep(0.012)
        try:
            data = self.readValue(6)
            if (self.checkCrc(data[0], data[1], data[2]) and self.checkCrc(data[3], data[4], data[5])):
                temp = data[0] << 8 | data[1]
                rh = data[3] << 8 | data[4]
                return temp,rh
            else:
                return False,False
        except Exception:
            return False,False

    def readValue(self, lens):
        return self._i2c.readfrom(self.i2c_addr, lens)

    def writeCommand(self, cmd):
        data = [(cmd >> 8) & 0xff,cmd & 0xff]
        self._i2c.writeto(self.i2c_addr, data)
