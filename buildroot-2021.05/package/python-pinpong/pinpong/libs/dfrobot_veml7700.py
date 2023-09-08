# -*- coding: utf-8 -*-
import time
from pinpong.board import gboard,I2C

import logging
from ctypes import *


logger = logging.getLogger()
logger.setLevel(logging.INFO)  #显示所有的打印信息。只打印错误：logging.FATAL
ph = logging.StreamHandler()
formatter = logging.Formatter("%(asctime)s - [%(filename)s %(funcName)s]:%(lineno)d - %(levelname)s: %(message)s")
ph.setFormatter(formatter) 
logger.addHandler(ph)


VEML7000_I2C_ADDRESS = 0x10

COMMAND_ALS_SM = 0x00
ALS_SM_MASK = 0x1800
ALS_SM_SHIFT = 11

COMMAND_ALS_IT = 0x00
ALS_IT_MASK = 0x03c0
ALS_IT_SHIFT = 6

COMMAND_ALS_PERS = 0x00
ALS_PERS_MASK = 0x0030
ALS_PERS_SHIFT = 4

COMMAND_ALS_INT_EN = 0x00
ALS_INT_EN_MASK = 0x0002
ALS_INT_EN_SHIFT = 1

COMMAND_ALS_SD = 0x00
ALS_SD_MASK = 0x0001
ALS_SD_SHIFT = 0

COMMAND_ALS_WH = 0x01

COMMAND_ALS_WL = 0x02

COMMAND_PSM = 0x03
PSM_MASK = 0x0006
PSM_SHIFT = 1

COMMAND_PSM_EN = 0x03
PSM_EN_MASK = 0x0001
PSM_EN_SHIFT = 0

COMMAND_ALS = 0x04

COMMAND_WHITE = 0x05

COMMAND_ALS_IF_L = 0x06
ALS_IF_L_MASK = 0x8000
ALS_IF_L_SHIFT = 15

COMMAND_ALS_IF_H = 0x06
ALS_IF_H_MASK = 0x4000
ALS_IF_H_SHIFT = 14


class VEML7700(object):

    ID = 0x0
    ID_DEFAULT = 0xc0

    ALS_GAIN_x1 = 0x0   # x 1
    ALS_GAIN_x2 = 0x1   # x 2
    ALS_GAIN_d8 = 0x2   # x 1/8
    ALS_GAIN_d4 = 0x3   # x 1/4
    
    ALS_INTEGRATION_25ms = 0xc
    ALS_INTEGRATION_50ms = 0x8
    ALS_INTEGRATION_100ms = 0x0
    ALS_INTEGRATION_200ms = 0x1
    ALS_INTEGRATION_400ms = 0x2
    ALS_INTEGRATION_800ms = 0x3
    
    ALS_PERSISTENCE_1 = 0x0
    ALS_PERSISTENCE_2 = 0x1
    ALS_PERSISTENCE_4 = 0x2
    ALS_PERSISTENCE_8 = 0x3
    
    ALS_POWER_MODE_1 = 0x0
    ALS_POWER_MODE_2 = 0x1
    ALS_POWER_MODE_3 = 0x2
    ALS_POWER_MODE_4 = 0x3
    
    STATUS_OK = 0
    STATUS_ERROR = 0xff

    '''
      @brief Module init
    '''
    def __init__(self):
        self.register_cache =[ 
            (self.ALS_GAIN_x2 << ALS_SM_SHIFT) | 
            (self.ALS_INTEGRATION_100ms << ALS_IT_SHIFT) | 
            (self.ALS_PERSISTENCE_1 << ALS_PERS_SHIFT) | 
            (0 << ALS_INT_EN_SHIFT) | (0 << ALS_SD_SHIFT),
            0x0000, 0xffff, 
            (self.ALS_POWER_MODE_3 << PSM_SHIFT) | (0 << PSM_EN_SHIFT)
            ]

    '''
      @brief Initialize sensor
      @return 返回True表示初始化成功，返回False表示初始化失败
    '''
    def begin(self):
        ret = True
        chip_id = self._read_reg(self.ID, 2)
        #logger.info(1)
        #logger.info(chip_id[0])
        if not len(chip_id):
            ret = False
        else:
            self.register_cache[0] = ((self.ALS_GAIN_x2 << ALS_SM_SHIFT) | 
            (self.ALS_INTEGRATION_100ms << ALS_IT_SHIFT) | 
            (self.ALS_PERSISTENCE_1 << ALS_PERS_SHIFT) |
            (0 << ALS_INT_EN_SHIFT) | (0 << ALS_SD_SHIFT))
            #logger.info((self.register_cache[0] & ALS_SM_MASK) >> ALS_SM_SHIFT)
            self._write_reg(COMMAND_ALS_SM, [self.register_cache[COMMAND_ALS_SM] & 0xff, self.register_cache[COMMAND_ALS_SM] >> 8])
            self._write_reg(COMMAND_ALS_WH, [self.register_cache[COMMAND_ALS_WH] & 0xff, self.register_cache[COMMAND_ALS_WH] >> 8])
            self._write_reg(COMMAND_ALS_WL, [self.register_cache[COMMAND_ALS_WL] & 0xff, self.register_cache[COMMAND_ALS_WL] >> 8])
            self._write_reg(COMMAND_PSM, [self.register_cache[COMMAND_PSM] & 0xff, self.register_cache[COMMAND_PSM] >> 8])
            time.sleep(0.003)
        return ret

    def begin_with_gain(self, als_gain):
        self.register_cache =[ 
            (self.als_gain << ALS_SM_SHIFT) | 
            (self.ALS_INTEGRATION_100ms << ALS_IT_SHIFT) | 
            (self.ALS_PERSISTENCE_1 << ALS_PERS_SHIFT) | 
            (0 << ALS_INT_EN_SHIFT) | (0 << ALS_SD_SHIFT),
            0x0000, 0xffff, 
            (self.ALS_POWER_MODE_3 << PSM_SHIFT) | (0 << PSM_EN_SHIFT)
            ] 
        #logger.info(self.get_gain())
        self._write_reg(COMMAND_ALS_SM, [self.register_cache[COMMAND_ALS_SM] & 0xff, self.register_cache[COMMAND_ALS_SM] >> 8])
        self._write_reg(COMMAND_ALS_WH, [self.register_cache[COMMAND_ALS_WH] & 0xff, self.register_cache[COMMAND_ALS_WH] >> 8])
        self._write_reg(COMMAND_ALS_WL, [self.register_cache[COMMAND_ALS_WL] & 0xff, self.register_cache[COMMAND_ALS_WL] >> 8])
        self._write_reg(COMMAND_PSM, [self.register_cache[COMMAND_PSM] & 0xff, self.register_cache[COMMAND_PSM] >> 8])
        time.sleep(0.003)

    def set_gain(self, gain):
        reg = (self.register_cache[COMMAND_ALS_SM] & ~ALS_SM_MASK) | ((gain << ALS_SM_SHIFT) & ALS_SM_MASK)
        self.register_cache[COMMAND_ALS_SM] = reg
        self._write_reg(COMMAND_ALS_SM, [reg & 0xff, reg >> 8])
        #logger.info("set_gain")

    def get_gain(self):
        #logger.info(self.register_cache[COMMAND_ALS_SM] & ALS_SM_MASK >> ALS_SM_SHIFT)
        return (self.register_cache[COMMAND_ALS_SM] & ALS_SM_MASK) >> ALS_SM_SHIFT

    def set_integration_time(self, itime):
        reg = (self.register_cache[COMMAND_ALS_IT] & ~ALS_IT_MASK) | ((itime << ALS_IT_SHIFT) & ALS_IT_MASK)
        self.register_cache[COMMAND_ALS_IT] = reg
        self._write_reg(COMMAND_ALS_IT, [reg & 0xff, reg >> 8])

    def get_integration_time(self):
        return self.register_cache[COMMAND_ALS_IT] & ALS_IT_MASK >> ALS_IT_SHIFT

    def set_persistence(self, persist):
        reg = (self.register_cache[COMMAND_ALS_PERS] & ~ALS_PERS_MASK) | ((persist << ALS_PERS_SHIFT) & ALS_PERS_MASK)
        self.register_cache[COMMAND_ALS_PERS] = reg
        self._write_reg(COMMAND_ALS_PERS, [reg & 0xff, reg >> 8])

    def set_power_saving_mode(self, power_mode):
        reg = (self.register_cache[COMMAND_PSM] & ~PSM_MASK) | ((power_mode << PSM_SHIFT) & PSM_MASK)
        self.register_cache[COMMAND_PSM] = reg
        self._write_reg(COMMAND_PSM, [reg & 0xff, reg >> 8])

    def set_power_saving(self, enabled):
        reg = (self.register_cache[COMMAND_PSM_EN] & ~PSM_EN_MASK) | ((enabled << PSM_EN_SHIFT) & PSM_EN_MASK)
        self.register_cache[COMMAND_PSM_EN] = reg
        self._write_reg(COMMAND_PSM_EN, [reg & 0xff, reg >> 8])

    def set_interrupts(self, enabled):
        reg = (self.register_cache[COMMAND_ALS_INT_EN] & ~ALS_INT_EN_MASK) | ((enabled << ALS_INT_EN_SHIFT) & ALS_INT_EN_MASK)
        self.register_cache[COMMAND_ALS_INT_EN] = reg
        self._write_reg(COMMAND_ALS_INT_EN, [reg & 0xff, reg >> 8])

    def set_power(self, on):
        reg = (self.register_cache[COMMAND_ALS_SD] & ~ALS_SD_MASK) | ((~on << ALS_SD_SHIFT) & ALS_SD_MASK)
        self.register_cache[COMMAND_ALS_SD] = reg
        self._write_reg(COMMAND_ALS_SD, [reg & 0xff, reg >> 8])
        if on:
            time.sleep(0.003)

    def set_ALS_high_threshold(self, thresh):
        self._write_reg(COMMAND_ALS_WH, [thresh & 0xff, thresh >> 8])

    def set_ALS_low_threshold(self, thresh):
        self._write_reg(COMMAND_ALS_WL, [thresh & 0xff, thresh >> 8])

    def get_ALS(self):
        data = self._read_reg(COMMAND_ALS, 2)
        #logger.info(data)
        return data[0] | (data[1] << 8)

    def get_white(self):
        data = self._read_reg(COMMAND_WHITE, 2)
        return data[0] | (data[1] << 8)

    def get_high_threshold_event(self):
        data = self._read_reg(COMMAND_ALS_IF_H, 2)
        return (data[0] | (data[1] << 8)) & ALS_IF_H_MASK >> ALS_IF_H_SHIFT

    def get_low_threshold_event(self):
        data = self._read_reg(COMMAND_ALS_IF_L, 2)
        return (data[0] | (data[1] << 8)) & ALS_IF_L_MASK >> ALS_IF_L_SHIFT

    def scale_lux(self, raw_counts):
        x1, x2, d8 = 0, 1, 0
        gain = self.get_gain()
        itime = self.get_integration_time()

        if ((gain & 0x03) == self.ALS_GAIN_x1):
            factor1 = 1.0
            #logger.info(1)
        elif ((gain & 0x03) == self.ALS_GAIN_x2):
            factor1 = 0.5
            #logger.info(2)
        elif ((gain & 0x03) == self.ALS_GAIN_d8):
            factor1 = 8.0
        elif ((gain & 0x03) == self.ALS_GAIN_d4):
            factor1 = 4.0
        else:
            factor1 = 1.0
            #logger.info(0000)

        if (itime == self.ALS_INTEGRATION_25ms):
            factor2 = 0.2304
        elif (itime == self.ALS_INTEGRATION_50ms):
            factor2 = 0.1152
        elif (itime == self.ALS_INTEGRATION_100ms):
            factor2 = 0.0576
        elif (itime == self.ALS_INTEGRATION_200ms):
            factor2 = 0.0288
        elif (itime == self.ALS_INTEGRATION_400ms):
            factor2 = 0.0144
        elif (itime == self.ALS_INTEGRATION_800ms):
            factor2 = 0.0072
        else:
            factor2 = 0.2304
        
        result = raw_counts * factor1 * factor2
        #logger.info(factor1)
        #logger.info(factor2)
        if((result > 1880.00) and (result < 3771.00)):
            if(x1 == 1):
                self.begin_with_gain(self.ALS_GAIN_x1)
                x1, x2, d8 = 0, 1, 1
        elif(result>3770.00):
            if(d8 == 1):
                self.begin_with_gain(self.ALS_GAIN_d8)
                x1, x2, d8 = 1, 1, 0
        else:
            if(x2 == 1):
                self.begin()
                x1, x2, d8 = 1, 0, 1
        
        # apply correction from App. Note for all readings
        # using Horner's method
        lux = result
        lux = lux * (1.0023 + lux * (8.1488e-5 + lux * (-9.3924e-9 + lux * 6.0135e-13)))
        #logger.info(lux)

        return round(lux, 2)

    def get_ALS_lux(self):
        raw_counts = self.get_ALS()
        #logger.info(raw_counts)
        return self.scale_lux(raw_counts)

    def get_white_lux(self):
        raw_counts = self.get_white()
        return self.scale_lux(raw_counts)

    def get_auto_X_lux(self, counts_func):
        gains = [self.ALS_GAIN_d8, self.ALS_GAIN_d4, self.ALS_GAIN_x1, self.ALS_GAIN_x2]
        itimes = [self.ALS_INTEGRATION_25ms, self.ALS_INTEGRATION_50ms, self.ALS_INTEGRATION_100ms,
                self.ALS_INTEGRATION_200ms, self.ALS_INTEGRATION_400ms, self.ALS_INTEGRATION_800ms]
        counts_threshold = 200

        self.set_power(0)
        for itime_idx in range(2, 6):
            self.set_integration_time(itimes[itime_idx])
            for gain_idx in range(0, 4):
                self.set_gain(gains[gain_idx])
                self.set_power(1)
                self.sample_delay()
                #logger.info(3)
                raw_counts = counts_func()

                if (raw_counts > counts_threshold):
                    while True:
                        if (raw_counts < 10000):
                            lux = self.scale_lux(raw_counts)
                            auto_gain = gains[gain_idx]
                            auto_itime = itimes[itime_idx]
                        self.set_power(0)
                        itime_idx -= 1
                        self.set_integration_time(itimes[itime_idx])
                        self.set_power(1)
                        self.sample_delay()
                        #logger.info(4)
                        raw_counts = counts_func()
                        if itime_idx <= 0:
                            break
                    lux = self.scale_lux(raw_counts)
                    auto_gain = gains[gain_idx]
                    auto_itime = itimes[itime_idx]
                self.set_power(0)
        lux = self.scale_lux(raw_counts)
        auto_gain = gains[gain_idx]
        auto_itime = itimes[itime_idx]

        return round(lux, 2)

    def get_auto_ALS_lux(self):
        return self.get_auto_X_lux(self.get_ALS)

    def get_auto_white_lux(self):
        return self.get_auto_X_lux(self.get_ALS)

    def sample_delay(self):
        #logger.info(2)
        itime = self.get_integration_time()
        # extend nominal delay to ensure new sample is generated
        if itime == self.ALS_INTEGRATION_25ms:
            time.sleep(0.05)
        elif itime == self.ALS_INTEGRATION_50ms:
            time.sleep(0.1)
        elif itime == self.ALS_INTEGRATION_100ms:
            time.sleep(0.2)
        elif itime == self.ALS_INTEGRATION_200ms:
            time.sleep(0.4)
        elif itime == self.ALS_INTEGRATION_400ms:
            time.sleep(0.8)
        elif itime == self.ALS_INTEGRATION_800ms:
            time.sleep(1.6)
        else:
            time.sleep(0.2)

    '''
      @brief writes data to a register
      @param reg register address
             data written data
    '''
    def _write_reg(self, reg, data):
        '''Low level register writing, not implemented in base class'''
        raise NotImplementedError()

    '''
      @brief read the data from the register
      @param reg register address
             length read data length
    '''
    def _read_reg(self, reg, length):
        '''Low level register writing, not implemented in base class'''
        raise NotImplementedError()


'''@brief An example of an i2c interface module'''
class VEML7700_I2C(VEML7700):
    '''
      @brief Module I2C communication init
    '''
    def __init__(self, bus_num=1):
        self._addr = VEML7000_I2C_ADDRESS
        self.i2c = I2C(bus_num)
        super(VEML7700_I2C, self).__init__()

    ''' 
      @brief   开始函数，探测传感器是否正常连接
      @return  初始化成功，返回ERR_OK，失败返回对应的错误码
    '''
    def begin(self):
     return super(VEML7700_I2C,self).begin()

    '''
      @brief writes data to a register
      @param reg register address
             data written data
    '''
    def _write_reg(self, reg, data):
        if isinstance(data, int):
            data = [data]
            #logger.info(data)
        self.i2c.writeto_mem(self._addr, reg, data)

    '''
      @brief read the data from the register
      @param reg register address
             length read data length
    '''
    def _read_reg(self, reg, length):
        return self.i2c.readfrom_mem(self._addr, reg, length)
