# -*- coding: utf-8 -*-
import time
import logging
from pinpong.board import gboard,I2C
from ctypes import *

logger = logging.getLogger()
logger.setLevel(logging.INFO)  #显示所有的打印信息
#logger.setLevel(logging.FATAL)#如果不想显示过多打印，只打印错误，请使用这个选项
ph = logging.StreamHandler()
formatter = logging.Formatter("%(asctime)s - [%(filename)s %(funcName)s]:%(lineno)d - %(levelname)s: %(message)s")
ph.setFormatter(formatter) 
logger.addHandler(ph)

DFROBOT_BMP3XX_IIC_ADDR_SDO_GND = 0x76  # SDO接地时IIC通信地址
DFROBOT_BMP3XX_IIC_ADDR_SDO_VDD = 0x77  # SDO接电时IIC通信地址

BMP388_CHIP_ID = 0x50      # BMP388芯片版本
BMP390L_CHIP_ID = 0x60     # BMP390L芯片版本

# BMP3XX寄存器地址
BMP3XX_CHIP_ID = 0x00      # The “CHIP_ID” register contains the chip identification code.
BMP3XX_REV_ID = 0x01       # The “Rev_ID” register contains the mask revision of the ASIC.
BMP3XX_ERR_REG = 0x02      # Sensor Error conditions are reported in the “ERR_REG” register.
BMP3XX_STATUS = 0x03       # The Sensor Status Flags are stored in the “STATUS” register.
BMP3XX_P_DATA_PA = 0x04    # The 24Bit pressure data is split and stored in three consecutive registers.
BMP3XX_T_DATA_C = 0x07     # The 24Bit temperature data is split and stored in three consecutive registered.
BMP3XX_TIME = 0x0C         # The 24Bit sensor time is split and stored in three consecutive registers.
BMP3XX_EVENT = 0x10        # The “EVENT” register contains the sensor status flags.
BMP3XX_INT_STATUS = 0x11   # The “INT_STATUS” register shows interrupt status and is cleared after reading.
BMP3XX_FIFO_LENGTH = 0x12  # The FIFO byte counter indicates the current fill level of the FIFO buffer.
BMP3XX_FIFO_DATA = 0x14    # The “FIFO_DATA” is the data output register.
BMP3XX_FIFO_WTM_0 = 0x15   # The FIFO Watermark size is 9 Bit and therefore written to the FIFO_WTM_0 and FIFO_WTM_1 registers.
BMP3XX_FIFO_WTM_1 = 0x16   # The FIFO Watermark size is 9 Bit and therefore written to the FIFO_WTM_0 and FIFO_WTM_1 registers.
BMP3XX_FIFO_CONF_1 = 0x17  # The “FIFO_CONFIG_1” register contains the FIFO frame content configuration.
BMP3XX_FIFO_CONF_2 = 0x18  # The “FIFO_CONFIG_2” register extends the FIFO_CONFIG_1 register.
BMP3XX_INT_CTRL = 0x19     # Interrupt configuration can be set in the “INT_CTRL” register.
BMP3XX_IF_CONF = 0x1A      # The “IF_CONF” register controls the serial interface settings.
BMP3XX_PWR_CTRL = 0x1B     # The “PWR_CTRL” register enables or disables pressure and temperature measurement.
BMP3XX_OSR = 0x1C          # The “OSR” register controls the oversampling settings for pressure and temperature measurements.
BMP3XX_ODR = 0x1D          # The “ODR” register set the configuration of the output data rates by means of setting the subdivision/subsampling.
BMP3XX_IIR_CONFIG = 0x1F   # The “CONFIG” register controls the IIR filter coefficients
BMP3XX_CALIB_DATA = 0x31   # 0x31-0x45为校准数据
BMP3XX_CMD = 0x7E          # 命令寄存器,可软复位和清除FIFO所有数据

# 传感器配置
SLEEP_MODE  = 0x00   # 睡眠模式：上电复位后默认设置为睡眠模式。在睡眠模式下，不执行任何测量，并且功耗最少。所有寄存器均可访问；可以读取芯片ID和补偿系数。 */
FORCED_MODE = 0x10   # 强制模式：在强制模式下，根据选择的测量和滤波选项进行单个测量。测量完成后，传感器返回睡眠模式，测量结果可从数据寄存器中获得。 */
NORMAL_MODE = 0x30   # 正常模式：在测量周期和待机周期之间连续循环。输出数据率(output data rates)与ODR模式设置有关。 */

BMP3XX_PRESS_OSR_SETTINGS = (0x00, 0x01, 0x02, 0x03, 0x04, 0x05)  # pressure oversampling settings
BMP3XX_TEMP_OSR_SETTINGS = (0x00, 0x08, 0x10, 0x18, 0x20, 0x28)  # temperature oversampling settings

BMP3XX_IIR_CONFIG_COEF_0   = 0x00   # 滤波系数为0 ->旁路模式
BMP3XX_IIR_CONFIG_COEF_1   = 0x02   # 滤波系数为1
BMP3XX_IIR_CONFIG_COEF_3   = 0x04   # 滤波系数为3
BMP3XX_IIR_CONFIG_COEF_7   = 0x06   # 滤波系数为7
BMP3XX_IIR_CONFIG_COEF_15  = 0x08   # 滤波系数为15
BMP3XX_IIR_CONFIG_COEF_31  = 0x0A   # 滤波系数为31
BMP3XX_IIR_CONFIG_COEF_63  = 0x0C   # 滤波系数为63
BMP3XX_IIR_CONFIG_COEF_127 = 0x0E   # 滤波系数为127

BMP3XX_ODR_200_HZ    = 0x00   # 预分频:1; ODR 200Hz; 采样周期:5 ms
BMP3XX_ODR_100_HZ    = 0x01   # 预分频:2; 采样周期:10 ms
BMP3XX_ODR_50_HZ     = 0x02   # 预分频:4; 采样周期:20 ms
BMP3XX_ODR_25_HZ     = 0x03   # 预分频:8; 采样周期:40 ms
BMP3XX_ODR_12P5_HZ   = 0x04   # 预分频:16; 采样周期:80 ms
BMP3XX_ODR_6P25_HZ   = 0x05   # 预分频:32; 采样周期:160 ms
BMP3XX_ODR_3P1_HZ    = 0x06   # 预分频:64; 采样周期:320 ms
BMP3XX_ODR_1P5_HZ    = 0x07   # 预分频:127; 采样周期:640 ms
BMP3XX_ODR_0P78_HZ   = 0x08   # 预分频:256; 采样周期:1.280 s
BMP3XX_ODR_0P39_HZ   = 0x09   # 预分频:512; 采样周期:2.560 s
BMP3XX_ODR_0P2_HZ    = 0x0A   # 预分频:1024 采样周期:5.120 s
BMP3XX_ODR_0P1_HZ    = 0x0B   # 预分频:2048; 采样周期:10.24 s
BMP3XX_ODR_0P05_HZ   = 0x0C   # 预分频:4096; 采样周期:20.48 s
BMP3XX_ODR_0P02_HZ   = 0x0D   # 预分频:8192; 采样周期:40.96 s
BMP3XX_ODR_0P01_HZ   = 0x0E   # 预分频:16384; 采样周期:81.92 s
BMP3XX_ODR_0P006_HZ  = 0x0F   # 预分频:32768; 采样周期:163.84 s
BMP3XX_ODR_0P003_HZ  = 0x10   # 预分频:65536; 采样周期:327.68 s
BMP3XX_ODR_0P0015_HZ = 0x11   # 预分频:131072; ODR 25/16384Hz; 采样周期:655.36 s

# 6种常用采样模式
ULTRA_LOW_PRECISION = 0   # 超低精度，适合天气监控（最低功耗），电源模式为强制模式
LOW_PRECISION = 1         # 低精度，适合随意的检测，电源模式为正常模式
NORMAL_PRECISION1 = 2     # 标准精度1，适合在手持式设备上动态检测（例如在手机上），电源模式为正常模式
NORMAL_PRECISION2 = 3     # 标准精度2，适合无人机，电源模式为正常模式
HIGH_PRECISION = 4        # 高精度，适合在低功耗手持式设备上（例如在手机上），电源模式为正常模式
ULTRA_PRECISION = 5       # 超高精度，适合室内的导航，采集速率会极低，采集周期1000ms，电源模式为正常模式

# CMD(0x7E)寄存器命令
BMP3XX_CMD_NOP = 0x00         # reserved. No command.
BMP3XX_CMD_FIFO_FLUSH = 0xB0  # Clears all data in the FIFO, does not change FIFO_CONFIG registers.
BMP3XX_CMD_RESET = 0xB6       # Triggers a reset, all user configuration settings are overwritten with their default state.

# FIFO Header
BMP3_FIFO_TEMP_PRESS_FRAME = 0x94   # FIFO temperature pressure header frame
BMP3_FIFO_TEMP_FRAME = 0x90         # FIFO temperature header frame
BMP3_FIFO_PRESS_FRAME = 0x84        # FIFO pressure header frame
BMP3_FIFO_TIME_FRAME = 0xA0         # FIFO time header frame
BMP3_FIFO_CONFIG_CHANGE = 0x48      # FIFO configuration change header frame
BMP3_FIFO_ERROR_FRAME = 0x44        # FIFO error header frame

# 立即数
BMP3XX_FIFO_DATA_FRAME_LENGTH = 7         # 一帧FIFO数据中每种数据的字节长度为3
STANDARD_SEA_LEVEL_PRESSURE_PA = 101325   # 标准海平面气压，单位pa


class BMP3XX:
    '''
      @brief Module init
    '''
    def __init__(self):
        '''Sea level pressure in Pa.'''
        self.sea_level_pressure = STANDARD_SEA_LEVEL_PRESSURE_PA

    '''
      @brief Initialize sensor
      @return 返回True表示初始化成功，返回False表示初始化失败
    '''
    def begin(self):
        ret = True
        chip_id = self._read_reg(BMP3XX_CHIP_ID, 1)
        logger.info(chip_id[0])
        if chip_id[0] not in (BMP388_CHIP_ID, BMP390L_CHIP_ID):
            ret = False
        self._get_coefficients()
        self.reset()
        return ret

    '''
      @brief 从寄存器获取压力测量值，工作范围（300‒1250 hPa）
             若之前提供了基准值，则根据校准的海平面大气压，计算当前位置气压的绝对值
      @return 返回压力测量值，单位是Pa
    '''
    @property
    def get_pressure(self):
        adc_p, adc_t = self._get_reg_temp_press_data()
        return round(self._compensate_data(adc_p, adc_t)[0], 2)

    '''
      @brief 从寄存器获取温度测量值，工作范围（-40 ‒ +85 °C）
      @return 返回温度测量值，单位是℃
    '''
    @property
    def get_temperature(self):
        adc_p, adc_t = self._get_reg_temp_press_data()
        return round(self._compensate_data(adc_p, adc_t)[1], 2)

    '''
      @brief 以给定的当前位置海拔做为基准值，为后续压力和海拔数据消除绝对差
      @param altitude 当前位置海拔高度
      @return 传入基准值成功，返回ture，失败返回false
    '''
    def calibrated_absolute_difference(self, altitude):
        '''The altitude in meters based on the currently set sea level pressure.'''
        ret = False
        if STANDARD_SEA_LEVEL_PRESSURE_PA == self.sea_level_pressure:
            self.sea_level_pressure = (self.get_pressure / pow(1.0 - (altitude / 44307.7), 5.255302))
            ret = True
        return ret

    '''
      @brief 根据传感器所测量大气压，计算海拔高度
             若之前提供了基准值，则根据校准的海平面大气压，计算当前位置海拔绝对高度
      @return 返回海拔高度，单位m
    '''
    @property
    def get_altitude(self):
        # see https://www.weather.gov/media/epz/wxcalc/pressureAltitude.pdf
        return round(44307.7 * (1 - (self.get_pressure / STANDARD_SEA_LEVEL_PRESSURE_PA) ** 0.190284), 2)

    '''
      @brief 测量模式和电源模式的配置
      @param mode 需要设置的测量电源模式:
             SLEEP_MODE(睡眠模式)：上电复位后默认设置为睡眠模式。在睡眠模式下，不执行任何测量，并且功耗最少。所有寄存器均可访问；可以读取芯片ID和补偿系数。
             FORCED_MODE(强制模式)：在强制模式下，根据选择的测量和滤波选项进行单个测量。测量完成后，传感器返回睡眠模式，测量结果可从数据寄存器中获得。
             NORMAL_MODE(正常模式)：在测量周期和待机周期之间连续循环，输出数据率(output data rates)与ODR模式设置有关。
    '''
    def set_power_mode(self, mode):
        temp = self._read_reg(BMP3XX_PWR_CTRL, 1)[0]
        if (mode | 0x03) == temp:
            logger.info("Same configuration as before!")
        else:
            if mode != SLEEP_MODE:
                self._write_reg(BMP3XX_PWR_CTRL, (SLEEP_MODE & 0x30) | 0x03)
                time.sleep(0.02)
            self._write_reg(BMP3XX_PWR_CTRL, (mode & 0x30) | 0x03)
            time.sleep(0.02)

    '''
      @brief 打开或者关闭FIFO
      @param mode：
             True：打开FIFO
             False：关闭FIFO
    '''
    def enable_fifo(self, mode):
        if mode:
            '''使能，并初始化FIFO配置。'''
            self._write_reg(BMP3XX_FIFO_CONF_1, 0x1D)
            self._write_reg(BMP3XX_FIFO_CONF_2, 0x0C)
        else:
            '''关闭FIFO。'''
            self._write_reg(BMP3XX_FIFO_CONF_1, 0x1C)
            self._write_reg(BMP3XX_FIFO_CONF_2, 0x0C)
        time.sleep(0.02)

    '''
      @brief 压力和温度测量的过采样配置(OSR:over-sampling register)
      @param mode 需要设置的压力和温度测量的过采样模式:
             6种压力过采样模式:
               BMP3XX_PRESS_OSR_SETTINGS[0], 压力采样×1，16 bit / 2.64 Pa（推荐温度过采样×1）
               BMP3XX_PRESS_OSR_SETTINGS[1], 压力采样×2，16 bit / 2.64 Pa（推荐温度过采样×1）
               BMP3XX_PRESS_OSR_SETTINGS[2], 压力采样×4，18 bit / 0.66 Pa（推荐温度过采样×1）
               BMP3XX_PRESS_OSR_SETTINGS[3], 压力采样×8，19 bit / 0.33 Pa（推荐温度过采样×2）
               BMP3XX_PRESS_OSR_SETTINGS[4], 压力采样×16，20 bit / 0.17 Pa（推荐温度过采样×2）
               BMP3XX_PRESS_OSR_SETTINGS[5], 压力采样×32，21 bit / 0.085 Pa（推荐温度过采样×2）
             6种温度过采样模式
               BMP3XX_TEMP_OSR_SETTINGS[0], 温度采样×1，16 bit / 0.0050 °C
               BMP3XX_TEMP_OSR_SETTINGS[1], 温度采样×2，16 bit / 0.0025 °C
               BMP3XX_TEMP_OSR_SETTINGS[2], 温度采样×4，18 bit / 0.0012 °C
               BMP3XX_TEMP_OSR_SETTINGS[3], 温度采样×8，19 bit / 0.0006 °C
               BMP3XX_TEMP_OSR_SETTINGS[4], 温度采样×16，20 bit / 0.0003 °C
               BMP3XX_TEMP_OSR_SETTINGS[5], 温度采样×32，21 bit / 0.00015 °C
    '''
    def set_oversampling(self, press_osr_set, temp_osr_set):
        self._write_reg(BMP3XX_OSR, (press_osr_set + temp_osr_set) & 0x3F)

    '''
      @brief IIR滤波系数配置（IIR filtering）
      @param mode IIR滤波系数设置，可配置模式：
             BMP3XX_IIR_CONFIG_COEF_0，BMP3XX_IIR_CONFIG_COEF_1，BMP3XX_IIR_CONFIG_COEF_3，
             BMP3XX_IIR_CONFIG_COEF_7，BMP3XX_IIR_CONFIG_COEF_15，BMP3XX_IIR_CONFIG_COEF_31，
             BMP3XX_IIR_CONFIG_COEF_63，BMP3XX_IIR_CONFIG_COEF_127
    '''
    def filter_coefficient(self, iir_config_coef):
        '''The IIR filter coefficient.'''
        self._write_reg(BMP3XX_IIR_CONFIG, iir_config_coef & 0x0E)

    '''
      @brief 细分/二次采样的方式设置输出数据率配置(ODR:output data rates)
      @param mode 需要设置的输出数据率,可配置模式：
             BMP3XX_ODR_200_HZ，BMP3XX_ODR_100_HZ，BMP3XX_ODR_50_HZ，BMP3XX_ODR_25_HZ，BMP3XX_ODR_12P5_HZ，
             BMP3XX_ODR_6P25_HZ，BMP3XX_ODR_3P1_HZ，BMP3XX_ODR_1P5_HZ，BMP3XX_ODR_0P78_HZ，BMP3XX_ODR_0P39_HZ，
             BMP3XX_ODR_0P2_HZ，BMP3XX_ODR_0P1_HZ，BMP3XX_ODR_0P05_HZ，BMP3XX_ODR_0P02_HZ，BMP3XX_ODR_0P01_HZ，
             BMP3XX_ODR_0P006_HZ，BMP3XX_ODR_0P003_HZ，BMP3XX_ODR_0P0015_HZ
      @return 返回True表示配置成功，返回False表示配置失败，保持原来的状态
    '''
    def set_output_data_rates(self, odr_set):
        '''The IIR filter coefficient.'''
        ret = True
        self._write_reg(BMP3XX_ODR, odr_set & 0x1F)
        if (self._read_reg(BMP3XX_ERR_REG, 1)[0] & 0x04):
            logger.warning("Sensor configuration error detected!")
            ret = False
        return ret

    '''
      @brief 将传入的uint8类型的数据转换为int型
      @return 转化为int类型的数据
    '''
    def _uint8_to_int(self,num):
        if(num>127):
            num = num - 256
        return num

    '''
      @brief 将传入的uint16类型的数据转换为int型
      @return 转化为int类型的数据
    '''
    def _uint16_to_int(self,num):
        if(num>32767):
            num = num - 65536
        return num

    '''
      @brief 让用户方便配置常用的采样模式
      @param mode:
             ULTRA_LOW_PRECISION，超低精度，适合天气监控（最低功耗），电源模式为强制模式
             LOW_PRECISION，低精度，适合随意的检测，电源模式为正常模式
             NORMAL_PRECISION1，标准精度1，适合在手持式设备上动态检测（例如在手机上），电源模式为正常模式
             NORMAL_PRECISION2，标准精度2，适合无人机，电源模式为正常模式
             HIGH_PRECISION，高精度，适合在低功耗手持式设备上（例如在手机上），电源模式为正常模式
             ULTRA_PRECISION，超高精度，适合室内的导航，采集速率会极低，采集周期1000ms，电源模式为正常模式
      @return 返回True表示配置成功，返回False表示配置失败，保持原来的状态
    '''
    def set_common_sampling_mode(self, mode):
        ret = True
        if mode == ULTRA_LOW_PRECISION:
            self.set_power_mode(FORCED_MODE)
            self.set_oversampling(BMP3XX_PRESS_OSR_SETTINGS[0], BMP3XX_TEMP_OSR_SETTINGS[0])
            self.filter_coefficient(BMP3XX_IIR_CONFIG_COEF_0)
            self.set_output_data_rates(BMP3XX_ODR_0P01_HZ)
        elif mode == LOW_PRECISION:
            self.set_power_mode(NORMAL_MODE)
            self.set_oversampling(BMP3XX_PRESS_OSR_SETTINGS[1], BMP3XX_TEMP_OSR_SETTINGS[0])
            self.filter_coefficient(BMP3XX_IIR_CONFIG_COEF_0)
            self.set_output_data_rates(BMP3XX_ODR_100_HZ)
        elif mode == NORMAL_PRECISION1:
            self.set_power_mode(NORMAL_MODE)
            self.set_oversampling(BMP3XX_PRESS_OSR_SETTINGS[2], BMP3XX_TEMP_OSR_SETTINGS[0])
            self.filter_coefficient(BMP3XX_IIR_CONFIG_COEF_3)
            self.set_output_data_rates(BMP3XX_ODR_50_HZ)
        elif mode == NORMAL_PRECISION2:
            self.set_power_mode(NORMAL_MODE)
            self.set_oversampling(BMP3XX_PRESS_OSR_SETTINGS[3], BMP3XX_TEMP_OSR_SETTINGS[0])
            self.filter_coefficient(BMP3XX_IIR_CONFIG_COEF_1)
            self.set_output_data_rates(BMP3XX_ODR_50_HZ)
        elif mode == HIGH_PRECISION:
            self.set_power_mode(NORMAL_MODE)
            self.set_oversampling(BMP3XX_PRESS_OSR_SETTINGS[3], BMP3XX_TEMP_OSR_SETTINGS[0])
            self.filter_coefficient(BMP3XX_IIR_CONFIG_COEF_1)
            self.set_output_data_rates(BMP3XX_ODR_12P5_HZ)
        elif mode == ULTRA_PRECISION:
            self.set_power_mode(NORMAL_MODE)
            self.set_oversampling(BMP3XX_PRESS_OSR_SETTINGS[4], BMP3XX_TEMP_OSR_SETTINGS[1])
            self.filter_coefficient(BMP3XX_IIR_CONFIG_COEF_3)
            self.set_output_data_rates(BMP3XX_ODR_25_HZ)
        else:
            ret = False
        return ret

    '''
      @brief 使能传感器数据准备就绪信号的中断
             注：因中断引脚唯一，故3种中断设定为单独使用，使用时请注释其余两个中断函数
    '''
    def enable_data_ready_int(self):
        self._write_reg(BMP3XX_INT_CTRL, 0x42)
        time.sleep(0.02)

    '''
      @brief 使能传感器FIFO到达水位信号的中断
             注：因中断引脚唯一，故3种中断设定为单独使用，使用时请注释其余两个中断函数
      @param wtm_value:设定FIFO的水位值（范围：0-511）
    '''
    def enable_fifo_wtm_int(self, wtm_value):
        self._write_reg(BMP3XX_INT_CTRL, 0x0A)
        self._write_reg(BMP3XX_FIFO_WTM_0, wtm_value & 0xFF)
        self._write_reg(BMP3XX_FIFO_WTM_1, (wtm_value >> 8) & 0x01)
        time.sleep(0.02)

    '''
      @brief 使能传感器FIFO存满信号的中断
             注：因中断引脚唯一，故3种中断设定为单独使用，使用时请注释其余两个中断函数
    '''
    def enable_fifo_full_int(self):
        self._write_reg(BMP3XX_INT_CTRL, 0x12)
        time.sleep(0.02)

    '''
      @brief 获取传感器NVM寄存器中的校准数据
    '''
    def _get_coefficients(self):
        calib = self._read_reg(BMP3XX_CALIB_DATA, 21)
        self._data_calib = (
            ((calib[1] << 8) | calib[0]) / 2 ** -8.0,  # T1
            ((calib[3] << 8) | calib[2]) / 2 ** 30.0,  # T2
            self._uint8_to_int(calib[4]) / 2 ** 48.0,  # T3
            (self._uint16_to_int((calib[6] << 8) | calib[5]) - 2 ** 14.0) / 2 ** 20.0,  # P1
            (self._uint16_to_int((calib[8] << 8) | calib[7]) - 2 ** 14.0) / 2 ** 29.0,  # P2
            self._uint8_to_int(calib[9]) / 2 ** 32.0,  # P3
            self._uint8_to_int(calib[10]) / 2 ** 37.0,  # P4
            ((calib[12] << 8) | calib[11]) / 2 ** -3.0,  # P5
            ((calib[14] << 8) | calib[13]) / 2 ** 6.0,  # P6
            self._uint8_to_int(calib[15]) / 2 ** 8.0,  # P7
            self._uint8_to_int(calib[16]) / 2 ** 15.0,  # P8
            (self._uint16_to_int(calib[18] << 8) | calib[17]) / 2 ** 48.0,  # P9
            self._uint8_to_int(calib[19]) / 2 ** 48.0,  # P10
            self._uint8_to_int(calib[20]) / 2 ** 65.0,  # P11
        )

    '''
      @brief 利用以获取的校准数据，对测量数据原始值进行校准补偿
      @return 返回校准后的压力数据pressure和校准后的温度数据temperature
              温度单位摄氏度，压力单位帕
    '''
    def _compensate_data(self, adc_p, adc_t):
        # datasheet, p28，Trimming Coefficient listing in register map with size and sign attributes
        t1, t2, t3, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11 = self._data_calib
        #logger.info(self._data_calib)

        # 温度补偿
        pd1 = adc_t - t1
        pd2 = pd1 * t2
        temperature = pd2 + (pd1 * pd1) * t3

        # 压力补偿
        pd1 = p6 * temperature
        pd2 = p7 * temperature ** 2.0
        pd3 = p8 * temperature ** 3.0
        po1 = p5 + pd1 + pd2 + pd3
        pd1 = p2 * temperature
        pd2 = p3 * temperature ** 2.0
        pd3 = p4 * temperature ** 3.0
        po2 = adc_p * (p1 + pd1 + pd2 + pd3)
        pd1 = adc_p ** 2.0
        pd2 = p9 + p10 * temperature
        pd3 = pd1 * pd2
        pd4 = pd3 + p11 * adc_p ** 3.0
        pressure = po1 + po2 + pd4 - self.sea_level_pressure + STANDARD_SEA_LEVEL_PRESSURE_PA

        return round(pressure, 2), round(temperature, 2)

    '''
      @brief 从寄存器中获取未补偿校准的压力和温度的原始测量数据
      @return 返回压力和温度的原始测量数据：adc_p, adc_t
    '''
    def _get_reg_temp_press_data(self):
        data = self._read_reg(BMP3XX_P_DATA_PA, 6)
        adc_p = data[2] << 16 | data[1] << 8 | data[0]
        adc_t = data[5] << 16 | data[4] << 8 | data[3]
        #logger.info(adc_p)
        #logger.info(adc_t)
        return adc_p, adc_t

    '''
      @brief 获取FIFO中缓存的数据
      @return 返回校准后的压力数据pressure和校准后的温度数据temperature
              温度单位摄氏度，压力单位帕
    '''
    def get_fifo_temp_press_data(self):
        data = self._read_reg(BMP3XX_FIFO_DATA, BMP3XX_FIFO_DATA_FRAME_LENGTH)
        adc_p, adc_t = 0, 0
        if data[0] == BMP3_FIFO_TEMP_PRESS_FRAME:
            adc_t = data[3] << 16 | data[2] << 8 | data[1]
            adc_p = data[6] << 16 | data[5] << 8 | data[4]
        elif data[0] == BMP3_FIFO_TEMP_FRAME:
            adc_t = data[3] << 16 | data[2] << 8 | data[1]
        elif data[0] == BMP3_FIFO_PRESS_FRAME:
            adc_p = data[3] << 16 | data[2] << 8 | data[1]
        elif data[0] == BMP3_FIFO_TIME_FRAME:
            logger.info(f"FIFO time:{data[3] << 16 | data[2] << 8 | data[1]}")
        elif data[0] == BMP3_FIFO_CONFIG_CHANGE:
            logger.warning("FIFO config change!!!")
        else:
            logger.warning("FIFO ERROR!!!")
        #logger.info(data[0])
        #logger.info(adc_p)
        #logger.info(adc_t)

        if (adc_p > 0):
            adc_p, adc_t = self._compensate_data(adc_p, adc_t)

        return adc_p, adc_t

    '''
      @brief 获取FIFO已缓存数据大小
      @return 返回值范围为：0-511
    '''
    def get_fifo_length(self):
        len = self._read_reg(BMP3XX_FIFO_LENGTH, 2)
        return len[0] + (len[1] << 8)

    '''
      @brief 清空FIFO已缓存的数据，不改变FIFO配置
    '''
    def empty_fifo(self):
        self._write_reg(BMP3XX_CMD, BMP3XX_CMD_FIFO_FLUSH)
        time.sleep(0.02)

    '''
      @brief 传感器复位，重启传感器，使传感器配置全部恢复为默认配置
    '''
    def reset(self):
        self._write_reg(BMP3XX_CMD, BMP3XX_CMD_RESET)

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


class BMP3XX_I2C(BMP3XX):
    '''
      @brief Module I2C communication init
    '''
    def __init__(self, iic_addr=0x77, bus_num=1):
        self._addr = iic_addr
        self.i2c = I2C(bus_num)
        super(BMP3XX_I2C, self).__init__()

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


class BMP3XX_SPI(BMP3XX):
    '''
      @brief Module SPI communication init
    '''
    def __init__(self, cs=8, bus=0, dev=0, speed=8000000):
        self._cs = cs
        self.spi = spidev.SpiDev()
        self.spi.open(bus, dev)
        self.spi.no_cs = True
        self.spi.max_speed_hz = speed
        super(BMP3XX_SPI, self).__init__()

    '''
      @brief writes data to a register
      @param reg register address
             data written data
    '''
    def _write_reg(self, reg, data):
        if isinstance(data, int):
            data = [data]
            #logger.info(data)
        reg_addr = [reg & 0x7f]
        self.spi.xfer(reg_addr)
        self.spi.xfer(data)

    '''
      @brief read the data from the register
      @param reg register address
             length read data length
    '''
    def _read_reg(self, reg, length):
        reg_addr = [reg | 0x80]
        #logger.info(reg_addr)
        self.spi.xfer(reg_addr)
        time.sleep(0.01)
        self.spi.readbytes(1)
        rslt = self.spi.readbytes(length)
        return rslt
