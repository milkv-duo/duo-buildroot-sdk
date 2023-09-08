# -*- coding: utf-8 -*-
from pinpong.board import gboard,UART
import time

class Windspeed:
    def __init__(self, board=None, uart_num=0):
        self.board = gboard
        self.uart = UART()
        self.uart.init(baud_rate = 9600, bits=8, parity=0, stop = 1)
        self.uart.write([0x00])
        self.addr2 = 0

    def read_wind_speed(self):
        Address = self.addr2
        COM = [0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00]
        COM[0] = Address
        WindSpeed = -1
        self.addedCRC(COM, 6)
        self.uart.write(COM)
        time.sleep(0.3)
        timeout = time.time()
        while True:
            if(self.uart.any() == 7):
                break
            else:
                self.uart.readall()
                self.uart.write(COM)
                time.sleep(0.3)
                if(time.time() - timeout > 2):
                    return -1
        data = self.uart.read(7)
        if(self.CRC16_2(data, 5) == data[5] * 256 + data[6]):
            WindSpeed = (data[3] * 256 + data[4]) / 10.00
        return WindSpeed

    def modify_address(self, addr1, addr2):
        self.addr2 = addr2
        buf = [0x00, 0x10, 0x10, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00]
        ret = False
        curr = time.time()
        buf[0] = addr1
        buf[8] = addr2
        self.addedCRC(buf, 9)
        time.sleep(0.1)
        self.uart.write(buf)
        time.sleep(1)
        timeout = time.time()
        while True:
            if(self.uart.any() == 8):
                break
            else:
                self.uart.readall()
                self.uart.write(buf)
                time.sleep(0.5)
                if(time.time() - timeout > 2):
                    return -1
        rxbuf = self.uart.readall()
        if(rxbuf[0] == addr1 and rxbuf[1] == 0x10 and rxbuf[2] == 0x10 and rxbuf[3] == 0x00 and rxbuf[4] == 0x00 and rxbuf[5] == 0x01):
            ret = True
        return ret
        
    def addedCRC(self, buf, len):
        crc = 0xFFFF
        for pos in range(len):
            crc ^= buf[pos]
            for i in range(8, 0, -1):
                if((crc & 0x0001) != 0):
                   crc >>= 1
                   crc ^= 0xA001
                else:
                    crc >>= 1
        buf[len] = crc % 0x100
        buf[len+1] = crc // 0x100

    def CRC16_2(self, buf, len):
        crc = 0xFFFF
        for pos in range(len):
            crc ^= buf[pos]
            for i in range(8, 0, -1):
                if((crc & 0x0001) != 0):
                   crc >>= 1
                   crc ^= 0xA001
                else:
                    crc >>= 1
        crc = ((crc & 0x00ff) << 8) | ((crc & 0xff00) >> 8)
        return crc
