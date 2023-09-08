# -*- coding: utf-8 -*-
STK_SIGN_ON_MESSAGE ="AVR STK"   #// Sign on string for Cmnd_STK_GET_SIGN_ON


Resp_STK_OK                = 0x10  # ' '
Resp_STK_FAILED            = 0x11  # ' '
Resp_STK_UNKNOWN           = 0x12  # ' '
Resp_STK_NODEVICE          = 0x13  # ' '
Resp_STK_INSYNC            = 0x14  # ' '
Resp_STK_NOSYNC            = 0x15  # ' '

Resp_ADC_CHANNEL_ERROR     = 0x16  # ' '
Resp_ADC_MEASURE_OK        = 0x17  # ' '
Resp_PWM_CHANNEL_ERROR     = 0x18  # ' '
Resp_PWM_ADJUST_OK         = 0x19  # ' '

Sync_CRC_EOP               = 0x20  # 'SPACE'

Cmnd_STK_GET_SYNC          = 0x30  # ' '
Cmnd_STK_GET_SIGN_ON       = 0x31  # ' '

Cmnd_STK_SET_PARAMETER     = 0x40  # ' '
Cmnd_STK_GET_PARAMETER     = 0x41  # ' '
Cmnd_STK_SET_DEVICE        = 0x42  # ' '
Cmnd_STK_SET_DEVICE_EXT    = 0x45  # ' '

Cmnd_STK_ENTER_PROGMODE    = 0x50  # ' '
Cmnd_STK_LEAVE_PROGMODE    = 0x51  # ' '
Cmnd_STK_CHIP_ERASE        = 0x52  # ' '
Cmnd_STK_CHECK_AUTOINC     = 0x53  # ' '
Cmnd_STK_LOAD_ADDRESS      = 0x55  # ' '
Cmnd_STK_UNIVERSAL         = 0x56  # ' '
Cmnd_STK_UNIVERSAL_MULTI   = 0x57  # ' '

Cmnd_STK_PROG_FLASH        = 0x60  # ' '
Cmnd_STK_PROG_DATA         = 0x61  # ' '
Cmnd_STK_PROG_FUSE         = 0x62  # ' '
Cmnd_STK_PROG_LOCK         = 0x63  # ' '
Cmnd_STK_PROG_PAGE         = 0x64  # ' '
Cmnd_STK_PROG_FUSE_EXT     = 0x65  # ' '

Cmnd_STK_READ_FLASH        = 0x70  # ' '
Cmnd_STK_READ_DATA         = 0x71  # ' '
Cmnd_STK_READ_FUSE         = 0x72  # ' '
Cmnd_STK_READ_LOCK         = 0x73  # ' '
Cmnd_STK_READ_PAGE         = 0x74  # ' '
Cmnd_STK_READ_SIGN         = 0x75  # ' '
Cmnd_STK_READ_OSCCAL       = 0x76  # ' '
Cmnd_STK_READ_FUSE_EXT     = 0x77  # ' '
Cmnd_STK_READ_OSCCAL_EXT   = 0x78  # ' '

Parm_STK_HW_VER            = 0x80  # ' ' - R
Parm_STK_SW_MAJOR          = 0x81  # ' ' - R
Parm_STK_SW_MINOR          = 0x82  # ' ' - R
Parm_STK_LEDS              = 0x83  # ' ' - R/W
Parm_STK_VTARGET           = 0x84  # ' ' - R/W
Parm_STK_VADJUST           = 0x85  # ' ' - R/W
Parm_STK_OSC_PSCALE        = 0x86  # ' ' - R/W
Parm_STK_OSC_CMATCH        = 0x87  # ' ' - R/W
Parm_STK_RESET_DURATION    = 0x88  # ' ' - R/W
Parm_STK_SCK_DURATION      = 0x89  # ' ' - R/W

Parm_STK_BUFSIZEL          = 0x90  # ' ' - R/W, Range {0..255}
Parm_STK_BUFSIZEH          = 0x91  # ' ' - R/W, Range {0..255}
Parm_STK_DEVICE            = 0x92  # ' ' - R/W, Range {0..255}
Parm_STK_PROGMODE          = 0x93  # ' ' - 'P' or 'S'
Parm_STK_PARAMODE          = 0x94  # ' ' - TRUE or FALSE
Parm_STK_POLLING           = 0x95  # ' ' - TRUE or FALSE
Parm_STK_SELFTIMED         = 0x96  # ' ' - TRUE or FALSE
Param_STK500_TOPCARD_DETECT=  0x98 #/ ' ' - Detect top-card attached

Stat_STK_INSYNC            = 0x01  # INSYNC status bit, '1' - INSYNC
Stat_STK_PROGMODE          = 0x02  # Programming mode,  '1' - PROGMODE
Stat_STK_STANDALONE        = 0x04  # Standalone mode,   '1' - SM mode
Stat_STK_RESET             = 0x08  # RESET button,      '1' - Pushed
Stat_STK_PROGRAM           = 0x10  # Program button, '   1' - Pushed
Stat_STK_LEDG              = 0x20  # Green LED status,  '1' - Lit
Stat_STK_LEDR              = 0x40  # Red LED status,    '1' - Lit
Stat_STK_LEDBLINK          = 0x80  # LED blink ON/OFF,  '1' - Blink