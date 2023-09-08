import time
from pinpong.board import gboard,I2C
import math

# BME680 I2C addresses
_BME680_I2C_ADDR_PRIMARY = 0x76
_BME680_I2C_ADDR_SECONDARY = 0x77

# BME680 unique chip identifier
_BME680_CHIP_ID = 0x61

# BME680 coefficients related defines
_BME680_COEFF_SIZE = 41
_BME680_COEFF_ADDR1_LEN = 25
_BME680_COEFF_ADDR2_LEN = 16

# BME680 field_x related defines
_BME680_FIELD_LENGTH = 15
_BME680_FIELD_ADDR_OFFSET = 17

# BME680 coefficients related defines
_BME680_COEFF_SIZE = 41
_BME680_COEFF_ADDR1_LENCOEFF_ADDR1_LEN = 25
_BME680_COEFF_ADDR2_LENCOEFF_ADDR2_LEN = 16

# BME680 field_x related defines
_BME680_FIELD_LENGTHFIELD_LENGTH = 15
_BME680_FIELD_ADDR_OFFSETFIELD_ADDR_OFFSET = 17

# Soft reset command
_BME680_SOFT_RESET_CMD = 0xb6

# Error code definitions
_BME680_OK = 0
# Errors
_BME680_E_NULL_PTR = -1
_BME680_E_COM_FAIL = -2
_BME680_E_DEV_NOT_FOUND = -3
_BME680_E_INVALID_LENGTH = -4

# Warnings
_BME680_W_DEFINE_PWR_MODE = 1
_BME680_W_NO_NEW_DATA = 2

# Info's
_BME680_I_MIN_CORRECTION = 1
_BME680_I_MAX_CORRECTION = 2

# Register map
# Other coefficient's address
_BME680_ADDR_RES_HEAT_VAL_ADDR = 0x00
_BME680_ADDR_RES_HEAT_RANGE_ADDR = 0x02
_BME680_ADDR_RANGE_SW_ERR_ADDR = 0x04
_BME680_ADDR_SENS_CONF_START = 0x5A
_BME680_ADDR_GAS_CONF_START = 0x64

# Field settings
_BME680_FIELD0_ADDR = 0x1d

# Heater settings
_BME680_RES_HEAT0_ADDR = 0x5a
_BME680_GAS_WAIT0_ADDR = 0x64

# Sensor configuration registers
_BME680_CONF_HEAT_CTRL_ADDR = 0x70
_BME680_CONF_ODR_RUN_GAS_NBC_ADDR = 0x71
_BME680_CONF_OS_H_ADDR = 0x72
_BME680_MEM_PAGE_ADDR = 0xf3
_BME680_CONF_T_P_MODE_ADDR = 0x74
_BME680_CONF_ODR_FILT_ADDR = 0x75

# Coefficient's address
_BME680_COEFF_ADDR1 = 0x89
_BME680_COEFF_ADDR2 = 0xe1

# Chip identifier
_BME680_CHIP_ID_ADDR = 0xd0

# Soft reset register
_BME680_SOFT_RESET_ADDR = 0xe0

# Array Index to Field data mapping for Calibration Data
_BME680_T2_LSB_REG = 1
_BME680_T2_MSB_REG = 2
_BME680_T3_REG = 3
_BME680_P1_LSB_REG = 5
_BME680_P1_MSB_REG = 6
_BME680_P2_LSB_REG = 7
_BME680_P2_MSB_REG = 8
_BME680_P3_REG = 9
_BME680_P4_LSB_REG = 11
_BME680_P4_MSB_REG = 12
_BME680_P5_LSB_REG = 13
_BME680_P5_MSB_REG = 14
_BME680_P7_REG = 15
_BME680_P6_REG = 16
_BME680_P8_LSB_REG = 19
_BME680_P8_MSB_REG = 20
_BME680_P9_LSB_REG = 21
_BME680_P9_MSB_REG = 22
_BME680_P10_REG = 23
_BME680_H2_MSB_REG = 25
_BME680_H2_LSB_REG = 26
_BME680_H1_LSB_REG = 26
_BME680_H1_MSB_REG = 27
_BME680_H3_REG = 28
_BME680_H4_REG = 29
_BME680_H5_REG = 30
_BME680_H6_REG = 31
_BME680_H7_REG = 32
_BME680_T1_LSB_REG = 33
_BME680_T1_MSB_REG = 34
_BME680_GH2_LSB_REG = 35
_BME680_GH2_MSB_REG = 36
_BME680_GH1_REG = 37
_BME680_GH3_REG = 38

# BME680 register buffer index settings
_BME680_REG_FILTER_INDEX = 5
_BME680_REG_TEMP_INDEX = 4
_BME680_REG_PRES_INDEX = 4
_BME680_REG_HUM_INDEX = 2
_BME680_REG_NBCONV_INDEX = 1
_BME680_REG_RUN_GAS_INDEX = 1
_BME680_REG_HCTRL_INDEX = 0

# Ambient humidity shift value for compensation
_BME680_HUM_REG_SHIFT_VAL = 4

# Delay related macro declaration
_BME680_RESET_PERIOD = 10

# SPI memory page settings
_BME680_MEM_PAGE0 = 0x10
_BME680_MEM_PAGE1 = 0x00

# Run gas enable and disable settings
_BME680_RUN_GAS_DISABLE = 0
_BME680_RUN_GAS_ENABLE = 1

# Buffer length macro declaration
_BME680_TMP_BUFFER_LENGTH = 40
_BME680_REG_BUFFER_LENGTH = 6
_BME680_FIELD_DATA_LENGTH = 3
_BME680_GAS_REG_BUF_LENGTH = 20
_BME680_GAS_HEATER_PROF_LEN_MAX  = 10

# Settings selector
_BME680_OST_SEL = 1
_BME680_OSP_SEL = 2
_BME680_OSH_SEL = 4
_BME680_GAS_MEAS_SEL = 8
_BME680_FILTER_SEL = 16
_BME680_HCNTRL_SEL = 32
_BME680_RUN_GAS_SEL = 64
_BME680_NBCONV_SEL = 128
_BME680_GAS_SENSOR_SEL = _BME680_GAS_MEAS_SEL | _BME680_RUN_GAS_SEL | _BME680_NBCONV_SEL

# Number of conversion settings
_BME680_NBCONV_MIN = 0
_BME680_NBCONV_MAX = 9 # Was 10, but there are only 10 settings: 0 1 2 ... 8 9

# Mask definitions
_BME680_GAS_MEAS_MSK = 0x30
_BME680_NBCONV_MSK = 0X0F
_BME680_FILTER_MSK = 0X1C
_BME680_OST_MSK = 0XE0
_BME680_OSP_MSK = 0X1C
_BME680_OSH_MSK = 0X07
_BME680_HCTRL_MSK = 0x08
_BME680_RUN_GAS_MSK = 0x10
_BME680_MODE_MSK = 0x03
_BME680_RHRANGE_MSK = 0x30
_BME680_RSERROR_MSK = 0xf0
_BME680_NEW_DATA_MSK = 0x80
_BME680_GAS_INDEX_MSK = 0x0f
_BME680_GAS_RANGE_MSK = 0x0f
_BME680_GASM_VALID_MSK = 0x20
_BME680_HEAT_STAB_MSK = 0x10
_BME680_MEM_PAGE_MSK = 0x10
_BME680_SPI_RD_MSK = 0x80
_BME680_SPI_WR_MSK = 0x7f
_BME680_BIT_H1_DATA_MSK = 0x0F

# Bit position definitions for sensor settings
_BME680_GAS_MEAS_POS = 4
_BME680_FILTER_POS = 2
_BME680_OST_POS = 5
_BME680_OSP_POS = 2
_BME680_OSH_POS = 0
_BME680_RUN_GAS_POS = 4
_BME680_MODE_POS = 0
_BME680_NBCONV_POS = 0

# Look up tables for the possible gas range values
lookupTable1 = [2147483647, 2147483647, 2147483647, 2147483647,
        2147483647, 2126008810, 2147483647, 2130303777, 2147483647,
        2147483647, 2143188679, 2136746228, 2147483647, 2126008810,
        2147483647, 2147483647]

lookupTable2 = [4096000000, 2048000000, 1024000000, 512000000,
        255744255, 127110228, 64000000, 32258064,
        16016016, 8000000, 4000000, 2000000,
        1000000, 500000, 250000, 125000]

def bytes_to_word(msb, lsb, bits=16, signed=False):
    word = (msb << 8) | lsb
    if signed:
        word = twos_comp(word, bits)
    return word

def twos_comp(val, bits=16):
    if val & (1 << (bits - 1)) != 0:
        val = val - (1 << bits)
    return val

# Sensor field data structure

class FieldData:
    def __init__(self):
        # Contains new_data, gasm_valid & heat_stab
        self.status = None
        self.heat_stable = False
        # The index of the heater profile used
        self.gas_index = None
        # Measurement index to track order
        self.meas_index = None
        # Temperature in degree celsius x100
        self.temperature = None
        # Pressure in Pascal
        self.pressure = None
        # Humidity in % relative humidity x1000
        self.humidity = None
        # Gas resistance in Ohms
        self.gas_resistance = None
        
        self.altitudu = None

# Structure to hold the Calibration data

class CalibrationData:
    def __init__(self):
        self.par_h1 = None
        self.par_h2 = None
        self.par_h3 = None
        self.par_h4 = None
        self.par_h5 = None
        self.par_h6 = None
        self.par_h7 = None
        self.par_gh1 = None
        self.par_gh2 = None
        self.par_gh3 = None
        self.par_t1 = None
        self.par_t2 = None
        self.par_t3 = None
        self.par_p1 = None
        self.par_p2 = None
        self.par_p3 = None
        self.par_p4 = None
        self.par_p5 = None
        self.par_p6 = None
        self.par_p7 = None
        self.par_p8 = None
        self.par_p9 = None
        self.par_p10 = None
        # Variable to store t_fine size
        self.t_fine = None
        # Variable to store heater resistance range
        self.res_heat_range = None
        # Variable to store heater resistance value
        self.res_heat_val = None
        # Variable to store error range
        self.range_sw_err = None

    def set_from_array(self, calibration):
        # Temperature related coefficients
        self.par_t1 = bytes_to_word(calibration[_BME680_T1_MSB_REG], calibration[_BME680_T1_LSB_REG])
        self.par_t2 = bytes_to_word(calibration[_BME680_T2_MSB_REG], calibration[_BME680_T2_LSB_REG], bits=16, signed=True)
        self.par_t3 = twos_comp(calibration[_BME680_T3_REG], bits=8)

        # Pressure related coefficients
        self.par_p1 = bytes_to_word(calibration[_BME680_P1_MSB_REG], calibration[_BME680_P1_LSB_REG])
        self.par_p2 = bytes_to_word(calibration[_BME680_P2_MSB_REG], calibration[_BME680_P2_LSB_REG], bits=16, signed=True)
        self.par_p3 = twos_comp(calibration[_BME680_P3_REG], bits=8)
        self.par_p4 = bytes_to_word(calibration[_BME680_P4_MSB_REG], calibration[_BME680_P4_LSB_REG], bits=16, signed=True)
        self.par_p5 = bytes_to_word(calibration[_BME680_P5_MSB_REG], calibration[_BME680_P5_LSB_REG], bits=16, signed=True)
        self.par_p6 = twos_comp(calibration[_BME680_P6_REG], bits=8)
        self.par_p7 = twos_comp(calibration[_BME680_P7_REG], bits=8)
        self.par_p8 = bytes_to_word(calibration[_BME680_P8_MSB_REG], calibration[_BME680_P8_LSB_REG], bits=16, signed=True)
        self.par_p9 = bytes_to_word(calibration[_BME680_P9_MSB_REG], calibration[_BME680_P9_LSB_REG], bits=16, signed=True)
        self.par_p10 = calibration[_BME680_P10_REG]

        # Humidity related coefficients
        self.par_h1 = (calibration[_BME680_H1_MSB_REG] << _BME680_HUM_REG_SHIFT_VAL) | (calibration[_BME680_H1_LSB_REG] & _BME680_BIT_H1_DATA_MSK)
        self.par_h2 = (calibration[_BME680_H2_MSB_REG] << _BME680_HUM_REG_SHIFT_VAL) | (calibration[_BME680_H2_LSB_REG] >> _BME680_HUM_REG_SHIFT_VAL)
        self.par_h3 = twos_comp(calibration[_BME680_H3_REG], bits=8)
        self.par_h4 = twos_comp(calibration[_BME680_H4_REG], bits=8)
        self.par_h5 = twos_comp(calibration[_BME680_H5_REG], bits=8)
        self.par_h6 = calibration[_BME680_H6_REG]
        self.par_h7 = twos_comp(calibration[_BME680_H7_REG], bits=8)

        # Gas heater related coefficients
        self.par_gh1 = twos_comp(calibration[_BME680_GH1_REG], bits=8)
        self.par_gh2 = bytes_to_word(calibration[_BME680_GH2_MSB_REG], calibration[_BME680_GH2_LSB_REG], bits=16, signed=True)
        self.par_gh3 = twos_comp(calibration[_BME680_GH3_REG], bits=8)

    def set_other(self, heat_range, heat_value, sw_error):
        self.res_heat_range = (heat_range & _BME680_RHRANGE_MSK) // 16
        self.res_heat_val = heat_value
        self.range_sw_err = (sw_error & _BME680_RSERROR_MSK) // 16

# BME680 sensor settings structure which comprises of ODR,
# over-sampling and filter settings.

class TPHSettings:
    def __init__(self):
        # Humidity oversampling
        self.os_hum = None
        # Temperature oversampling
        self.os_temp = None
        # Pressure oversampling
        self.os_pres = None
        # Filter coefficient
        self.filter = None

# BME680 gas sensor which comprises of gas settings
## and status parameters

class GasSettings:
    def __init__(self):
        # Variable to store nb conversion
        self.nb_conv = None
        # Variable to store heater control
        self.heatr_ctrl = None
        # Run gas enable value
        self.run_gas = None
        # Pointer to store heater temperature
        self.heatr_temp = None
        # Pointer to store duration profile
        self.heatr_dur = None

# BME680 device structure

class BME680Data:
    def __init__(self):
        # Chip Id
        self.chip_id = None
        # Device Id
        self.dev_id = None
        # SPI/I2C interface
        self.intf = None
        # Memory page used
        self.mem_page = None
        # Ambient temperature in Degree C
        self.ambient_temperature = None
        # Field Data
        self.data = FieldData()
        # Sensor calibration data
        self.calibration_data = CalibrationData()
        # Sensor settings
        self.tph_settings = TPHSettings()
        # Gas Sensor settings
        self.gas_settings = GasSettings()
        # Sensor power modes
        self.power_mode = None
        # New sensor fields
        self.new_fields = None


class DFRobot_BME680(BME680Data):
    
    # Heater control settings
    ENABLE_HEATER = 0x00
    DISABLE_HEATER = 0x08
    
    # Gas measurement settings
    DISABLE_GAS_MEAS = 0x00
    ENABLE_GAS_MEAS = 0x01
    
    # Over-sampling settings
    OS_NONE = 0
    OS_1X = 1
    OS_2X = 2
    OS_4X = 3
    OS_8X = 4
    OS_16X = 5
    
    # IIR filter settings
    FILTER_SIZE_0 = 0
    FILTER_SIZE_1 = 1
    FILTER_SIZE_3 = 2
    FILTER_SIZE_7 = 3
    FILTER_SIZE_15 = 4
    FILTER_SIZE_31 = 5
    FILTER_SIZE_63 = 6
    FILTER_SIZE_127 = 7
    
    # Power mode settings
    SLEEP_MODE = 0
    FORCED_MODE = 1
    
    def __init__(self, board = None, i2c_addr = _BME680_I2C_ADDR_SECONDARY, bus_num=0):
        BME680Data.__init__(self)

        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.i2c_addr = i2c_addr
        self._i2c = I2C(bus_num)

        self.chip_id = self._get_regs(_BME680_CHIP_ID_ADDR, 1)
        if self.chip_id != _BME680_CHIP_ID:
            raise RuntimeError("BME680 Not Found. Invalid CHIP ID: 0x{0:02x}".format(self.chip_id))

        self.soft_reset()
        self.set_power_mode(self.SLEEP_MODE)

        self._get_calibration_data()

        self.set_humidity_oversample(self.OS_2X)
        self.set_pressure_oversample(self.OS_4X)
        self.set_temperature_oversample(self.OS_8X)
        self.set_filter(self.FILTER_SIZE_3)
        self.set_gas_status(self.ENABLE_GAS_MEAS)
        self.set_temp_offset(0)
        self.get_sensor_data()
        self.set_gas_heater_temperature(320) #value:target temperature in degrees celsius, between 200 ~ 400
        self.set_gas_heater_duration(150) #value:target duration in milliseconds, between 1 and 4032
        self.select_gas_heater_profile(0) #value:current gas sensor conversion profile: 0 to 9
        
        
        

    def _get_calibration_data(self):
        calibration = self._get_regs(_BME680_COEFF_ADDR1, _BME680_COEFF_ADDR1_LEN)
        calibration += self._get_regs(_BME680_COEFF_ADDR2, _BME680_COEFF_ADDR2_LEN)

        heat_range = self._get_regs(_BME680_ADDR_RES_HEAT_RANGE_ADDR, 1)
        heat_value = twos_comp(self._get_regs(_BME680_ADDR_RES_HEAT_VAL_ADDR, 1), bits=8)
        sw_error = twos_comp(self._get_regs(_BME680_ADDR_RANGE_SW_ERR_ADDR, 1), bits=8)

        self.calibration_data.set_from_array(calibration)
        self.calibration_data.set_other(heat_range, heat_value, sw_error)

    def soft_reset(self):
        self._set_regs(_BME680_SOFT_RESET_ADDR, _BME680_SOFT_RESET_CMD) 
        time.sleep(0.01)

    def set_temp_offset(self, value):
        if value == 0:
            self.offset_temp_in_t_fine = 0
        else:
            self.offset_temp_in_t_fine = int(math.copysign((((int(abs(value) * 100)) << 8) - 128) / 5, value))

    def set_humidity_oversample(self, value):
        self.tph_settings.os_hum = value
        self._set_bits(_BME680_CONF_OS_H_ADDR, _BME680_OSH_MSK, _BME680_OSH_POS, value)

    def get_humidity_oversample(self):
        return (self._get_regs(_BME680_CONF_OS_H_ADDR, 1) & _BME680_OSH_MSK) >> _BME680_OSH_POS

    def set_pressure_oversample(self, value):
        self.tph_settings.os_pres = value
        self._set_bits(_BME680_CONF_T_P_MODE_ADDR, _BME680_OSP_MSK, _BME680_OSP_POS, value)

    def get_pressure_oversample(self):
        return (self._get_regs(_BME680_CONF_T_P_MODE_ADDR, 1) & _BME680_OSP_MSK) >> _BME680_OSP_POS

    def set_temperature_oversample(self, value):
        self.tph_settings.os_temp = value
        self._set_bits(_BME680_CONF_T_P_MODE_ADDR, _BME680_OST_MSK, _BME680_OST_POS, value)

    def get_temperature_oversample(self):
        return (self._get_regs(_BME680_CONF_T_P_MODE_ADDR, 1) & _BME680_OST_MSK) >> _BME680_OST_POS

    def set_filter(self, value):
        self.tph_settings.filter = value
        self._set_bits(_BME680_CONF_ODR_FILT_ADDR, _BME680_FILTER_MSK, _BME680_FILTER_POS, value)

    def get_filter(self):
        return (self._get_regs(_BME680_CONF_ODR_FILT_ADDR, 1) & _BME680_FILTER_MSK) >> _BME680_FILTER_POS

    def select_gas_heater_profile(self, value):
        if value > _BME680_NBCONV_MAX or value < _BME680_NBCONV_MIN:
            raise ValueError("Profile '{}' should be between {} and {}".format(value, _BME680_NBCONV_MIN, _BME680_NBCONV_MAX))

        self.gas_settings.nb_conv = value
        self._set_bits(_BME680_CONF_ODR_RUN_GAS_NBC_ADDR, _BME680_NBCONV_MSK, _BME680_NBCONV_POS, value)

    def get_gas_heater_profile(self):
        return self._get_regs(_BME680_CONF_ODR_RUN_GAS_NBC_ADDR, 1) & _BME680_NBCONV_MSK

    def set_gas_status(self, value):
        self.gas_settings.run_gas = value
        self._set_bits(_BME680_CONF_ODR_RUN_GAS_NBC_ADDR, _BME680_RUN_GAS_MSK, _BME680_RUN_GAS_POS, value)

    def get_gas_status(self):
        return (self._get_regs(_BME680_CONF_ODR_RUN_GAS_NBC_ADDR, 1) & _BME680_RUN_GAS_MSK) >> _BME680_RUN_GAS_POS

    def set_gas_heater_profile(self, temperature, duration, nb_profile=0):
        self.set_gas_heater_temperature(temperature, nb_profile=nb_profile)
        self.set_gas_heater_duration(duration, nb_profile=nb_profile)

    def set_gas_heater_temperature(self, value, nb_profile=0):
        if nb_profile > _BME680_NBCONV_MAX or value < _BME680_NBCONV_MIN:
            raise ValueError("Profile '{}' should be between {} and {}".format(nb_profile, _BME680_NBCONV_MIN, _BME680_NBCONV_MAX))

        self.gas_settings.heatr_temp = value
        temp = int(self._calc_heater_resistance(self.gas_settings.heatr_temp))
        self._set_regs(_BME680_RES_HEAT0_ADDR + nb_profile, temp)

    def set_gas_heater_duration(self, value, nb_profile=0):
        if nb_profile > _BME680_NBCONV_MAX or value < _BME680_NBCONV_MIN:
            raise ValueError("Profile '{}' should be between {} and {}".format(nb_profile, _BME680_NBCONV_MIN, _BME680_NBCONV_MAX))

        self.gas_settings.heatr_dur = value
        temp = self._calc_heater_duration(self.gas_settings.heatr_dur)
        self._set_regs(_BME680_GAS_WAIT0_ADDR + nb_profile, temp)

    def set_power_mode(self, value):
        if value not in (self.SLEEP_MODE, self.FORCED_MODE):
            print("Power mode should be one of SLEEP_MODE or FORCED_MODE")

        self.power_mode = value

        self._set_bits(_BME680_CONF_T_P_MODE_ADDR, _BME680_MODE_MSK, _BME680_MODE_POS, value)


    def get_power_mode(self):
        self.power_mode = self._get_regs(_BME680_CONF_T_P_MODE_ADDR, 1)
        return self.power_mode

    def get_sensor_data(self):
        self.set_power_mode(self.FORCED_MODE)

        for attempt in range(10):
            status = self._get_regs(_BME680_FIELD0_ADDR, 1)

            if (status & _BME680_NEW_DATA_MSK) == 0:
                time.sleep(0.01)
                continue

            regs = self._get_regs(_BME680_FIELD0_ADDR, _BME680_FIELD_LENGTH)

            self.data.status = regs[0] & _BME680_NEW_DATA_MSK
            # Contains the nb_profile used to obtain the current measurement
            self.data.gas_index = regs[0] & _BME680_GAS_INDEX_MSK
            self.data.meas_index = regs[1]

            adc_pres = (regs[2] << 12) | (regs[3] << 4) | (regs[4] >> 4)
            adc_temp = (regs[5] << 12) | (regs[6] << 4) | (regs[7] >> 4)
            adc_hum = (regs[8] << 8) | regs[9]
            adc_gas_res = (regs[13] << 2) | (regs[14] >> 6)
            gas_range = regs[14] & _BME680_GAS_RANGE_MSK

            self.data.status |= regs[14] & _BME680_GASM_VALID_MSK
            self.data.status |= regs[14] & _BME680_HEAT_STAB_MSK

            self.data.heat_stable = (self.data.status & _BME680_HEAT_STAB_MSK) > 0

            temperature = self._calc_temperature(adc_temp)
            self.data.temperature = temperature / 100.0
            self.ambient_temperature = temperature # Saved for heater calc

            self.data.pressure = self._calc_pressure(adc_pres)
            self.data.humidity = self._calc_humidity(adc_hum) / 1000.0
            self.data.gas_resistance = round((self._calc_gas_resistance(adc_gas_res, gas_range)), 2)
            self.data.altitudu = self.cal_altitudu()
            return True

        return False
    
    def cal_altitudu(self):
        sea_level_pressure = self.get_sealevel()
        return round(((1.0 - pow(self.data.pressure / sea_level_pressure, 0.190284)) * 287.15 / 0.0065), 2)
        
    def get_sealevel(self):
        return (self.data.pressure / pow(1.0 - (525 / 44330), 5.255))
    
    def _set_bits(self, register, mask, position, value):
        temp = self._get_regs(register, 1)
        temp &= ~mask
        temp |= value << position
        self._set_regs(register, temp)

    def _set_regs(self, register, value):
        if isinstance(value, int):
            self._i2c.writeto_mem(self.i2c_addr, register, [value])
        else:
            self._i2c.writeto_mem(self.i2c_addr, register, value)

    def _get_regs(self, register, length):
        if length == 1:
            return self._i2c.readfrom_mem(self.i2c_addr, register, length)[0]
        else:
            return self._i2c.readfrom_mem(self.i2c_addr, register, length)
        
    def _calc_temperature(self, temperature_adc):
        var1 = (temperature_adc >> 3) - (self.calibration_data.par_t1 << 1)
        var2 = (var1 * self.calibration_data.par_t2) >> 11
        var3 = ((var1 >> 1) * (var1 >> 1)) >> 12
        var3 = ((var3) * (self.calibration_data.par_t3 << 4)) >> 14

        self.calibration_data.t_fine = (var2 + var3) + self.offset_temp_in_t_fine
        calc_temp = (((self.calibration_data.t_fine * 5) + 128) >> 8)

        return calc_temp

    def _calc_pressure(self, pressure_adc):
        var1 = ((self.calibration_data.t_fine) >> 1) - 64000
        var2 = ((((var1 >> 2) * (var1 >> 2)) >> 11) *
            self.calibration_data.par_p6) >> 2
        var2 = var2 + ((var1 * self.calibration_data.par_p5) << 1)
        var2 = (var2 >> 2) + (self.calibration_data.par_p4 << 16)
        var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13 ) *
                ((self.calibration_data.par_p3 << 5)) >> 3) +
                ((self.calibration_data.par_p2 * var1) >> 1))
        var1 = var1 >> 18

        var1 = ((32768 + var1) * self.calibration_data.par_p1) >> 15
        calc_pressure = 1048576 - pressure_adc
        calc_pressure = ((calc_pressure - (var2 >> 12)) * (3125))

        if calc_pressure >= (1 << 31):
            calc_pressure = ((calc_pressure // var1) << 1)
        else:
            calc_pressure = ((calc_pressure << 1) // var1)

        var1 = (self.calibration_data.par_p9 * (((calc_pressure >> 3) *
            (calc_pressure >> 3)) >> 13)) >> 12
        var2 = ((calc_pressure >> 2) *
            self.calibration_data.par_p8) >> 13
        var3 = ((calc_pressure >> 8) * (calc_pressure >> 8) *
            (calc_pressure >> 8) *
            self.calibration_data.par_p10) >> 17

        calc_pressure = (calc_pressure) + ((var1 + var2 + var3 +
            (self.calibration_data.par_p7 << 7)) >> 4)

        return calc_pressure

    def _calc_humidity(self, humidity_adc):
        temp_scaled = ((self.calibration_data.t_fine * 5) + 128) >> 8
        var1 = (humidity_adc - ((self.calibration_data.par_h1 * 16))) \
                - (((temp_scaled * self.calibration_data.par_h3) // (100)) >> 1)
        var2 = (self.calibration_data.par_h2
                * (((temp_scaled * self.calibration_data.par_h4) // (100))
                + (((temp_scaled * ((temp_scaled * self.calibration_data.par_h5) // (100))) >> 6)
                // (100)) + (1 * 16384))) >> 10
        var3 = var1 * var2
        var4 = self.calibration_data.par_h6 << 7
        var4 = ((var4) + ((temp_scaled * self.calibration_data.par_h7) // (100))) >> 4
        var5 = ((var3 >> 14) * (var3 >> 14)) >> 10
        var6 = (var4 * var5) >> 1
        calc_hum = (((var3 + var6) >> 10) * (1000)) >> 12

        return min(max(calc_hum,0),100000)

    def _calc_gas_resistance(self, gas_res_adc, gas_range):
        var1 = ((1340 + (5 * self.calibration_data.range_sw_err)) * (lookupTable1[gas_range])) >> 16
        var2 = (((gas_res_adc << 15) - (16777216)) + var1)
        var3 = ((lookupTable2[gas_range] * var1) >> 9)
        calc_gas_res = ((var3 + (var2 >> 1)) / var2)

        if calc_gas_res < 0:
            calc_gas_res = (1<<32) + calc_gas_res

        return calc_gas_res

    def _calc_heater_resistance(self, temperature):
        temperature = min(max(temperature,200),400)

        var1 = ((self.ambient_temperature * self.calibration_data.par_gh3) / 1000) * 256
        var2 = (self.calibration_data.par_gh1 + 784) * (((((self.calibration_data.par_gh2 + 154009) * temperature * 5) / 100) + 3276800) / 10)
        var3 = var1 + (var2 / 2)
        var4 = (var3 / (self.calibration_data.res_heat_range + 4))
        var5 = (131 * self.calibration_data.res_heat_val) + 65536
        heatr_res_x100 = (((var4 / var5) - 250) * 34)
        heatr_res = ((heatr_res_x100 + 50) / 100)

        return heatr_res

    def _calc_heater_duration(self, duration):
        if duration < 0xfc0:
            factor = 0

            while duration > 0x3f:
                duration /= 4
                factor += 1

            return int(duration + (factor * 64))

        return 0xff