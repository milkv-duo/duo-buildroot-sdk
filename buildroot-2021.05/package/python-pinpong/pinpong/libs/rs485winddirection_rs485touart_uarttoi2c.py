# -*- coding: utf-8 -*-
from pinpong.board import gboard,UART
from pinpong.libs.dfrobot_uarttoi2c import DFRobot_IIC_Serial
import time

class IICSerialWindDirection:
    V1   =   1
    V2   =   2
    ## Sub UART channel1 
    SUBUART_CHANNEL_1   = 0x00
    ## Sub UART channel2   
    SUBUART_CHANNEL_2   = 0x01
    ## All sub channels   
    SUBUART_CHANNEL_ALL = 0x11
    DIRECTION = {
            0:"北",
            1:"东北偏北",
            2:"东北",
            3:"东北偏东",
            4:"东",
            5:"东南偏东",
            6:"东南",
            7:"东南偏南",
            8:"南",
            9:"西南偏南",
            10:"西南",
            11:"西南偏西",
            12:"西",
            13:"西北偏西",
            14:"西北",
            15:"西北偏北"
        }
    DIRECTIONV2 = ["北", "东北偏北", "东北", "东北偏东", "东", "东南偏东", "东南", "东南偏南", "南","西南偏南", "西南", "西南偏西", "西", "西北偏西", "西北", "西北偏北", "北"]
    def __init__(self, sub_uart_channel=0x00, IA1=1, IA0 = 1):
        self.uart = DFRobot_IIC_Serial(sub_uart_channel, IA1, IA0)
        self.uart.begin(baud = 9600, format = self.uart.IIC_Serial_8N1)
        self.addr2 = 0

    def read_wind_direction(self):
        Address = self.addr2
        index = self._read_wind_direction(Address)
        try:
            return self.DIRECTION[index] if self.version == self.V1 else self.DIRECTIONV2[index]
        except:
            return -1

    def _read_wind_direction(self, Address):
        if self.version == self.V2:
            data = [0, 0]
            self.read_holding_register_V2(0, 0x0001, data)
            return data[0] * 0x100 + data[1]
        elif self.version == self.V1:
            return self.read_holding_register_V1(Address)

    def modify_address(self, version, addr1, addr2):
        self.version = version
        self.addr2 = addr2
        if self.version == self.V2:
            return self.write_holding_register_V2(addr1, 0x1000, addr2)
        elif self.version == self.V1:
            self.write_holding_register_V1(addr1, addr2)
    
    def get_wind_angle(self):
        data = [0, 0]
        self.read_holding_register_V2(0, 0x0000, data)
        return (data[0] * 0x100 + data[1]) / 10.0
    
    def GetWindDirection(self, Address):
        data = [0, 0]
        self.read_holding_register_V2(0, 0x0001, data)
        return data[0] * 0x100 + data[1]
    
    def read_holding_register_V1(self, Address):
        COM = [0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00]
        COM[0] = Address
        Winddirection = -1
        self.addedCRC(COM, 6)
        self.uart.write(COM)
        time.sleep(0.3)
        timeout = time.time()
        while True:
            if(self.uart.available() == 7):
                break
            else:
                if(time.time() - timeout > 2):
                    return -1
        rxbuf = self.readall()
        if(self.CRC16_2(rxbuf, 5) == rxbuf[5] * 256 + rxbuf[6]):
            Winddirection = (rxbuf[3] * 256 + rxbuf[4])
        return Winddirection
        
    def read_holding_register_V2(self, Address, reg, data):
        buf = [0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00]
        timeout = time.time()
        curr1 = timeout
        ch = 0
        buf[0] = Address
        buf[2] = (reg // 0x100) % 0x100
        buf[3] = reg % 0x100
        self.addedCRC(buf, 6)
        time.sleep(0.1)
        self.uart.write(buf)
        while True:
            if(self.uart.available() == 7):
                break
            else:
                if(time.time() - timeout > 2):
                    return -1
        rxbuf = self.readall()
        if (self.CRC16_2(rxbuf, 5) == (rxbuf[5] * 256 + rxbuf[6])):
            data[0] = rxbuf[3]
            data[1] = rxbuf[4]
            return True
    
    def write_holding_register_V1(self, addr1, addr2):
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
            if(self.uart.available() == 8):
                break
            else:
                if(time.time() - timeout > 2):
                    return -1
        rxbuf = self.readall()
        if(rxbuf[0] == addr1 and rxbuf[1] == 0x10 and rxbuf[2] == 0x10 and rxbuf[3] == 0x00 and rxbuf[4] == 0x00 and rxbuf[5] == 0x01):
            ret = True
        return ret
    
    def write_holding_register_V2(self, Address, reg, parameter):
        buf = [0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00]
        ret = False
        curr = time.time()
        buf[0] = Address
        buf[2] = (reg // 0x100) % 0x100
        buf[3] = reg % 0x100
        buf[7] = (parameter // 0x100) % 0x100
        buf[8] = parameter % 0x100
        self.addedCRC(buf, 9)
        time.sleep(0.1)
        self.uart.write(buf)
        time.sleep(1)
        timeout = time.time()
        while True:
            if(self.uart.available() == 8):
                break
            else:
                if(time.time() - timeout > 2):
                    return -1
        rxbuf = self.readall()
        if (self.CRC16_2(rxbuf, 6) == (rxbuf[6] * 256 + rxbuf[7])):
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

    def readall(self):
        c = []
        while self.uart.available():
            c.append(self.uart.read())
        return c
