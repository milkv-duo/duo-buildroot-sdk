import time
from pinpong.board import gboard,I2C
import math

class MPU6050():
    X                           = 0
    Y                           = 1
    Z                           = 2
    MPU6050_ADDRESS_AD0_LOW     = 0x68
    MPU6050_ADDRESS_AD0_HIGH    = 0x69
    MPU6050_DEFAULT_ADDRESS     = 0x68
    MPU6050_RA_XG_OFFS_TC       = 0x00
    MPU6050_RA_YG_OFFS_TC       = 0x01
    MPU6050_RA_ZG_OFFS_TC       = 0x02
    MPU6050_RA_X_FINE_GAIN      = 0x03
    MPU6050_RA_Y_FINE_GAIN      = 0x04
    MPU6050_RA_Z_FINE_GAIN      = 0x05
    MPU6050_RA_XA_OFFS_H        = 0x06
    MPU6050_RA_XA_OFFS_L_TC     = 0x07
    MPU6050_RA_YA_OFFS_H        = 0x08
    MPU6050_RA_YA_OFFS_L_TC     = 0x09
    MPU6050_RA_ZA_OFFS_H        = 0x0A
    MPU6050_RA_ZA_OFFS_L_TC     = 0x0B
    MPU6050_RA_XG_OFFS_USRH     = 0x13
    MPU6050_RA_XG_OFFS_USRL     = 0x14
    MPU6050_RA_YG_OFFS_USRH     = 0x15
    MPU6050_RA_YG_OFFS_USRL     = 0x16
    MPU6050_RA_ZG_OFFS_USRH     = 0x17
    MPU6050_RA_ZG_OFFS_USRL     = 0x18
    MPU6050_RA_SMPLRT_DIV       = 0x19
    MPU6050_RA_CONFIG           = 0x1A
    MPU6050_RA_GYRO_CONFIG      = 0x1B
    MPU6050_RA_ACCEL_CONFIG     = 0x1C
    MPU6050_RA_FF_THR           = 0x1D
    MPU6050_RA_FF_DUR           = 0x1E
    MPU6050_RA_MOT_THR          = 0x1F
    MPU6050_RA_MOT_DUR          = 0x20
    MPU6050_RA_ZRMOT_THR        = 0x21
    MPU6050_RA_ZRMOT_DUR        = 0x22
    MPU6050_RA_FIFO_EN          = 0x23
    MPU6050_RA_I2C_MST_CTRL     = 0x24
    MPU6050_RA_I2C_SLV0_ADDR    = 0x25
    MPU6050_RA_I2C_SLV0_REG     = 0x26
    MPU6050_RA_I2C_SLV0_CTRL    = 0x27
    MPU6050_RA_I2C_SLV1_ADDR    = 0x28
    MPU6050_RA_I2C_SLV1_REG     = 0x29
    MPU6050_RA_I2C_SLV1_CTRL    = 0x2A
    MPU6050_RA_I2C_SLV2_ADDR    = 0x2B
    MPU6050_RA_I2C_SLV2_REG     = 0x2C
    MPU6050_RA_I2C_SLV2_CTRL    = 0x2D
    MPU6050_RA_I2C_SLV3_ADDR    = 0x2E
    MPU6050_RA_I2C_SLV3_REG     = 0x2F
    MPU6050_RA_I2C_SLV3_CTRL    = 0x30
    MPU6050_RA_I2C_SLV4_ADDR    = 0x31
    MPU6050_RA_I2C_SLV4_REG     = 0x32
    MPU6050_RA_I2C_SLV4_DO      = 0x33
    MPU6050_RA_I2C_SLV4_CTRL    = 0x34
    MPU6050_RA_I2C_SLV4_DI      = 0x35
    MPU6050_RA_I2C_MST_STATUS   = 0x36
    MPU6050_RA_INT_PIN_CFG      = 0x37
    MPU6050_RA_INT_ENABLE       = 0x38
    MPU6050_RA_DMP_INT_STATUS   = 0x39
    MPU6050_RA_INT_STATUS       = 0x3A
    MPU6050_RA_ACCEL_XOUT_H     = 0x3B
    MPU6050_RA_ACCEL_XOUT_L     = 0x3C
    MPU6050_RA_ACCEL_YOUT_H     = 0x3D
    MPU6050_RA_ACCEL_YOUT_L     = 0x3E
    MPU6050_RA_ACCEL_ZOUT_H     = 0x3F
    MPU6050_RA_ACCEL_ZOUT_L     = 0x40
    MPU6050_RA_TEMP_OUT_H       = 0x41
    MPU6050_RA_TEMP_OUT_L       = 0x42
    MPU6050_RA_GYRO_XOUT_H      = 0x43
    MPU6050_RA_GYRO_XOUT_L      = 0x44
    MPU6050_RA_GYRO_YOUT_H      = 0x45
    MPU6050_RA_GYRO_YOUT_L      = 0x46
    MPU6050_RA_GYRO_ZOUT_H      = 0x47
    MPU6050_RA_GYRO_ZOUT_L      = 0x48
    MPU6050_RA_EXT_SENS_DATA_00 = 0x49
    MPU6050_RA_EXT_SENS_DATA_01 = 0x4A
    MPU6050_RA_EXT_SENS_DATA_02 = 0x4B
    MPU6050_RA_EXT_SENS_DATA_03 = 0x4C
    MPU6050_RA_EXT_SENS_DATA_04 = 0x4D
    MPU6050_RA_EXT_SENS_DATA_05 = 0x4E
    MPU6050_RA_EXT_SENS_DATA_06 = 0x4F
    MPU6050_RA_EXT_SENS_DATA_07 = 0x50
    MPU6050_RA_EXT_SENS_DATA_08 = 0x51
    MPU6050_RA_EXT_SENS_DATA_09 = 0x52
    MPU6050_RA_EXT_SENS_DATA_10 = 0x53
    MPU6050_RA_EXT_SENS_DATA_11 = 0x54
    MPU6050_RA_EXT_SENS_DATA_12 = 0x55
    MPU6050_RA_EXT_SENS_DATA_13 = 0x56
    MPU6050_RA_EXT_SENS_DATA_14 = 0x57
    MPU6050_RA_EXT_SENS_DATA_15 = 0x58
    MPU6050_RA_EXT_SENS_DATA_16 = 0x59
    MPU6050_RA_EXT_SENS_DATA_17 = 0x5A
    MPU6050_RA_EXT_SENS_DATA_18 = 0x5B
    MPU6050_RA_EXT_SENS_DATA_19 = 0x5C
    MPU6050_RA_EXT_SENS_DATA_20 = 0x5D
    MPU6050_RA_EXT_SENS_DATA_21 = 0x5E
    MPU6050_RA_EXT_SENS_DATA_22 = 0x5F
    MPU6050_RA_EXT_SENS_DATA_23 = 0x60
    MPU6050_RA_MOT_DETECT_STATUS=     0x61
    MPU6050_RA_I2C_SLV0_DO      = 0x63
    MPU6050_RA_I2C_SLV1_DO      = 0x64
    MPU6050_RA_I2C_SLV2_DO      = 0x65
    MPU6050_RA_I2C_SLV3_DO      = 0x66
    MPU6050_RA_I2C_MST_DELAY_CTRL =    0x67
    MPU6050_RA_SIGNAL_PATH_RESET=     0x68
    MPU6050_RA_MOT_DETECT_CTRL  =     0x69
    MPU6050_RA_USER_CTRL        = 0x6A
    MPU6050_RA_PWR_MGMT_1       = 0x6B
    MPU6050_RA_PWR_MGMT_2       = 0x6C
    MPU6050_RA_BANK_SEL         = 0x6D
    MPU6050_RA_MEM_START_ADDR   = 0x6E
    MPU6050_RA_MEM_R_W          = 0x6F
    MPU6050_RA_DMP_CFG_1        = 0x70
    MPU6050_RA_DMP_CFG_2        = 0x71
    MPU6050_RA_FIFO_COUNTH      = 0x72
    MPU6050_RA_FIFO_COUNTL      = 0x73
    MPU6050_RA_FIFO_R_W         = 0x74
    MPU6050_RA_WHO_AM_I         = 0x75
    
    MPU6050_TC_PWR_MODE_BIT     = 7
    MPU6050_TC_OFFSET_BIT       = 6
    MPU6050_TC_OFFSET_LENGTH    = 6
    MPU6050_TC_OTP_BNK_VLD_BIT  = 0
    MPU6050_VDDIO_LEVEL_VLOGIC  = 0
    MPU6050_VDDIO_LEVEL_VDD     = 1
    
    MPU6050_CFG_EXT_SYNC_SET_BIT    =5
    MPU6050_CFG_EXT_SYNC_SET_LENGTH =3
    MPU6050_CFG_DLPF_CFG_BIT    =2
    MPU6050_CFG_DLPF_CFG_LENGTH =3
    
    MPU6050_EXT_SYNC_DISABLED       =0x0
    MPU6050_EXT_SYNC_TEMP_OUT_L     =0x1
    MPU6050_EXT_SYNC_GYRO_XOUT_L    =0x2
    MPU6050_EXT_SYNC_GYRO_YOUT_L    =0x3
    MPU6050_EXT_SYNC_GYRO_ZOUT_L    =0x4
    MPU6050_EXT_SYNC_ACCEL_XOUT_L   =0x5
    MPU6050_EXT_SYNC_ACCEL_YOUT_L   =0x6
    MPU6050_EXT_SYNC_ACCEL_ZOUT_L   =0x7
    
    MPU6050_DLPF_BW_256         =0x00
    MPU6050_DLPF_BW_188         =0x01
    MPU6050_DLPF_BW_98          =0x02
    MPU6050_DLPF_BW_42          =0x03
    MPU6050_DLPF_BW_20          =0x04
    MPU6050_DLPF_BW_10          =0x05
    MPU6050_DLPF_BW_5           =0x06
    
    MPU6050_GCONFIG_FS_SEL_BIT     = 4
    MPU6050_GCONFIG_FS_SEL_LENGTH  = 2
    
    MPU6050_GYRO_FS_250        = 0x00
    MPU6050_GYRO_FS_500        = 0x01
    MPU6050_GYRO_FS_1000       = 0x02
    MPU6050_GYRO_FS_2000       = 0x03
    
    MPU6050_ACONFIG_XA_ST_BIT          = 7
    MPU6050_ACONFIG_YA_ST_BIT          = 6
    MPU6050_ACONFIG_ZA_ST_BIT          = 5
    MPU6050_ACONFIG_AFS_SEL_BIT        = 4
    MPU6050_ACONFIG_AFS_SEL_LENGTH     = 2
    MPU6050_ACONFIG_ACCEL_HPF_BIT      = 2
    MPU6050_ACONFIG_ACCEL_HPF_LENGTH   = 3
    
    MPU6050_ACCEL_FS_2         = 0x00
    MPU6050_ACCEL_FS_4         = 0x01
    MPU6050_ACCEL_FS_8         = 0x02
    MPU6050_ACCEL_FS_16        = 0x03
    
    MPU6050_DHPF_RESET         = 0x00
    MPU6050_DHPF_5             = 0x01
    MPU6050_DHPF_2P5           = 0x02
    MPU6050_DHPF_1P25          = 0x03
    MPU6050_DHPF_0P63          = 0x04
    MPU6050_DHPF_HOLD          = 0x07
    
    MPU6050_TEMP_FIFO_EN_BIT   = 7
    MPU6050_XG_FIFO_EN_BIT     = 6
    MPU6050_YG_FIFO_EN_BIT     = 5
    MPU6050_ZG_FIFO_EN_BIT     = 4
    MPU6050_ACCEL_FIFO_EN_BIT  = 3
    MPU6050_SLV2_FIFO_EN_BIT   = 2
    MPU6050_SLV1_FIFO_EN_BIT   = 1
    MPU6050_SLV0_FIFO_EN_BIT   = 0
    
    MPU6050_MULT_MST_EN_BIT    = 7
    MPU6050_WAIT_FOR_ES_BIT    = 6
    MPU6050_SLV_3_FIFO_EN_BIT  = 5
    MPU6050_I2C_MST_P_NSR_BIT  = 4
    MPU6050_I2C_MST_CLK_BIT    = 3
    MPU6050_I2C_MST_CLK_LENGTH = 4
    
    MPU6050_CLOCK_DIV_348      = 0x0
    MPU6050_CLOCK_DIV_333      = 0x1
    MPU6050_CLOCK_DIV_320      = 0x2
    MPU6050_CLOCK_DIV_308      = 0x3
    MPU6050_CLOCK_DIV_296      = 0x4
    MPU6050_CLOCK_DIV_286      = 0x5
    MPU6050_CLOCK_DIV_276      = 0x6
    MPU6050_CLOCK_DIV_267      = 0x7
    MPU6050_CLOCK_DIV_258      = 0x8
    MPU6050_CLOCK_DIV_500      = 0x9
    MPU6050_CLOCK_DIV_471      = 0xA
    MPU6050_CLOCK_DIV_444      = 0xB
    MPU6050_CLOCK_DIV_421      = 0xC
    MPU6050_CLOCK_DIV_400      = 0xD
    MPU6050_CLOCK_DIV_381      = 0xE
    MPU6050_CLOCK_DIV_364      = 0xF
    
    MPU6050_I2C_SLV_RW_BIT     = 7
    MPU6050_I2C_SLV_ADDR_BIT   = 6
    MPU6050_I2C_SLV_ADDR_LENGTH= 7
    MPU6050_I2C_SLV_EN_BIT     = 7
    MPU6050_I2C_SLV_BYTE_SW_BIT= 6
    MPU6050_I2C_SLV_REG_DIS_BIT= 5
    MPU6050_I2C_SLV_GRP_BIT    = 4
    MPU6050_I2C_SLV_LEN_BIT    = 3
    MPU6050_I2C_SLV_LEN_LENGTH = 4
    
    MPU6050_I2C_SLV4_RW_BIT        = 7
    MPU6050_I2C_SLV4_ADDR_BIT      = 6
    MPU6050_I2C_SLV4_ADDR_LENGTH   = 7
    MPU6050_I2C_SLV4_EN_BIT        = 7
    MPU6050_I2C_SLV4_INT_EN_BIT    = 6
    MPU6050_I2C_SLV4_REG_DIS_BIT   = 5
    MPU6050_I2C_SLV4_MST_DLY_BIT   = 4
    MPU6050_I2C_SLV4_MST_DLY_LENGTH= 5
    
    MPU6050_MST_PASS_THROUGH_BIT   = 7
    MPU6050_MST_I2C_SLV4_DONE_BIT  = 6
    MPU6050_MST_I2C_LOST_ARB_BIT   = 5
    MPU6050_MST_I2C_SLV4_NACK_BIT  = 4
    MPU6050_MST_I2C_SLV3_NACK_BIT  = 3
    MPU6050_MST_I2C_SLV2_NACK_BIT  = 2
    MPU6050_MST_I2C_SLV1_NACK_BIT  = 1
    MPU6050_MST_I2C_SLV0_NACK_BIT  = 0
    
    MPU6050_INTCFG_INT_LEVEL_BIT       = 7
    MPU6050_INTCFG_INT_OPEN_BIT        = 6
    MPU6050_INTCFG_LATCH_INT_EN_BIT    = 5
    MPU6050_INTCFG_INT_RD_CLEAR_BIT    = 4
    MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT = 3
    MPU6050_INTCFG_FSYNC_INT_EN_BIT    = 2
    MPU6050_INTCFG_I2C_BYPASS_EN_BIT   = 1
    MPU6050_INTCFG_CLKOUT_EN_BIT       = 0
    
    MPU6050_INTMODE_ACTIVEHIGH = 0x00
    MPU6050_INTMODE_ACTIVELOW  = 0x01
    
    MPU6050_INTDRV_PUSHPULL    = 0x00
    MPU6050_INTDRV_OPENDRAIN   = 0x01
    
    MPU6050_INTLATCH_50USPULSE = 0x00
    MPU6050_INTLATCH_WAITCLEAR = 0x01
    
    MPU6050_INTCLEAR_STATUSREAD= 0x00
    MPU6050_INTCLEAR_ANYREAD   = 0x01
    
    MPU6050_INTERRUPT_FF_BIT           = 7
    MPU6050_INTERRUPT_MOT_BIT          = 6
    MPU6050_INTERRUPT_ZMOT_BIT         = 5
    MPU6050_INTERRUPT_FIFO_OFLOW_BIT   = 4
    MPU6050_INTERRUPT_I2C_MST_INT_BIT  = 3
    MPU6050_INTERRUPT_PLL_RDY_INT_BIT  = 2
    MPU6050_INTERRUPT_DMP_INT_BIT      = 1
    MPU6050_INTERRUPT_DATA_RDY_BIT     = 0
    
    MPU6050_DMPINT_5_BIT          =  5
    MPU6050_DMPINT_4_BIT          =  4
    MPU6050_DMPINT_3_BIT          =  3
    MPU6050_DMPINT_2_BIT          =  2
    MPU6050_DMPINT_1_BIT          =  1
    MPU6050_DMPINT_0_BIT          =  0
    
    MPU6050_MOTION_MOT_XNEG_BIT    = 7
    MPU6050_MOTION_MOT_XPOS_BIT    = 6
    MPU6050_MOTION_MOT_YNEG_BIT    = 5
    MPU6050_MOTION_MOT_YPOS_BIT    = 4
    MPU6050_MOTION_MOT_ZNEG_BIT    = 3
    MPU6050_MOTION_MOT_ZPOS_BIT    = 2
    MPU6050_MOTION_MOT_ZRMOT_BIT   = 0
    
    MPU6050_DELAYCTRL_DELAY_ES_SHADOW_BIT  = 7
    MPU6050_DELAYCTRL_I2C_SLV4_DLY_EN_BIT  = 4
    MPU6050_DELAYCTRL_I2C_SLV3_DLY_EN_BIT  = 3
    MPU6050_DELAYCTRL_I2C_SLV2_DLY_EN_BIT  = 2
    MPU6050_DELAYCTRL_I2C_SLV1_DLY_EN_BIT  = 1
    MPU6050_DELAYCTRL_I2C_SLV0_DLY_EN_BIT  = 0
    
    MPU6050_PATHRESET_GYRO_RESET_BIT   = 2
    MPU6050_PATHRESET_ACCEL_RESET_BIT  = 1
    MPU6050_PATHRESET_TEMP_RESET_BIT   = 0
    
    MPU6050_DETECT_ACCEL_ON_DELAY_BIT      = 5
    MPU6050_DETECT_ACCEL_ON_DELAY_LENGTH   = 2
    MPU6050_DETECT_FF_COUNT_BIT            = 3
    MPU6050_DETECT_FF_COUNT_LENGTH         = 2
    MPU6050_DETECT_MOT_COUNT_BIT           = 1
    MPU6050_DETECT_MOT_COUNT_LENGTH        = 2
    
    MPU6050_DETECT_DECREMENT_RESET = 0x0
    MPU6050_DETECT_DECREMENT_1     = 0x1
    MPU6050_DETECT_DECREMENT_2     = 0x2
    MPU6050_DETECT_DECREMENT_4     = 0x3
    
    MPU6050_USERCTRL_DMP_EN_BIT            = 7
    MPU6050_USERCTRL_FIFO_EN_BIT           = 6
    MPU6050_USERCTRL_I2C_MST_EN_BIT        = 5
    MPU6050_USERCTRL_I2C_IF_DIS_BIT        = 4
    MPU6050_USERCTRL_DMP_RESET_BIT         = 3
    MPU6050_USERCTRL_FIFO_RESET_BIT        = 2
    MPU6050_USERCTRL_I2C_MST_RESET_BIT     = 1
    MPU6050_USERCTRL_SIG_COND_RESET_BIT    = 0
    
    MPU6050_PWR1_DEVICE_RESET_BIT  = 7
    MPU6050_PWR1_SLEEP_BIT         = 6
    MPU6050_PWR1_CYCLE_BIT         = 5
    MPU6050_PWR1_TEMP_DIS_BIT      = 3
    MPU6050_PWR1_CLKSEL_BIT        = 2
    MPU6050_PWR1_CLKSEL_LENGTH     = 3
    
    MPU6050_CLOCK_INTERNAL        =  0x00
    MPU6050_CLOCK_PLL_XGYRO       =  0x01
    MPU6050_CLOCK_PLL_YGYRO       =  0x02
    MPU6050_CLOCK_PLL_ZGYRO       =  0x03
    MPU6050_CLOCK_PLL_EXT32K      =  0x04
    MPU6050_CLOCK_PLL_EXT19M      =  0x05
    MPU6050_CLOCK_KEEP_RESET      =  0x07
    
    MPU6050_PWR2_LP_WAKE_CTRL_BIT     =  7
    MPU6050_PWR2_LP_WAKE_CTRL_LENGTH  =  2
    MPU6050_PWR2_STBY_XA_BIT          =  5
    MPU6050_PWR2_STBY_YA_BIT          =  4
    MPU6050_PWR2_STBY_ZA_BIT          =  3
    MPU6050_PWR2_STBY_XG_BIT          =  2
    MPU6050_PWR2_STBY_YG_BIT          =  1
    MPU6050_PWR2_STBY_ZG_BIT          =  0
    
    MPU6050_WAKE_FREQ_1P25    =  0x0
    MPU6050_WAKE_FREQ_2P5     =  0x1
    MPU6050_WAKE_FREQ_5       =  0x2
    MPU6050_WAKE_FREQ_10      =  0x3
    
    MPU6050_BANKSEL_PRFTCH_EN_BIT     =  6
    MPU6050_BANKSEL_CFG_USER_BANK_BIT =  5
    MPU6050_BANKSEL_MEM_SEL_BIT       =  4
    MPU6050_BANKSEL_MEM_SEL_LENGTH    =  5
    
    MPU6050_WHO_AM_I_BIT       = 6
    MPU6050_WHO_AM_I_LENGTH    = 6
    
    MPU6050_DMP_MEMORY_BANKS       = 8
    MPU6050_DMP_MEMORY_BANK_SIZE   = 256
    MPU6050_DMP_MEMORY_CHUNK_SIZE  = 16
    
    dmpMemory = [
    0xFB, 0x00, 0x00, 0x3E, 0x00, 0x0B, 0x00, 0x36, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00,
    0x00, 0x65, 0x00, 0x54, 0xFF, 0xEF, 0x00, 0x00, 0xFA, 0x80, 0x00, 0x0B, 0x12, 0x82, 0x00, 0x01,
    0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x28, 0x00, 0x00, 0xFF, 0xFF, 0x45, 0x81, 0xFF, 0xFF, 0xFA, 0x72, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x03, 0xE8, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x7F, 0xFF, 0xFF, 0xFE, 0x80, 0x01,
    0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x3E, 0x03, 0x30, 0x40, 0x00, 0x00, 0x00, 0x02, 0xCA, 0xE3, 0x09, 0x3E, 0x80, 0x00, 0x00,
    0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00,
    0x41, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x2A, 0x00, 0x00, 0x16, 0x55, 0x00, 0x00, 0x21, 0x82,
    0xFD, 0x87, 0x26, 0x50, 0xFD, 0x80, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x05, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x6F, 0x00, 0x02, 0x65, 0x32, 0x00, 0x00, 0x5E, 0xC0,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFB, 0x8C, 0x6F, 0x5D, 0xFD, 0x5D, 0x08, 0xD9, 0x00, 0x7C, 0x73, 0x3B, 0x00, 0x6C, 0x12, 0xCC,
    0x32, 0x00, 0x13, 0x9D, 0x32, 0x00, 0xD0, 0xD6, 0x32, 0x00, 0x08, 0x00, 0x40, 0x00, 0x01, 0xF4,
    0xFF, 0xE6, 0x80, 0x79, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD0, 0xD6, 0x00, 0x00, 0x27, 0x10,
    0xFB, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFA, 0x36, 0xFF, 0xBC, 0x30, 0x8E, 0x00, 0x05, 0xFB, 0xF0, 0xFF, 0xD9, 0x5B, 0xC8,
    0xFF, 0xD0, 0x9A, 0xBE, 0x00, 0x00, 0x10, 0xA9, 0xFF, 0xF4, 0x1E, 0xB2, 0x00, 0xCE, 0xBB, 0xF7,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00, 0x02, 0x02, 0x00, 0x00, 0x0C,
    0xFF, 0xC2, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0xCF, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x14,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x3F, 0x68, 0xB6, 0x79, 0x35, 0x28, 0xBC, 0xC6, 0x7E, 0xD1, 0x6C,
    0x80, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB2, 0x6A, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xF0, 0x00, 0x00, 0x00, 0x30,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x25, 0x4D, 0x00, 0x2F, 0x70, 0x6D, 0x00, 0x00, 0x05, 0xAE, 0x00, 0x0C, 0x02, 0xD0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x00, 0x54, 0xFF, 0xEF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0xFF, 0xEF, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xD8, 0xDC, 0xBA, 0xA2, 0xF1, 0xDE, 0xB2, 0xB8, 0xB4, 0xA8, 0x81, 0x91, 0xF7, 0x4A, 0x90, 0x7F,
    0x91, 0x6A, 0xF3, 0xF9, 0xDB, 0xA8, 0xF9, 0xB0, 0xBA, 0xA0, 0x80, 0xF2, 0xCE, 0x81, 0xF3, 0xC2,
    0xF1, 0xC1, 0xF2, 0xC3, 0xF3, 0xCC, 0xA2, 0xB2, 0x80, 0xF1, 0xC6, 0xD8, 0x80, 0xBA, 0xA7, 0xDF,
    0xDF, 0xDF, 0xF2, 0xA7, 0xC3, 0xCB, 0xC5, 0xB6, 0xF0, 0x87, 0xA2, 0x94, 0x24, 0x48, 0x70, 0x3C,
    0x95, 0x40, 0x68, 0x34, 0x58, 0x9B, 0x78, 0xA2, 0xF1, 0x83, 0x92, 0x2D, 0x55, 0x7D, 0xD8, 0xB1,
    0xB4, 0xB8, 0xA1, 0xD0, 0x91, 0x80, 0xF2, 0x70, 0xF3, 0x70, 0xF2, 0x7C, 0x80, 0xA8, 0xF1, 0x01,
    0xB0, 0x98, 0x87, 0xD9, 0x43, 0xD8, 0x86, 0xC9, 0x88, 0xBA, 0xA1, 0xF2, 0x0E, 0xB8, 0x97, 0x80,
    0xF1, 0xA9, 0xDF, 0xDF, 0xDF, 0xAA, 0xDF, 0xDF, 0xDF, 0xF2, 0xAA, 0xC5, 0xCD, 0xC7, 0xA9, 0x0C,
    0xC9, 0x2C, 0x97, 0x97, 0x97, 0x97, 0xF1, 0xA9, 0x89, 0x26, 0x46, 0x66, 0xB0, 0xB4, 0xBA, 0x80,
    0xAC, 0xDE, 0xF2, 0xCA, 0xF1, 0xB2, 0x8C, 0x02, 0xA9, 0xB6, 0x98, 0x00, 0x89, 0x0E, 0x16, 0x1E,
    0xB8, 0xA9, 0xB4, 0x99, 0x2C, 0x54, 0x7C, 0xB0, 0x8A, 0xA8, 0x96, 0x36, 0x56, 0x76, 0xF1, 0xB9,
    0xAF, 0xB4, 0xB0, 0x83, 0xC0, 0xB8, 0xA8, 0x97, 0x11, 0xB1, 0x8F, 0x98, 0xB9, 0xAF, 0xF0, 0x24,
    0x08, 0x44, 0x10, 0x64, 0x18, 0xF1, 0xA3, 0x29, 0x55, 0x7D, 0xAF, 0x83, 0xB5, 0x93, 0xAF, 0xF0,
    0x00, 0x28, 0x50, 0xF1, 0xA3, 0x86, 0x9F, 0x61, 0xA6, 0xDA, 0xDE, 0xDF, 0xD9, 0xFA, 0xA3, 0x86,
    0x96, 0xDB, 0x31, 0xA6, 0xD9, 0xF8, 0xDF, 0xBA, 0xA6, 0x8F, 0xC2, 0xC5, 0xC7, 0xB2, 0x8C, 0xC1,
    0xB8, 0xA2, 0xDF, 0xDF, 0xDF, 0xA3, 0xDF, 0xDF, 0xDF, 0xD8, 0xD8, 0xF1, 0xB8, 0xA8, 0xB2, 0x86,
    0xB4, 0x98, 0x0D, 0x35, 0x5D, 0xB8, 0xAA, 0x98, 0xB0, 0x87, 0x2D, 0x35, 0x3D, 0xB2, 0xB6, 0xBA,
    0xAF, 0x8C, 0x96, 0x19, 0x8F, 0x9F, 0xA7, 0x0E, 0x16, 0x1E, 0xB4, 0x9A, 0xB8, 0xAA, 0x87, 0x2C,
    0x54, 0x7C, 0xB9, 0xA3, 0xDE, 0xDF, 0xDF, 0xA3, 0xB1, 0x80, 0xF2, 0xC4, 0xCD, 0xC9, 0xF1, 0xB8,
    0xA9, 0xB4, 0x99, 0x83, 0x0D, 0x35, 0x5D, 0x89, 0xB9, 0xA3, 0x2D, 0x55, 0x7D, 0xB5, 0x93, 0xA3,
    0x0E, 0x16, 0x1E, 0xA9, 0x2C, 0x54, 0x7C, 0xB8, 0xB4, 0xB0, 0xF1, 0x97, 0x83, 0xA8, 0x11, 0x84,
    0xA5, 0x09, 0x98, 0xA3, 0x83, 0xF0, 0xDA, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0xD8, 0xF1, 0xA5,
    0x29, 0x55, 0x7D, 0xA5, 0x85, 0x95, 0x02, 0x1A, 0x2E, 0x3A, 0x56, 0x5A, 0x40, 0x48, 0xF9, 0xF3,
    0xA3, 0xD9, 0xF8, 0xF0, 0x98, 0x83, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0x97, 0x82, 0xA8, 0xF1,
    0x11, 0xF0, 0x98, 0xA2, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0xDA, 0xF3, 0xDE, 0xD8, 0x83, 0xA5,
    0x94, 0x01, 0xD9, 0xA3, 0x02, 0xF1, 0xA2, 0xC3, 0xC5, 0xC7, 0xD8, 0xF1, 0x84, 0x92, 0xA2, 0x4D,
    0xDA, 0x2A, 0xD8, 0x48, 0x69, 0xD9, 0x2A, 0xD8, 0x68, 0x55, 0xDA, 0x32, 0xD8, 0x50, 0x71, 0xD9,
    0x32, 0xD8, 0x70, 0x5D, 0xDA, 0x3A, 0xD8, 0x58, 0x79, 0xD9, 0x3A, 0xD8, 0x78, 0x93, 0xA3, 0x4D,
    0xDA, 0x2A, 0xD8, 0x48, 0x69, 0xD9, 0x2A, 0xD8, 0x68, 0x55, 0xDA, 0x32, 0xD8, 0x50, 0x71, 0xD9,
    0x32, 0xD8, 0x70, 0x5D, 0xDA, 0x3A, 0xD8, 0x58, 0x79, 0xD9, 0x3A, 0xD8, 0x78, 0xA8, 0x8A, 0x9A,
    0xF0, 0x28, 0x50, 0x78, 0x9E, 0xF3, 0x88, 0x18, 0xF1, 0x9F, 0x1D, 0x98, 0xA8, 0xD9, 0x08, 0xD8,
    0xC8, 0x9F, 0x12, 0x9E, 0xF3, 0x15, 0xA8, 0xDA, 0x12, 0x10, 0xD8, 0xF1, 0xAF, 0xC8, 0x97, 0x87,
    0x34, 0xB5, 0xB9, 0x94, 0xA4, 0x21, 0xF3, 0xD9, 0x22, 0xD8, 0xF2, 0x2D, 0xF3, 0xD9, 0x2A, 0xD8,
    0xF2, 0x35, 0xF3, 0xD9, 0x32, 0xD8, 0x81, 0xA4, 0x60, 0x60, 0x61, 0xD9, 0x61, 0xD8, 0x6C, 0x68,
    0x69, 0xD9, 0x69, 0xD8, 0x74, 0x70, 0x71, 0xD9, 0x71, 0xD8, 0xB1, 0xA3, 0x84, 0x19, 0x3D, 0x5D,
    0xA3, 0x83, 0x1A, 0x3E, 0x5E, 0x93, 0x10, 0x30, 0x81, 0x10, 0x11, 0xB8, 0xB0, 0xAF, 0x8F, 0x94,
    0xF2, 0xDA, 0x3E, 0xD8, 0xB4, 0x9A, 0xA8, 0x87, 0x29, 0xDA, 0xF8, 0xD8, 0x87, 0x9A, 0x35, 0xDA,
    0xF8, 0xD8, 0x87, 0x9A, 0x3D, 0xDA, 0xF8, 0xD8, 0xB1, 0xB9, 0xA4, 0x98, 0x85, 0x02, 0x2E, 0x56,
    0xA5, 0x81, 0x00, 0x0C, 0x14, 0xA3, 0x97, 0xB0, 0x8A, 0xF1, 0x2D, 0xD9, 0x28, 0xD8, 0x4D, 0xD9,
    0x48, 0xD8, 0x6D, 0xD9, 0x68, 0xD8, 0xB1, 0x84, 0x0D, 0xDA, 0x0E, 0xD8, 0xA3, 0x29, 0x83, 0xDA,
    0x2C, 0x0E, 0xD8, 0xA3, 0x84, 0x49, 0x83, 0xDA, 0x2C, 0x4C, 0x0E, 0xD8, 0xB8, 0xB0, 0xA8, 0x8A,
    0x9A, 0xF5, 0x20, 0xAA, 0xDA, 0xDF, 0xD8, 0xA8, 0x40, 0xAA, 0xD0, 0xDA, 0xDE, 0xD8, 0xA8, 0x60,
    0xAA, 0xDA, 0xD0, 0xDF, 0xD8, 0xF1, 0x97, 0x86, 0xA8, 0x31, 0x9B, 0x06, 0x99, 0x07, 0xAB, 0x97,
    0x28, 0x88, 0x9B, 0xF0, 0x0C, 0x20, 0x14, 0x40, 0xB8, 0xB0, 0xB4, 0xA8, 0x8C, 0x9C, 0xF0, 0x04,
    0x28, 0x51, 0x79, 0x1D, 0x30, 0x14, 0x38, 0xB2, 0x82, 0xAB, 0xD0, 0x98, 0x2C, 0x50, 0x50, 0x78,
    0x78, 0x9B, 0xF1, 0x1A, 0xB0, 0xF0, 0x8A, 0x9C, 0xA8, 0x29, 0x51, 0x79, 0x8B, 0x29, 0x51, 0x79,
    0x8A, 0x24, 0x70, 0x59, 0x8B, 0x20, 0x58, 0x71, 0x8A, 0x44, 0x69, 0x38, 0x8B, 0x39, 0x40, 0x68,
    0x8A, 0x64, 0x48, 0x31, 0x8B, 0x30, 0x49, 0x60, 0xA5, 0x88, 0x20, 0x09, 0x71, 0x58, 0x44, 0x68,
    0x11, 0x39, 0x64, 0x49, 0x30, 0x19, 0xF1, 0xAC, 0x00, 0x2C, 0x54, 0x7C, 0xF0, 0x8C, 0xA8, 0x04,
    0x28, 0x50, 0x78, 0xF1, 0x88, 0x97, 0x26, 0xA8, 0x59, 0x98, 0xAC, 0x8C, 0x02, 0x26, 0x46, 0x66,
    0xF0, 0x89, 0x9C, 0xA8, 0x29, 0x51, 0x79, 0x24, 0x70, 0x59, 0x44, 0x69, 0x38, 0x64, 0x48, 0x31,
    0xA9, 0x88, 0x09, 0x20, 0x59, 0x70, 0xAB, 0x11, 0x38, 0x40, 0x69, 0xA8, 0x19, 0x31, 0x48, 0x60,
    0x8C, 0xA8, 0x3C, 0x41, 0x5C, 0x20, 0x7C, 0x00, 0xF1, 0x87, 0x98, 0x19, 0x86, 0xA8, 0x6E, 0x76,
    0x7E, 0xA9, 0x99, 0x88, 0x2D, 0x55, 0x7D, 0x9E, 0xB9, 0xA3, 0x8A, 0x22, 0x8A, 0x6E, 0x8A, 0x56,
    0x8A, 0x5E, 0x9F, 0xB1, 0x83, 0x06, 0x26, 0x46, 0x66, 0x0E, 0x2E, 0x4E, 0x6E, 0x9D, 0xB8, 0xAD,
    0x00, 0x2C, 0x54, 0x7C, 0xF2, 0xB1, 0x8C, 0xB4, 0x99, 0xB9, 0xA3, 0x2D, 0x55, 0x7D, 0x81, 0x91,
    0xAC, 0x38, 0xAD, 0x3A, 0xB5, 0x83, 0x91, 0xAC, 0x2D, 0xD9, 0x28, 0xD8, 0x4D, 0xD9, 0x48, 0xD8,
    0x6D, 0xD9, 0x68, 0xD8, 0x8C, 0x9D, 0xAE, 0x29, 0xD9, 0x04, 0xAE, 0xD8, 0x51, 0xD9, 0x04, 0xAE,
    0xD8, 0x79, 0xD9, 0x04, 0xD8, 0x81, 0xF3, 0x9D, 0xAD, 0x00, 0x8D, 0xAE, 0x19, 0x81, 0xAD, 0xD9,
    0x01, 0xD8, 0xF2, 0xAE, 0xDA, 0x26, 0xD8, 0x8E, 0x91, 0x29, 0x83, 0xA7, 0xD9, 0xAD, 0xAD, 0xAD,
    0xAD, 0xF3, 0x2A, 0xD8, 0xD8, 0xF1, 0xB0, 0xAC, 0x89, 0x91, 0x3E, 0x5E, 0x76, 0xF3, 0xAC, 0x2E,
    0x2E, 0xF1, 0xB1, 0x8C, 0x5A, 0x9C, 0xAC, 0x2C, 0x28, 0x28, 0x28, 0x9C, 0xAC, 0x30, 0x18, 0xA8,
    0x98, 0x81, 0x28, 0x34, 0x3C, 0x97, 0x24, 0xA7, 0x28, 0x34, 0x3C, 0x9C, 0x24, 0xF2, 0xB0, 0x89,
    0xAC, 0x91, 0x2C, 0x4C, 0x6C, 0x8A, 0x9B, 0x2D, 0xD9, 0xD8, 0xD8, 0x51, 0xD9, 0xD8, 0xD8, 0x79,
    0xD9, 0xD8, 0xD8, 0xF1, 0x9E, 0x88, 0xA3, 0x31, 0xDA, 0xD8, 0xD8, 0x91, 0x2D, 0xD9, 0x28, 0xD8,
    0x4D, 0xD9, 0x48, 0xD8, 0x6D, 0xD9, 0x68, 0xD8, 0xB1, 0x83, 0x93, 0x35, 0x3D, 0x80, 0x25, 0xDA,
    0xD8, 0xD8, 0x85, 0x69, 0xDA, 0xD8, 0xD8, 0xB4, 0x93, 0x81, 0xA3, 0x28, 0x34, 0x3C, 0xF3, 0xAB,
    0x8B, 0xF8, 0xA3, 0x91, 0xB6, 0x09, 0xB4, 0xD9, 0xAB, 0xDE, 0xFA, 0xB0, 0x87, 0x9C, 0xB9, 0xA3,
    0xDD, 0xF1, 0xA3, 0xA3, 0xA3, 0xA3, 0x95, 0xF1, 0xA3, 0xA3, 0xA3, 0x9D, 0xF1, 0xA3, 0xA3, 0xA3,
    0xA3, 0xF2, 0xA3, 0xB4, 0x90, 0x80, 0xF2, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
    0xA3, 0xB2, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xB0, 0x87, 0xB5, 0x99, 0xF1, 0xA3, 0xA3, 0xA3,
    0x98, 0xF1, 0xA3, 0xA3, 0xA3, 0xA3, 0x97, 0xA3, 0xA3, 0xA3, 0xA3, 0xF3, 0x9B, 0xA3, 0xA3, 0xDC,
    0xB9, 0xA7, 0xF1, 0x26, 0x26, 0x26, 0xD8, 0xD8, 0xFF]
    
    dmpConfig = [
    0x03,   0x7B,   0x03,   0x4C, 0xCD, 0x6C,
    0x03,   0xAB,   0x03,   0x36, 0x56, 0x76,
    0x00,   0x68,   0x04,   0x02, 0xCB, 0x47, 0xA2,
    0x02,   0x18,   0x04,   0x00, 0x05, 0x8B, 0xC1,   
    0x01,   0x0C,   0x04,   0x00, 0x00, 0x00, 0x00,   
    0x03,   0x7F,   0x06,   0x0C, 0xC9, 0x2C, 0x97, 0x97, 0x97, 
    0x03,   0x89,   0x03,   0x26, 0x46, 0x66,         
    0x00,   0x6C,   0x02,   0x20, 0x00,               
    0x02,   0x40,   0x04,   0x00, 0x00, 0x00, 0x00,   
    0x02,   0x44,   0x04,   0x00, 0x00, 0x00, 0x00,   
    0x02,   0x48,   0x04,   0x00, 0x00, 0x00, 0x00,   
    0x02,   0x4C,   0x04,   0x00, 0x00, 0x00, 0x00,   
    0x02,   0x50,   0x04,   0x00, 0x00, 0x00, 0x00,   
    0x02,   0x54,   0x04,   0x00, 0x00, 0x00, 0x00,   
    0x02,   0x58,   0x04,   0x00, 0x00, 0x00, 0x00,   
    0x02,   0x5C,   0x04,   0x00, 0x00, 0x00, 0x00,   
    0x02,   0xBC,   0x04,   0x00, 0x00, 0x00, 0x00,   
    0x01,   0xEC,   0x04,   0x00, 0x00, 0x40, 0x00,   
    0x03,   0x7F,   0x06,   0x0C, 0xC9, 0x2C, 0x97, 0x97, 0x97, 
    0x04,   0x02,   0x03,   0x0D, 0x35, 0x5D,         
    0x04,   0x09,   0x04,   0x87, 0x2D, 0x35, 0x3D,   
    0x00,   0xA3,   0x01,   0x00,                     
    0x00,   0x00,   0x00,   0x01,
    0x07,   0x86,   0x01,   0xFE,                        
    0x07,   0x41,   0x05,   0xF1, 0x20, 0x28, 0x30, 0x38,
    0x07,   0x7E,   0x01,   0x30,                        
    0x07,   0x46,   0x01,   0x9A,                        
    0x07,   0x47,   0x04,   0xF1, 0x28, 0x30, 0x38,      
    0x07,   0x6C,   0x04,   0xF1, 0x28, 0x30, 0x38,      
    0x02,   0x16,   0x02,   0x00, 0x01]
    
    dmpUpdates = [
    0x01,   0xB2,   0x02,   0xFF, 0xFF,
    0x01,   0x90,   0x04,   0x09, 0x23, 0xA1, 0x35,
    0x01,   0x6A,   0x02,   0x06, 0x00,
    0x01,   0x60,   0x08,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00,   0x60,   0x04,   0x40, 0x00, 0x00, 0x00,
    0x01,   0x62,   0x02,   0x00, 0x00,
    0x00,   0x60,   0x04,   0x00, 0x40, 0x00, 0x00]
    
    def __init__(self, board = None, i2c_addr = 0x68, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.i2c_addr = i2c_addr
        self._i2c = I2C(bus_num)
        self.w = 0
        self.x = 0
        self.y = 0
        self.z = 0
        self.init()
        
        if self.connection():
            print("MPU6050 connection success")
        else: 
            print("MPU6050 connection failed")
        #devStatus = self.dmp_initialize()
        #if devStatus == 0:
        #  self.setDMPEnabled(1)
          
    
    def dmp_initialize(self):
        print("Resetting MPU6050...")
        self.reset()
        time.sleep(0.03)
        print("Disabling sleep mode...")
        self.setSleepEnabled(0)
        print("Selecting user bank 16...")
        self.setMemoryBank(0x10, True, True)
        
        print("Selecting memory byte 6...")
        self.setMemoryStartAddress(0x06)
        print("Checking hardware revision...")
        hwRevision = self.readMemoryByte()
        print(hex(hwRevision[0]))
        print("Revision @ user[16][6] = ")
        self.setMemoryBank(0, False, False)
        
        print("Reading OTP bank valid flag...")
        otpValid = self.getOTPBankValid()
        print("OTP bank is ")
        print(otpValid)
        print("valid" if otpValid else "invalid!")
        
        xgOffset = self.getXGyroOffset()
        ygOffset = self.getYGyroOffset()
        zgOffset = self.getZGyroOffset()
        print("X gyro offset = {}".format(xgOffset))
        print("Y gyro offset = {}".format(ygOffset))
        print("Z gyro offset = {}".format(zgOffset))
        print("Setting slave 0 address to 0x7F...")
        self.setSlaveAddress(0, 0x7F)
        print("Disabling I2C Master mode...")
        self.setI2CMasterModeEnabled(0)
        print("Setting slave 0 address to 0x68 (self)...")
        self.setSlaveAddress(0, 0x68)
        print("Resetting I2C Master control...")
        self.resetI2CMaster()
        time.sleep(0.02)
        
        print("Writing DMP code to MPU memory banks (1929) bytes)")
        print("Success! DMP code written and verified.")
        if self.writeProgMemoryBlock(self.dmpMemory, 1929):
          print("Writing DMP configuration to MPU memory banks (192 bytes in config def)")
          if self.writeProgDMPConfigurationSet(self.dmpConfig, 192):
            print("Success! DMP configuration written and verified.")
            print("Setting clock source to Z Gyro...")
            self.setClockSource(self.MPU6050_CLOCK_PLL_ZGYRO)
            print("Setting DMP and FIFO_OFLOW interrupts enabled...")
            self.setIntEnabled(0x12)
            self.setRate(4)
            self.setExternalFrameSync(self.MPU6050_EXT_SYNC_TEMP_OUT_L)
            self.setDLPFMode(self.MPU6050_DLPF_BW_42)
            self.set_full_scale_GyroRange(self.MPU6050_GYRO_FS_2000)
            self.setDMPConfig1(0x03)
            self.setDMPConfig2(0x00)
            print("Clearing OTP Bank flag...")
            self.setOTPBankValid(0)
            print("Setting X/Y/Z gyro offsets to previous values...")
            self.setXGyroOffset(xgOffset)
            self.setYGyroOffset(ygOffset)
            self.setZGyroOffset(zgOffset)
            print("Setting X/Y/Z gyro user offsets to zero...")
            self.setXGyroOffsetUser(0)
            self.setYGyroOffsetUser(0)
            self.setZGyroOffsetUser(0)
            #print("Writing final memory update 1/7 (function unknown)...")
            dmpUpdate = [0 for i in range(16)]
            j = 0
            pos = 0
            while j < 4 or j < dmpUpdate[2] + 3:
              dmpUpdate[j] = self.dmpUpdates[pos]
              j += 1
              pos += 1
            self.writeMemoryBlock(dmpUpdate[3:], dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], True ,True)
            print("update 1/7")
            print(dmpUpdate[3:])
            #print(dmpUpdate[2])
            #print(dmpUpdate[0])
            #print(dmpUpdate[1])
            #print("Writing final memory update 2/7 (function unknown)...")
            j = 0
            while j < 4 or j < dmpUpdate[2] + 3:
              dmpUpdate[j] = self.dmpUpdates[pos]
              j += 1
              pos += 1
            self.writeMemoryBlock(dmpUpdate[3:], dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], True ,True)
            print("update 2/7")
            print(dmpUpdate[3:])
            #print(dmpUpdate[2])
            #print(dmpUpdate[0])
            #print(dmpUpdate[1])
            
            print("Resetting FIFO...")
            self.resetFIFO()
            print("Reading FIFO count...")
            fifoCount = self.getFIFOCount()
            print(fifoCount)
            fifoBuffer = [0 for i in range(128)]
            fifoBuffer = self.getFIFOBytes(fifoCount)
            print("Setting motion detection threshold to 2...")
            self.setMotionDetectionThreshold(2)
            print("Setting zero-motion detection threshold to 156...")
            self.setZeroMotionDetectionThreshold(156)
            print("Setting motion detection duration to 80...")
            self.setMotionDetectionDuration(80)
            print("Setting zero-motion detection duration to 0...")
            self.setZeroMotionDetectionDuration(0)
            print("Resetting FIFO...")
            self.resetFIFO()
            print("Enabling FIFO...")
            self.setFIFOEnabled(1)
            print("Enabling DMP...")
            self.setDMPEnabled(1)
            print("Resetting DMP...")
            self.resetDMP()
            print("Writing final memory update 3/7 (function unknown)...")
            j = 0
            while j < 4 or j < dmpUpdate[2] + 3:
              dmpUpdate[j] = self.dmpUpdates[pos]
              j += 1
              pos += 1
            self.writeMemoryBlock(dmpUpdate[3:], dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], True ,True)
            print("update 3/7")
            print(dmpUpdate[3:])
            #print(dmpUpdate[2])
            #print(dmpUpdate[0])
            #print(dmpUpdate[1])
            print("Writing final memory update 4/7 (function unknown)...")
            j = 0
            while j < 4 or j < dmpUpdate[2] + 3:
              dmpUpdate[j] = self.dmpUpdates[pos]
              j += 1
              pos += 1
            self.writeMemoryBlock(dmpUpdate[3:], dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], True ,True)
            print("update 4/7")
            print(dmpUpdate[3:])
            #print(dmpUpdate[2])
            #print(dmpUpdate[0])
            #print(dmpUpdate[1])
            print("Writing final memory update 5/7 (function unknown)...")
            j = 0
            while j < 4 or j < dmpUpdate[2] + 3:
              dmpUpdate[j] = self.dmpUpdates[pos]
              j += 1
              pos += 1
            self.writeMemoryBlock(dmpUpdate[3:], dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], True ,True)
            print("update 5/7")
            print(dmpUpdate[3:])
            #print(dmpUpdate[2])
            #print(dmpUpdate[0])
            #print(dmpUpdate[1])
            print("Waiting for FIFO count > 2...")
            fifoCount=self.getFIFOCount()
            while fifoCount < 3:
              fifoCount=self.getFIFOCount()
            #time.sleep(0.1)
            print("Current FIFO count=", end = "")
            print(fifoCount)
            j = 0
            while j < 4 or j < dmpUpdate[2] + 3:
              dmpUpdate[j] = self.dmpUpdates[pos]
              j += 1
              pos += 1
            self.writeMemoryBlock(dmpUpdate[3:], dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], True ,True)
            fifoCount=self.getFIFOCount()
            while fifoCount < 3:
              fifoCount=self.getFIFOCount()
            #  time.sleep(0.1)
            print("Current FIFO count=")
            print(fifoCount)
            IntStatus = self.getIntStatus()
            j = 0
            while j < 4 or j < dmpUpdate[2] + 3:
              dmpUpdate[j] = self.dmpUpdates[pos]
              j += 1
              pos += 1
            self.writeMemoryBlock(dmpUpdate[3:], dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], True ,True)
            self.setDMPEnabled(0)
            self.resetFIFO()
            self.getIntStatus()
          else:
            return 2
        return 0
    
    def mpuIntStatus(self):
        return self.readByte(self.i2c_addr, self.MPU6050_RA_INT_STATUS , 1)[0]
    
    def dmpGetEuler(self, data):
        data[0] = math.atan2(2 * self.x * self.y - 2 * self.w *self.z, 2*self.w*self.w+2*self.x*self.x - 1)
        data[1] = -math.asin(2 * self.x *self.z + 2*self.w*self.y)
        data[2] = math.atan2(2 * self.y*self.z - 2 * self.w*self.x, 2*self.w*self.w+2*self.z*self.z-1)
    
    def dmpGetQuaternion(self, packet):
        qI = [0 for i in range(4)]
        status = self._dmpGetQuaternion(qI, packet)
        if status == 0:
          self.w = qI[0] / 16384.0
          self.x = qI[1] / 16384.0
          self.y = qI[2] / 16384.0
          self.z = qI[3] / 16384.0
          return 0
    
    def _dmpGetQuaternion(self, data, packet):
        data[0] = self.bin2dec_auto(packet[1], packet[0])
        data[1] = self.bin2dec_auto(packet[5], packet[4])
        data[2] = self.bin2dec_auto(packet[9], packet[8])
        data[3] = self.bin2dec_auto(packet[13], packet[12])
        #print(data)
        return 0
    
    def getIntStatus(self):
        return self.readByte(self.i2c_addr, self.MPU6050_RA_INT_STATUS, 1)
    
    def resetDMP(self):
        self.writeBit(self.i2c_addr, self.MPU6050_RA_USER_CTRL, self.MPU6050_USERCTRL_DMP_RESET_BIT, 1)
    
    def setDMPEnabled(self, enabled):
        self.writeBit(self.i2c_addr, self.MPU6050_RA_USER_CTRL, self.MPU6050_USERCTRL_DMP_EN_BIT, enabled)
    
    def setFIFOEnabled(self, enabled):
        self.writeBit(self.i2c_addr, self.MPU6050_RA_USER_CTRL, self.MPU6050_USERCTRL_FIFO_EN_BIT, enabled)
    
    def setZeroMotionDetectionDuration(self, duration):
        self.writeByte(self.i2c_addr, self.MPU6050_RA_ZRMOT_DUR, [duration])
    
    def setMotionDetectionDuration(self, duration):
        self.writeByte(self.i2c_addr, self.MPU6050_RA_MOT_DUR, [duration])
          
    def setZeroMotionDetectionThreshold(self, threshold):
        self.writeByte(self.i2c_addr, self.MPU6050_RA_ZRMOT_THR, [threshold])
          
    def setMotionDetectionThreshold(self, threshold):
        self.writeByte(self.i2c_addr, self.MPU6050_RA_MOT_THR, [threshold])
    
    def getFIFOBytes(self, length):
        return self.readByte(self.i2c_addr, self.MPU6050_RA_FIFO_R_W, length)
    
    def getFIFOCount(self):
        buf = self.readByte(self.i2c_addr, self.MPU6050_RA_FIFO_COUNTH, 2)
        #print("weisiqi")
        #print(buf[0])
        #print(buf[1])
        return buf[0] << 8 | buf[1]
    
    def resetFIFO(self):
        self.writeBit(self.i2c_addr, self.MPU6050_RA_USER_CTRL, self.MPU6050_USERCTRL_FIFO_RESET_BIT, 1)
    
    def setZGyroOffsetUser(self, offset):
        self.writeWord(self.i2c_addr, self.MPU6050_RA_ZG_OFFS_USRH, [offset])
    
    def setYGyroOffsetUser(self, offset):
        self.writeWord(self.i2c_addr, self.MPU6050_RA_YG_OFFS_USRH, [offset])
    
    def setXGyroOffsetUser(self, offset):
        self.writeWord(self.i2c_addr, self.MPU6050_RA_XG_OFFS_USRH, [offset])
    
    def setZGyroOffset(self, offset):
        self.writeBits(self.i2c_addr, self.MPU6050_RA_ZG_OFFS_TC, self.MPU6050_TC_OFFSET_BIT, self.MPU6050_TC_OFFSET_LENGTH, offset)
    
    def setYGyroOffset(self, offset):
        self.writeBits(self.i2c_addr, self.MPU6050_RA_YG_OFFS_TC, self.MPU6050_TC_OFFSET_BIT, self.MPU6050_TC_OFFSET_LENGTH, offset)
          
    def setXGyroOffset(self, offset):
        self.writeBits(self.i2c_addr, self.MPU6050_RA_XG_OFFS_TC, self.MPU6050_TC_OFFSET_BIT, self.MPU6050_TC_OFFSET_LENGTH, offset)
    
    def setOTPBankValid(self, enabled):
        self.writeBit(self.i2c_addr, self.MPU6050_RA_XG_OFFS_TC, self.MPU6050_TC_OTP_BNK_VLD_BIT, enabled)
    
    def setDMPConfig2(self, config):
        self.writeByte(self.i2c_addr, self.MPU6050_RA_DMP_CFG_2, [config])
    
    def setDMPConfig1(self, config):
        self.writeByte(self.i2c_addr, self.MPU6050_RA_DMP_CFG_1, [config])
    
    def setDLPFMode(self, mode):
        self.writeBits(self.i2c_addr, self.MPU6050_RA_CONFIG, self.MPU6050_CFG_DLPF_CFG_BIT, self.MPU6050_CFG_DLPF_CFG_LENGTH, mode)
    
    def setExternalFrameSync(self, sync):
        self.writeBits(self.i2c_addr, self.MPU6050_RA_CONFIG, self.MPU6050_CFG_EXT_SYNC_SET_BIT, self.MPU6050_CFG_EXT_SYNC_SET_LENGTH, sync)
    
    def setRate(self, rate):
        self.writeByte(self.i2c_addr, self.MPU6050_RA_SMPLRT_DIV, [rate])
    
    def setIntEnabled(self, enabled):
        self.writeByte(self.i2c_addr, self.MPU6050_RA_INT_ENABLE, [enabled])
    
    def writeDMPConfigurationSet(self, data, dataSize, useProgMem):
        progBuffer = [0 for i in range(8)]
        i = 0
        while i < dataSize:
          bank = data[i]
          i += 1
          offset = data[i]
          i += 1
          length = data[i]
          i += 1
          if length > 0:
            #print("Writing config block to bank {} , offset {}, length= {}".format(bank, offset, length))
            if length > 8:
              progBuffer = [0 for i in range(length)]
            for j in range(length):
              progBuffer[j] = data[i+j]
            success = self.writeMemoryBlock(progBuffer, length, bank, offset, True, True)
            i += length
            #print(length)
            #print(bank)
            #print(offset)
            #print(progBuffer)
            #while True:pass
          else:
            if useProgMem:
              special = data[i]
              i += 1
            if special == 0x01:
              self.writeByte(self.i2c_addr, self.MPU6050_RA_INT_ENABLE, [0x32])
              success = True
            else:
              success = False
              print("unknown special command")
        return success
    
    def writeProgDMPConfigurationSet(self, data, dataSize):
        return self.writeDMPConfigurationSet(data, dataSize, True)
    
    def writeProgMemoryBlock(self, data, dataSize, bank = 0, address = 0, verify = True):
        return  self.writeMemoryBlock(data, dataSize, bank, address, verify, True)
        
    def writeMemoryBlock(self, data, dataSize, bank, address, verify = True, useProgMem = False):
        self.setMemoryBank(bank)
        self.setMemoryStartAddress(address)
        #print(bank)
        #print(address)
        #print(int(verify))
        #print(int(useProgMem)) 
        chunkSize = 16
        progBuffer = []
        count = 0
        i = 0
        while i < dataSize:
          if (i + chunkSize > dataSize):
            chunkSize = dataSize - i
          if (chunkSize > 256 - address):
            chunkSize = 256 - address
          #print("chunkSize")
          #print(chunkSize)
          if useProgMem:
            for j in range(chunkSize):
              progBuffer.append(data[i+j])
          self.writeBytes(self.i2c_addr, self.MPU6050_RA_MEM_R_W, chunkSize, progBuffer)
          #print(progBuffer)
          #print(i)
          #print(chunkSize)
          progBuffer = []
          if verify:
            self.setMemoryBank(bank)
            self.setMemoryStartAddress(address)
            verifyBuffer = self.readByte(self.i2c_addr, self.MPU6050_RA_MEM_R_W, chunkSize)
            #print(verifyBuffer)
          i = i + chunkSize
          address += chunkSize
          address = address % 256
          #print(count)
          if i < dataSize:
            if address == 0: bank = bank + 1
            self.setMemoryBank(bank)
            self.setMemoryStartAddress(address)
        return True
        
    def writeBytes(self, i2c_addr, reg, lens, progBuffer):
        data = []
        for i in range(lens):
          data.append(progBuffer[i])
        self.writeByte(i2c_addr, reg, data)
    
    def resetI2CMaster(self):
        self.writeBit(self.i2c_addr, self.MPU6050_RA_USER_CTRL, self.MPU6050_USERCTRL_I2C_MST_RESET_BIT, 1)
    
    def setI2CMasterModeEnabled(self, enabled):
        self.writeBit(self.i2c_addr, self.MPU6050_RA_USER_CTRL, self.MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled)
    
    def setSlaveAddress(self, num, address):
        if num > 3:
          return
        self.writeByte(self.i2c_addr, self.MPU6050_RA_I2C_SLV0_ADDR + num*3, [address])
    
    def getZGyroOffset(self):
        return self.read_bits(self.i2c_addr, self.MPU6050_RA_ZG_OFFS_TC, self.MPU6050_TC_OFFSET_BIT, self.MPU6050_TC_OFFSET_LENGTH)
    
    def getYGyroOffset(self):
        return self.read_bits(self.i2c_addr, self.MPU6050_RA_YG_OFFS_TC, self.MPU6050_TC_OFFSET_BIT, self.MPU6050_TC_OFFSET_LENGTH)
    
    def getXGyroOffset(self):
        return self.read_bits(self.i2c_addr, self.MPU6050_RA_XG_OFFS_TC, self.MPU6050_TC_OFFSET_BIT, self.MPU6050_TC_OFFSET_LENGTH)
    
    def getOTPBankValid(self):
        return self.readBit(self.i2c_addr, self.MPU6050_RA_XG_OFFS_TC, self.MPU6050_TC_OTP_BNK_VLD_BIT)
    
    def readMemoryByte(self):
        return self.readByte(self.i2c_addr, self.MPU6050_RA_MEM_R_W, 1)
    
    def setMemoryStartAddress(self, address):
        self.writeByte(self.i2c_addr, self.MPU6050_RA_MEM_START_ADDR, [address])
    
    def setMemoryBank(self, bank, prefetchEnabled=False, userBank=False):
        bank &= 0x1F
        if userBank: bank |= 0x20
        if prefetchEnabled: bank |= 0x40
        #print("22222222")
        #print(bank)
        self.writeByte(self.i2c_addr, self.MPU6050_RA_BANK_SEL, [bank])
        
    def reset(self):
        self.writeBit(self.i2c_addr, self.MPU6050_RA_PWR_MGMT_1, self.MPU6050_PWR1_DEVICE_RESET_BIT, 1)
        
    def gyro(self, mark):
        buf = self.get_motion6()
        if mark == 0:
            return buf[3]
        elif mark == 1:
            return buf[4]
        elif mark == 2:
            return buf[5]
    
    def acc(self, mark):
        buf = self.get_motion6()
        if mark == 0:
            return buf[0] / 2 
        elif mark == 1:
            return buf[1] / 2
        elif mark == 2:
            return buf[2] / 2
    
    def init(self):
        self.setClockSource(self.MPU6050_CLOCK_PLL_XGYRO)
        self.set_full_scale_GyroRange(self.MPU6050_GYRO_FS_250)
        self.set_full_scale_AccelRange(self.MPU6050_ACCEL_FS_16)
        self.setSleepEnabled(0)
        
    
    def get_motion6(self):
        val = []
        buf = self.read_reg(self.MPU6050_RA_ACCEL_XOUT_H, 14)
        num = [0,2,4,8,10,12]
        for i in num:
            if (buf[i] << 8) < 0x8000:
                buf[i] = (buf[i] << 8)
            else:
                buf[i] = (buf[i] << 8) - 0x10000
        val.append((buf[0]) | buf[1])
        val.append((buf[2]) | buf[3])
        val.append((buf[4]) | buf[5])
        val.append((buf[8]) | buf[9])
        val.append((buf[10]) | buf[11])
        val.append((buf[12]) | buf[13])
        return val
        
    
    def connection(self):
        if self.get_deviceID() == 0x34:
            return True
        else:
            return False
    
    def get_deviceID(self):
        buf = self.read_bits(self.i2c_addr, self.MPU6050_RA_WHO_AM_I, self.MPU6050_WHO_AM_I_BIT, self.MPU6050_WHO_AM_I_LENGTH)
        return buf
    
    def setSleepEnabled(self, enable):
        self.writeBit(self.i2c_addr, self.MPU6050_RA_PWR_MGMT_1, self.MPU6050_PWR1_SLEEP_BIT, enable)
    
    def set_full_scale_AccelRange(self, ranges):
        self.writeBits(self.i2c_addr, self.MPU6050_RA_ACCEL_CONFIG, self.MPU6050_ACONFIG_AFS_SEL_BIT, self.MPU6050_ACONFIG_AFS_SEL_LENGTH, ranges)
    
    def set_full_scale_GyroRange(self, ranges):
        self.writeBits(self.i2c_addr, self.MPU6050_RA_GYRO_CONFIG, self.MPU6050_GCONFIG_FS_SEL_BIT, self.MPU6050_GCONFIG_FS_SEL_LENGTH, ranges)
    
    def setClockSource(self, source):
        self.writeBits(self.i2c_addr, self.MPU6050_RA_PWR_MGMT_1, self.MPU6050_PWR1_CLKSEL_BIT, self.MPU6050_PWR1_CLKSEL_LENGTH, source)
        
    def writeBit(self, devAddr, regAddr, bitNum, data):
        b = self.readByte(devAddr, regAddr, 1)[0]
        b = (b | 1 << bitNum)if (data != 0) else (b & ~(1 << bitNum))
        #print(b)
        self.writeByte(devAddr, regAddr, [b])
        
    def writeBits(self, devAddr, regAddr, bitStart, length, data):
        b = self.readByte(devAddr, regAddr, 1)[0]
        #print("1111111111111111111")
        #print(b)
        mask = ((1 << length) - 1) << (bitStart - length + 1)
        data <<= (bitStart - length + 1)
        data &= mask
        b &= ~(mask)
        b |= data
        #print(devAddr)
        #print(regAddr)
        #print(b)
        
        self.writeByte(devAddr, regAddr, [b])
    
    def writeWord(self, devAddr, reg, data):
        self._i2c.writeto_mem(devAddr, reg, data)
    
    def writeByte(self, devAddr, reg, data):
        #print("I2C ({}) writing {} bytes to {}...".format(hex(self.i2c_addr), len(data), hex(reg)), end = "")
        #for i in data:
        #  print(hex(i), end = " ")
        #print(". Done.")
        self._i2c.writeto_mem(devAddr, reg, data)
    
    def readBit(self, devAddr, regAddr, bitNum):
        b = self.readByte(devAddr, regAddr, 1)[0]
        data = b & (1 << bitNum)
        return data
    
    def read_bits(self, devAddr, regAddr, bitStart, length):
        b = self.readByte(devAddr, regAddr, 1)[0]
        mask = ((1 << length) - 1) << (bitStart - length + 1)
        b &= mask
        b >>= (bitStart - length + 1)
        return b
    
    def readByte(self, devAddr, regAddr, lens):
        data = self._i2c.readfrom_mem(devAddr, regAddr, lens)
        #print("I2C ({}) reading {} bytes from {}...".format(hex(devAddr), lens, hex(regAddr)), end = "")
        #for i in data:
        #  print(hex(i), end = " ")
        #print(". Done.({} read)".format(len(data)))
        return data
    
    def write_reg(self, reg, value):
        self._i2c.writeto_mem(self.i2c_addr, reg, data)
         
    def read_reg(self, reg, lens):
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
