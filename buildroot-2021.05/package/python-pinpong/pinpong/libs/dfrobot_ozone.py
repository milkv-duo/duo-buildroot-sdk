import time
import datetime
from pinpong.board import gboard, I2C
import math


class Ozone():
    ADDRESS_0 = 0x70
    ADDRESS_1 = 0x71
    ADDRESS_2 = 0x72
    ADDRESS_3 = 0x73

    MEASURE_MODE_AUTOMATIC = 0x00
    MEASURE_MODE_PASSIVE = 0x01

    AUTO_READ_DATA = 0x00
    PASSIVE_READ_DATA = 0x01

    MODE_REGISTER = 0x03
    SET_PASSIVE_REGISTER = 0x04

    AUTO_DATA_HIGE_REGISTER = 0x09
    AUTO_DATA_LOW_REGISTER = 0x0A

    PASS_DATA_HIGE_REGISTER = 0x07
    PASS_DATA_LOW_REGISTER = 0x08

    OCOUNT = 100

    def __init__(self, board=None, i2c_addr=0x73, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.i2c_addr = i2c_addr
        self._i2c = I2C(bus_num)
        self.OzoneData = [0 for i in range(100)]

    def set_mode(self, mode):
        if mode == self.MEASURE_MODE_AUTOMATIC:
            self.__write_reg(self.MODE_REGISTER, self.MEASURE_MODE_AUTOMATIC)
            time.sleep(0.1)
            self._M_Flag = 0
        elif mode == self.MEASURE_MODE_PASSIVE:
            self.__write_reg(self.MODE_REGISTER, self.MEASURE_MODE_PASSIVE)
            time.sleep(0.1)
            self._M_Flag = 1
        else:
            print("Please enter the correct parameters")

    def read_ozone_data(self, CollectNum=20):
        k = 0
        if CollectNum > 0:
            for j in range(CollectNum-1, -1, -1):
                self.OzoneData[j] = self.OzoneData[j-1]
            if self._M_Flag == 0:
                self.__write_reg(self.SET_PASSIVE_REGISTER, self.AUTO_READ_DATA)
                time.sleep(0.1)
                self.OzoneData[0] = self.__i2c_read_ozone_data(self.AUTO_DATA_HIGE_REGISTER)
            elif self._M_Flag == 1:
                self.__write_reg(self.SET_PASSIVE_REGISTER, self.PASSIVE_READ_DATA)
                time.sleep(0.1)
                self.OzoneData[0] = self.__i2c_read_ozone_data(self.PASS_DATA_HIGE_REGISTER)
            if k < CollectNum:
                k += 1
            return round(self.__get_average_num(self.OzoneData, k), 2)
        else:
            return -1
    
    def __get_average_num(self, bArray, iFilterLen):
        bTemp = 0
        for i in range(iFilterLen):
            bTemp += bArray[i]
        return bTemp / iFilterLen
            
    def __bin2dec(self, a):
        a___reverse = self.__reverse(a)  # 取反
        a_add_1 = self.__add_1(a___reverse)  # 二进制加1
        a_int = -int(a_add_1, 2)
        return a_int

    def __bin2dec_auto(self, a, b):
        tmp = (b << 8) | a
        my_list = list(bin(tmp))
        c = len(my_list)
        if c < 18:
            for i in range(18-c):
                my_list.insert(2, '0')
        value = my_list[2:]
        if value[0] == '1':  # 如果首位是1，复数转换
            a_output = self.__bin2dec(value)
        else:
            a_output = tmp
        return a_output

    def __add_1(self, binary_inpute):  # 二进制编码加1
        _, out = bin(int(binary_inpute, 2) + 1).split("b")
        return out
 
    def __reverse(self, binary_inpute):  # 取反操作
        binary_out = list(binary_inpute)
        for epoch, i in enumerate(binary_out):
            if i == "0":
                binary_out[epoch] = "1"
            else:
                binary_out[epoch] = "0"
        return "".join(binary_out)


    def __i2c_read_ozone_data(self, reg):
        data = self.__read_reg(reg, 2)
        val = self.__bin2dec_auto(data[1], data[0])
        return val

    def __write_reg(self, reg, value):
        if not isinstance(value, list):
            value = [value]
        self._i2c.writeto_mem(self.i2c_addr, reg, value)
    
    def __read_reg(self, reg, lens):
        return self._i2c.readfrom_mem(self.i2c_addr, reg, lens)
    
