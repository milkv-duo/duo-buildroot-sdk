import time
from pinpong.board import gboard,I2C

class Ens160():
    INTDataDrdyEN = 1 << 1
    INTDataDrdyDIS = 0 << 1
    IntGprDrdyDIS = 0 << 3
    IntGprDrdyEN = 1 << 3

    ENS160_DATA_STATUS_REG   =  0x20
    ENS160_DATA_AQI_REG      =  0x21
    ENS160_DATA_TVOC_REG     =  0x22
    ENS160_DATA_ECO2_REG     =  0x24

    ENS160_STANDARD_MODE     =  0x02
    ENS160_IDLE_MODE         =  0x01
    ENS160_SLEEP_MODE        =  0x00
    def __init__(self, board = None, i2c_addr = 0x53, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.i2c_addr = i2c_addr
        self._i2c = I2C(bus_num)
        self.__begin()
    
    def get_status(self):
        data = self.__read_reg(self.ENS160_DATA_STATUS_REG, 1)
        return ((data[0] >> 2) & 0b00000011)

    def get_aqi(self):
        data = self.__read_reg(self.ENS160_DATA_AQI_REG, 1)
        if data != []:
            return data[0]
        else:
            raise ValueError("read reg error")

    def get_tvoc(self):
        data = self.__read_reg(self.ENS160_DATA_TVOC_REG, 2)
        value = self.__concat_bytes(data)
        return value
    
    def get_eco2(self):
        data = self.__read_reg(self.ENS160_DATA_ECO2_REG, 2)
        value = self.__concat_bytes(data)
        return value
    
    def set_temp_hum(self, temp, humi):
        buf = []
        temp = int((temp + 273.15) * 64)
        rh = int(humi * 512)
        buf.append(temp & 0xff)
        buf.append((temp & 0xff00) >> 8)
        buf.append(rh & 0xff)
        buf.append((rh & 0xff00) >> 8)
        self.__write_reg(0x13, buf)

    def set_pwr_mode(self, data):
        self.__write_reg(0x10, [data])
        time.sleep(0.02)

    def __begin(self):
        data = self.__read_reg(0x00, 2)
        if 0x160 != self.__concat_bytes(data):
            raise ValueError("ERR_IC_VERSION")
        self.set_pwr_mode(0x02)
        self.__set_int_mode(0x00)

    def __set_int_mode(self, mode):
        mode |= self.INTDataDrdyEN | self.IntGprDrdyDIS 
        self.__write_reg(0x11, [mode])
        time.sleep(0.02)

    def __concat_bytes(self, data):
        return data[0] | (data[1] << 8)
    
    def __read_reg(self, reg, lens):
        return self._i2c.readfrom_mem(self.i2c_addr, reg, lens)
    
    def __write_reg(self, reg, value):
        self._i2c.writeto_mem(self.i2c_addr, reg, value)
    
