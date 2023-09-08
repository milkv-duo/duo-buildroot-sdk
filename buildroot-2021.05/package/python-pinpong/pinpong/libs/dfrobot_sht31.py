import time
import datetime
from pinpong.board import gboard, I2C
import math


class SHT31():
    OneShot                                             = 1
    ERR_OK                                              = 0
    ERR_DATA_BUS                                        = -1
    ERR_IC_VERSION                                      = -2

    Repeatability_High                                  = 0

    SHT3X_CMD_READ_SERIAL_NUMBER               = (0x3780)
    SHT3X_CMD_GETDATA_H_CLOCKENBLED            = (0x2C06)
    SHT3X_CMD_GETDATA_M_CLOCKENBLED            = (0x2C0D)
    SHT3X_CMD_GETDATA_L_CLOCKENBLED            = (0x2C10)
    
    SHT3X_CMD_SETMODE_H_FREQUENCY_HALF_HZ      = (0x2032)
    SHT3X_CMD_SETMODE_M_FREQUENCY_HALF_HZ      = (0x2024)
    SHT3X_CMD_SETMODE_L_FREQUENCY_HALF_HZ      = (0x202F)
    SHT3X_CMD_SETMODE_H_FREQUENCY_1_HZ         = (0x2130)
    SHT3X_CMD_SETMODE_M_FREQUENCY_1_HZ         = (0x2126)
    SHT3X_CMD_SETMODE_L_FREQUENCY_1_HZ         = (0x212D)
    SHT3X_CMD_SETMODE_H_FREQUENCY_2_HZ         = (0x2236)
    SHT3X_CMD_SETMODE_M_FREQUENCY_2_HZ         = (0x2220)
    SHT3X_CMD_SETMODE_L_FREQUENCY_2_HZ         = (0x222B)
    SHT3X_CMD_SETMODE_H_FREQUENCY_4_HZ         = (0x2334)
    SHT3X_CMD_SETMODE_M_FREQUENCY_4_HZ         = (0x2322)
    SHT3X_CMD_SETMODE_L_FREQUENCY_4_HZ         = (0x2329)
    SHT3X_CMD_SETMODE_H_FREQUENCY_10_HZ        = (0x2737)
    SHT3X_CMD_SETMODE_M_FREQUENCY_10_HZ        = (0x2721)
    SHT3X_CMD_SETMODE_L_FREQUENCY_10_HZ        = (0x272A)
    SHT3X_CMD_GETDATA                          = (0xE000)
    
    SHT3X_CMD_STOP_PERIODIC_ACQUISITION_MODE   = (0x3093)
    SHT3X_CMD_SOFT_RESET                       = (0x30A2)
    SHT3X_CMD_HEATER_ENABLE                    = (0x306D)
    SHT3X_CMD_HEATER_DISABLE                   = (0x3066)
    SHT3X_CMD_READ_STATUS_REG                  = (0xF32D)
    SHT3X_CMD_CLEAR_STATUS_REG                 = (0x3041)
    
    SHT3X_CMD_READ_HIGH_ALERT_LIMIT_SET        = (0xE11F)
    SHT3X_CMD_READ_HIGH_ALERT_LIMIT_CLEAR      = (0xE114)
    SHT3X_CMD_READ_LOW_ALERT_LIMIT_CLEAR       = (0xE109)
    SHT3X_CMD_READ_LOW_ALERT_LIMIT_SET         = (0xE102)
    SHT3X_CMD_WRITE_HIGH_ALERT_LIMIT_SET       = (0x611D)
    SHT3X_CMD_WRITE_HIGH_ALERT_LIMIT_CLEAR     = (0x6116)
    SHT3X_CMD_WRITE_LOW_ALERT_LIMIT_CLEAR      = (0x610B)
    SHT3X_CMD_WRITE_LOW_ALERT_LIMIT_SET        = (0x6100)

    def __init__(self, board=None, i2c_addr=0x45, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.i2c_addr = i2c_addr
        self._i2c = I2C(bus_num)
        self.measurementMode = self.OneShot
        self.begin()
        self.softReset()
        
    def begin(self):
        if self.read_serial_number() == 0:
            print("bus data access error")
            return self.ERR_DATA_BUS
        return self.ERR_OK

    def read_serial_number(self):
        result = 0
        self.writeCommand(self.SHT3X_CMD_READ_SERIAL_NUMBER, 2)
        time.sleep(0.001)
        rawData = self.readData(6)
        serialNumber1 = rawData[0:3]
        serialNumber2 = rawData[3:]
        if self.checkCrc(serialNumber1) == serialNumber1[2] and self.checkCrc(serialNumber2) == serialNumber2[2]:
            result = serialNumber1[0]
            result = (result << 8) | serialNumber1[1]
            result = (result << 8) | serialNumber2[0]
            result = (result << 8) | serialNumber2[1]
        return result

    def softReset(self):
        self.writeCommand(self.SHT3X_CMD_SOFT_RESET, 2)
        time.sleep(0.001)
        registerRaw = self.readStatusRegister()
        if (registerRaw & (1 << 1)) == 0:
            return True
        else:
            return False

    def temp_c(self):
        if self.measurementMode == self.OneShot:
            self.readTemperatureAndHumidity(self.Repeatability_High)
        else:
            self.TemperatureAndHumidity()
        return round(self.TemperatureC, 2)

    def temp_f(self):
        if self.measurementMode == self.OneShot:
            self.readTemperatureAndHumidity(self.Repeatability_High)
        else:
            self.TemperatureAndHumidity()
        return round(self.TemperatureF,2)

    def humidity(self):
        if self.measurementMode == self.OneShot:
            self.readTemperatureAndHumidity(self.Repeatability_High)
        else:
            self.TemperatureAndHumidity()
        return round(self.Humidity, 2)

    def TemperatureAndHumidity():
        self.TemperatureC = 0
        self.TemperatureF = 0
        self.Humidity = 0
        self.writeCommand(self.SHT3X_CMD_GETDATA, 2)
        rawData = self.readData(6)
        rawTemperature = rawData[0:3]
        rawHumidity = rawData[3:]
        if self.checkCrc(rawTemperature) != rawTemperature[2] and self.checkCrc(rawHumidity) != rawHumidity[2]:
            return -1
        self.TemperatureC = self.convertTemperature(rawTemperature)
        self.TemperatureF = (9//5)*self.TemperatureC + 32
        self.Humidity = self.convertHumidity(rawHumidity)

    def readTemperatureAndHumidity(self, repeatability):
        self.TemperatureC = 0
        self.TemperatureF = 0
        self.Humidity = 0
        if repeatability == self.Repeatability_High:
            self.writeCommand(self.SHT3X_CMD_GETDATA_H_CLOCKENBLED, 2)
        elif repeatability == self.Repeatability_Medium:
            self.writeCommand(self.SHT3X_CMD_GETDATA_M_CLOCKENBLED, 2)
        elif repeatability == self.eRepeatability_Low:
            self.writeCommand(self.SHT3X_CMD_GETDATA_L_CLOCKENBLED, 2)
        time.sleep(0.015)
        rawData = self.readData(6)
        rawTemperature = rawData[0:3]
        rawHumidity = rawData[3:]
        if self.checkCrc(rawTemperature) != rawTemperature[2] and self.checkCrc(rawHumidity) != rawHumidity[2]:
            return -1
        self.TemperatureC = self.convertTemperature(rawTemperature)
        self.TemperatureF = (9//5)*self.TemperatureC + 32
        self.Humidity = self.convertHumidity(rawHumidity)

    def convertHumidity(self, data):
        rawValue = (data[0] << 8) | data[1]
        return 100 * rawValue / 65535

    def convertTemperature(self, data):
        rawValue = (data[0] << 8) | data[1]
        return 175 * rawValue / 65535 - 45

    def readStatusRegister(self):
        for i in range(10):
            self.writeCommand(self.SHT3X_CMD_READ_STATUS_REG, 2)
            time.sleep(0.001)
            register1 = self.readData(3)
            if self.checkCrc(register1) == register1[2]:
                break
        data = (register1[0] <<8 ) | register1[1]
        return data

    def checkCrc(self, val):
        crc = 0xFF
        for i in range(2):
            crc ^= (val[i])
            if crc > 256:
                crc = crc % 256
            for j in range(8, 0, -1):
                if crc & 0x80:
                    crc = (crc << 1) ^ 0x31
                else:
                    crc = (crc << 1)
        return (crc % 256)

    def writeCommand(self, cmd, size):
        _pBuf = [0,0]
        _pBuf[0] = cmd >> 8
        _pBuf[1] = cmd & 0xFF
        time.sleep(0.001)
        self.write(_pBuf)

    def write(self, value):
        if not isinstance(value, list):
            value = [value]
        self._i2c.writeto(self.i2c_addr, value)
    
    def readData(self, lens):
        return self._i2c.readfrom(self.i2c_addr, lens)
    