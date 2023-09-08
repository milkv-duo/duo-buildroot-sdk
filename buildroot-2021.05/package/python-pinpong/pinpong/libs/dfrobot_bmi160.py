import time
from pinpong.board import gboard,I2C
import math

class BMI160():
    acc                                 = 0
    step                                = 1
    BMI160_I2C_INTF                     = 0

    BMI160_OK                           = 0
    BMI160_E_NULL_PTR                   = -1
    BMI160_E_COM_FAIL                   = -2
    BMI160_E_DEV_NOT_FOUND              = -3
    BMI160_E_OUT_OF_RANGE               = -4
    BMI160_E_INVALID_INPUT              = -5
    BMI160_E_ACCEL_ODR_BW_INVALID       = -6
    BMI160_E_GYRO_ODR_BW_INVALID        = -7
    BMI160_E_LWP_PRE_FLTR_INT_INVALID   = -8
    BMI160_E_LWP_PRE_FLTR_INVALID       = -9
    BMI160_E_AUX_NOT_FOUND              = -10
    BMI160_FOC_FAILURE                  = -11
    BMI160_ERR_CHOOSE                   = -12

    BMI160_CHIP_ID_ADDR                 = 0x00
    BMI160_CHIP_ID                      = 0xD1
    
    Bmi160BothAnySigMotionDisabled      = -1
    
    BMI160_ACCEL_ODR_RESERVED        = (0x00)
    BMI160_ACCEL_ODR_0_78HZ          = (0x01)
    BMI160_ACCEL_ODR_1_56HZ          = (0x02)
    BMI160_ACCEL_ODR_3_12HZ          = (0x03)
    BMI160_ACCEL_ODR_6_25HZ          = (0x04)
    BMI160_ACCEL_ODR_12_5HZ          = (0x05)
    BMI160_ACCEL_ODR_25HZ            = (0x06)
    BMI160_ACCEL_ODR_50HZ            = (0x07)
    BMI160_ACCEL_ODR_100HZ           = (0x08)
    BMI160_ACCEL_ODR_200HZ           = (0x09)
    BMI160_ACCEL_ODR_400HZ           = (0x0A)
    BMI160_ACCEL_ODR_800HZ           = (0x0B)
    BMI160_ACCEL_ODR_1600HZ          = (0x0C)
    BMI160_ACCEL_ODR_RESERVED0       = (0x0D)
    BMI160_ACCEL_ODR_RESERVED1       = (0x0E)
    BMI160_ACCEL_ODR_RESERVED2       = (0x0F)
    
    BMI160_ACCEL_RANGE_2G            = (0x03)
    BMI160_ACCEL_RANGE_4G            = (0x05)
    BMI160_ACCEL_RANGE_8G            = (0x08)
    BMI160_ACCEL_RANGE_16G           = (0x0C)
    
    BMI160_ACCEL_BW_OSR4_AVG1        = (0x00)
    BMI160_ACCEL_BW_OSR2_AVG2        = (0x01)
    BMI160_ACCEL_BW_NORMAL_AVG4      = (0x02)
    BMI160_ACCEL_BW_RES_AVG8         = (0x03)
    BMI160_ACCEL_BW_RES_AVG16        = (0x04)
    BMI160_ACCEL_BW_RES_AVG32        = (0x05)
    BMI160_ACCEL_BW_RES_AVG64        = (0x06)
    BMI160_ACCEL_BW_RES_AVG128       = (0x07)
    
    BMI160_ACCEL_NORMAL_MODE         = (0x11)
    BMI160_ACCEL_LOWPOWER_MODE       = (0x12)
    BMI160_ACCEL_SUSPEND_MODE        = (0x10)
    
    BMI160_GYRO_ODR_RESERVED         = (0x00)
    BMI160_GYRO_ODR_25HZ             = (0x06)
    BMI160_GYRO_ODR_50HZ             = (0x07)
    BMI160_GYRO_ODR_100HZ            = (0x08)
    BMI160_GYRO_ODR_200HZ            = (0x09)
    BMI160_GYRO_ODR_400HZ            = (0x0A)
    BMI160_GYRO_ODR_800HZ            = (0x0B)
    BMI160_GYRO_ODR_1600HZ           = (0x0C)
    BMI160_GYRO_ODR_3200HZ           = (0x0D)
    
    BMI160_GYRO_RANGE_2000_DPS       = (0x00)
    BMI160_GYRO_RANGE_1000_DPS       = (0x01)
    BMI160_GYRO_RANGE_500_DPS        = (0x02)
    BMI160_GYRO_RANGE_250_DPS        = (0x03)
    BMI160_GYRO_RANGE_125_DPS        = (0x04)
    
    BMI160_GYRO_BW_OSR4_MODE         = (0x00)
    BMI160_GYRO_BW_OSR2_MODE         = (0x01)
    BMI160_GYRO_BW_NORMAL_MODE       = (0x02)
    
    BMI160_GYRO_SUSPEND_MODE         = (0x14)
    BMI160_GYRO_NORMAL_MODE          = (0x15)
    BMI160_GYRO_FASTSTARTUP_MODE     = (0x17)
    
    BMI160_CHIP_ID_ADDR              = (0x00)
    BMI160_ERROR_REG_ADDR            = (0x02)
    BMI160_AUX_DATA_ADDR             = (0x04)
    BMI160_GYRO_DATA_ADDR            = (0x0C)
    BMI160_ACCEL_DATA_ADDR           = (0x12)
    BMI160_STATUS_ADDR               = (0x1B)
    BMI160_INT_STATUS_ADDR           = (0x1C)
    BMI160_FIFO_LENGTH_ADDR          = (0x22)
    BMI160_FIFO_DATA_ADDR            = (0x24)
    BMI160_ACCEL_CONFIG_ADDR         = (0x40)
    BMI160_ACCEL_RANGE_ADDR          = (0x41)
    BMI160_GYRO_CONFIG_ADDR          = (0x42)
    BMI160_GYRO_RANGE_ADDR           = (0x43)
    BMI160_AUX_ODR_ADDR              = (0x44)
    BMI160_FIFO_DOWN_ADDR            = (0x45)
    BMI160_FIFO_CONFIG_0_ADDR        = (0x46)
    BMI160_FIFO_CONFIG_1_ADDR        = (0x47)
    BMI160_AUX_IF_0_ADDR             = (0x4B)
    BMI160_AUX_IF_1_ADDR             = (0x4C)
    BMI160_AUX_IF_2_ADDR             = (0x4D)
    BMI160_AUX_IF_3_ADDR             = (0x4E)
    BMI160_AUX_IF_4_ADDR             = (0x4F)
    BMI160_INT_ENABLE_0_ADDR         = (0x50)
    BMI160_INT_ENABLE_1_ADDR         = (0x51)
    BMI160_INT_ENABLE_2_ADDR         = (0x52)
    BMI160_INT_OUT_CTRL_ADDR         = (0x53)
    BMI160_INT_LATCH_ADDR            = (0x54)
    BMI160_INT_MAP_0_ADDR            = (0x55)
    BMI160_INT_MAP_1_ADDR            = (0x56)
    BMI160_INT_MAP_2_ADDR            = (0x57)
    BMI160_INT_DATA_0_ADDR           = (0x58)
    BMI160_INT_DATA_1_ADDR           = (0x59)
    BMI160_INT_LOWHIGH_0_ADDR        = (0x5A)
    BMI160_INT_LOWHIGH_1_ADDR        = (0x5B)
    BMI160_INT_LOWHIGH_2_ADDR        = (0x5C)
    BMI160_INT_LOWHIGH_3_ADDR        = (0x5D)
    BMI160_INT_LOWHIGH_4_ADDR        = (0x5E)
    BMI160_INT_MOTION_0_ADDR         = (0x5F)
    BMI160_INT_MOTION_1_ADDR         = (0x60)
    BMI160_INT_MOTION_2_ADDR         = (0x61)
    BMI160_INT_MOTION_3_ADDR         = (0x62)
    BMI160_INT_TAP_0_ADDR            = (0x63)
    BMI160_INT_TAP_1_ADDR            = (0x64)
    BMI160_INT_ORIENT_0_ADDR         = (0x65)
    BMI160_INT_ORIENT_1_ADDR         = (0x66)
    BMI160_INT_FLAT_0_ADDR           = (0x67)
    BMI160_INT_FLAT_1_ADDR           = (0x68)
    BMI160_FOC_CONF_ADDR             = (0x69)
    BMI160_CONF_ADDR                 = (0x6A)
    
    BMI160_ACCEL_ODR_MAX             = (15)
    BMI160_ACCEL_BW_MAX              = (2)
    BMI160_ACCEL_RANGE_MAX           = (12)
    BMI160_GYRO_ODR_MAX              = (13)
    BMI160_GYRO_BW_MAX               = (2)
    BMI160_GYRO_RANGE_MAX            = (4)
    
    BMI160_SOFT_RESET_CMD            = (0xb6)
    BMI160_SOFT_RESET_DELAY_MS       = (0.015)
    
    BMI160_IF_CONF_ADDR              = (0x6B)
    BMI160_SELF_TEST_ADDR            = (0x6D)
    BMI160_OFFSET_ADDR               = (0x71)
    BMI160_OFFSET_CONF_ADDR          = (0x77)
    BMI160_INT_STEP_CNT_0_ADDR       = (0x78)
    BMI160_INT_STEP_CONFIG_0_ADDR    = (0x7A)
    BMI160_INT_STEP_CONFIG_1_ADDR    = (0x7B)
    BMI160_COMMAND_REG_ADDR          = (0x7E)
    BMI160_SPI_COMM_TEST_ADDR        = (0x7F)
    BMI160_INTL_PULLUP_CONF_ADDR     = (0x85)
    
    BMI160_ACCEL_BW_MASK                    = (0x70)
    BMI160_ACCEL_ODR_MASK                   = (0x0F)
    BMI160_ACCEL_UNDERSAMPLING_MASK         = (0x80)
    BMI160_ACCEL_RANGE_MASK                 = (0x0F)
    BMI160_GYRO_BW_MASK                     = (0x30)
    BMI160_GYRO_ODR_MASK                    = (0x0F)
    BMI160_GYRO_RANGE_MSK                   = (0x07)
    
    BMI160_ERR_REG_MASK    = (0x0F)
    
    BMI160_ACC_ANY_MOTION_INT                  = 0
    BMI160_ACC_SIG_MOTION_INT                  = 1
    BMI160_STEP_DETECT_INT                     = 2
    BMI160_ACC_DOUBLE_TAP_INT                  = 3
    BMI160_ACC_SINGLE_TAP_INT                  = 4
    BMI160_ACC_ORIENT_INT                      = 5
    BMI160_ACC_FLAT_INT                        = 6
    BMI160_ACC_HIGH_G_INT                      = 7
    BMI160_ACC_LOW_G_INT                       = 8
    BMI160_ACC_SLOW_NO_MOTION_INT              = 9
    BMI160_ACC_GYRO_DATA_RDY_INT               = 10
    BMI160_ACC_GYRO_FIFO_FULL_INT              = 11
    BMI160_ACC_GYRO_FIFO_WATERMARK_INT         = 12
    
    BMI160_ENABLE                              = (0x01)
    BMI160_DISABLE                             = (0x00)
    
    BMI160_LATCH_DUR_NONE                       = (0x00)
    BMI160_LATCH_DUR_312_5_MICRO_SEC            = (0x01)
    BMI160_LATCH_DUR_625_MICRO_SEC              = (0x02)
    BMI160_LATCH_DUR_1_25_MILLI_SEC             = (0x03)
    BMI160_LATCH_DUR_2_5_MILLI_SEC              = (0x04)
    BMI160_LATCH_DUR_5_MILLI_SEC                = (0x05)
    BMI160_LATCH_DUR_10_MILLI_SEC               = (0x06)
    BMI160_LATCH_DUR_20_MILLI_SEC               = (0x07)
    BMI160_LATCH_DUR_40_MILLI_SEC               = (0x08)
    BMI160_LATCH_DUR_80_MILLI_SEC               = (0x09)
    BMI160_LATCH_DUR_160_MILLI_SEC              = (0x0A)
    BMI160_LATCH_DUR_320_MILLI_SEC              = (0x0B)
    BMI160_LATCH_DUR_640_MILLI_SEC              = (0x0C)
    BMI160_LATCH_DUR_1_28_SEC                   = (0x0D)
    BMI160_LATCH_DUR_2_56_SEC                   = (0x0E)
    BMI160_LATCHED                              = (0x0F)
                                                
    BMI160_STEP_DETECT_NORMAL                   = 0
    BMI160_STEP_DETECT_SENSITIVE                = 1
    BMI160_STEP_DETECT_ROBUST                   = 2
    BMI160_STEP_DETECT_USER_DEFINE              = 3
    
    BMI160_ANY_MOTION_X_INT_EN_MASK         = (0x01)
    BMI160_HIGH_G_X_INT_EN_MASK             = (0x01)
    BMI160_NO_MOTION_X_INT_EN_MASK          = (0x01)
    BMI160_ANY_MOTION_Y_INT_EN_MASK         = (0x02)
    BMI160_HIGH_G_Y_INT_EN_MASK             = (0x02)
    BMI160_NO_MOTION_Y_INT_EN_MASK          = (0x02)
    BMI160_ANY_MOTION_Z_INT_EN_MASK         = (0x04)
    BMI160_HIGH_G_Z_INT_EN_MASK             = (0x04)
    BMI160_NO_MOTION_Z_INT_EN_MASK          = (0x04)
    BMI160_SIG_MOTION_INT_EN_MASK           = (0x07)
    BMI160_ANY_MOTION_ALL_INT_EN_MASK       = (0x07)
    BMI160_STEP_DETECT_INT_EN_MASK          = (0x08)
    BMI160_DOUBLE_TAP_INT_EN_MASK           = (0x10)
    BMI160_SINGLE_TAP_INT_EN_MASK           = (0x20)
    BMI160_FIFO_FULL_INT_EN_MASK            = (0x20)
    BMI160_ORIENT_INT_EN_MASK               = (0x40)
    BMI160_FIFO_WATERMARK_INT_EN_MASK       = (0x40)
    BMI160_LOW_G_INT_EN_MASK                = (0x08)
    BMI160_STEP_DETECT_EN_MASK              = (0x08)
    BMI160_FLAT_INT_EN_MASK                 = (0x80)
    BMI160_DATA_RDY_INT_EN_MASK             = (0x10)
    
    BMI160_INT1_EDGE_CTRL_MASK              = (0x01)
    BMI160_INT1_OUTPUT_MODE_MASK            = (0x04)
    BMI160_INT1_OUTPUT_TYPE_MASK            = (0x02)
    BMI160_INT1_OUTPUT_EN_MASK              = (0x08)
    BMI160_INT2_EDGE_CTRL_MASK              = (0x10)
    BMI160_INT2_OUTPUT_MODE_MASK            = (0x40)
    BMI160_INT2_OUTPUT_TYPE_MASK            = (0x20)
    BMI160_INT2_OUTPUT_EN_MASK              = (0x80)
    
    BMI160_INT1_INPUT_EN_MASK               = (0x10)
    BMI160_INT2_INPUT_EN_MASK               = (0x20)
    BMI160_INT_LATCH_MASK                   = (0x0F)

    BMI160_INT1_LOW_G_MASK                  = (0x01)
    BMI160_INT1_HIGH_G_MASK                 = (0x02)
    BMI160_INT1_SLOPE_MASK                  = (0x04)
    BMI160_INT1_NO_MOTION_MASK              = (0x08)
    BMI160_INT1_DOUBLE_TAP_MASK             = (0x10)
    BMI160_INT1_SINGLE_TAP_MASK             = (0x20)
    BMI160_INT1_FIFO_FULL_MASK              = (0x20)
    BMI160_INT1_FIFO_WM_MASK                = (0x40)
    BMI160_INT1_ORIENT_MASK                 = (0x40)
    BMI160_INT1_FLAT_MASK                   = (0x80)
    BMI160_INT1_DATA_READY_MASK             = (0x80)
    BMI160_INT2_LOW_G_MASK                  = (0x01)
    BMI160_INT1_LOW_STEP_DETECT_MASK        = (0x01)
    BMI160_INT2_LOW_STEP_DETECT_MASK        = (0x01)
    BMI160_INT2_HIGH_G_MASK                 = (0x02)
    BMI160_INT2_FIFO_FULL_MASK              = (0x02)
    BMI160_INT2_FIFO_WM_MASK                = (0x04)
    BMI160_INT2_SLOPE_MASK                  = (0x04)
    BMI160_INT2_DATA_READY_MASK             = (0x08)
    BMI160_INT2_NO_MOTION_MASK              = (0x08)
    BMI160_INT2_DOUBLE_TAP_MASK             = (0x10)
    BMI160_INT2_SINGLE_TAP_MASK             = (0x20)
    BMI160_INT2_ORIENT_MASK                 = (0x40)
    BMI160_INT2_FLAT_MASK                   = (0x80)
    
    BMI160_INT_CHANNEL_NONE                 = 0
    BMI160_INT_CHANNEL_1                    = 1
    BMI160_INT_CHANNEL_2                    = 2
    BMI160_INT_CHANNEL_BOTH                 = 3
    
    BMI160_STEP_COUNT_EN_BIT_MASK             = (0x08)
    BMI160_STEP_DETECT_MIN_THRES_MASK         = (0x18)
    BMI160_STEP_DETECT_STEPTIME_MIN_MASK      = (0x07)
    BMI160_STEP_MIN_BUF_MASK                  = (0x07)
    
    onlyAccel=1
    onlyGyro=2
    bothAccelGyro=3
    stepNormalPowerMode=0
    stepLowPowerMode=1
    
    BMI160_ACCEL_SEL                           = (0x01)
    BMI160_GYRO_SEL                            = (0x02)
    BMI160_TIME_SEL                            = (0x04)
    
    Bmi160AccelOnly                            = 1
    Bmi160GyroOnly                             = 2
    Bmi160BothAccelAndGyro                     = 3
    
    BMI160_SEN_SEL_MASK   = (0x07)
    
    int_mask_lookup_table = [
        BMI160_INT1_SLOPE_MASK,
        BMI160_INT1_SLOPE_MASK,
        BMI160_INT2_LOW_STEP_DETECT_MASK,
        BMI160_INT1_DOUBLE_TAP_MASK,
        BMI160_INT1_SINGLE_TAP_MASK,
        BMI160_INT1_ORIENT_MASK,
        BMI160_INT1_FLAT_MASK,
        BMI160_INT1_HIGH_G_MASK,
        BMI160_INT1_LOW_G_MASK,
        BMI160_INT1_NO_MOTION_MASK,
        BMI160_INT2_DATA_READY_MASK,
        BMI160_INT2_FIFO_FULL_MASK,
        BMI160_INT2_FIFO_WM_MASK]

    def __init__(self, board = None, i2c_addr = 0x69, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board == None:
            board = gboard
        self.i2c_addr = i2c_addr
        self._i2c = I2C(bus_num)
        
        self.prevAccelCfg_power = 0
        self.prevGyroCfg_power = 0

    def begin(self, types):
        self.i2c_init(self.i2c_addr)
        if types == self.step:
            self.setInt(2)
            self.setStepCounter()
            self.setStepPowerMode(self.stepNormalPowerMode)
    
    def get_gyr_x(self):
        accelGyro = [0,0,0,0,0,0]
        self.getAccelGyroData(accelGyro)
        k=accelGyro[0]*3.14/180.0
        return round(k,2)
    
    def get_gyr_y(self):
        accelGyro = [0,0,0,0,0,0]
        self.getAccelGyroData(accelGyro)
        k=accelGyro[1]*3.14/180
        return round(k,2)
    
    def get_gyr_z(self):
        accelGyro = [0,0,0,0,0,0]
        self.getAccelGyroData(accelGyro)
        k=accelGyro[2]*3.14/180
        return round(k,2)
        
    def get_acc_x(self):
        accelGyro = [0,0,0,0,0,0]
        self.getAccelGyroData(accelGyro)
        k=accelGyro[3]/16384
        return round(k,2)
    
    def get_acc_y(self):
        accelGyro = [0,0,0,0,0,0]
        self.getAccelGyroData(accelGyro)
        k=accelGyro[4]/16384
        return round(k,2)
        
    def get_acc_z(self):
        accelGyro = [0,0,0,0,0,0]
        self.getAccelGyroData(accelGyro)
        k=accelGyro[5]/16384
        return round(-k,2)
    
    def getAccelGyroData(self, accelGyro):
        Oaccel = [0,0,0]
        Ogyro = [0,0,0]
        self.getSensorData((self.BMI160_ACCEL_SEL) | (self.BMI160_GYRO_SEL), Oaccel, Ogyro)
        accelGyro[0] = Ogyro[0]
        accelGyro[1] = Ogyro[1]
        accelGyro[2] = Ogyro[2]
        accelGyro[3] = Oaccel[0]
        accelGyro[4] = Oaccel[1]
        accelGyro[5] = Oaccel[2]
        
        
    def getSensorData(self, select_sensor, Oaccel, Ogyro):
        sen_sel = select_sensor & self.BMI160_SEN_SEL_MASK
        time_sel = ((sen_sel & self.BMI160_TIME_SEL) >> 2)
        sen_sel = sen_sel & (self.BMI160_ACCEL_SEL | self.BMI160_GYRO_SEL)
        if time_sel == 1:
            lens = 3
        if sen_sel == self.Bmi160AccelOnly:pass
#            self.getAccelData(lens, Oaccel)
        elif sen_sel == self.Bmi160GyroOnly:pass
        elif sen_sel == self.Bmi160BothAccelAndGyro:
            self.getAccelGyroData1(Oaccel, Ogyro)


    
    def getAccelGyroData1(self, Oaccel, Ogyro):
        data_array = self.get_regs(self.BMI160_GYRO_DATA_ADDR, 12)
        lsb = data_array[0]
        msb = data_array[1]
        Ogyro[0] = self.bin2dec_auto(lsb, msb)
        lsb = data_array[2]
        msb = data_array[3]
        Ogyro[1] = self.bin2dec_auto(lsb, msb)
        lsb = data_array[4]
        msb = data_array[5]
        Ogyro[2] = self.bin2dec_auto(lsb, msb)
        lsb = data_array[6]
        msb = data_array[7]
        Oaccel[0] = self.bin2dec_auto(lsb, msb)
        lsb = data_array[8]
        msb = data_array[9]
        Oaccel[1] = self.bin2dec_auto(lsb, msb)
        lsb = data_array[10]
        msb = data_array[11]
        Oaccel[2] = self.bin2dec_auto(lsb, msb)
    
    def getAccelData(self):
        data = self.get_regs(self.BMI160_ACCEL_DATA_ADDR, )
        
    def get_step(self):
        stepCounter = 0
        stepCounter = self.readStepCounter()
        k = stepCounter
        return k
    
    def readStepCounter(self):
        data = self.get_regs(self.BMI160_INT_STEP_CNT_0_ADDR, 2)
        lsb = data[0]
        msb = data[1] << 8
        return (msb | lsb)
    
    def setStepPowerMode(self, model):
        if model == self.stepNormalPowerMode:
            self.accelCfg_odr = self.BMI160_ACCEL_ODR_1600HZ
            self.accelCfg_power = self.BMI160_ACCEL_NORMAL_MODE
            self.gyroCfg_odr = self.BMI160_GYRO_ODR_3200HZ
            self.gyroCfg_power = self.BMI160_GYRO_NORMAL_MODE
        elif model == self.stepLowPowerMode:
            self.accelCfg_odr = self.BMI160_ACCEL_ODR_50HZ
            self.accelCfg_power = self.BMI160_ACCEL_LOWPOWER_MODE
            self.gyroCfg_odr = self.BMI160_GYRO_ODR_50HZ
            self.gyroCfg_power = self.BMI160_GYRO_SUSPEND_MODE
        else:
            self.accelCfg_odr = self.BMI160_ACCEL_ODR_1600HZ
            self.accelCfg_power = self.BMI160_ACCEL_NORMAL_MODE
            self.gyroCfg_odr = self.BMI160_GYRO_ODR_3200HZ
            self.gyroCfg_power = self.BMI160_GYRO_NORMAL_MODE
        self.accelCfg_bw = self.BMI160_ACCEL_BW_NORMAL_AVG4
        self.accelCfg_range = self.BMI160_ACCEL_RANGE_2G
        self.gyroCfg_range = self.BMI160_GYRO_RANGE_2000_DPS
        self.gyroCfg_bw = self.BMI160_GYRO_BW_NORMAL_MODE
        self.setAccelConf()
        self.setGyroConf()
        self.setPowerMode()
        self.checkInvalidSettg()
    
    def setStepCounter(self):
        step_cnt_enable = 1
        data = self.get_regs(self.BMI160_INT_STEP_CONFIG_1_ADDR, 1)[0]
        if step_cnt_enable == self.BMI160_ENABLE:
            data |= (step_cnt_enable << 3)
        else:
            data &= ~self.BMI160_STEP_COUNT_EN_BIT_MASK
        self.set_Regs(self.BMI160_INT_STEP_CONFIG_1_ADDR, data)
    
    def setInt(self, val):
        return self.setInt2(val)
    
    def setInt2(self, val):
        if val == 1:
            self.intConfig_intChannel = self.BMI160_INT_CHANNEL_1
        elif val == 2:
            self.intConfig_intChannel = self.BMI160_INT_CHANNEL_2
        else:
            return self.BMI160_E_NULL_PTR
        self.intConfig_intType = self.BMI160_STEP_DETECT_INT
        self.intConfig_intPinSettg_outputEn = self.BMI160_ENABLE
        self.intConfig_intPinSettg_outputMode = self.BMI160_DISABLE
        self.intConfig_intPinSettg_outputType = self.BMI160_ENABLE
        self.intConfig_intPinSettg_edgeCtrl = self.BMI160_ENABLE
        self.intConfig_intPinSettg_inputEn = self.BMI160_DISABLE
        self.intConfig_intPinSettg_latchDur =self.BMI160_LATCH_DUR_NONE
        self.intConfig_intTypeCfg_accStepDetectInt_stepDetectorMode = self.BMI160_STEP_DETECT_NORMAL
        self.intConfig_intTypeCfg_accStepDetectInt_stepDetectorEn = self.BMI160_ENABLE
        self.setIntConfig()
    
    def setIntConfig(self):
        if self.intConfig_intType == self.BMI160_ACC_ANY_MOTION_INT:
            pass
        elif self.intConfig_intType == self.BMI160_ACC_SIG_MOTION_INT:
            pass
        elif self.intConfig_intType == self.BMI160_ACC_SLOW_NO_MOTION_INT:
            pass
        elif self.intConfig_intType == self.BMI160_ACC_DOUBLE_TAP_INT or self.intConfig_intType == self.BMI160_ACC_SINGLE_TAP_INT:
            pass
        elif self.intConfig_intType == self.BMI160_STEP_DETECT_INT:
            self.setAccelStepDetectInt()
        elif self.intConfig_intType == self.BMI160_ACC_ORIENT_INT:
            pass
        elif self.intConfig_intType == self.BMI160_ACC_FLAT_INT:
            pass
        elif self.intConfig_intType == self.BMI160_ACC_LOW_G_INT:
            pass
        elif self.intConfig_intType == self.BMI160_ACC_HIGH_G_INT:
            pass
        elif self.intConfig_intType == self.BMI160_ACC_GYRO_DATA_RDY_INT:
            pass
        elif self.intConfig_intType == self.BMI160_ACC_GYRO_FIFO_FULL_INT:
            pass
        elif self.intConfig_intType == self.BMI160_ACC_GYRO_FIFO_WATERMARK_INT:
            pass
    
    
    def setAccelStepDetectInt(self):
        self.enableStepDetectInt()
        self.setIntrPinConfig()
        self.mapFeatureInterrupt()
        self.configStepDetect()
    
    def configStepDetect(self):
        data_array = [0,0]
        if self.intConfig_intTypeCfg_accStepDetectInt_stepDetectorMode == self.BMI160_STEP_DETECT_NORMAL:
            data_array[0] = 0x15
            data_array[1] = 0x03
        elif self.intConfig_intTypeCfg_accStepDetectInt_stepDetectorMode == self.BMI160_STEP_DETECT_SENSITIVE:
            data_array[0] = 0x2D
            data_array[1] = 0x00
        elif self.intConfig_intTypeCfg_accStepDetectInt_stepDetectorMode == self.BMI160_STEP_DETECT_ROBUST:
            data_array[0] = 0x1D
            data_array[1] = 0x07
        elif self.intConfig_intTypeCfg_accStepDetectInt_stepDetectorMode == self.BMI160_STEP_DETECT_USER_DEFINE:
            data = self.get_regs(self.BMI160_INT_STEP_CONFIG_0_ADDR, 2)
            temp = data_array[0] & ~self.BMI160_STEP_DETECT_MIN_THRES_MASK
            data_array[0] = temp | ((self.intConfig_intTypeCfg_accStepDetectInt_minThreshold << 3) & self.BMI160_STEP_DETECT_MIN_THRES_MASK)
            
            temp = data_array[0] & ~self.BMI160_STEP_DETECT_STEPTIME_MIN_MASK
            data_array[0] = temp | ((self.intConfig_intTypeCfg_accStepDetectInt_steptimeMin) & self.BMI160_STEP_DETECT_MIN_THRES_MASK)
            
            temp = data_array[1] & ~self.BMI160_STEP_MIN_BUF_MASK
            data_array[1] = temp | ((intConfig_intTypeCfg_accStepDetectInt_stepMinBuf) & BMI160_STEP_MIN_BUF_MASK)
        
        self.set_Regs(self.BMI160_INT_STEP_CONFIG_0_ADDR, data_array)
            
        
    def mapFeatureInterrupt(self):
        temp = [0,0,0]
        data = self.get_regs(self.BMI160_INT_MAP_0_ADDR, 3)
        temp[0] = data[0] & ~self.int_mask_lookup_table[self.intConfig_intType]
        temp[2] = data[2] & ~self.int_mask_lookup_table[self.intConfig_intType]
        if self.intConfig_intChannel == self.BMI160_INT_CHANNEL_NONE:
            data[0] = temp[0]
            data[2] = temp[2]
        elif self.intConfig_intChannel == self.BMI160_INT_CHANNEL_1:
            data[0] = temp[0] | self.int_mask_lookup_table[self.intConfig_intType]
            data[2] = temp[2]
        elif self.intConfig_intChannel == self.BMI160_INT_CHANNEL_2:
            data[2] = temp[2] | self.int_mask_lookup_table[self.intConfig_intType]
            data[0] = temp[0]
        elif self.intConfig_intChannel == self.BMI160_INT_CHANNEL_BOTH:
            data[0] = temp[0] | self.int_mask_lookup_table[self.intConfig_intType]
            data[2] = temp[2] | self.int_mask_lookup_table[self.intConfig_intType]
        self.set_Regs(self.BMI160_INT_MAP_0_ADDR, data)
        
    def setIntrPinConfig(self):
        self.configIntOutCtrl()
        self.configIntLatch()
        
    def configIntLatch(self):
        data = self.get_regs(self.BMI160_INT_LATCH_ADDR, 1)[0]
        if self.intConfig_intChannel == self.BMI160_INT_CHANNEL_1:
            temp = data & ~self.BMI160_INT1_INPUT_EN_MASK
            data = temp | ((self.intConfig_intPinSettg_inputEn << 4) & self.BMI160_INT1_INPUT_EN_MASK)
        else:
            temp = data & ~self.BMI160_INT2_INPUT_EN_MASK
            data = temp | ((self.intConfig_intPinSettg_inputEn << 5) & self.BMI160_INT2_INPUT_EN_MASK)
        temp = data & ~self.BMI160_INT_LATCH_MASK
        data = temp | (self.intConfig_intPinSettg_latchDur & self.BMI160_INT_LATCH_MASK)
        self.set_Regs(self.BMI160_INT_LATCH_ADDR, data)
    
    def configIntOutCtrl(self):
        data = self.get_regs(self.BMI160_INT_OUT_CTRL_ADDR, 1)[0]
        if self.intConfig_intChannel == self.BMI160_INT_CHANNEL_1:
            temp = data & ~self.BMI160_INT1_OUTPUT_EN_MASK
            data = temp | (self.intConfig_intPinSettg_outputEn << 3) & self.BMI160_INT1_OUTPUT_EN_MASK
            
            temp = data & ~self.BMI160_INT1_OUTPUT_MODE_MASK
            data = temp | ((self.intConfig_intPinSettg_outputMode << 2) & self.BMI160_INT1_OUTPUT_MODE_MASK)
            
            temp = data & ~self.BMI160_INT1_OUTPUT_TYPE_MASK
            data = temp | ((self.intConfig_intPinSettg_outputType << 1) &self.BMI160_INT1_OUTPUT_TYPE_MASK)
            
            temp = data & ~self.BMI160_INT1_EDGE_CTRL_MASK
            data = temp | ((self.intConfig_intPinSettg_edgeCtrl) & self.BMI160_INT1_EDGE_CTRL_MASK)
        else:
            temp = data & ~self.BMI160_INT2_OUTPUT_EN_MASK
            data = temp | (self.intConfig_intPinSettg_outputEn << 7) & self.BMI160_INT2_OUTPUT_EN_MASK
            
            temp = data & ~self.BMI160_INT2_OUTPUT_MODE_MASK
            data = temp | ((self.intConfig_intPinSettg_outputMode << 6) & self.BMI160_INT2_OUTPUT_MODE_MASK)
            
            temp = data & ~self.BMI160_INT2_OUTPUT_TYPE_MASK
            data = temp | ((self.intConfig_intPinSettg_outputType << 5) &self.BMI160_INT2_OUTPUT_TYPE_MASK)
            
            temp = data & ~self.BMI160_INT2_EDGE_CTRL_MASK
            data = temp | ((self.intConfig_intPinSettg_edgeCtrl << 4) & self.BMI160_INT2_EDGE_CTRL_MASK)
        self.set_Regs(self.BMI160_INT_OUT_CTRL_ADDR, data)
    
    def enableStepDetectInt(self):
        data = self.get_regs(self.BMI160_INT_ENABLE_2_ADDR, 1)[0]
        temp = data & ~self.BMI160_STEP_DETECT_INT_EN_MASK
        data = temp | ((self.intConfig_intTypeCfg_accStepDetectInt_stepDetectorEn << 3) & self.BMI160_STEP_DETECT_INT_EN_MASK)
        self.set_Regs(self.BMI160_INT_ENABLE_2_ADDR, data)
    
    def i2c_init(self, i2c_addr):
        self.i2c_addr = i2c_addr
        self.interface = self.BMI160_I2C_INTF
        return self.i2c_init2()
    
    def i2c_init2(self):
        rslt = self.BMI160_OK
        chip_id = 0
        data = 0
        if rslt == self.BMI160_OK:
            chip_id = self.get_regs(self.BMI160_CHIP_ID_ADDR, 1)
            if chip_id[0] == self.BMI160_CHIP_ID:
                self.any_sig_sel = self.Bmi160BothAnySigMotionDisabled
                self.chipId = chip_id[0]
                self.softReset()
                rslt = self.setSensConf()
        return rslt
    
    def softReset(self):
        data = self.BMI160_SOFT_RESET_CMD
        self.set_Regs(self.BMI160_COMMAND_REG_ADDR, data)
        time.sleep(self.BMI160_SOFT_RESET_DELAY_MS)
        self.defaultParamSettg()
    
    def defaultParamSettg(self):
        self.accelCfg_odr = self.BMI160_ACCEL_ODR_100HZ
        self.accelCfg_range = self.BMI160_ACCEL_RANGE_2G
        self.accelCfg_bw = self.BMI160_ACCEL_BW_NORMAL_AVG4
        self.accelCfg_power = self.BMI160_ACCEL_SUSPEND_MODE
        self.gyroCfg_odr = self.BMI160_GYRO_ODR_100HZ
        self.gyroCfg_range = self.BMI160_GYRO_RANGE_2000_DPS
        self.gyroCfg_bw = self.BMI160_GYRO_BW_NORMAL_MODE
        self.gyroCfg_power = self.BMI160_GYRO_SUSPEND_MODE
        self.prevAccelCfg_odr = self.BMI160_ACCEL_ODR_100HZ
        self.prevAccelCfg_range = self.BMI160_ACCEL_RANGE_2G
        self.prevAccelCfg_bw = self.BMI160_ACCEL_BW_NORMAL_AVG4
        self.prevAccelCfg_power = self.BMI160_ACCEL_SUSPEND_MODE
        self.prevGyroCfg_odr = self.BMI160_GYRO_ODR_100HZ
        self.prevGyroCfg_range = self.BMI160_GYRO_RANGE_2000_DPS
        self.prevGyroCfg_bw = self.BMI160_GYRO_BW_NORMAL_MODE
        self.prevGyroCfg_power = self.BMI160_GYRO_SUSPEND_MODE
    
    def setSensConf(self):
        self.accelCfg_odr = self.BMI160_ACCEL_ODR_1600HZ
        self.accelCfg_range = self.BMI160_ACCEL_RANGE_2G
        self.accelCfg_bw = self.BMI160_ACCEL_BW_NORMAL_AVG4
        self.accelCfg_power = self.BMI160_ACCEL_NORMAL_MODE
        self.gyroCfg_odr = self.BMI160_GYRO_ODR_3200HZ
        self.gyroCfg_range = self.BMI160_GYRO_RANGE_2000_DPS
        self.gyroCfg_bw = self.BMI160_GYRO_BW_NORMAL_MODE
        self.gyroCfg_power = self.BMI160_GYRO_NORMAL_MODE
        self.setAccelConf()
        self.setGyroConf()
        self.setPowerMode()
        rslt = self.checkInvalidSettg()
       
    def checkInvalidSettg(self):
        data = self.get_regs(self.BMI160_ERROR_REG_ADDR, 1)[0]
        rslt = 0
        data = data >> 1
        data = data & self.BMI160_ERR_REG_MASK
        if data == 1:
            rslt = self.BMI160_E_ACCEL_ODR_BW_INVALID
        elif data == 2:
            rslt = self.BMI160_E_GYRO_ODR_BW_INVALID
        elif data == 3:
            rslt = self.BMI160_E_LWP_PRE_FLTR_INT_INVALID
        elif data == 7:
            rslt = self.BMI160_E_LWP_PRE_FLTR_INVALID
        return rslt
    
    def setPowerMode(self):
        self.setAccelPwr()
        self.setGyroPwr()
    
    def setGyroPwr(self):
        if self.gyroCfg_power == self.BMI160_GYRO_SUSPEND_MODE or self.gyroCfg_power == self.BMI160_GYRO_NORMAL_MODE or self.gyroCfg_power ==self.BMI160_GYRO_FASTSTARTUP_MODE:
            if self.gyroCfg_power != self.prevGyroCfg_power:
                self.set_Regs(self.BMI160_COMMAND_REG_ADDR, self.gyroCfg_power)
                if self.prevGyroCfg_power == self.BMI160_GYRO_SUSPEND_MODE:
                    time.sleep(0.081)
                elif self.prevGyroCfg_power == self.BMI160_GYRO_FASTSTARTUP_MODE and self.gyroCfg_power == self.BMI160_GYRO_NORMAL_MODE:
                    time.sleep(0.01)
                self.prevGyroCfg_power = self.gyroCfg_power
                    
    
    def setAccelPwr(self):
        data = 0
        if (self.accelCfg_power >= self.BMI160_ACCEL_SUSPEND_MODE) and self.accelCfg_power <= self.BMI160_ACCEL_LOWPOWER_MODE:
            if self.accelCfg_power != self.prevAccelCfg_power:
                data = self.processUnderSampling()
                self.set_Regs(self.BMI160_COMMAND_REG_ADDR, self.accelCfg_power)
                if self.prevAccelCfg_power == self.BMI160_ACCEL_SUSPEND_MODE:
                    time.sleep(0.005)
                self.prevAccelCfg_power = self.accelCfg_power
                
    def processUnderSampling(self):
        data = self.get_regs(self.BMI160_ACCEL_CONFIG_ADDR, 1)[0]
        if self.accelCfg_power == self.BMI160_ACCEL_LOWPOWER_MODE:
            temp = data & ~self.BMI160_ACCEL_UNDERSAMPLING_MASK
            data = temp | ((1 << 7) & self.BMI160_ACCEL_UNDERSAMPLING_MASK)
            self.set_Regs(self.BMI160_ACCEL_CONFIG_ADDR, data)
            pre_filter = self.set_Regs(self.BMI160_INT_DATA_0_ADDR, 2)
            return data
        else:
            if data & self.BMI160_ACCEL_UNDERSAMPLING_MASK:
                temp = data & ~self.BMI160_ACCEL_UNDERSAMPLING_MASK
                data = temp
                self.set_Regs(self.BMI160_ACCEL_CONFIG_ADDR, 1)
                return data
        return data
        
        
    def setGyroConf(self):
        data = self.checkGyroConfig()
        self.set_Regs(self.BMI160_GYRO_CONFIG_ADDR, data[0])
        self.prevGyroCfg_odr = self.gyroCfg_odr
        self.prevGyroCfg_bw = self.gyroCfg_bw
        time.sleep(0.001)
        self.set_Regs(self.BMI160_GYRO_RANGE_ADDR, data[1])
        self.prevGyroCfg_range = self.gyroCfg_range
        
    
    def processGyroRange(self, val):
        if self.gyroCfg_range <= self.BMI160_GYRO_RANGE_MAX:
            if self.gyroCfg_range != self.prevGyroCfg_range:
                ranges = self.gyroCfg_range
                temp = val & ~self.BMI160_GYRO_RANGE_MSK
                data = temp | (ranges & self.BMI160_GYRO_RANGE_MSK)
                return data
            else:
                return val
    
    def processGyroBw(self, val):
        if self.gyroCfg_bw <= self.BMI160_GYRO_BW_MAX:
            bw = self.gyroCfg_bw
            temp = val & ~self.BMI160_GYRO_BW_MASK
            data = temp | ((bw << 4) & self.BMI160_GYRO_BW_MASK)
            return data
    
    def processGyroOdr(self, val):
        if self.gyroCfg_odr <= self.BMI160_GYRO_ODR_MAX:
            if self.gyroCfg_odr != self.prevGyroCfg_odr:
                odr = self.gyroCfg_odr
                temp = (val & ~self.BMI160_GYRO_ODR_MASK)
                data = temp | (odr & self.BMI160_GYRO_ODR_MASK)
                return data
            else:
                return val
    
    def setAccelConf(self):
        data = self.checkAccelConfig()
        self.set_Regs(self.BMI160_ACCEL_CONFIG_ADDR, data[0])
        self.prevAccelCfg_odr = self.accelCfg_odr
        self.prevAccelCfg_bw = self.accelCfg_bw
        time.sleep(0.001)
        self.set_Regs(self.BMI160_ACCEL_RANGE_ADDR, data[1])
        self.prevAccelCfg_range = self.accelCfg_range
    
    def checkGyroConfig(self):
        data = self.get_regs(self.BMI160_GYRO_CONFIG_ADDR, 2)
        data[0] = self.processGyroOdr(data[0])
        data[0] = self.processGyroBw(data[0])
        data[1] = self.processGyroRange(data[1])
        return data
    
    def checkAccelConfig(self):
        data = self.get_regs(self.BMI160_ACCEL_CONFIG_ADDR, 2)
        data[0] = self.processAccelOdr(data[0])
        data[0] = self.processAccelBw(data[0])
        data[1] = self.processAccelRange(data[1])
        return data
    
    def processAccelRange(self, val):
        if self.accelCfg_range <= self.BMI160_ACCEL_RANGE_MAX:
            if self.accelCfg_range != self.prevAccelCfg_range:
                ranges = self.accelCfg_range
                temp = val & ~self.BMI160_ACCEL_RANGE_MASK
                data = temp | (ranges & self.BMI160_ACCEL_RANGE_MASK)
                return data
            else:
                return val
                
    def processAccelBw(self, val):
        if self.accelCfg_bw <= self.BMI160_ACCEL_BW_MAX:
            if self.accelCfg_bw != self.prevAccelCfg_bw:
                bw = self.accelCfg_bw
                temp = val & ~self.BMI160_ACCEL_BW_MASK
                data = temp | ((bw << 4) & self.BMI160_ACCEL_ODR_MASK)
                return data
            else:
                return val
    
    def processAccelOdr(self, val):
        if self.accelCfg_range < self.BMI160_ACCEL_RANGE_MAX:
            if self.accelCfg_odr != self.prevAccelCfg_odr:
                odr = self.accelCfg_odr
                temp = val & ~self.BMI160_ACCEL_ODR_MASK
                data = temp | (odr & self.BMI160_ACCEL_ODR_MASK)
                return data
            else:
                return val
    
    def set_Regs(self, reg, data):
        if not isinstance(data, list):
            data= [data]
        if self.prevAccelCfg_power == self.BMI160_ACCEL_NORMAL_MODE or self.prevGyroCfg_power == self.BMI160_GYRO_NORMAL_MODE:
            self._i2c.writeto_mem(self.i2c_addr, reg, data)
        else:
            self._i2c.writeto_mem(self.i2c_addr, reg, data)
    
    def get_regs(self, reg, lens):
        data = self._i2c.readfrom_mem(self.i2c_addr, reg, lens)
        return data

    def bin2dec(self, a):
        a_reverse = self.reverse(a)  # 取反
        a_add_1 = self.add_1(a_reverse)  # 二进制加1
        a_int = -int(a_add_1, 2)
        return a_int
 
    def bin2dec_auto(self, a, b):
        tmp = (b << 8) | a
        my_list = list(bin(tmp))
        c = len(my_list)
        if c < 18:
            for i in range(18-c):
                my_list.insert(2, '0')
        value = my_list[2:]
        if value[0] == '1':  # 如果首位是1，复数转换
            a_output = self.bin2dec(value)
        else:
            a_output = tmp
        return a_output
 
    def add_1(self, binary_inpute):  # 二进制编码加1
        _, out = bin(int(binary_inpute, 2) + 1).split("b")
        return out
 
    def reverse(self, binary_inpute):  # 取反操作
        binary_out = list(binary_inpute)
        for epoch, i in enumerate(binary_out):
            if i == "0":
                binary_out[epoch] = "1"
            else:
                binary_out[epoch] = "0"
        return "".join(binary_out)
