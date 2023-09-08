# -*- coding: utf-8 -*
import time
from pinpong.board import gboard, I2C


class GravityPM25(object):
    # Select PM type
    PARTICLE_PM1_0_STANDARD = 0X05
    PARTICLE_PM2_5_STANDARD = 0X07
    PARTICLE_PM10_STANDARD = 0X09
    PARTICLE_PM1_0_ATMOSPHERE = 0X0B
    PARTICLE_PM2_5_ATMOSPHERE = 0X0D
    PARTICLE_PM10_ATMOSPHERE = 0X0F
    PARTICLENUM_0_3_UM_EVERY0_1L_AIR = 0X11
    PARTICLENUM_0_5_UM_EVERY0_1L_AIR = 0X13
    PARTICLENUM_1_0_UM_EVERY0_1L_AIR = 0X15
    PARTICLENUM_2_5_UM_EVERY0_1L_AIR = 0X17
    PARTICLENUM_5_0_UM_EVERY0_1L_AIR = 0X19
    PARTICLENUM_10_UM_EVERY0_1L_AIR = 0X1B
    PARTICLENUM_GAIN_VERSION = 0X1D

    def __init__(self):
        pass

    def gain_particle_concentration_ugm3(self, PMtype):
        '''!
        @brief Get PM concentration of a specified type
        @param type PARTICLE_PM1_0_STANDARD
        @n          PARTICLE_PM2_5_STANDARD  
        @n          PARTICLE_PM10_STANDARD   
        @n          PARTICLE_PM1_0_ATMOSPHERE
        @n          PARTICLE_PM2_5_ATMOSPHERE
        @n          PARTICLE_PM10_ATMOSPHERE 
        @return Concentration（ug/m3）
        '''
        buf = self.read_reg(PMtype, 2)
        concentration = (buf[0] << 8) + buf[1]
        return concentration

    def gain_particlenum_every0_1l(self,PMtype):
        '''!
        @brief Get the number of PM in 0.1L of air
        @param type PARTICLENUM_0_3_UM_EVERY0_1L_AIR
        @n          PARTICLENUM_0_5_UM_EVERY0_1L_AIR
        @n          PARTICLENUM_1_0_UM_EVERY0_1L_AIR
        @n          PARTICLENUM_2_5_UM_EVERY0_1L_AIR
        @n          PARTICLENUM_5_0_UM_EVERY0_1L_AIR
        @n          PARTICLENUM_10_UM_EVERY0_1L_AIR 
        @return PM number
        '''
        buf = self.read_reg(PMtype,2)
        particlenum = (buf[0] << 8) + buf[1]
        return particlenum
    
    def gain_version(self):
        '''!
        @brief Write data to the specified register of the sensor
        @return Firmware version
        '''
        version = self.read_reg(self.PARTICLENUM_GAIN_VERSION,1)
        return version[0]

    def set_lowpower(self):
        '''
        @brief Control the sensor to enter low-power mode
        '''
        mode = [0x01]
        self.write_reg(0x01,mode)
        
    def awake(self):
        '''
        @brief Wake up sensor
        '''
        mode = [0x02]
        self.write_reg(0x01,mode)

class DFRobot_GravityPM25(GravityPM25):
    def __init__(self, board=None, i2c_addr=0x19, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.__addr = i2c_addr
        self._i2c = I2C(bus_num)
        super(DFRobot_GravityPM25, self).__init__()

    def write_reg(self, reg, data):
        self._i2c.writeto_mem(self.__addr, reg, data)

    def read_reg(self, reg, len):
        rslt = self._i2c.readfrom_mem(self.__addr, reg, len)
        return rslt
