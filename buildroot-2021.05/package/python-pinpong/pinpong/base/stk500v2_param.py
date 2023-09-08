# -*- coding: utf-8 -*-
#**** ATMEL AVR - A P P L I C A T I O N   N O T E  ************************
#*
#* Title:		AVR068 - STK500 Communication Protocol
#* Filename:		command.h
#* Version:		1.0
#* Last updated:	10.01.2005
#*
#* Support E-mail:	avr@atmel.com
#*
#**************************************************************************
#
# *****************[ STK message constants ]***************************

MESSAGE_START                      = 0x1B
TOKEN                              = 0x0E

# *****************[ STK general command constants ]**************************

CMD_SIGN_ON                         = 0x01
CMD_SET_PARAMETER                   = 0x02
CMD_GET_PARAMETER                   = 0x03
CMD_SET_DEVICE_PARAMETERS           = 0x04
CMD_OSCCAL                          = 0x05
CMD_LOAD_ADDRESS                    = 0x06
CMD_FIRMWARE_UPGRADE                = 0x07
CMD_CHECK_TARGET_CONNECTION         = 0x0D
CMD_LOAD_RC_ID_TABLE                = 0x0E
CMD_LOAD_EC_ID_TABLE                = 0x0F


# *****************[ STK ISP command constants ]******************************

CMD_ENTER_PROGMODE_ISP              = 0x10
CMD_LEAVE_PROGMODE_ISP              = 0x11
CMD_CHIP_ERASE_ISP                  = 0x12
CMD_PROGRAM_FLASH_ISP               = 0x13
CMD_READ_FLASH_ISP                  = 0x14
CMD_PROGRAM_EEPROM_ISP              = 0x15
CMD_READ_EEPROM_ISP                 = 0x16
CMD_PROGRAM_FUSE_ISP                = 0x17
CMD_READ_FUSE_ISP                   = 0x18
CMD_PROGRAM_LOCK_ISP                = 0x19
CMD_READ_LOCK_ISP                   = 0x1A
CMD_READ_SIGNATURE_ISP              = 0x1B
CMD_READ_OSCCAL_ISP                 = 0x1C
CMD_SPI_MULTI                       = 0x1D

# *****************[ STK PP command constants ]*******************************

CMD_ENTER_PROGMODE_PP               = 0x20
CMD_LEAVE_PROGMODE_PP               = 0x21
CMD_CHIP_ERASE_PP                   = 0x22
CMD_PROGRAM_FLASH_PP                = 0x23
CMD_READ_FLASH_PP                   = 0x24
CMD_PROGRAM_EEPROM_PP               = 0x25
CMD_READ_EEPROM_PP                  = 0x26
CMD_PROGRAM_FUSE_PP                 = 0x27
CMD_READ_FUSE_PP                    = 0x28
CMD_PROGRAM_LOCK_PP                 = 0x29
CMD_READ_LOCK_PP                    = 0x2A
CMD_READ_SIGNATURE_PP               = 0x2B
CMD_READ_OSCCAL_PP                  = 0x2C
CMD_SET_CONTROL_STACK               = 0x2D

# *****************[ STK HVSP command constants ]*****************************

CMD_ENTER_PROGMODE_HVSP             = 0x30
CMD_LEAVE_PROGMODE_HVSP             = 0x31
CMD_CHIP_ERASE_HVSP                 = 0x32
CMD_PROGRAM_FLASH_HVSP              = 0x33
CMD_READ_FLASH_HVSP                 = 0x34
CMD_PROGRAM_EEPROM_HVSP             = 0x35
CMD_READ_EEPROM_HVSP                = 0x36
CMD_PROGRAM_FUSE_HVSP               = 0x37
CMD_READ_FUSE_HVSP                  = 0x38
CMD_PROGRAM_LOCK_HVSP               = 0x39
CMD_READ_LOCK_HVSP                  = 0x3A
CMD_READ_SIGNATURE_HVSP             = 0x3B
CMD_READ_OSCCAL_HVSP                = 0x3C
# These two are redefined since 0x30/0x31 collide
# with the STK600 bootloader.
CMD_ENTER_PROGMODE_HVSP_STK600     = 0x3D
CMD_LEAVE_PROGMODE_HVSP_STK600     = 0x3E

# *** XPROG command constants ***

CMD_XPROG                          = 0x50
CMD_XPROG_SETMODE                  = 0x51


# *** AVR32 JTAG Programming command ***

CMD_JTAG_AVR32                      = 0x80
CMD_ENTER_PROGMODE_JTAG_AVR32       = 0x81
CMD_LEAVE_PROGMODE_JTAG_AVR32       = 0x82


# *** AVR JTAG Programming command ***

#define CMD_JTAG_AVR                        0x90

# *****************[ STK test command constants ]***************************

CMD_ENTER_TESTMODE                  = 0x60
CMD_LEAVE_TESTMODE                  = 0x61
CMD_CHIP_WRITE                      = 0x62
CMD_PROGRAM_FLASH_PARTIAL           = 0x63
CMD_PROGRAM_EEPROM_PARTIAL          = 0x64
CMD_PROGRAM_SIGNATURE_ROW           = 0x65
CMD_READ_FLASH_MARGIN               = 0x66
CMD_READ_EEPROM_MARGIN              = 0x67
CMD_READ_SIGNATURE_ROW_MARGIN       = 0x68
CMD_PROGRAM_TEST_FUSE               = 0x69
CMD_READ_TEST_FUSE                  = 0x6A
CMD_PROGRAM_HIDDEN_FUSE_LOW         = 0x6B
CMD_READ_HIDDEN_FUSE_LOW            = 0x6C
CMD_PROGRAM_HIDDEN_FUSE_HIGH        = 0x6D
CMD_READ_HIDDEN_FUSE_HIGH           = 0x6E
CMD_PROGRAM_HIDDEN_FUSE_EXT         = 0x6F
CMD_READ_HIDDEN_FUSE_EXT            = 0x70

# *****************[ STK status constants ]***************************

# Success
STATUS_CMD_OK                       = 0x00

# Warnings
STATUS_CMD_TOUT                     = 0x80
STATUS_RDY_BSY_TOUT                 = 0x81
STATUS_SET_PARAM_MISSING            = 0x82

# Errors
STATUS_CMD_FAILED                   = 0xC0
STATUS_CKSUM_ERROR                  = 0xC1
STATUS_CMD_UNKNOWN                  = 0xC9
STATUS_CMD_ILLEGAL_PARAMETER        = 0xCA

# Status
STATUS_CONN_FAIL_MOSI               = 0x01
STATUS_CONN_FAIL_RST                = 0x02
STATUS_CONN_FAIL_SCK                = 0x04
STATUS_TGT_NOT_DETECTED             = 0x00
STATUS_ISP_READY                    = 0x10
STATUS_TGT_REVERSE_INSERTED         = 0x20

# hw_status
# Bits in status variable
# Bit 0-3: Slave MCU
# Bit 4-7: Master MCU

STATUS_AREF_ERROR    = 0
# Set to '1' if AREF is short circuited

STATUS_VTG_ERROR    = 4
# Set to '1' if VTG is short circuited

STATUS_RC_CARD_ERROR = 5
# Set to '1' if board id changes when board is powered

STATUS_PROGMODE     = 6
# Set to '1' if board is in programming mode

STATUS_POWER_SURGE   = 7
# Set to '1' if board draws excessive current

# *****************[ STK parameter constants ]***************************
PARAM_BUILD_NUMBER_LOW             = 0x80
PARAM_BUILD_NUMBER_HIGH            = 0x81
PARAM_HW_VER                       = 0x90
PARAM_SW_MAJOR                     = 0x91
PARAM_SW_MINOR                     = 0x92
PARAM_VTARGET                      = 0x94
PARAM_VADJUST                      = 0x95
PARAM_OSC_PSCALE                   = 0x96
PARAM_OSC_CMATCH                   = 0x97
PARAM_SCK_DURATION                 = 0x98
PARAM_TOPCARD_DETECT               = 0x9A
PARAM_STATUS                       = 0x9C
PARAM_DATA                         = 0x9D
PARAM_RESET_POLARITY               = 0x9E
PARAM_CONTROLLER_INIT              = 0x9F

# STK600 parameters 
PARAM_STATUS_TGT_CONN              = 0xA1
PARAM_DISCHARGEDELAY               = 0xA4
PARAM_SOCKETCARD_ID                = 0xA5
PARAM_ROUTINGCARD_ID               = 0xA6
PARAM_EXPCARD_ID                   = 0xA7
PARAM_SW_MAJOR_SLAVE1              = 0xA8
PARAM_SW_MINOR_SLAVE1              = 0xA9
PARAM_SW_MAJOR_SLAVE2              = 0xAA
PARAM_SW_MINOR_SLAVE2              = 0xAB
PARAM_BOARD_ID_STATUS              = 0xAD
PARAM_RESET                        = 0xB4

PARAM_JTAG_ALLOW_FULL_PAGE_STREAM  = 0x50
PARAM_JTAG_EEPROM_PAGE_SIZE        = 0x52
PARAM_JTAG_DAISY_BITS_BEFORE       = 0x53
PARAM_JTAG_DAISY_BITS_AFTER        = 0x54
PARAM_JTAG_DAISY_UNITS_BEFORE      = 0x55
PARAM_JTAG_DAISY_UNITS_AFTER       = 0x56

# *** Parameter constants for 2 byte values ***
PARAM2_SCK_DURATION                = 0xC0
PARAM2_CLOCK_CONF                  = 0xC1
PARAM2_AREF0                       = 0xC2
PARAM2_AREF1                       = 0xC3

PARAM2_JTAG_FLASH_SIZE_H           = 0xC5
PARAM2_JTAG_FLASH_SIZE_L           = 0xC6
PARAM2_JTAG_FLASH_PAGE_SIZE        = 0xC7
PARAM2_RC_ID_TABLE_REV             = 0xC8
PARAM2_EC_ID_TABLE_REV             = 0xC9

# STK600 XPROG section
# XPROG modes
XPRG_MODE_PDI                      = 0
XPRG_MODE_JTAG                     = 1
XPRG_MODE_TPI                      = 2

# XPROG commands
XPRG_CMD_ENTER_PROGMODE            = 0x01
XPRG_CMD_LEAVE_PROGMODE            = 0x02
XPRG_CMD_ERASE                     = 0x03
XPRG_CMD_WRITE_MEM                 = 0x04
XPRG_CMD_READ_MEM                  = 0x05
XPRG_CMD_CRC                       = 0x06
XPRG_CMD_SET_PARAM                 = 0x07

# Memory types
XPRG_MEM_TYPE_APPL                  = 1
XPRG_MEM_TYPE_BOOT                  = 2
XPRG_MEM_TYPE_EEPROM                = 3
XPRG_MEM_TYPE_FUSE                  = 4
XPRG_MEM_TYPE_LOCKBITS              = 5
XPRG_MEM_TYPE_USERSIG               = 6
XPRG_MEM_TYPE_FACTORY_CALIBRATION   = 7

# Erase types
XPRG_ERASE_CHIP                     = 1
XPRG_ERASE_APP                      = 2
XPRG_ERASE_BOOT                     = 3
XPRG_ERASE_EEPROM                   = 4
XPRG_ERASE_APP_PAGE                 = 5
XPRG_ERASE_BOOT_PAGE                = 6
XPRG_ERASE_EEPROM_PAGE              = 7
XPRG_ERASE_USERSIG                  = 8
XPRG_ERASE_CONFIG                   = 9  # TPI only, prepare fuse write

# Write mode flags
XPRG_MEM_WRITE_ERASE                = 0
XPRG_MEM_WRITE_WRITE                = 1

# CRC types
XPRG_CRC_APP                        = 1
XPRG_CRC_BOOT                       = 2
XPRG_CRC_FLASH                      = 3

# Error codes
XPRG_ERR_OK                         = 0
XPRG_ERR_FAILED                     = 1
XPRG_ERR_COLLISION                  = 2
XPRG_ERR_TIMEOUT                    = 3

# XPROG parameters of different sizes
# 4-byte address
XPRG_PARAM_NVMBASE                 = 0x01
# 2-byte page size
XPRG_PARAM_EEPPAGESIZE             = 0x02
# 1-byte, undocumented TPI param
XPRG_PARAM_TPI_3                   = 0x03
# 1-byte, undocumented TPI param
XPRG_PARAM_TPI_4                   = 0x04

# *****************[ STK answer constants ]***************************

ANSWER_CKSUM_ERROR                 = 0xB0

