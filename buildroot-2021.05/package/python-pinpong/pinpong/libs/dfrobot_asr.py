# -*- coding: utf-8 -*
import time
from pinpong.board import gboard, I2C


class ASR(object):
    ''' register configuration '''
    I2C_ADDR = 0x50
    ASR_BEGIN = 0xA1  # module init command
    ASR_ADDCOMMAND = 0xA2  # command to start writing command
    ASR_ADDCOMMAND_END = 0xA3  # command to end writing command
    ASR_START = 0xA4  # command to start recognition
    ASR_LOOP = 0xA5  # Loop mode command
    ASR_BUTTON = 0xA7  # Button mode command
    ASR_PASSWORD = 0xA6  # Password mode command
    ASR_IDLE = 0xA8  # Idle mode command
    ASR_MIC_MODE = 0xA9  # mic mode command
    ASR_MONO_MODE = 0xAA  # mono mode command
    ASR_SET_IIC_ADDR = 0xAB  # Set address command
    LOOP = 0x01  # Loop mode
    PASSWORD = 0x02  # Password mode
    BUTTON = 0x03  # Button mode
    MIC = 0x04
    MONO = 0x05

    def __init__(self):
        pass

    def begin(self, mode, miMode):
        '''
        @fn begin
        @brief Init function
        @param mode Speech recognition mode
        @param miMode Mic mode
        @return Returning 0 indicates init succeeded, non-zero indicates init failed, return error code.
        '''
        self._mode = mode
        self.idle = 0
        self.write_data(self.ASR_BEGIN)
        if miMode == self.MIC:
            self.write_data(self.ASR_MIC_MODE)
        else:
            self.write_data(self.ASR_MONO_MODE)
        time.sleep(0.05)

    def start(self):
        '''
        @fn start
        @brief The speech recognition module start to recognize.
        '''
        self.write_reg(self.ASR_START)
        time.sleep(0.05)

    def add_command(self, words, idNum):
        '''
        @fn add_command
        @brief Add phrases to the module
        @param words Character strings of the phrase
        @param idNum The id number of the phrase
        @return Boolean type, the result of seted
        @retval Adding phrases succeeded
        @retval Adding phrases failed
        '''
        buf = []
        buf.append(self.ASR_ADDCOMMAND)
        buf.append(idNum)
        word = list(words)
        buf.append(len(word))
        time.sleep(0.05)
        for i in range(0, len(word)):
            buf.append(ord(word[i]))
        buf.append(self.ASR_ADDCOMMAND_END)
        self.write_data(buf)

    def read(self):
        '''
        @fn read
        @brief Read the recognized phrase
        @return Return the id number of the phrase
        '''
        rslt = -1
        #print(self._mode)
        if self._mode == self.BUTTON:
            rslt = self.read_reg(self.ASR_BUTTON,1)
        elif self._mode == self.LOOP:
            rslt = self.read_reg(self.ASR_LOOP,1)
        elif self._mode == self.PASSWORD:
            rslt = self.read_reg(self.ASR_PASSWORD,1)
        if self._mode == self.PASSWORD:
            self.idle = self.idle + 1
            if self.idle >= 500:
                self.write_data(self.ASR_IDLE)
                self.idle = 0
        if rslt[0] == 0xff:
            return -1
        else:
            self.idle=0
            return rslt[0]

class DFRobot_ASR(ASR):
    def __init__(self, board=None, i2c_addr=0x4F, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.__addr = i2c_addr
        self._i2c = I2C(bus_num)
        super(DFRobot_ASR, self).__init__()

    def write_reg(self, reg):
        self._i2c.writeto_mem(self.__addr, reg, [])
    
    def write_data(self, data):
        if not isinstance(data, list):
            data = [data]
        self._i2c.writeto(self.__addr, data)

    def read_reg(self, reg, len):
        self.write_data(reg)
        time.sleep(0.05)
        rslt = self._i2c.readfrom(self.__addr, len)
        return rslt
