# -*- coding: utf-8 -*-
from pinpong.board import gboard,I2C

class CCS811_Emode(enumerate):
    eMode0 = 0 #Idle (Measurements are disabled in this mode)
    eMode1 = 1 #Constant power mode, IAQ measurement every second
    eMode2 = 2#Pulse heating mode IAQ measurement every 10 seconds
    eMode3 = 3 #Low power pulse heating mode IAQ measurement every 60 seconds
    eMode4 = 4 #Constant power mode, sensor measurement every 250ms 1xx: Reserved modes (For future use)

class CCS811_Ecycle(enumerate):
    eClosed = 0      #Idle (Measurements are disabled in this mode)
    eCycle_1s = 1    #Constant power mode, IAQ measurement every second
    eCycle_10s = 2   #Pulse heating mode IAQ measurement every 10 seconds
    eCycle_60s = 3   #Low power pulse heating mode IAQ measurement every 60 seconds
    eCycle_250ms = 4   #Constant power mode, sensor measurement every 250ms 1xx: Reserved modes (For future use)

class CCS811:
    CCS811_REG_STATUS                      =  0x00
    CCS811_REG_MEAS_MODE                   =  0x01
    CCS811_REG_ALG_RESULT_DATA             =  0x02
    CCS811_REG_RAW_DATA                    =  0x03
    CCS811_REG_ENV_DATA                    =  0x05
    CCS811_REG_NTC                         =  0x06
    CCS811_REG_THRESHOLDS                  =  0x10
    CCS811_REG_BASELINE                    =  0x11
    CCS811_REG_HW_ID                       =  0x20
    CCS811_REG_HW_VERSION                  =  0x21
    CCS811_REG_FW_BOOT_VERSION             =  0x23
    CCS811_REG_FW_APP_VERSION              =  0x24
    CCS811_REG_INTERNAL_STATE              =  0xA0
    CCS811_REG_ERROR_ID                    =  0xE0
    CCS811_REG_SW_RESET                    =  0xFF

    CCS811_BOOTLOADER_APP_ERASE            =  0xF1
    CCS811_BOOTLOADER_APP_DATA             =  0xF2
    CCS811_BOOTLOADER_APP_VERIFY           =  0xF3
    CCS811_BOOTLOADER_APP_START            =  0xF4

    CCS811_HW_ID                           =  0x81

    CCS811_IIC_ADDR                        =  0x5A
    
    def __init__(self, board=None, i2c_addr=CCS811_IIC_ADDR,bus_num=0):
        if isinstance(board, int):
            i2c_addr = board 
            board = gboard 

        self.board = board
        self.i2c_addr = i2c_addr
        self.i2c = I2C(bus_num)
        self.buf=[]

        #set default parameters
        self.i2c.writeto_mem(self.i2c_addr, self.CCS811_BOOTLOADER_APP_START, []) 
        self._set_measurement_mode(0,0,CCS811_Emode.eMode4)
        self._set_in_temp_hum(25,50)
        self.set_meas_cycle(CCS811_Ecycle.eCycle_250ms)
        #obtain sensor ID
        # self._read_reg(self.CCS811_REG_HW_ID, 1)
        # if self.buf[0] != 129:
        #     print("chip version mismatch")
        #     print("returned ID:",self.buf)
        

    # def begin(self):
    #     self._read_reg(self.CCS811_REG_HW_ID, 1)
    #     if self.buf[0] != 129:
    #         print("chip version mismatch")
    #         print("returned ID:",self.buf)
    #     else:
    #         self.i2c.writeto_mem(self.i2c_addr, self.CCS811_BOOTLOADER_APP_START, []) 
    #         self._set_measurement_mode(0,0,CCS811_Emode.eMode4)
    #         self._set_in_temp_hum(25,50)
    #         return True        

    def _read_reg(self, reg, size):
        #print("reg:", reg)
        self.buf = self.i2c.readfrom_mem(self.i2c_addr, reg, size)
        #print("buf:",self.buf)

        
    def _set_measurement_mode(self, thresh, interrupt, mode):  
        measurement=[0]    
        measurement[0] = (thresh<<2)|(interrupt<<3)|(mode<<4)
        self.i2c.writeto_mem(self.i2c_addr, self.CCS811_REG_MEAS_MODE, measurement)
        #print("_setMeasurementMode",measurement)

    def _set_in_temp_hum(self, temperature, humidity):# compensate for temperature and relative humidity
        envData=[0]*4

        if(temperature>0):
            _temp=int(temperature+0.5)
        else:
            _temp=int(temperature-0.5)

        _rh = int(humidity+0.5)
        envData[0] = _rh << 1
        envData[1] = 0
        envData[2] = _temp << 1
        envData[3] = 0
        self.i2c.writeto_mem(self.i2c_addr, self.CCS811_REG_ENV_DATA, envData)
        #print("_setInTempHum",envData)


    def check_data_ready(self):
        self._read_reg(self.CCS811_REG_STATUS,1)
        #print("check data ready:", self.buf)
        #print("if", not((self.buf[0]>>3)& 0x01))
        if(not((self.buf[0]>>3)& 0x01)):
            #print("data not ready")
            return False
        else:
            #print("data's ready")
            return True 

    def set_meas_cycle(self, ecycle):
        measurement=[0]  
        measurement[0] = ecycle << 4
        self.i2c.writeto_mem(self.i2c_addr, self.CCS811_REG_MEAS_MODE, measurement)
        #print("set_meas_cycle",measurement)

    def read_baseline(self):
        self.buf = self.i2c.readfrom_mem_restart_transmission(self.i2c_addr, self.CCS811_REG_BASELINE, 2)
        #print("baseline:",self.buf)
        #print("raw base line", self.buf)
        self.baseline = self.buf[0]<<8|self.buf[1]
        return self.baseline

    def co2_ppm(self):
        self.buf = self.i2c.readfrom_mem_restart_transmission(self.i2c_addr, self.CCS811_REG_ALG_RESULT_DATA, 8)
        #print("baseline:",self.buf)
        #print("raw base line", self.buf)
        self.eCO2 = self.buf[0]<<8|self.buf[1]
        return self.eCO2

    def tvoc_ppb(self):
        self.buf = self.i2c.readfrom_mem_restart_transmission(self.i2c_addr, self.CCS811_REG_ALG_RESULT_DATA, 8)
        #print("baseline:",self.buf)
        #print("raw base line", self.buf)
        self.eTVOC = self.buf[2]<<8|self.buf[3]
        return self.eTVOC

    def write_base_line(self,baseline):
        data=[0]*2
        data[0] = baseline>>8
        data[1] = baseline
        self.i2c.writeto_mem(self.i2c_addr, self.CCS811_REG_BASELINE, data)


