# -*- coding: utf-8 -*-
"""
 Copyright (c) 2015-2019 Alan Yorinks All rights reserved.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE
 Version 3 as published by the Free Software Foundation; either
 or (at your option) any later version.
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE
 along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""


class PrivateConstants:
    """
    This class contains a set of constants for PyMata internal use .
    """
    # the following defines are from FirmataExpress.h
    # message command bytes (128-255/ 0x80- 0xFF)
    # from this client to firmata
    MSG_CMD_MIN = 0x80  # minimum value for a message from firmata
    REPORT_ANALOG = 0xC0  # enable analog input by pin #
    REPORT_DIGITAL = 0xD0  # enable digital input by port pair
    SET_PIN_MODE = 0xF4  # set a pin to INPUT/OUTPUT/PWM/etc
    SET_DIGITAL_PIN_VALUE = 0xF5  # set a single digital pin value instead of entire port
    START_SYSEX = 0xF0  # start a MIDI Sysex message
    END_SYSEX = 0xF7  # end a MIDI Sysex message
    SYSTEM_RESET = 0xFF  # reset from MIDI

    # messages from firmata
    DIGITAL_MESSAGE = 0x90  # send or receive data for a digital pin
    ANALOG_MESSAGE = 0xE0  # send or receive data for a PWM configured pin
    PWM_MESSAGE = 0xE0 # Firmata confusingly conflates analog input with PWM output
    SERVO_MESSAGE = 0xE0
    REPORT_VERSION = 0xF9  # report protocol version

    # start of FirmataExpress defined SYSEX commands
    NEOPIXEL_DATA = 0x30  # NEOPIXEL message
    NEOPIXEL_CONFIG = 0x31
    KEEP_ALIVE = 0x50  # keep alive message
    ARE_YOU_THERE = 0x51  # poll for boards existence
    I_AM_HERE = 0x52  # response to poll
    TONE_DATA = 0x5F  # play a tone at a specified frequency and duration
    SONAR_CONFIG = 0x62  # configure pins to control a sonar distance device
    SONAR_DATA = 0x63  # distance data returned
    # end of FirmataExpress defined SYSEX commands

    SERVO_CONFIG = 0x70  # set servo pin and max and min angles
    STRING_DATA = 0x71  # a string message with 14-bits per char
    STEPPER_DATA = 0x72  # Stepper motor command
    I2C_REQUEST = 0x76  # send an I2C read/write request
    I2C_REPLY = 0x77  # a reply to an I2C read request
    I2C_CONFIG = 0x78  # config I2C settings such as delay times and power pins
    REPORT_FIRMWARE = 0x79  # report name and version of the firmware
    SAMPLING_INTERVAL = 0x7A  # modify the sampling interval
    RESERVED_1 = 0x7B

    EXTENDED_PWM = 0x6F  # analog write (PWM, Servo, etc) to any pin
    PIN_STATE_QUERY = 0x6D  # ask for a pin's current mode and value
    PIN_STATE_RESPONSE = 0x6E  # reply with pin's current mode and value
    CAPABILITY_QUERY = 0x6B  # ask for supported modes of all pins
    CAPABILITY_RESPONSE = 0x6C  # reply with supported modes and resolution
    ANALOG_MAPPING_QUERY = 0x69  # ask for mapping of analog to pin numbers
    ANALOG_MAPPING_RESPONSE = 0x6A  # reply with analog mapping data

    # reserved values
    SYSEX_NON_REALTIME = 0x7E  # MIDI Reserved for non-realtime messages
    SYSEX_REALTIME = 0x7F  # MIDI Reserved for realtime messages

    # reserved for PyMata
    PYMATA_EXPRESS_THREADED_VERSION = "1.4"

    # each byte represents a digital port
    #  and its value contains the current port settings
    DIGITAL_OUTPUT_PORT_PINS = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]

    # These values are the index into the data passed by _arduino and
    # used to reassemble integer values
    MSB = 2
    LSB = 1

    # enable reporting for REPORT_ANALOG or REPORT_DIGITAL message
    # sent to firmata
    REPORTING_ENABLE = 1
    # disable reporting for REPORT_ANALOG or REPORT_DIGITAL message
    # sent to firmata
    REPORTING_DISABLE = 0

    # Stepper Motor Sub-commands
    STEPPER_CONFIGURE = 0  # configure a stepper motor for operation
    STEPPER_STEP = 1  # command a motor to move at the provided speed
    STEPPER_LIBRARY_VERSION = 2  # used to get stepper library version number

    # pin modes
    INPUT = 0x00  # pin set as input
    OUTPUT = 0x01  # pin set as output
    ANALOG = 0x02  # analog pin in analogInput mode
    PWM = 0x03  # digital pin in PWM output mode
    SERVO = 0x04  # digital pin in Servo output mode
    I2C = 0x06  # pin included in I2C setup
    STEPPER = 0x08  # any pin in stepper mode
    SERIAL = 0x0a
    PULLUP = 0x0b  # Any pin in pullup mode
    SONAR = 0x0c  # Any pin in SONAR mode
    TONE = 0x0d  # Any pin in tone mode
    PIXY = 0x0e  # reserved for pixy camera mode
    DHT = 0x0f  # DHT sensor
    NEOPIXEL = 0x60  # Any pin in neopixel mode
    UART = 0x61  # uart mode
    IGNORE = 0x7f

    # Tone commands
    TONE_TONE = 0  # play a tone
    TONE_NO_TONE = 1  # turn off tone

    # DHT command
    DHT_CONFIG = 0x64  # dht config command
    DHT_DATA = 0x65  # dht sensor command
    ###

    # I2C command operation modes
    I2C_WRITE = 0B00000000
    I2C_READ = 0B00001000
    I2C_READ_CONTINUOUSLY = 0B00010000
    I2C_STOP_READING = 0B00011000
    I2C_READ_WRITE_MODE_MASK = 0B00011000
    I2C_10BIT_ADDRESS_MODE_MASK = 0B00100000
    I2C_END_TX_MASK = 0B01000000
    I2C_STOP_TX = 1
    I2C_RESTART_TX = 0

    IRQ_FALLING = 2
    IRQ_RISING = 1

    DFROBOT_MESSAGE             = 0x0D
    SUB_MESSAGE_DFROBOT_ESP32_REPORTS = 0x0A
    SUB_MESSAGE_DFROBOT_MICROBIT_REPORTS = 0x0B
    SUB_MESSAGE_DFROBOT_GD32V_REPORTS = 0x0C
    SUB_MESSAGE_IR              = 0x00
    SUB_MESSAGE_PULSE           = 0x03
    SUB_MESSAGE_MILLIS          = 0x04
    SUB_MESSAGE_I2CSCAN         = 0x0E
    SUB_MESSAGE_DS18B20         = 0x11
    SUB_MESSAGE_VIBRATION       = 0x15
    SUB_MESSAGE_RESET           = 0x7f
    SUB_MESSAGE_NEOPIXEL        = 0x17
    SUB_MESSAGE_18B20           = 0x19
    SUB_MESSAGE_GP2Y1010AU0F    = 0x58
    SUB_MESSAGE_AUDIO           = 0x21
    SUB_MESSAGE_HX711           = 0x23
    SUB_MESSAGE_HEARTRATE       = 0x25
    SUB_MESSAGE_UART_READ       = 0x26


    SUB_REPORT_FLAG             = 0x20
    SUB_MESSAGE_OLED12864       = 0x16
    SUB_MESSAGE_WS2812          = 0x14
    SUB_MESSAGE_SOUND           = 0x09
    SOUND_SETTICKSTEMPO         = 0x06
    SOUND_PLAYNOTE              = 0x01

    SUB_MICROBIT_REPORT_FLAG    = 0x2A

    SUB_MESSAGE_ssid            = 0x40
    SUB_MESSAGE_password        = 0x41
    SUB_MESSAGE_CONNECT_WIFI    = 0x43
    SUB_MESSAGE_CONNECT_PESPOND = 0x50
    
    SUB_MESSAGE_MATRIX          = 0x05
    MATRIX_SHOWBITMAP           = 0x03
    MATRIX_SHOWSTRING           = 0x04
    MATRIX_DRAWPIXEL            = 0x00
    MATRIX_CLEAR                = 0x02
    SUB_MESSAGE_SOUND           = 0x09
    SOUND_PLAYSOUND             = 0x00
    SOUND_SETSPEED              = 0x03
    CAL_COMPASS                 = 0x14
    SUB_MESSAGE_WIRELESS        = 0x10

    HX711_INIT                  = 0x07
    HX711_READ                  = 0x0A
    AUDIO_VALUE                 = 0X07
    AUDIO_INIT                  = 0x16
    AUDIO_read                  = 0x12
    DS18B20_READ                = 0x09
    SERVO_CONTROL               = 0x56
    UART_CONFIG                 = 0x0C
    UART_WRITE                  = 0x0D
    UART_READ                   = 0x0E
    UART_DEINIT                 = 0x0F
    
    SUB_MESSAGE_OLED            = 0x30

    MODE_8N1                    = 0x00
    SUB_MESSAGE_SOUND           = 0x09
    SUB_MESSAGE_COMPASS         = 0x14
    
    SUB_MESSAGE_BUZZ            = 0x55

    SUB_MESSAGE_SPI_INIT        = 0x32
    SUB_MESSAGE_SPI_SENDTO      = 0x33
    SUB_MESSAGE_SPI_RECVFROM    = 0x34
    SUB_MESSAGE_SPI             = 0x35
    