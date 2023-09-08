# -*- coding: utf-8 -*-
"""
 Copyright (c) 2020 Alan Yorinks All rights reserved.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE
 Version 3 as published by the Free Software Foundation; either
 or (at your option) any later version.
 This library is distributed in the hope that it will be useful,f
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE
 along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""

from collections import deque
import serial
import signal
import types
# noinspection PyPackageRequirements
from serial.tools import list_ports
# noinspection PyPackageRequirementscd
from serial.serialutil import SerialException
import socket
import os
import sys
import threading
import time
import math

from pinpong.base.pin_data import PinData
from pinpong.base.private_constants import PrivateConstants


# noinspection PyPep8
class Pymata4(threading.Thread):
    """
    This class exposes and implements the PymataExpress Non-asyncio API.
    It uses threading to accommodate concurrency.
    It includes the public API methods as well as
    a set of private methods.
    """

    # noinspection PyPep8,PyPep8,PyPep8
    def __init__(self, com_port=None, baud_rate=115200,
                 arduino_instance_id=1, arduino_wait=4,
                 sleep_tune=0.000001,
                 shutdown_on_exception=True, ip_address=None,
                 ip_port=None, name = None):
        """
        If you are using the Firmata Express Arduino sketch,
        and have a single Arduino connected to your computer,
        then you may accept all the default values.

        If you are using some other Firmata sketch, then
        you must specify both the com_port and baudrate.

        :param com_port: e.g. COM3 or /dev/ttyACM0.

        :param baud_rate: Match this to the Firmata sketch in use.

        :param arduino_instance_id: If you are using the Firmata
                                    Express sketch, match this
                                    value to that in the sketch.

        :param arduino_wait: Amount of time to wait for an Arduino to
                             fully reset itself.

        :param sleep_tune: A tuning parameter (typically not changed by user)

        :param shutdown_on_exception: call shutdown before raising
                                      a RunTimeError exception, or
                                      receiving a KeyboardInterrupt exception

        """
        # initialize threading parent
        threading.Thread.__init__(self)

        # create the threads and set them as daemons so
        # that they stop when the program is closed

        # create a thread to interpret received serial data
        self.the_reporter_thread = threading.Thread(target=self._reporter)
        self.the_reporter_thread.daemon = True

        self.ip_address = ip_address
        self.ip_port = ip_port
        self.name = name

        # create a thread to continuously receive serial data
        if self.ip_address:
            self.the_data_receive_thread = threading.Thread(
                target=self.__tcp_receiver)
        else:
            self.the_data_receive_thread = threading.Thread(
                target=self._serial_receiver)

        self.the_data_receive_thread.daemon = True

        if not self.ip_address:
            self.the_serial_monitor_thread = threading.Thread(
                target=self._serial_monitor)
            self.the_serial_monitor_thread.daemon = True

        # keep alive variables
        self.keep_alive_interval = []
        self.period = 0
        self.margin = 0

        # create a thread for the keep alives
        self.the_keep_alive_thread = threading.Thread(
            target=self._send_keep_alive)
        self.the_keep_alive_thread.daemon = True

        # flag to allow the reporter and receive threads to run.
        self.run_event = threading.Event()
        self.iic_async_lock = threading.Lock()
        self.other_async_lock = threading.Lock()
        self.spi_async_lock = threading.Lock()
        # check to make sure that Python interpreter is version 3.7 or greater
        python_version = sys.version_info
        if python_version[0] >= 3:
            if python_version[1] >= 5:
                pass
            else:
                raise RuntimeError("ERROR: Python 3.5 or greater is "
                                   "required for use of this program.")

        # save input parameters as instance variables
        self.com_port = com_port
        self.baud_rate = baud_rate
        self.arduino_instance_id = arduino_instance_id
        self.arduino_wait = arduino_wait
        self.sleep_tune = sleep_tune
        self.shutdown_on_exception = shutdown_on_exception

        # create a deque to receive and process data from the arduino
        self.the_deque = deque()
        # The report_dispatch dictionary is used to process
        # incoming report sysex message by looking up the sysex command
        # and executing its associated processing method.
        # The value following the method is the number of bytes to
        # retrieve from the deque to process the command.
        self.report_dispatch = {}
        self.dfrobot_dispatch = {}
        self.microbit_callback = None
        self.GD32V_buttonA_callback = None
        self.GD32V_buttonB_callback = None
        self.GD32V_buttonA = 0
        self.GD32V_buttonB = 0
        self.report = False
        self.compass = False
        self.music = False
        self.buzz_flag = False
        self._report = {
            "buttonA": 0,
            "buttonB": 0,
            "touchP": {
                "value": 80,
                "touchNum": 0
            },
            "touchY": {
                "value": 80,
                "touchNum": 0
            },
            "touchT": {
                "value": 80,
                "touchNum": 0
            },
            "touchH": {
                "value": 80,
                "touchNum": 0
            },
            "touchO": {
                "value": 80,
                "touchNum": 0
            },
            "touchN": {
                "value": 80,
                "touchNum": 0
            },
            "gesture": 7,
            "microphone": 0,
            "lightness": 0,
            "accelerationX": 0,
            "accelerationY": 0,
            "accelerationZ": 0,
            "accelerationStrength": 0,
            "brightness": 0,
            "millis": 0,
            "touchThreshold": {
                "P": 50,
                "Y": 50,
                "T": 50,
                "H": 50,
                "O": 50,
                "N": 50
            },
            "touch0": 0,
            "touch1": 0,
            "touch2": 0,
            "Mgesture": 0,
            "Mbrightness": 0,
            "heading": 0,
            "temperature": 0,
            "MaccelerationX": 0,
            "MaccelerationY": 0,
            "MaccelerationZ": 0,
            "MaccelerationStrength": 0
        }
        # To add a command to the command dispatch table, append here.
        self.report_dispatch.update(
            {PrivateConstants.REPORT_VERSION: [self._report_version, 2]})
        self.report_dispatch.update(
            {PrivateConstants.REPORT_FIRMWARE: [self._report_firmware, 1]})
        self.report_dispatch.update(
            {PrivateConstants.ANALOG_MESSAGE: [self._analog_message, 2]})
        self.report_dispatch.update(
            {PrivateConstants.DIGITAL_MESSAGE: [self._digital_message, 2]})
        self.report_dispatch.update(
            {PrivateConstants.SONAR_DATA: [self._sonar_data, 3]})
        self.report_dispatch.update(
            {PrivateConstants.STRING_DATA: [self._string_data, 2]})
        self.report_dispatch.update(
            {PrivateConstants.I2C_REPLY: [self._i2c_reply, 2]})
        self.report_dispatch.update(
            {PrivateConstants.CAPABILITY_RESPONSE: [self._capability_response, 2]})
        self.report_dispatch.update(
            {PrivateConstants.PIN_STATE_RESPONSE: [self._pin_state_response, 2]})
        self.report_dispatch.update({PrivateConstants.ANALOG_MAPPING_RESPONSE: [
                                    self._analog_mapping_response, 4]})
        self.report_dispatch.update(
            {PrivateConstants.DHT_DATA: [self._dht_read_response, 7]})
        self.report_dispatch.update(
            {PrivateConstants.DFROBOT_MESSAGE: [self._dfrobot_message, 7]})

        self.dfrobot_dispatch.update({PrivateConstants.SUB_MESSAGE_IR: {}})
        self.dfrobot_dispatch.update({PrivateConstants.SUB_MESSAGE_18B20: {}})
        self.dfrobot_dispatch.update({PrivateConstants.SUB_MESSAGE_GP2Y1010AU0F: {}})
        self.dfrobot_dispatch.update({PrivateConstants.SUB_MESSAGE_SPI: {}})
        self.dfrobot_dispatch.update({PrivateConstants.SUB_MESSAGE_AUDIO: {}})
        self.dfrobot_dispatch.update({PrivateConstants.SUB_MESSAGE_HX711: {}})
        self.dfrobot_dispatch.update({PrivateConstants.SUB_MESSAGE_UART_READ: {}})
        self.dfrobot_dispatch.update(
            {PrivateConstants.SUB_MESSAGE_HEARTRATE: {}})
        self.dfrobot_dispatch.update(
            {PrivateConstants.SUB_MESSAGE_CONNECT_PESPOND: {}})
        # report query results are stored in this dictionary
        self.query_reply_data = {PrivateConstants.REPORT_VERSION: '',
                                 PrivateConstants.STRING_DATA: '',
                                 PrivateConstants.REPORT_FIRMWARE: '',
                                 PrivateConstants.CAPABILITY_RESPONSE: None,
                                 PrivateConstants.ANALOG_MAPPING_RESPONSE:
                                     None,
                                 PrivateConstants.PIN_STATE_RESPONSE: None}

        self.firmata_firmware = []

        # dht error flag
        self.dht_sensor_error = False

        # a list of PinData objects - one for each pin segregated by pin type
        # see pin_data.py
        self.analog_pins = []
        self.digital_pins = []

        # a list of pins assigned to DHT devices
        self.dht_list = []

        # This lock is used when the PinData object is update or contents
        # are retrieved
        self.the_pin_data_lock = threading.Lock()

        # a lock for the i2c map data structure
        self.the_i2c_map_lock = threading.Lock()

        # a lock for the uart map data structure
        self.the_uart_map_lock = threading.Lock()

        # a lock for the sonar map
        self.the_sonar_map_lock = threading.Lock()

        # a when sending data to the arduino
        self.the_send_sysex_lock = threading.Lock()

        # serial port in use
        self.serial_port = None

        self.sock = None

        signal.signal(signal.SIGINT, self._exit_handler)
        # An i2c_map entry consists of a device i2c address as the key, and
        #  the value of the key consists of a dictionary containing 2 entries.
        #  The first entry. 'value' contains the last value reported, and
        # the second, 'callback' contains a reference to a callback function,
        # and the third, a time-stamp
        # For example:
        # {12345: {'value': 23, 'callback': None, time_stamp:None}}
        self.i2c_map = {}
        self.uart_map = {}
        # The active_sonar_map maps the sonar trigger pin number (the key)
        # to the current data value returned
        # if a callback was specified, it is stored in the map as well.
        # A map entry consists of:
        #   pin: [callback, current_data_returned, time_stamp]
        self.active_sonar_map = {}

        # first analog pin number
        self.first_analog_pin = None

        # flag to indicate we are in shutdown mode
        self.shutdown_flag = False

        # print("pymata4:  Version %s\n\nCopyright (c) 2020 Alan Yorinks All Rights Reserved.\n"%(PrivateConstants.PYMATA_EXPRESS_THREADED_VERSION))
        if not self.ip_address:
            if not self.com_port:
                # user did not specify a com_port
                try:
                    self._find_arduino()
                except KeyboardInterrupt:
                    if self.shutdown_on_exception:
                        self.shutdown()
            else:
                # com_port specified - set com_port and baud rate
                try:
                    if isinstance(self.com_port, str):
                        self._manual_open()
                    else:
                        self.serial_port = self.com_port
                except KeyboardInterrupt:
                    if self.shutdown_on_exception:
                        self.shutdown()
            if self.serial_port:
                print("[22] Arduino compatible device found and connected to %s" % (
                    self.serial_port.port))

            # no com_port found - raise a runtime exception
            else:
                if self.shutdown_on_exception:
                    self.shutdown()
                raise RuntimeError('No Arduino Found or User Aborted Program')
        else:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, True)
            self.sock.connect((self.ip_address, self.ip_port))
            self.sock.setblocking(0)
            print("[28]Successfully connected to ip: {} port:{}".format(self.ip_address, self.ip_port))
            times = time.time()
            time.sleep(1)
            while time.time() - times < 1:
                payload=self.sock.recv(1)

        self.the_reporter_thread.start()
        self.the_data_receive_thread.start()

        # allow the threads to run
        self._run_threads()
        
#        if self.ip_address:
#            self.set_flag()
        # try:
        #     print('[30] Retrieving Arduino Firmware ID...')

        #     firmware_version = self.get_firmware_version()
        #     if not firmware_version:
        #         firmware_version = self.get_firmware_version()

        #     if not firmware_version:
        #         if self.shutdown_on_exception:
        #             self.shutdown()
        #         raise RuntimeError('Firmata Sketch Firmware Version Not Found')
        #     else:
        #         print('[32] Arduino Firmware ID: %s' % (firmware_version))
        # except TypeError:
        #     print('\nIs your serial cable plugged in and do you have the '
        #           'correct Firmata sketch loaded?')
        #     print('Is the COM port correct?')
        #     print('To see a list of serial ports, type: "list_serial_ports" '
        #           'in your console.')
        #     raise RuntimeError
        # except KeyboardInterrupt:
        #     if self.shutdown_on_exception:
        #         self.shutdown()
        #     raise RuntimeError('No Arduino Found or User Aborted Program')

        print('[40] Retrieving analog map...')

        # try to get an analog pin map. if it comes back as none raise an exception
        report = self.get_analog_map()
        if not report:
            if self.shutdown_on_exception:
                self.shutdown()
            raise RuntimeError('*** Analog map retrieval timed out. ***'
                               '\nDo you have Arduino connectivity and do you have the '
                               'correct Firmata sketch uploaded to the board?')

        # custom assemble the pin lists
        try:
            for pin in report:
                digital_data = PinData(self.the_pin_data_lock)
                self.digital_pins.append(digital_data)
                if pin != PrivateConstants.IGNORE:
                    analog_data = PinData(self.the_pin_data_lock)
                    self.analog_pins.append(analog_data)

            print('[42] Auto-discovery complete. Found %d Digital Pins'
                  ' and %d Analog Pins' % (len(self.digital_pins), len(self.analog_pins)))

            print("------------------------------")
            print("All right. PinPong go...")
            print("------------------------------")
            # print(time.clock())

            self.first_analog_pin = len(
                self.digital_pins) - len(self.analog_pins)
            # print(time.clock())
            if not self.ip_address and self.name != "UNIHIKER":
                self.the_serial_monitor_thread.start()
        except KeyboardInterrupt:
            if self.shutdown_on_exception:
                self.shutdown()
            raise RuntimeError('User Hit Control-C')
        # Set the sampling interval to the standard value
        # so the the DHT and HC-SRO4 device report at the right
        # time frame.
        # self.set_sampling_interval(19)
        # print(time.clock())

    def _serial_monitor(self):
        while True:
            plist = list(serial.tools.list_ports.comports())
            ports = []
            for port in plist:
                ports.append(list(port)[0])

            if self.com_port.port not in ports:
                print(self.com_port.port+" lost, quit process")
                self.quit()
            time.sleep(1)

    def _exit_handler(self, signum, frame):
        self.shutdown()
        self.quit()

    def disconnect(self):
        self.shutdown()
        self.quit()

    def _find_arduino(self):
        """
        This method will search all potential serial ports for an Arduino
        containing a sketch that has a matching arduino_instance_id as
        specified in the input parameters of this class.

        This is used explicitly with the FirmataExpress sketch.
        """

        # a list of serial ports to be checked
        serial_ports = []

        print('Opening all potential serial ports...')
        the_ports_list = list_ports.comports()
        for port in the_ports_list:
            if port.pid is None:
                continue
            try:
                if self.name == "HANDPY":
                    if sys.platform == "win32":
                        self.serial_port = serial.Serial(self.com_port, self.baud_rate, dsrdtr = True,
                                             timeout=1)
                    else:
                        self.serial_port = serial.Serial(self.com_port, self.baud_rate, 
                                             timeout=1)
                else:
                    self.serial_port = serial.Serial(self.com_port, self.baud_rate,
                                             timeout=1)
            except SerialException:
                continue
            # create a list of serial ports that we opened
            serial_ports.append(self.serial_port)

            # display to the user
            print('\t' + port.device)

            # clear out any possible data in the input buffer
            self.serial_port.reset_input_buffer()
            self.serial_port.reset_output_buffer()

        # wait for arduino to reset
        # print('\nWaiting %d seconds(arduino_wait) for Arduino devices to reset...'%self.arduino_wait)
        try:
            time.sleep(self.arduino_wait)

            print(
                '\nSearching for an Arduino configured with an arduino_instance = {self.arduino_instance_id}')

            for serial_port in serial_ports:
                self.serial_port = serial_port
                # send the "are you there" sysex request to the arduino
                self._send_sysex(PrivateConstants.ARE_YOU_THERE)

                # wait until the END_SYSEX comes back
                i_am_here = self.serial_port.read_until(b'\xf7')
                if not i_am_here:
                    continue

                # make sure we get back the expected length
                if len(i_am_here) != 4:
                    continue

                # convert i_am_here to a list
                i_am_here = list(i_am_here)

                # check sysex command is I_AM_HERE
                if i_am_here[1] != PrivateConstants.I_AM_HERE:
                    continue
                else:
                    # got an I am here message - is it the correct ID?
                    if i_am_here[2] == self.arduino_instance_id:
                        self.com_port = self.serial_port
        except KeyboardInterrupt:
            raise RuntimeError('User Hit Control-C')

    def _manual_open(self):
        """
        Com port was specified by the user - try to open up that port

        """
        # if port is not found, a serial exception will be thrown
        try:
            print('[10] Opening %s...' % (self.com_port))
            if self.name == "HANDPY":                        #debug wsq handpy关闭串口复位问题,mac和win32下打开串口不同
                if sys.platform == "win32":
                    self.serial_port = serial.Serial(self.com_port, self.baud_rate, dsrdtr = True,
                                             timeout=1)
                else:
                    self.serial_port = serial.Serial(self.com_port, self.baud_rate, 
                                             timeout=1)
            else:
                self.serial_port = serial.Serial(self.com_port, self.baud_rate,
                                             timeout=1)
            time.sleep(2)
            # time.sleep(self.arduino_wait)
            print('[20] Waiting %d seconds(arduino_wait) for Arduino devices to reset...' % (
                self.arduino_wait))
            if self.baud_rate == 115200:
                self.serial_port.read(self.serial_port.in_waiting)
                time.sleep(0.01)
                self._send_sysex(PrivateConstants.ARE_YOU_THERE)
                # wait until the END_SYSEX comes back
                i_am_here = self.serial_port.read_until(b'\xf7')

                # convert i_am_here to a list
                i_am_here = list(i_am_here)
                if len(i_am_here) != 4:
                    raise RuntimeError('Invalid Arduino ID reply length')

                # check sysex command is I_AM_HERE
                if i_am_here[1] != PrivateConstants.I_AM_HERE:
                    raise RuntimeError('Retrieving ID From Arduino Failed.')
                else:
                    # got an I am here message - is it the correct ID?
                    if i_am_here[2] == self.arduino_instance_id:
                        return
                    else:
                        raise RuntimeError(
                            'Invalid Arduino identifier retrieved')
        except KeyboardInterrupt:
            raise RuntimeError('User Hit Control-C')

    def analog_read(self, pin):
        """
        Retrieve the last data update for the specified analog pin.

        :param pin: Analog pin number (ex. A2 is specified as 2)

        :returns: A list = [last value change,  time_stamp]
        """
        return self.analog_pins[pin].current_value

    def dht_read(self, pin):
        """
        Retrieve the last data update for the specified dht pin.

        :param pin: digital pin number

        :return: A list = [humidity, temperature  time_stamp]

                 ERROR CODES: If either humidity or temperature value:
                              == -1 Configuration Error
                              == -2 Checksum Error
                              == -3 Timeout Error

        """
        return self.digital_pins[pin].current_value[0], \
            self.digital_pins[pin].current_value[1]

    def dfrobot_dht_read(self, pin, type):
        data = [PrivateConstants.DHT_DATA, pin, type]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        time.sleep(0.1)

    def digital_read(self, pin):
        """
        Retrieve the last data update for the specified digital pin.

        :param pin: Digital pin number

        :returns: A list = [last value change,  time_stamp]

        """
        return self.digital_pins[pin].current_value

    def digital_pin_write(self, pin, value):
        """
        Set the specified pin to the specified value directly without port manipulation.

        :param pin: arduino pin number

        :param value: pin value

        """

        command = (PrivateConstants.SET_DIGITAL_PIN_VALUE, pin, value)

        self._send_command(command)

    def digital_write(self, pin, value):
        """
        Set the specified pin to the specified value.

        :param pin: arduino pin number

        :param value: pin value (1 or 0)

        """
        # The command value is not a fixed value, but needs to be calculated
        # using the pin's port number
        port = pin // 8

        calculated_command = PrivateConstants.DIGITAL_MESSAGE + port
        mask = 1 << (pin % 8)
        # Calculate the value for the pin's position in the port mask
        if value == 1:
            PrivateConstants.DIGITAL_OUTPUT_PORT_PINS[port] |= mask
        else:
            PrivateConstants.DIGITAL_OUTPUT_PORT_PINS[port] &= ~mask

        # Assemble the command
        command = (calculated_command,
                   PrivateConstants.DIGITAL_OUTPUT_PORT_PINS[port] & 0x7f,
                   (PrivateConstants.DIGITAL_OUTPUT_PORT_PINS[port] >> 7)
                   & 0x7f)

        self._send_command(command)

    def set_digital_pin_params(self, pin, mode, callback):
        self.digital_pins[pin].mode = mode
        self.digital_pins[pin].cb = callback

    def disable_analog_reporting(self, pin):
        """
        Disables analog reporting for a single analog pin.

        :param pin: Analog pin number. For example for A0, the number is 0.

        """
        pin = pin + self.first_analog_pin
        self.set_pin_mode_digital_input(pin)

    def disable_digital_reporting(self, pin):
        """
        Disables digital reporting. By turning reporting off for this pin,
        Reporting is disabled for all 8 bits in the "port"

        :param pin: Pin and all pins for this port

        """
        port = pin // 8
        command = [PrivateConstants.REPORT_DIGITAL + port,
                   PrivateConstants.REPORTING_DISABLE]
        self._send_command(command)

    def enable_analog_reporting(self, pin, callback=None, differential=1):
        """
        Enables analog reporting. This is an alias for set_pin_mode_analog_input.
        Disabling analog reporting sets the pin to a digital input pin,
        so we need to provide the callback and differential if we wish
        to specify it.

        :param pin: Analog pin number. For example for A0, the number is 0.

        :param callback: callback function

        :param differential: This value needs to be met for a callback
                             to be invoked.
        """
        self.set_pin_mode_analog_input(pin, callback, differential)

    def enable_digital_reporting(self, pin):
        """
        Enables digital reporting. By turning reporting on for all 8 bits
        in the "port" - this is part of Firmata's protocol specification.

        :param pin: Pin and all pins for this port

        :returns: No return value
            """
        port = pin // 8
        command = [PrivateConstants.REPORT_DIGITAL + port,
                   PrivateConstants.REPORTING_ENABLE]
        self._send_command(command)

    def get_analog_map(self):
        """
        This method requests a Firmata analog map query and returns the
        results.

        :returns: An analog map response or None if a timeout occurs
        """
        # get the current time to make sure a report is retrieved
        current_time = time.time()

        # if we do not have existing report results, send a Firmata
        # message to request one

        self._send_sysex(PrivateConstants.ANALOG_MAPPING_QUERY)
        # wait for the report results to return for 4 seconds
        # if the timer expires, return None
        while self.query_reply_data.get(
                PrivateConstants.ANALOG_MAPPING_RESPONSE) is None:
            elapsed_time = time.time()
            if elapsed_time - current_time > 4:
                return None
            # time.sleep(self.sleep_tune)
            time.sleep(.01)
        return self.query_reply_data.get(PrivateConstants.ANALOG_MAPPING_RESPONSE)

    def get_capability_report(self):
        """
        This method requests and returns a Firmata capability query report

        :returns: A capability report in the form of a list
        """

        self._send_sysex(PrivateConstants.CAPABILITY_QUERY)
        while self.query_reply_data.get(
                PrivateConstants.CAPABILITY_RESPONSE) is None:
            time.sleep(self.sleep_tune)
        return self.query_reply_data.get(PrivateConstants.CAPABILITY_RESPONSE)
    
    def set_flag(self):
        self._send_sysex(0x48)
    
    def get_firmware_version(self):
        """
        This method retrieves the Firmata firmware version

        :returns: Firmata firmware version
        """
        self._send_sysex(PrivateConstants.REPORT_FIRMWARE)

        current_time = time.time()
        while self.query_reply_data.get(PrivateConstants.REPORT_FIRMWARE) == '':
            elapsed_time = time.time()
            if elapsed_time - current_time > 4:
                return None
            time.sleep(self.sleep_tune)
        return self.query_reply_data.get(PrivateConstants.REPORT_FIRMWARE)

    def get_protocol_version(self):
        """
        This method returns the major and minor values for the protocol
        version, i.e. 2.5

        :returns: Firmata protocol version
        """
        self._send_command([PrivateConstants.REPORT_VERSION])
        while self.query_reply_data.get(
                PrivateConstants.REPORT_VERSION) == '':
            time.sleep(self.sleep_tune)
        # v_major =
        return self.query_reply_data.get(PrivateConstants.REPORT_VERSION)

    def get_pin_state(self, pin):
        """
        This method retrieves a pin state report for the specified pin.
        Pin modes reported:

        INPUT   = 0x00  # digital input mode

        OUTPUT  = 0x01  # digital output mode

        ANALOG  = 0x02  # analog input mode

        PWM     = 0x03  # digital pin in PWM output mode

        SERVO   = 0x04  # digital pin in Servo output mode

        I2C     = 0x06  # pin included in I2C setup

        STEPPER = 0x08  # digital pin in stepper mode

        PULLUP  = 0x0b  # digital pin in input pullup mode

        SONAR   = 0x0c  # digital pin in SONAR mode

        TONE    = 0x0d  # digital pin in tone mode

        :param pin: Pin of interest

        :returns: pin state report

        """
        # place pin in a list to keep _send_sysex happy
        self._send_sysex(PrivateConstants.PIN_STATE_QUERY, [pin])
        while self.query_reply_data.get(
                PrivateConstants.PIN_STATE_RESPONSE) is None:
            time.sleep(self.sleep_tune)
        pin_state_report = self.query_reply_data.get(
            PrivateConstants.PIN_STATE_RESPONSE)
        self.query_reply_data[PrivateConstants.PIN_STATE_RESPONSE] = None
        return pin_state_report

    # noinspection PyMethodMayBeStatic
    def get_pymata_version(self):
        """
        This method retrieves the PyMata Express version number

        :returns: PyMata Express version number.
        """
        return PrivateConstants.PYMATA_EXPRESS_THREADED_VERSION

    def i2c_scan(self, callback=None):
        if callback is None:
            if not self.iic_async_lock.locked():
                self.iic_async_lock.acquire()
        data = [PrivateConstants.SUB_MESSAGE_I2CSCAN]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        if callback is None:
            self.iic_async_lock.acquire(blocking=True,  timeout=3)
            with self.the_i2c_map_lock:
                return self.i2c_map.get('scan')

    def i2c_read_saved_data(self, address):
        """
        This method retrieves cached i2c data to support a polling mode.

        :param address: I2C device address

        :returns data: [raw data returned from i2c device, time-stamp]

        """
        if address in self.i2c_map:
            with self.the_i2c_map_lock:
                map_entry = self.i2c_map.get(address)
                return map_entry.get('value')
        else:
            return None

    def i2c_addr_read(self, address, number_of_bytes, callback=None):
        if callback == None:
            if not self.iic_async_lock.locked():
                self.iic_async_lock.acquire()
        self._i2c_addr_read_request(address, number_of_bytes,
                                    PrivateConstants.I2C_READ | PrivateConstants.I2C_END_TX_MASK, callback)
        if callback == None:
            self.iic_async_lock.acquire(blocking=True,  timeout=2)
            # self.iic_async_lock.acquire()
            return self.i2c_read_saved_data(address)

    def _i2c_addr_read_request(self, address, number_of_bytes, read_type,
                               callback=None):
        if address not in self.i2c_map:
            with self.the_i2c_map_lock:
                self.i2c_map[address] = {'value': None, 'callback': callback}
        data = [address, read_type, number_of_bytes &
                0x7f, (number_of_bytes >> 7) & 0x7f]
        self._send_sysex(PrivateConstants.I2C_REQUEST, data)

    def i2c_read(self, address, register, number_of_bytes,
                 callback=None):
        """
        Read the specified number of bytes from the specified register for
        the i2c device.


        :param address: i2c device address

        :param register: i2c register

        :param number_of_bytes: number of bytes to be read

        :param callback: Optional callback function to report i2c data as a
                   result of read command


        callback returns a data list:

        [pin_type, i2c_device_address, i2c_read_register, data_bytes returned, time_stamp]

        The pin_type for i2c = 6

        """
        if not callback:
            if not self.iic_async_lock.locked():
                self.iic_async_lock.acquire()
        self._i2c_read_request(address, register, number_of_bytes,
                               PrivateConstants.I2C_READ, callback)
        if callback is None:
            self.iic_async_lock.acquire(blocking=True,  timeout=2)
            # self.iic_async_lock.acquire()
            return self.i2c_read_saved_data(address)

    def i2c_read_continuous(self, address, register, number_of_bytes,
                            callback=None):
        """
        Some i2c devices support a continuous streaming data output.
        This command enables that mode for the device that supports
        continuous reads.


        :param address: i2c device address

        :param register: i2c register

        :param number_of_bytes: number of bytes to be read

        :param callback: Optional callback function to report i2c data as a
                   result of read command


        callback returns a data list:

        [pin_type, i2c_device_address, i2c_read_register, data_bytes returned, time_stamp]

        The pin_type for i2c = 6


        """
        if callback is None:
            if not self.iic_async_lock.locked():
                self.iic_async_lock.acquire()
        self._i2c_read_request(address, register, number_of_bytes,
                               PrivateConstants.I2C_READ_CONTINUOUSLY,
                               callback)
        if callback is None:
            self.iic_async_lock.acquire(blocking=True,  timeout=2)
            # self.iic_async_lock.acquire()
            return self.i2c_read_saved_data(address)

    def i2c_read_restart_transmission(self, address, register,
                                      number_of_bytes,
                                      callback=None):
        """
        Read the specified number of bytes from the specified register for
        the i2c device. This restarts the transmission after the read. It is
        required for some i2c devices such as the MMA8452Q accelerometer.


        :param address: i2c device address

        :param register: i2c register

        :param number_of_bytes: number of bytes to be read

        :param callback: Optional callback function to report i2c data as a
                   result of read command


        callback returns a data list:

        [pin_type, i2c_device_address, i2c_read_register, data_bytes returned, time_stamp]

        The pin_type for i2c pins = 6

        """
        if callback is None:
            if not self.iic_async_lock.locked():
                self.iic_async_lock.acquire()

        self._i2c_read_request(address, register, number_of_bytes,
                               PrivateConstants.I2C_READ
                               | PrivateConstants.I2C_END_TX_MASK,
                               callback)
        if callback is None:
            self.iic_async_lock.acquire(blocking=True,  timeout=2)
            # self.iic_async_lock.acquire()
            return self.i2c_read_saved_data(address)

    def _i2c_read_request(self, address, register, number_of_bytes, read_type,
                          callback=None):
        """
        This method requests the read of an i2c device. Results are retrieved
        by a call to i2c_get_read_data(). or by callback.

        If a callback method is provided, when data is received from the
        device it will be sent to the callback method.

        Some devices require that transmission be restarted
        (e.g. MMA8452Q accelerometer).

        I2C_READ | I2C_END_TX_MASK values for the read_type in those cases.

        I2C_READ = 0B00001000

        I2C_READ_CONTINUOUSLY = 0B00010000

        I2C_STOP_READING = 0B00011000

        I2C_END_TX_MASK = 0B01000000

        :param address: i2c device address

        :param register: register number (can be set to zero)

        :param number_of_bytes: number of bytes expected to be returned

        :param read_type: I2C_READ  or I2C_READ_CONTINUOUSLY. I2C_END_TX_MASK
                          may be OR'ed when required

        :param callback: Optional callback function to report i2c data as a
                   result of read command

        """
        #f0 76 50 08 00 02 00 02 f7   从寄存器xxx读取
        #f0 76 50 08 01 00 f7         读取一个字节
        if address not in self.i2c_map:#建议将上两条命令合并成一条
            with self.the_i2c_map_lock:
                self.i2c_map[address] = {'value': None, 'callback': callback}
        if isinstance(register,(list)):#16 bits 寄存器
          hi = register[0]
          lo = register[1]
          data= [address, read_type, hi&0x7F, (hi>>7)&0x7F, lo&0x7F, (lo>>7)&0x7F]
          self._send_sysex(PrivateConstants.I2C_REQUEST, data)
          data= [address, read_type, number_of_bytes & 0x7f, (number_of_bytes >> 7) & 0x7f]
          print("ret=list:",data)
          self._send_sysex(PrivateConstants.I2C_REQUEST, data)
        else:
          data = [address, read_type, register & 0x7f, (register >> 7) & 0x7f,
                number_of_bytes & 0x7f, (number_of_bytes >> 7) & 0x7f]
          self._send_sysex(PrivateConstants.I2C_REQUEST, data)

    def i2c_write(self, address, args):
        """
        Write data to an i2c device.

        :param address: i2c device address

        :param args: A variable number of bytes to be sent to the device
                     passed in as a list

        """
        '''if args[0] > 255:
          reg_hi = args[0]>>8;
          reg_lo = args[0]&0xff;
          #f0 76 50 00（表示iic写） 00 00 00 00（用于存放寄存器地址） 05 01（向指定寄存器写入数据） f7
          data = [address,PrivateConstants.I2C_WRITE, reg_hi&0x7F, ((reg_hi>>7)&0x7F)+0x02, reg_lo&0x7F, ((reg_lo>>7)&0x7F) + 0x02]
          for item in args[1:]:
              item_lsb = item & 0x7f
              data.append(item_lsb)
              item_msb = (item >> 7) & 0x7f
              data.append(item_msb)
        else:'''
        data = [address, PrivateConstants.I2C_WRITE]
        for item in args:
            item_lsb = item & 0x7f
            data.append(item_lsb)
            item_msb = (item >> 7) & 0x7f
            data.append(item_msb)
        self._send_sysex(PrivateConstants.I2C_REQUEST, data)
        time.sleep(0.005) #如果不加延迟，Linux+GD32V不能正常写入GD32V，待解决
#        print("i2c write:",data)

    def uart_write(self, bus_num, vals):
        data = [PrivateConstants.UART_WRITE, bus_num, len(vals)*2]
        for item in vals:
            item_lsb = item & 0x7f
            data.append(item_lsb)
            item_msb = (item >> 7) & 0x7f
            data.append(item_msb)
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def uart_readchar(self, bus_num):
        if self.uart_any(bus_num):
            data = self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_UART_READ][bus_num].pop(0)
        else:
            data = None
        return data

    def uart_readline(self, bus_num, timeout = 0.5):
        buf = []
        if 0xa in self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_UART_READ][bus_num]:
            while True:
                value = self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_UART_READ][bus_num].pop(0)
                buf.append(value)
                if value == 0x0a:
                    break
            return buf
        else:
            return None        

    def uart_readall(self, bus_num):
        value = self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_UART_READ][bus_num]
        self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_UART_READ][bus_num]=[]
        return None if value == [] else value

    def uart_read(self, bus_num, n):
        data = []
        while self.uart_any(bus_num) and n:
            data.append(self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_UART_READ][bus_num].pop(0))
            n = n - 1
        return None if data == [] else data
            
    def uart_readinto(self, bus_num, buf, num):
        lens = len(buf)
        cnt = 0
        if num == -1:
            while self.uart_any(bus_num) and lens:
                buf[cnt] = self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_UART_READ][bus_num].pop(0)
                lens = lens - 1
                cnt = cnt + 1
        else:
            while self.uart_any(bus_num) and num:
                buf[cnt] = self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_UART_READ][bus_num].pop(0)
                num = num - 1
                cnt = cnt + 1

    def uart_any(self,bus_num):
        return len(self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_UART_READ][bus_num])

    def keep_alive(self, period=1, margin=.3):
        """
        This is a FirmataExpress feature.

        Periodically send a keep alive message to the Arduino.

        If the Arduino does not received a keep alive, the Arduino
        will physically reset itself.

        Frequency of keep alive transmission is calculated as follows:
        keep_alive_sent = period - margin

        :param period: Time period between keepalives. Range is 0-10 seconds.
                       0 disables the keepalive mechanism.

        :param margin: Safety margin to assure keepalives are sent before
                    period expires. Range is 0.1 to 0.9
        """
        if period < 0:
            period = 0
        if period > 10:
            period = 10
        self.period = period
        if margin < .1:
            margin = .1
        if margin > .9:
            margin = .9
        self.margin = margin
        self.period = period
        self.keep_alive_interval = [
            self.period & 0x7f, (self.period >> 7) & 0x7f]
        self._send_sysex(PrivateConstants.SAMPLING_INTERVAL,
                         self.keep_alive_interval)
        self.the_keep_alive_thread.start()

    def dfrobot_play_tone(self, pin_number, frequency, duration):
        # duration is specified
        if duration:
            data = [PrivateConstants.SUB_MESSAGE_SOUND,0x04, pin_number, frequency & 0x7f,
                    (frequency >> 7) & 0x7f,
                    duration & 0x7f, (duration >> 7) & 0x7f]

        else:
            data = [PrivateConstants.SUB_MESSAGE_SOUND,0x04, pin_number,
                    frequency & 0x7f, (frequency >> 7) & 0x7f, 0, 0]
        # turn off tone
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def play_tone(self, pin_number, frequency, duration):
        """

        This is FirmataExpress feature

        Play a tone at the specified frequency for the specified duration.

        :param pin_number: arduino pin number

        :param frequency: tone frequency in hz

        :param duration: duration in milliseconds

        """
        self._play_tone(pin_number, PrivateConstants.TONE_TONE, frequency=frequency,
                        duration=duration)

    def play_tone_continuously(self, pin_number, frequency):
        """

        This is a FirmataExpress feature

        This method plays a tone continuously until play_tone_off is called.

        :param pin_number: arduino pin number

        :param frequency: tone frequency in hz

        """

        self._play_tone(pin_number, PrivateConstants.TONE_TONE, frequency=frequency,
                        duration=None)

    def play_tone_off(self, pin_number):
        """
        This is a FirmataExpress Feature

        This method turns tone off for the specified pin.
        :param pin_number: arduino pin number

        """

        self._play_tone(pin_number, PrivateConstants.TONE_NO_TONE,
                        frequency=None, duration=None)

    def _play_tone(self, pin, tone_command, frequency, duration):
        """
        This method will call the Tone library for the selected pin.
        It requires FirmataExpress to be loaded onto the arduino

        If the tone command is set to TONE_TONE, then the specified
        tone will be played.

        Else, if the tone command is TONE_NO_TONE, then any currently
        playing tone will be disabled.

        :param pin: arduino pin number

        :param tone_command: Either TONE_TONE, or TONE_NO_TONE

        :param frequency: Frequency of tone

        :param duration: Duration of tone in milliseconds

        """
        # convert the integer values to bytes
        if tone_command == PrivateConstants.TONE_TONE:
            # duration is specified
            if duration:
                data = [tone_command, pin, frequency & 0x7f,
                        (frequency >> 7) & 0x7f,
                        duration & 0x7f, (duration >> 7) & 0x7f]

            else:
                data = [tone_command, pin,
                        frequency & 0x7f, (frequency >> 7) & 0x7f, 0, 0]
        # turn off tone
        else:
            data = [tone_command, pin]
        self._send_sysex(PrivateConstants.TONE_DATA, data)
    #协议不一致，所以用了2个pwm_write
    def dfrobot_pwm_write(self, pin, freq_value, duty_value):
        if PrivateConstants.PWM_MESSAGE + pin < 0xf0:
            command = [PrivateConstants.PWM_MESSAGE + pin,freq_value & 0x7f, (duty_value >> 7) & 0x7f,duty_value & 0x7f]
            self._send_command(command)
        else:
            self._pwm_write_extended(pin, duty_value)
    
    def pwm_write(self, pin, freq_value, duty_value):
        """
        Set the selected pwm pin to the specified value.

        :param pin: PWM pin number

        :param value: Pin value (0 - 0x4000)

        """
        # 32（duty的0~6位）01（duty的7位）32（fre的0~6位）08（fre的7~10位）频率控制在1khz以内
        if PrivateConstants.PWM_MESSAGE + pin < 0xf0:
            command = [PrivateConstants.PWM_MESSAGE + pin, duty_value & 0x7f,
                       (freq_value >> 7) & 0x7f, freq_value & 0x7f]
            self._send_command(command)
        else:
            self._pwm_write_extended(pin, duty_value)

    def _pwm_write_extended(self, pin, data, freq=100):
        """
        This method will send an extended-data analog write command to the
        selected pin.

        :param pin: 0 - 127

        :param data: 0 - 0xfffff

        :returns: No return value
        """
        #解决行空板舵机bug6431 更改
        pwm_data = [pin, data & 0x7f, (data >> 7) & 0x7f, (data >> 14) & 0x7f, freq & 0x7f, (freq >> 7) & 0x7f] 
        self._send_sysex(PrivateConstants.EXTENDED_PWM, pwm_data)

    def send_reset(self):              #软件软复位
        """
        Send a Sysex reset command to the arduino

        """
        try:
            self._send_command([PrivateConstants.SYSTEM_RESET])
        except RuntimeError:
            raise RuntimeError('send_reset exception')

    def set_pin_mode_analog_input(self, pin_number, callback=None,
                                  differential=1):
        """
        Set a pin as an analog input.

        :param pin_number: arduino pin number

        :param callback: callback function

        :param differential: This value needs to be met for a callback
                             to be invoked.


        callback returns a data list:

        [pin_type, pin_number, pin_value, raw_time_stamp]

        The pin_type for analog input pins = 2

        """
        self._set_pin_mode(pin_number, PrivateConstants.ANALOG,
                           callback=callback,
                           differential=differential)

    def set_pin_analog_input(self, pin_number, callback=None):

        command = [PrivateConstants.SET_PIN_MODE,
                   pin_number, PrivateConstants.ANALOG]

        self._send_command(command)

    def set_pin_mode_ir_recv(self, pin_number, callback=None):
        # 设置引脚模式：f0 0D 00（表示红外） 01（表示解码） 02 （表示引脚）f7
        data = [PrivateConstants.SUB_MESSAGE_IR, 0x01, pin_number]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        if callback:
            self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_IR][pin_number] = callback
        else:
            self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_IR][pin_number] = deque(
            )

    def set_pin_mode_DS18B20(self, pin_number, callback=None):
        if callback:
            self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_18B20][pin_number] = callback
        else:
            self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_18B20][pin_number] = 0.0

    def set_pin_mode_dht(self, pin_number, sensor_type=22, differential=.1, callback=None):
        """
        Configure a DHT sensor prior to operation.
        Up to 6 DHT sensors are supported

        :param pin_number: digital pin number on arduino.

        :param sensor_type: type of dht sensor
                            Valid values = DHT11, DHT12, DHT22, DHT21, AM2301

        :param differential: This value needs to be met for a callback
                             to be invoked.

        :param callback: callback function

        callback: returns a data list:

        [pin_type, pin_number, DHT type, humidity value, temperature raw_time_stamp]

        The pin_type for DHT input pins = 15

                ERROR CODES: If either humidity or temperature value:
                              == -1 Configuration Error
                              == -2 Checksum Error
                              == -3 Timeout Error
        """

        # if the pin is not currently associated with a DHT device
        # initialize it.
        if pin_number not in self.dht_list:
            self.dht_list.append(pin_number)
            self.digital_pins[pin_number].cb = callback
            self.digital_pins[pin_number].current_value = [0, 0]
            self.digital_pins[pin_number].differential = differential
            data = [pin_number, sensor_type]
            self._send_sysex(PrivateConstants.DHT_CONFIG, data)
        else:
            # allow user to change the differential value
            self.digital_pins[pin_number].differential = differential

    def set_pin_mode_digital_input(self, pin_number, callback=None):
        """
        Set a pin as a digital input.

        :param pin_number: arduino pin number

        :param callback: callback function


        callback returns a data list:

        [pin_type, pin_number, pin_value, raw_time_stamp]

        The pin_type for digital input pins = 0

        """
        self._set_pin_mode(pin_number, PrivateConstants.INPUT, callback)

    def set_pin_mode_digital_input_pullup(self, pin_number, callback=None):
        """
        Set a pin as a digital input with pullup enabled.

        :param pin_number: arduino pin number

        :param callback: callback function


        callback returns a data list:

        [pin_type, pin_number, pin_value, raw_time_stamp]

        The pin_type for digital input pins with pullups enabled = 11

        """
        self._set_pin_mode(pin_number, PrivateConstants.PULLUP, callback)

    def set_pin_mode_digital_output(self, pin_number):
        """
        Set a pin as a digital output pin.

        :param pin_number: arduino pin number
        """

        self._set_pin_mode(pin_number, PrivateConstants.OUTPUT)

    # noinspection PyIncorrectDocstring
    def set_pin_mode_i2c(self, read_delay_time=0):
        """
        Establish the standard Arduino i2c pins for i2c utilization.

        NOTE: THIS METHOD MUST BE CALLED BEFORE ANY I2C REQUEST IS MADE
        This method initializes Firmata for I2c operations.

        :param read_delay_time (in microseconds): an optional parameter,
                                                  default is 0

        NOTE: Callbacks are set within the individual i2c read methods of this
              API.
              See i2c_read, i2c_read_continuous, or i2c_read_restart_transmission.

        """
        data = [read_delay_time & 0x7f, (read_delay_time >> 7) & 0x7f]
        self._send_sysex(PrivateConstants.I2C_CONFIG, data)

    def set_pin_mode_pwm_output(self, pin_number):
        """
        Set a pin as a pwm (analog output) pin.

        :param pin_number:arduino pin number

        """
        self._set_pin_mode(pin_number, PrivateConstants.PWM)
    #两个都是设置舵机模式，一个是DF私有的，一个是标准的
    def set_mode_servo(self, pin_number):
        command = [PrivateConstants.SET_PIN_MODE, pin_number,
                   PrivateConstants.SERVO]
        self._send_command(command)

    def set_pin_mode_servo(self, pin, min_pulse=544, max_pulse=2400):
        """
        Configure a pin as a servo pin. Set pulse min, max in ms.

        :param pin: Servo Pin.

        :param min_pulse: Min pulse width in ms.

        :param max_pulse: Max pulse width in ms.

        """
        command = [pin, min_pulse & 0x7f, (min_pulse >> 7) & 0x7f,
                   max_pulse & 0x7f,
                   (max_pulse >> 7) & 0x7f]
        self._send_sysex(PrivateConstants.SERVO_CONFIG, command)

    def set_pin_mode_sonar(self, trigger_pin, echo_pin,
                           callback=None, timeout=80000):
        """
        This is a FirmataExpress feature.

        Configure the pins,ping interval and maximum distance for an HC-SR04
        type device.

        Up to a maximum of 6 SONAR devices is supported.
        If the maximum is exceeded a message is sent to the console and the
        request is ignored.

        NOTE: data is measured in centimeters. Callback is called only when the
              the latest value received is different than the previous.

        :param trigger_pin: The pin number of for the trigger (transmitter).

        :param echo_pin: The pin number for the received echo.

        :param callback: optional callback function to report sonar data changes

        :param timeout: a tuning parameter. 80000UL equals 80ms.


        callback returns a data list:

        [pin_type, trigger_pin_number, distance_value (in cm), raw_time_stamp]

        The pin_type for sonar pins = 12


        """
        # if there is an entry for the trigger pin in existence,
        # ignore the duplicate request.
        if trigger_pin in self.active_sonar_map:
            return

        timeout_lsb = timeout & 0x7f
        timeout_msb = (timeout >> 7) & 0x7f
        data = [trigger_pin, echo_pin, timeout_lsb,
                timeout_msb]

#        self._set_pin_mode(trigger_pin, PrivateConstants.SONAR,
#                           PrivateConstants.INPUT)
#        time.sleep(0.1)
#        self._set_pin_mode(echo_pin, PrivateConstants.SONAR,
#                           PrivateConstants.INPUT)
#        time.sleep(0.1)

        # update the ping data map for this pin
        if len(self.active_sonar_map) > 6:
            print('sonar_config: maximum number of devices assigned'
                  ' - ignoring request')
        else:
            # initialize map entry with callback, data value of 0 and time_stamp of 0
            self.active_sonar_map[trigger_pin] = [callback, 0, 0]

        self._send_sysex(PrivateConstants.SONAR_CONFIG, data)
    
    def dfrobot_set_pin_mode_sonar(self, trigger_pin, echo_pin,
                           callback=None, timeout=80000):
        data = [PrivateConstants.SONAR_DATA,trigger_pin, echo_pin]
        if len(self.active_sonar_map) > 6:
            print('sonar_config: maximum number of devices assigned'
                  ' - ignoring request')
        else:
            # initialize map entry with callback, data value of 0 and time_stamp of 0
            self.active_sonar_map[trigger_pin] = [callback, 0, 0]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def dfrobot_sonar_read(self, trigger_pin, echo_pin):
        data = [PrivateConstants.SONAR_DATA,trigger_pin, echo_pin]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def set_pin_mode_stepper(self, steps_per_revolution, stepper_pins):
        """
        This is a FirmataExpress feature.

        Configure stepper motor prior to operation.

        NOTE: Single stepper only. Multiple steppers not supported.

        :param steps_per_revolution: number of steps per motor revolution

        :param stepper_pins: a list of control pin numbers - either 4 or 2

        """
        data = [PrivateConstants.STEPPER_CONFIGURE,
                steps_per_revolution & 0x7f,
                (steps_per_revolution >> 7) & 0x7f]
        for pin in range(len(stepper_pins)):
            data.append(stepper_pins[pin])
        self._send_sysex(PrivateConstants.STEPPER_DATA, data)

    def set_pin_mode_tone(self, pin_number):
        """
        This is FirmataExpress feature.

        Set a PWM pin to tone mode.

        :param pin_number: arduino pin number

        """
        command = [PrivateConstants.SET_PIN_MODE, pin_number,
                   PrivateConstants.TONE]
        self._send_command(command)

    def set_pin_mode_neo(self, pin_number):
        command = [PrivateConstants.SET_PIN_MODE, pin_number,
                   PrivateConstants.NEOPIXEL]
        self._send_command(command)

    def set_pin_mode_uart(self, bus_number, baud_rate, bits = 8, parity = 0, stop = 1):
        #f0 0d（dfr私有协议标志） 0c（表示uart初始化） 00(bus_num) 00（A） 00（B） 25(C) 00 （D）00 (E)01（F） （组合成为波特率）XX (预留一个字节作为8N1的标志位，此处可填可不填)f7
        #组合方式：波特率=(A&0X7F |  B<<7)<<16 |  (C&0X7F |  D<<7)<<8 | (E&0X7F |  F<<7)
        hi = (baud_rate>>16) % 0xFF
        mi = (baud_rate>>8) & 0xFF
        lo = baud_rate&0xFF
        data = [PrivateConstants.UART_CONFIG, bus_number,
                hi&0x7F, (hi>>7 & 0x7F),mi&0x7F, (mi>>7 & 0x7F),
                lo&0x7F, (lo>>7 & 0x7F)]
        data.append(bits)
        data.append(parity)
        data.append(stop)
        self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_UART_READ][bus_number] = []
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def uart_deinit(self, bus_number):
        data = [PrivateConstants.UART_DEINIT, bus_number]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def set_pin_mode_spi(self, device_num, cs):
        data = [PrivateConstants.SUB_MESSAGE_SPI_INIT, device_num, cs]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def spi_write(self, device_num, buf):
        lens = len(buf)
        data = [PrivateConstants.SUB_MESSAGE_SPI_SENDTO, device_num, lens]
        for i in buf:
            data.append(i)
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def spi_read(self, device_num, lens):
        data = [PrivateConstants.SUB_MESSAGE_SPI_RECVFROM, device_num, lens]
        if not self.spi_async_lock.locked():
            self.spi_async_lock.acquire()
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        self.spi_async_lock.acquire(blocking=True,  timeout=2)
        temp = self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_SPI][device_num]
        return temp

    def _set_pin_mode(self, pin_number, pin_state, callback=None,
                      differential=1):
        """
        A private method to set the various pin modes.

        :param pin_number: arduino pin number

        :param pin_state: INPUT/OUTPUT/ANALOG/PWM/PULLUP
                         For SERVO use: set_pin_mode_servo
                         For DHT   use: set_pin_mode_dht

        :param callback: A reference to a call back function to be
                         called when pin data value changes

        :param differential: This value needs to be met for a callback
                             to be invoked

        """
        if callback:
            if pin_state == PrivateConstants.INPUT:
                self.digital_pins[pin_number].cb = callback
            elif pin_state == PrivateConstants.PULLUP:
                self.digital_pins[pin_number].cb = callback
                self.digital_pins[pin_number].pull_up = True
            elif pin_state == PrivateConstants.ANALOG:
                self.analog_pins[pin_number].cb = callback
                self.analog_pins[pin_number].differential = differential
            else:
                print('{} {}'.format('set_pin_mode: callback ignored for '
                                     'pin state:', pin_state))

        pin_mode = pin_state

        if pin_mode == PrivateConstants.ANALOG:
            pin_number = pin_number + self.first_analog_pin

        command = [PrivateConstants.SET_PIN_MODE, pin_number, pin_mode]

        self._send_command(command)

        if pin_state == PrivateConstants.INPUT or pin_state == PrivateConstants.PULLUP:
            self.enable_digital_reporting(pin_number)
        else:
            pass

    def set_sampling_interval(self, interval):
        """
        This method sends the desired sampling interval to Firmata.

        Note: Standard Firmata  will ignore any interval less than
              10 milliseconds

        :param interval: Integer value for desired sampling interval
                         in milliseconds

        """
        data = [interval & 0x7f, (interval >> 7) & 0x7f]
        self._send_sysex(PrivateConstants.SAMPLING_INTERVAL, data)
    #一个是DF私有的舵机写，一个是标准的舵机写
    def dfrobot_servo_write(self, pin, position):

        data = [PrivateConstants.SERVO_CONTROL,
                pin, (position >> 7) & 0x7f, position & 0x7f]

        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def servo_write(self, pin, position):
        """
        This is an alias for analog_write to set
        the position of a servo that has been
        previously configured using set_pin_mode_servo.

        :param pin: arduino pin number

        :param position: servo position

        """
        # position  duty
        # freq=50 duty是用position算出来的
        # 彭开兴用了一个单独的定时器控制舵机，应该设计了独立的api，不用再和pwm_write混用
        # 32（duty的0~6位）01（duty的7位）32（fre的0~6位）08（fre的7~10位）频率控制在1khz以内
        if PrivateConstants.SERVO_MESSAGE + pin < 0xf0:
            command = [PrivateConstants.SERVO_MESSAGE + pin, 50 & 0x7f,
                       (position >> 7) & 0x7f, position & 0x7f]
            self._send_command(command)
        else:
            self._pwm_write_extended(pin, position, 50)

    def quit(self):
        print("user quit process")
        if not self.shutdown_flag:
            self._stop_threads()

        if self.serial_port:
            self.serial_port.close()
            self.serial_port = None
        #os.kill(os.getpid(), signal.SIGINT) #注释掉这行，换为sys.exit(0)，保证系统正常退出，逻辑上有问题，待改进
        sys.exit(0)
        
    def shutdown(self):
        """
        This method attempts an orderly shutdown
        If any exceptions are thrown, they are ignored.
        """
        self._send_sysex(0x49)
        self.shutdown_flag = True
        self._stop_threads()

        try:
            if not self.ip_address:
                # stop all reporting - both analog and digital
                for pin in range(len(self.analog_pins)):
                    self.disable_analog_reporting(pin)

                for pin in range(len(self.digital_pins)):
                    self.disable_digital_reporting(pin)
                self.send_reset()
                self.serial_port.reset_input_buffer()
                self.serial_port.close()
                self.serial_port = None
            else:
                self.sock.close()
        except (RuntimeError, SerialException, OSError):
            # ignore error on shutdown
            pass

    def sonar_read(self, trigger_pin):
        """
        This is a FirmataExpress feature

        Retrieve Ping (HC-SR04 type) data. The data is presented as a
        dictionary.

        The 'key' is the trigger pin specified in sonar_config()
        and the 'data' is the current measured distance (in centimeters)
        for that pin. If there is no data, the value is set to None.

        :param trigger_pin: key into sonar data map

        :returns: A list = [last value, raw time_stamp]

        """

        sonar_pin_entry = self.active_sonar_map.get(trigger_pin)
        if sonar_pin_entry:
            return [sonar_pin_entry[1], sonar_pin_entry[2]]
        else:
            return[0, 0]

    def stepper_write(self, motor_speed, number_of_steps):
        """
        This is a FirmataExpress feature

        Move a stepper motor for the number of steps at the specified speed.

        :param motor_speed: 21 bits of data to set motor speed

        :param number_of_steps: 14 bits for number of steps & direction
                                positive is forward, negative is reverse

        """
        if number_of_steps > 0:
            direction = 1
        else:
            direction = 0
        abs_number_of_steps = abs(number_of_steps)
        data = [PrivateConstants.STEPPER_STEP, motor_speed & 0x7f,
                (motor_speed >> 7) & 0x7f, (motor_speed >> 14) & 0x7f,
                abs_number_of_steps & 0x7f, (abs_number_of_steps >> 7) & 0x7f,
                direction]
        self._send_sysex(PrivateConstants.STEPPER_DATA, data)

    def _report_sensor(self):
        data = [PrivateConstants.SUB_REPORT_FLAG, 0x01]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        self.report = True
        time.sleep(0.1)

    def set_audio_init(self, strobe_pin, RST_pin, DC_pin):
        self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_18B20][PrivateConstants.AUDIO_VALUE] = [
            0 for i in range(7)]
        data = [PrivateConstants.SUB_MESSAGE_AUDIO,
                PrivateConstants.AUDIO_INIT, strobe_pin, RST_pin, DC_pin]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def set_hx711_init(self, dout_pin, sck_pin, scale):
        self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_HX711][dout_pin] = 0.0
        data = [PrivateConstants.SUB_MESSAGE_HX711, PrivateConstants.HX711_INIT,
                dout_pin, sck_pin, (scale >> 7) & 0x7f, scale & 0x7f]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def heartrate_init(self, pin):
        self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_HEARTRATE][pin] = 0

    def hx711_read_weight(self, dout_pin):
        data = [PrivateConstants.SUB_MESSAGE_HX711,
                PrivateConstants.HX711_READ]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        time.sleep(0.3)
        value = self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_HX711][dout_pin]
        return round(value, 2)

    def get_heartrate_value(self, mode, pin):
        data = [PrivateConstants.SUB_MESSAGE_HEARTRATE, pin, mode]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        value = self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_HEARTRATE][pin]
        return value

    def audio_analyzer_read_freq(self):
        data = [PrivateConstants.SUB_MESSAGE_AUDIO,
                PrivateConstants.AUDIO_read]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        time.sleep(0.3)
        return self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_AUDIO][PrivateConstants.AUDIO_VALUE]

    def ds18b20_read(self, pin_number):
        data = [PrivateConstants.SUB_MESSAGE_18B20,
                PrivateConstants.DS18B20_READ, pin_number]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        time.sleep(0.2)
        temp = self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_18B20][pin_number]
        if temp == 85:
            return 0
        else:
            return temp
    
    def gp2y1010au0f_read(self, ana_pin, digpin):
        data = [PrivateConstants.SUB_MESSAGE_GP2Y1010AU0F, ana_pin, digpin]
        if not self.other_async_lock.locked():
            self.other_async_lock.acquire()
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        self.other_async_lock.acquire(blocking=True,  timeout=2)
        temp = self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_GP2Y1010AU0F][ana_pin]
        return temp

    def _set_ssid(self, data):
        buf = []
        text = str(data)
        text = text.encode(encoding='UTF-8',errors='strict')
        for i in text:
            buf.append(i)
        lens = len(buf)
        data = [PrivateConstants.SUB_MESSAGE_ssid, lens]
        for i in buf:
            data.append(i)
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        time.sleep(0.2)

    def _set_password(self, password):
        lens = len(password)
        data = [PrivateConstants.SUB_MESSAGE_password, lens]
        for i in password:
            data.append(ord(i))
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        time.sleep(0.2)

    def _connect_wifi(self):
        self.dfrobot_dispatch.get(PrivateConstants.SUB_MESSAGE_CONNECT_PESPOND)[
            "ip"] = None
        self.dfrobot_dispatch.get(PrivateConstants.SUB_MESSAGE_CONNECT_PESPOND)[
            "port"] = None
        data = [PrivateConstants.SUB_MESSAGE_CONNECT_WIFI]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        time.sleep(0.2)

    def _get_ip_port(self):
        ip = self.dfrobot_dispatch.get(
            PrivateConstants.SUB_MESSAGE_CONNECT_PESPOND)["ip"]
        port = self.dfrobot_dispatch.get(
            PrivateConstants.SUB_MESSAGE_CONNECT_PESPOND)["port"]
        return ip

    def neopixel_config(self, pin_number, n):
        data = [PrivateConstants.SUB_MESSAGE_NEOPIXEL,
                0x00, pin_number, n >> 7, n & 0x7F]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def neopixel_write(self, pin_number, index, color):
        self.neopixel_set_range_color(pin_number, index, index, color)

    def neopixel_set_range_color(self, pin_number, start, end, color):
        data = [PrivateConstants.SUB_MESSAGE_NEOPIXEL, 0x01, pin_number, start >> 7, start & 0x7F, end >>
                7, end & 0x7F, (color >> 21) & 0x7F, (color >> 14) & 0x7F, (color >> 7) & 0x7F, color & 0x7F]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def neopixel_set_rainbow(self, pin_number, start, end, hsv_start, hsv_end):
        data = [PrivateConstants.SUB_MESSAGE_NEOPIXEL, 0x03, pin_number, start >> 7, start & 0x7F, end >> 7, end & 0x7F, (hsv_start >> 21) & 0x7F, (
            hsv_start >> 14) & 0x7F, (hsv_start >> 7) & 0x7F, hsv_start & 0x7F, (hsv_end >> 21) & 0x7F, (hsv_end >> 14) & 0x7F, (hsv_end >> 7) & 0x7F, hsv_end & 0x7F]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def neopixel_shift(self, pin_number, n):
        data = [PrivateConstants.SUB_MESSAGE_NEOPIXEL, 0x04, pin_number, n]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def neopixel_rotate(self, pin_number, n):
        data = [PrivateConstants.SUB_MESSAGE_NEOPIXEL, 0x05, pin_number,  n]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def neopixel_set_brightness(self, pin_number, brightness):
        data = [PrivateConstants.SUB_MESSAGE_NEOPIXEL, 0x02, pin_number,  brightness]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def show_bar_graph(self, pin_number, start, end, numerator, denominator):
        data = [PrivateConstants.SUB_MESSAGE_NEOPIXEL, 0x06, pin_number,
                start >> 7, start & 0x7F, end >> 7, end & 0x7F, numerator, denominator]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def ir_read(self, pin_number=None):
        if pin_number is None:
            return None

        arg = self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_IR][pin_number]
        if isinstance(arg, types.FunctionType):
            return None

        if len(arg) == 0:
            return None

        return arg.popleft()

    def ir_send(self, pin_number, value):
        data = [PrivateConstants.SUB_MESSAGE_IR, 0x00, pin_number, (value >> 28) & 0x7F, (
            value >> 21) & 0x7F, (value >> 14) & 0x7F, (value >> 7) & 0x7F, value & 0x7F]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def handpy_buttonA_is_pressed(self):
        if self._report["buttonA"] == 1:
            return True
        else:
            return False

    def handpy_buttonB_is_pressed(self):
        if self._report["buttonB"] == 1:
            return True
        else:
            return False

    def handpy_buttonAB_is_pressed(self):
        if self._report["buttonB"] == 1 and self._report["buttonA"] == 1:
            return True
        else:
            return False

    def handpy_touchP(self):
        if self._report["touchP"]["touchNum"] != 0:
            return True
        else:
            return False

    def handpy_touchY(self):
        if self._report["touchY"]["touchNum"] != 0:
            return True
        else:
            return False
    
    def handpy_touchT(self):
        if self._report["touchT"]["touchNum"] != 0:
            return True
        else:
            return False
    
    def handpy_touchH(self):
        if self._report["touchH"]["touchNum"] != 0:
            return True
        else:
            return False

    def handpy_touchO(self):
        if self._report["touchO"]["touchNum"] != 0:
            return True
        else:
            return False
    
    def handpy_touchN(self):
        if self._report["touchN"]["touchNum"] != 0:
            return True
        else:
            return False

    def handpy_set_touch_threshold(self, obj, value):
        if obj == "ALL":
            self._report["touchThreshold"]["P"] = value
            self._report["touchThreshold"]["Y"] = value
            self._report["touchThreshold"]["T"] = value
            self._report["touchThreshold"]["H"] = value
            self._report["touchThreshold"]["O"] = value
            self._report["touchThreshold"]["N"] = value
        else:
            self._report["touchThreshold"][obj] = value

    def handpy_read_touch_P(self):
        return self._report["touchP"]["value"]

    def handpy_read_touch_Y(self):
        return self._report["touchY"]["value"]
    
    def handpy_read_touch_T(self):
        return self._report["touchT"]["value"]
    
    def handpy_read_touch_H(self):
        return self._report["touchH"]["value"]
    
    def handpy_read_touch_O(self):
        return self._report["touchO"]["value"]
    
    def handpy_read_touch_N(self):
        return self._report["touchN"]["value"]

    def handpy_read_sound(self):
        return self._report["microphone"]
    
    def handpy_read_light(self):
        return self._report["lightness"]

    def handpy_get_accelerometer_X(self):
        return self._report["accelerationX"]
    
    def handpy_get_accelerometer_Y(self):
        return self._report["accelerationY"]
    
    def handpy_get_accelerometer_Z(self):
        return self._report["accelerationZ"]
    
    def handpy_get_accelerometer_strength(self):
        return self._report["accelerationStrength"]
    
    def encode_8Bit_unsigned_integer_array(self, buf):
        data = []
        for value in buf:
            data.append(value & 0x7f)
            data.append((value >> 7) & 0x7f)
        return data

    def handpy_display_in_line(self, buf, line, length):
        header = [PrivateConstants.SUB_MESSAGE_OLED12864, 9]
        buf.insert(0, line)
        buf.insert(1, (length >> 8) & 0xff)
        buf.insert(2, length & 0xff)
        data = self.encode_8Bit_unsigned_integer_array(buf)
        data = header + data
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def handpy_display_clear_in_line(self, line):
        data = [PrivateConstants.SUB_MESSAGE_OLED12864, 10, line & 0x7f, (line >> 7) & 0x7f]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def handpy_display_in_XY(self, x, y, buf, length):
        header = [PrivateConstants.SUB_MESSAGE_OLED12864, 0]
        buf.insert(0, x)
        buf.insert(1, y)
        buf.insert(2, (length >> 8) & 0xff)
        buf.insert(3, length & 0xff)
        data = self.encode_8Bit_unsigned_integer_array(buf)
        data = header + data
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def handpy_fill_screen(self, color):
        data = [PrivateConstants.SUB_MESSAGE_OLED12864, 2, color & 0x7f, (color >> 7) & 0x7f]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def handpy_screen_rotation(self, angle):
        data = [PrivateConstants.SUB_MESSAGE_OLED12864, 3, angle & 0x7f, (angle >> 7) & 0x7f]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def handpy_point_drawing(self, x, y):
        data = [PrivateConstants.SUB_MESSAGE_OLED12864, 4, x & 0x7f, (x >> 7) & 0x7f, y & 0x7f, (y >> 7) & 0x7f]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def handpy_set_line_width(self, lineW):
        data = [PrivateConstants.SUB_MESSAGE_OLED12864, 5, lineW & 0x7f, (lineW >> 7) & 0x7f]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def handpy_line_drawing(self, x1, y1, x2, y2):
        header = [PrivateConstants.SUB_MESSAGE_OLED12864, 6]
        buf = [x1, y1, x2, y2]
        data = self.encode_8Bit_unsigned_integer_array(buf)
        data = header + data
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def handpy_circle_drawing(self, x, y, fill, r):
        header = [PrivateConstants.SUB_MESSAGE_OLED12864, 7]
        buf = [x, y, (r >> 24) & 0xff, (r >> 16) & 0xff, (r >> 8) & 0xff, r & 0xff, fill]
        data = self.encode_8Bit_unsigned_integer_array(buf)
        data = header + data
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def handpy_circle_drawing_fill(self, x, y, fill, r):
        header = [PrivateConstants.SUB_MESSAGE_OLED12864, 7]
        buf = [x, y, (r >> 24) & 0xff, (r >> 16) & 0xff, (r >> 8) & 0xff, r & 0xff, fill]
        data = self.encode_8Bit_unsigned_integer_array(buf)
        data = header + data
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def handpy_rectangle_drawing(self, x, y, width, height, fill):
        header = [PrivateConstants.SUB_MESSAGE_OLED12864, 8]
        buf = [x, y, width, height, fill]
        data = self.encode_8Bit_unsigned_integer_array(buf)
        data = header + data
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def handpy_rectangle_drawing_fill(self, x, y, width, height, fill):
        header = [PrivateConstants.SUB_MESSAGE_OLED12864, 8]
        buf = [x, y, width, height, fill]
        data = self.encode_8Bit_unsigned_integer_array(buf)
        data = header + data
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def handpy_set_lights_color(self, index, color):
        data = [PrivateConstants.SUB_MESSAGE_WS2812, 1, index, color & 0x7F, (color >> 7) & 0x7F, (color >> 14) & 0x7F, (color >> 21) & 0x7F, (color >> 28) & 0x7F]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def handpy_rgb_disable(self, index, color):
        data = [PrivateConstants.SUB_MESSAGE_WS2812, 1, index, color & 0x7F, (color >> 7) & 0x7F, (color >> 14) & 0x7F, (color >> 21) & 0x7F, (color >> 28) & 0x7F]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def handpy_set_brightness(self, brightness):
        data = [PrivateConstants.SUB_MESSAGE_WS2812, 4, brightness & 0x7f, (brightness >> 7) & 0x7f]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def handpy_get_brightness(self):
        return self._report["brightness"]
    
    def handpy_music_set_tempo(self, pin, ticks, tempo):
        data = [PrivateConstants.SUB_MESSAGE_SOUND, PrivateConstants.SOUND_SETTICKSTEMPO, pin, ticks & 0X7F, tempo & 0x7F]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def handpy_set_buzzer_freq(self, pin, note, beat):
        data = [PrivateConstants.SUB_MESSAGE_SOUND, PrivateConstants.SOUND_PLAYNOTE, pin, note & 0X7F, (note >> 7) & 0x7F, beat & 0x7F]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def handpy_stop_background_buzzer_freq(self):
        data = [PrivateConstants.SUB_MESSAGE_SOUND, 5]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def handpy_display_image(self, x, y, width, height, image, imgLen):
        header = [PrivateConstants.SUB_MESSAGE_OLED12864, 1]
        value = [x , y, width, height,  (imgLen >> 8) & 0xff, imgLen & 0xff]
        value = value + image
        buf = self.encode_8Bit_unsigned_integer_array(value)
        data = header + buf
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def handpy_oled12864_show(self, x, y , w, h):
        header = [PrivateConstants.SUB_MESSAGE_OLED12864, 11]
        buf = self.encode_8Bit_unsigned_integer_array([x, y, w, h])
        data = header + buf
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def microbit_show_shape(self, data):
        buf = [PrivateConstants.SUB_MESSAGE_MATRIX, PrivateConstants.MATRIX_SHOWBITMAP]
        data = buf + data
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def microbit_show_font(self, buf):
        data = []
        data.append(PrivateConstants.SUB_MESSAGE_MATRIX)
        data.append(PrivateConstants.MATRIX_SHOWSTRING)
        for i in buf:
            data.append(i & 0x7F)
            data.append((i >> 7) & 0x7f)
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def microbit_control_light_on(self, x, y, light):
        data = [PrivateConstants.SUB_MESSAGE_MATRIX, PrivateConstants.MATRIX_DRAWPIXEL, x, y , light]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def microbit_set_light_brightness(self, brightness):
        data = [PrivateConstants.SUB_MESSAGE_MATRIX, 0x06, brightness & 0x7f, (brightness >> 7) & 0x7f]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def microbit_set_pixel_brightness(self, mlist):
        temp = []
        for i in mlist:
            temp.append(int(i))
        data = [PrivateConstants.SUB_MESSAGE_MATRIX, 0x05,]
        data = data + temp
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def microbit_hide_all_lights(self):
        data = [PrivateConstants.SUB_MESSAGE_MATRIX, PrivateConstants.MATRIX_CLEAR]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def microbit_play_music_background(self, pin, sound, flag):
        data = [PrivateConstants.SUB_MESSAGE_SOUND, PrivateConstants.SOUND_PLAYSOUND, pin, sound, flag]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        while flag:
            if self.music:
                break

    def microbit_play_buzzer_freq(self, pin, note, beat):
        data = [PrivateConstants.SUB_MESSAGE_SOUND, PrivateConstants.SOUND_PLAYNOTE, pin, note & 0X7F, (note >> 7) & 0x7F, beat & 0x7F]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def microbit_set_speed(self, speed):
        data = [PrivateConstants.SUB_MESSAGE_SOUND,PrivateConstants.SOUND_SETSPEED, speed & 0X7F, (speed >> 7) & 0x7F]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def microbit_report_sensor(self):
        data = [PrivateConstants.SUB_MICROBIT_REPORT_FLAG]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        self.report = True

    def microbit_buttonA_is_pressed(self):
        if self._report["buttonA"] == 1:
            return True
        else:
            return False
    
    def microbit_buttonB_is_pressed(self):
        if self._report["buttonB"] == 1:
            return True
        else:
            return False
    
    def microbit_buttonAB_is_pressed(self):
        if self._report["buttonB"] == 1 and self._report["buttonA"] == 1:
            return True
        else:
            return False
    
    def microbit_touch0(self):
        if self._report["touch0"] == 1:
            return True
        else:
            return False
    
    def microbit_touch1(self):
        if self._report["touch1"] == 1:
            return True
        else:
            return False
    
    def microbit_touch2(self):
        if self._report["touch2"] == 1:
            return True
        else:
            return False
    
    def microbit_get_gesture(self):
        return self._report["Mgesture"]
    
    def microbit_get_brightness(self):
        return self._report["Mbrightness"]
    
    def microbit_get_microphone(self):
        return self._report["microphone"]
    
    def microbit_get_compass(self):
        return self._report["heading"]
    
    def microbit_cal_compass(self):
        data = [PrivateConstants.CAL_COMPASS]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
        while not self.compass:pass
    
    def microbit_get_temp(self):
        return self._report["temperature"]
    
    def microbit_get_accelerometer_X(self):
        return self._report["MaccelerationX"]
    
    def microbit_get_accelerometer_Y(self):
        return self._report["MaccelerationY"]
    
    def microbit_get_accelerometer_Z(self):
        return self._report["MaccelerationZ"]
    
    def microbit_get_accelerometer_strength(self):
        return self._report["MaccelerationStrength"]
    
    def microbit_set_wireless_channel(self, channel):
        data = [PrivateConstants.SUB_MESSAGE_WIRELESS, 0x01, channel]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def microbit_open_wireless(self, type):
        data = [PrivateConstants.SUB_MESSAGE_WIRELESS, 0x00, type]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def microbit_send_wireless(self, val):
        data = [PrivateConstants.SUB_MESSAGE_WIRELESS, 0x02]
        temp = self.encode_8Bit_unsigned_integer_array(val)
        data = data + temp
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def microbbit_recv_data(self, callback = None):
        self.microbit_callback = callback

    def GD32V_buttonA_is_pressed(self):           #固件已修改
        return self._report["buttonA"] == 1
    
    def GD32V_buttonA_irq(self,trigger=None, callback = None, Pin=None):
        self.GD32V_buttonA_mode = trigger
        self.GD32V_buttonA_callback = callback
        self.GD32V_a_Pin = Pin
    
    def GD32V_buttonB_is_pressed(self):           #固件已修改
        return self._report["buttonB"] == 1
    
    def GD32V_buttonB_irq(self,trigger=None, callback = None, Pin=None):
        self.GD32V_buttonB_mode = trigger
        self.GD32V_buttonB_callback = callback
        self.GD32V_b_Pin = Pin
    
    def GD32V_read_light(self):
        return self._report["lightness"]

    def GD32V_get_accelerometer_X(self):
        return self._report["accelerationX"]

    def GD32V_get_accelerometer_Y(self):
        return self._report["accelerationY"]
    
    def GD32V_get_accelerometer_Z(self):
        return self._report["accelerationZ"]
    
    def GD32V_get_Macceleration_X(self):
        return self._report["MaccelerationX"]

    def GD32V_get_Macceleration_Y(self):
        return self._report["MaccelerationY"]
    
    def GD32V_get_Macceleration_Z(self):
        return self._report["MaccelerationZ"]
    
    def GD32V_play_buzz(self, index, options):
        if options == 1:
          self.buzz_flag = False
        data = [0x00, options, index]
        self._send_sysex(0x54, data)
    
    def GD32V_play_note(self, _freq, beat=None):
        self.buzz_flag = False
        if beat == None:
          data = [0x01, 0x08, (_freq >> 7)&0x7f, _freq & 0x7f, 0x00, 0x04]  
        else:
          data = [0x01, 0x01, (_freq >> 7)&0x7f, _freq & 0x7f, (beat >> 7)&0x7f, beat & 0x7f]
        self._send_sysex(0x54, data)
    
    def GD32V_set_ticks_tempo(self, ticks, tempo):
        data = [0x02, (ticks >> 7)&0x7f, ticks & 0x7f, (tempo >> 7)&0x7f, tempo & 0x7f]
        self._send_sysex(0x54, data)
    
    def GD32V_buzz_stop(self):
        data = [0x03]
        self._send_sysex(0x54, data)
    
    def GD32V_buzz_redirect(self, pin):
        data = [0x04, pin]
        self._send_sysex(0x54, data)
    
    def GD32V_get_state(self):
        return self.buzz_flag
    
    def expand_oled_begin(self):
        data = [PrivateConstants.SUB_MESSAGE_OLED, 0x00]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)

    def expand_oled_text(self, index, value):
        lens = len(value)
        data = [PrivateConstants.SUB_MESSAGE_OLED, 0x02]
        data.append(index)
        data.append(lens)
        for i in value:
          data.append(ord(i))
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def expand_oled_text_XY(self, X, Y, string):
        lens = len(string)
        data = [PrivateConstants.SUB_MESSAGE_OLED, 0x03]
        data.append(X)
        data.append(Y)
        data.append(lens)
        for i in string:
          data.append(ord(i))
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    def expand_oled_fill(self, index):
        data = [PrivateConstants.SUB_MESSAGE_OLED, 0x01, index]
        self._send_sysex(PrivateConstants.DFROBOT_MESSAGE, data)
    
    '''
    Firmata message handlers
    '''

    def _analog_mapping_response(self, data):
        """
        This is a private message handler method.
        It is a message handler for the analog mapping response message.

        :param data: response data

        """
        self.query_reply_data[PrivateConstants.ANALOG_MAPPING_RESPONSE] = data

    def _analog_message(self, data):
        """
        This is a private message handler method.
        It is a message handler for analog messages.

        :param data: message data

        """
        pin = data[0]
        value = (data[PrivateConstants.MSB] << 7) + data[PrivateConstants.LSB]

        # only report when there is a change in value
        differential = abs(value - self.analog_pins[pin].current_value)
        if differential >= self.analog_pins[pin].differential:
            self.analog_pins[pin].current_value = value
            # time_stamp = time.time()
            # self.analog_pins[pin].event_time = time_stamp

            # append pin number, pin value, and pin type to return value and return as a list
            message = [PrivateConstants.ANALOG, pin, value]

            if self.analog_pins[pin].cb:
                self.analog_pins[pin].cb(message)

    def _capability_response(self, data):
        """
        This is a private message handler method.
        It is a message handler for capability report responses.

        :param data: capability report

        """
        self.query_reply_data[PrivateConstants.CAPABILITY_RESPONSE] = data

    def _dfrobot_message(self, data):
        if len(data) == 0:
            return
        if data[0] == PrivateConstants.SUB_MESSAGE_I2CSCAN:
            self.i2c_map['scan'] = data[1:]
            self.iic_async_lock.release()
        elif data[0] == PrivateConstants.SUB_MESSAGE_IR:
            value = data[2] + (data[3] << 7) + (data[4] << 14) + \
                (data[5] << 21) + (data[6] << 28)
#            value &= 0xFFFF 
#            temp = value & 0xff
#            value = value >> 8
            pin_number = data[1]
#            if value == (~temp & 0xff):
            # method = None
            # if pin_number in self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_IR].keys():
            arg = self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_IR][pin_number]

            if isinstance(arg, types.FunctionType):
                arg(value)
            else:
                arg.append(value)
                
        elif data[0] == PrivateConstants.SUB_MESSAGE_18B20:
            temp = (data[3] << 8 | data[2]) / 16
            pin_number = data[1]
            self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_18B20][pin_number] = temp
        elif data[0] == PrivateConstants.SUB_MESSAGE_GP2Y1010AU0F:
            temp = (data[2] << 7 | data[3])
            pin_number = data[1]
            self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_GP2Y1010AU0F][pin_number] = temp
            self.other_async_lock.release()
        elif data[0] == PrivateConstants.SUB_MESSAGE_AUDIO:
            value = []
            arr = data[1:]
            for i in range(0, 14, 2):
                tmp = (arr[i] << 7) | (arr[i + 1] & 0x7f)
                value.append(tmp)
            self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_AUDIO][PrivateConstants.AUDIO_VALUE] = value
        elif data[0] == PrivateConstants.SUB_MESSAGE_HX711:
            dout_pin = data[1]
            int_h = data[2]
            int_l = data[3]
            decimals = data[4]
            int_num = (int_h << 8) | int_l
            ret_value = int_num + decimals / 100
            self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_HX711][dout_pin] = ret_value
        elif data[0] == PrivateConstants.SUB_MESSAGE_UART_READ:
            bus_num = data[1]
            num = data[2]
            for i in range(0, num, 2):
                value = (data[3+i] << 7) | (data[4+i] & 0x7f)
                self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_UART_READ][bus_num].append(value)  
        elif data[0] == PrivateConstants.SUB_MESSAGE_HEARTRATE:
            rate_pin = data[1]
            rate_value = data[2]
            if rate_value != 0:
                self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_HEARTRATE][rate_pin] = rate_value
        elif data[0] == PrivateConstants.SUB_MESSAGE_CONNECT_PESPOND:
            if len(data) == 1:
                self.dfrobot_dispatch.get(PrivateConstants.SUB_MESSAGE_CONNECT_PESPOND)["ip"] = "Connection failed, please check wifi name and password"
            else:
                ip_addr = ""
                for i in data[1:]:
                    ip_addr += chr(i)
                self.dfrobot_dispatch.get(PrivateConstants.SUB_MESSAGE_CONNECT_PESPOND)[
                    "ip"] = ip_addr
                if ip_addr == "wifi no connect":
                    self.dfrobot_dispatch.get(PrivateConstants.SUB_MESSAGE_CONNECT_PESPOND)[
                        "port"] = None
                else:
                    self.dfrobot_dispatch.get(PrivateConstants.SUB_MESSAGE_CONNECT_PESPOND)[
                        "port"] = 8081
        elif data[0] == PrivateConstants.SUB_MESSAGE_DFROBOT_ESP32_REPORTS:
            self._report["buttonA"] = data[1]
            self._report["buttonB"] = data[2]
            self._report["touchP"]["value"] = (
                data[3] & 0x7f) | ((data[4] & 0x7f) << 7)
            if self._report["touchP"]["value"] < self._report["touchThreshold"]["P"]:
                self._report["touchP"]["touchNum"] += 1
                self._report["touchP"]["touchNum"] = min(
                    self._report["touchP"]["touchNum"], 10)
            else:
                self._report["touchP"]["touchNum"] = 0
            self._report["touchY"]["value"] = (
                data[5] & 0x7f) | ((data[6] & 0x7f) << 7)
            if self._report["touchY"]["value"] < self._report["touchThreshold"]["Y"]:
                self._report["touchY"]["touchNum"] += 1
                self._report["touchY"]["touchNum"] = min(
                    self._report["touchY"]["touchNum"], 10)
            else:
                self._report["touchY"]["touchNum"] = 0
            self._report["touchT"]["value"] = (
                data[7] & 0x7f) | ((data[8] & 0x7f) << 7)
            if self._report["touchT"]["value"] < self._report["touchThreshold"]["T"]:
                self._report["touchT"]["touchNum"] += 1
                self._report["touchT"]["touchNum"] = min(
                    self._report["touchT"]["touchNum"], 10)
            else:
                self._report["touchT"]["touchNum"] = 0
            self._report["touchH"]["value"] = (
                data[9] & 0x7f) | ((data[10] & 0x7f) << 7)
            if self._report["touchH"]["value"] < self._report["touchThreshold"]["H"]:
                self._report["touchH"]["touchNum"] += 1
                self._report["touchH"]["touchNum"] = min(
                    self._report["touchH"]["touchNum"], 10)
            else:
                self._report["touchH"]["touchNum"] = 0
            self._report["touchO"]["value"] = (
                data[11] & 0x7f) | ((data[12] & 0x7f) << 7)
            if self._report["touchO"]["value"] < self._report["touchThreshold"]["O"]:
                self._report["touchO"]["touchNum"] += 1
                self._report["touchO"]["touchNum"] = min(
                    self._report["touchO"]["touchNum"], 10)
            else:
                self._report["touchO"]["touchNum"] = 0
            self._report["touchN"]["value"] = (
                data[13] & 0x7f) | ((data[14] & 0x7f) << 7)
            if self._report["touchN"]["value"] < self._report["touchThreshold"]["N"]:
                self._report["touchN"]["touchNum"] += 1
                self._report["touchN"]["touchNum"] = min(
                    self._report["touchN"]["touchNum"], 10)
            else:
                self._report["touchN"]["touchNum"] = 0
            self._report["gesture"] = data[15]
            self._report["microphone"] = data[16] & 0x7f | ((data[17] & 0x7f) << 7)
            self._report["lightness"]= data[18] & 0x7f | ((data[19] & 0x7f) << 7)
            self._report["accelerationX"]=(-1 if data[20] else 1) * ((data[21] & 0x7f) | ((data[22] & 0x7f) << 7))
            self._report["accelerationY"] = (-1 if data[23] else 1) * ((data[24] & 0x7f) | ((data[25] & 0x7f) << 7))
            self._report["accelerationZ"]=(-1 if data[26] else 1) * ((data[27] & 0x7f) | ((data[28] & 0x7f) << 7))
            self._report["accelerationStrength"]=math.sqrt(math.pow(self._report["accelerationX"], 2) + math.pow(self._report["accelerationY"], 2) + math.pow(self._report["accelerationZ"], 2))
            self._report["brightness"]=data[29] & 0x7f
            self._report["millis"]=(data[30] & 0x7f) | ((data[31] & 0x7f) << 7) | ((data[32] & 0x7f) << 14) | ((data[33] & 0x7f) << 21) | ((data[34] & 0x7f) << 28)

        elif data[0] == PrivateConstants.SUB_MESSAGE_DFROBOT_MICROBIT_REPORTS:
            self._report["buttonA"] = data[1]
            self._report["buttonB"] = data[2]
            self._report["touch0"] = data[3]
            self._report["touch1"] = data[4]
            self._report["touch2"] = data[5]
            self._report["Mgesture"] = data[6]
            self._report["Mbrightness"] = (data[7] & 0x7f) | ((data[8] & 0x7f) << 7)
            self._report["heading"] = (data[9] & 0x7f) | ((data[10] & 0x7f) << 7)
            self._report["temperature"] = (-1 if data[11] else 1) * ((data[12] & 0x7f) | ((data[13] & 0x7f) << 7))
            self._report["MaccelerationX"]=(-1 if data[14] else 1) * ((data[15] & 0x7f) | ((data[16] & 0x7f) << 7))
            self._report["MaccelerationY"] = (-1 if data[17] else 1) * ((data[18] & 0x7f) | ((data[19] & 0x7f) << 7))
            self._report["MaccelerationZ"]=(-1 if data[20] else 1) * ((data[21] & 0x7f) | ((data[22] & 0x7f) << 7))
            try:
                self._report["microphone"] = (data[29] & 0x7f) | ((data[30] & 0x7f) << 7)                              #尝试是否是V2上传的麦克风数据,V1无
            except Exception:
                pass
            self._report["MaccelerationStrength"]=math.sqrt(math.pow(self._report["MaccelerationX"], 2) + math.pow(self._report["MaccelerationY"], 2) + math.pow(self._report["MaccelerationZ"], 2))

        elif data[0] == PrivateConstants.SUB_MESSAGE_WIRELESS:
            temp = ""
            for i in range(1, len(data)-1,2):
                val = (data[i+1] << 7) | data[i]
                temp += chr(val)
            if self.microbit_callback != None:
                self.microbit_callback(temp)
        
        elif data[0] == PrivateConstants.SUB_MESSAGE_DFROBOT_GD32V_REPORTS:
            if len(data)<23:
                return
            last_valueA = self.GD32V_buttonA
            last_valueB = self.GD32V_buttonB
            self.GD32V_buttonA = data[1]
            self.GD32V_buttonB = data[2]
            self._report["buttonA"] = data[1]
            self._report["buttonB"] = data[2]
            if last_valueA != data[1]:
                if self.GD32V_buttonA_callback and self.GD32V_a_Pin:
                    if self.GD32V_buttonA_mode == PrivateConstants.IRQ_FALLING and data[1] == 0:
                        self.GD32V_buttonA_callback(self.GD32V_a_Pin)
                    elif self.GD32V_buttonA_mode == PrivateConstants.IRQ_RISING and data[1]:
                        self.GD32V_buttonA_callback(self.GD32V_a_Pin)
                    elif self.GD32V_buttonA_mode == PrivateConstants.IRQ_RISING + PrivateConstants.IRQ_FALLING:
                        self.GD32V_buttonA_callback(self.GD32V_a_Pin)
            if last_valueB != data[2]:
                if self.GD32V_buttonB_callback and self.GD32V_b_Pin:
                    if self.GD32V_buttonB_mode == PrivateConstants.IRQ_FALLING and data[2] == 0:
                        self.GD32V_buttonB_callback(self.GD32V_b_Pin)
                    elif self.GD32V_buttonB_mode == PrivateConstants.IRQ_RISING and data[2]:
                        self.GD32V_buttonB_callback(self.GD32V_b_Pin)
                    elif self.GD32V_buttonB_mode == PrivateConstants.IRQ_RISING + PrivateConstants.IRQ_FALLING:
                        self.GD32V_buttonB_callback(self.GD32V_b_Pin)
            self._report["lightness"] = ((data[3] & 0x7f) << 7) | (data[4] & 0x7f)
            self._report["accelerationX"]=((-1 if data[5] else 1) * ((data[6] & 0x7f) | ((data[7] & 0x7f) << 7)))/10000
            self._report["accelerationY"] = ((-1 if data[8] else 1) * ((data[9] & 0x7f) | ((data[10] & 0x7f) << 7)))/10000
            self._report["accelerationZ"]=((-1 if data[11] else 1) * ((data[12] & 0x7f) | ((data[13] & 0x7f) << 7)))/10000
            self._report["MaccelerationX"]=((-1 if data[14] else 1) * ((data[15] & 0x7f) | ((data[16] & 0x7f) << 7)))/10000
            self._report["MaccelerationY"] = ((-1 if data[17] else 1) * ((data[18] & 0x7f) | ((data[19] & 0x7f) << 7)))/10000
            self._report["MaccelerationZ"]=((-1 if data[20] else 1) * ((data[21] & 0x7f) | ((data[22] & 0x7f) << 7)))/10000
        
        elif data[0] == PrivateConstants.ANALOG_MESSAGE:
            self._analog_message(data[1:])                  #解决模拟上报引脚大于15问题
        
        elif data[0] == PrivateConstants.SUB_MESSAGE_COMPASS:
            self.compass = True
        
        elif data[0] == PrivateConstants.SUB_MESSAGE_SOUND:
            self.music = True
        
        elif data[0] == PrivateConstants.SUB_MESSAGE_BUZZ:
            self.buzz_flag = True
        
        elif data[0] == PrivateConstants.SUB_MESSAGE_SPI:
            buf = data[2:]
            device_num = data[1]
            self.dfrobot_dispatch[PrivateConstants.SUB_MESSAGE_SPI][device_num] = buf
            self.spi_async_lock.release() 

    def _dht_read_response(self, data):
        """
        Process the dht response message.

        Values are calculated using:
                humidity = (_bits[0] * 256 + _bits[1]) * 0.1

                temperature = ((_bits[2] & 0x7F) * 256 + _bits[3]) * 0.1

        error codes:
        0 - OK
        1 - DHTLIB_ERROR_TIMEOUT
        2 - Checksum error

        :param: data - array of 9 7bit bytes ending with the error status
        """
        # get the time of the report
        time_stamp=time.time()
        # initiate a list for a potential call back
        reply_data=[PrivateConstants.DHT]

        # get the pin and type of the dht
        pin=data[0]
        reply_data.append(pin)
        dht_type=data[1]
        reply_data.append(dht_type)
        humidity=None
        temperature=None

        self.digital_pins[pin].event_time=time_stamp
        # if data read correctly process and return
        if len(data) != 7:  # data[9] is config flag
            return
#                self.dht_sensor_error = True
#                humidity = temperature = -1
            # return
        else:
            # if data read correctly process and return

            if data[6] == 0:
                # dht 22
                if data[1] == 22:
                    humidity=(data[2] * 256 + data[3]) * 0.1
                    temperature=((data[4] & 0x7F) * 256 + data[5]) * 0.1
                # dht 11
                elif data[1] == 11:
                    humidity=(data[2]) + (data[3]) * 0.1
                    temperature=(data[4]) + (data[5]) * 0.1
                else:
                    raise RuntimeError(
                        "Unknown DHT Sensor type reported: %d" % data[2])

                humidity=round(humidity, 2)
                temperature=round(temperature, 2)

                # check for negative temperature
                if data[6] & 0x80:
                    temperature=-temperature

        # since we initialize
        if humidity is None:
            return
        reply_data.append(humidity)
        reply_data.append(temperature)
        # reply_data.append(time_stamp)

        # retrieve the last reported values
        last_value=self.digital_pins[pin].current_value

        self.digital_pins[pin].current_value=[humidity, temperature]
        if self.digital_pins[pin].cb:
            # only report changes
            # has the humidity changed?
            if last_value[0] != humidity:

                differential=abs(humidity - last_value[0])
                if differential >= self.digital_pins[pin].differential:
                    self.digital_pins[pin].cb(reply_data)
                return
            if last_value[1] != temperature:
                differential=abs(temperature - last_value[1])
                if differential >= self.digital_pins[pin].differential:
                    self.digital_pins[pin].cb(reply_data)
                return

    def _digital_message(self, data):
        """
        This is a private message handler method.
        It is a message handler for Digital Messages.

        :param data: digital message

        """
        port=data[0]
        # noinspection PyPep8
        port_data=(data[PrivateConstants.MSB] << 7) + \
            data[PrivateConstants.LSB]
        pin=port * 8
        for pin in range(pin, min(pin + 8, len(self.digital_pins))):
            # get pin value
            value=port_data & 0x01

            # retrieve previous value
            last_value=self.digital_pins[pin].current_value

            # set the current value in the pin structure
            self.digital_pins[pin].current_value=value
            # time_stamp = time.time()
            # self.digital_pins[pin].event_time = time_stamp

            # append pin number, pin value, and pin type to return value and return as a list
            if self.digital_pins[pin].pull_up:
                message=[PrivateConstants.PULLUP, pin, value]
            else:
                message=[PrivateConstants.INPUT, pin, value]

            if last_value != value:
                if self.digital_pins[pin].cb:
                    if self.digital_pins[pin].mode == PrivateConstants.IRQ_FALLING and value == 0:
                        self.digital_pins[pin].cb(pin)
                    elif self.digital_pins[pin].mode == PrivateConstants.IRQ_RISING and value:
                        self.digital_pins[pin].cb(pin)
                    elif self.digital_pins[pin].mode == PrivateConstants.IRQ_RISING + PrivateConstants.IRQ_FALLING:
                        self.digital_pins[pin].cb(pin)

            port_data >>= 1

    # noinspection PyDictCreation

    def _i2c_reply(self, data):
        """
        This is a private message handler method.
        It handles replies to i2c_read requests. It stores the data
        for each i2c device address in a dictionary called i2c_map.
        The data may be retrieved via a polling call to i2c_get_read_data().
        It a callback was specified in pymata.i2c_read, the raw data is sent
        through the callback

        :param data: raw data returned from i2c device

        """
        # initialize the reply data with I2C pin mode
        reply_data=[PrivateConstants.I2C]
        # reassemble the data from the firmata 2 byte format
        address=(data[0] & 0x7f) + (data[1] << 7)

        # if we have an entry in the i2c_map, proceed
        if address in self.i2c_map:
            with self.the_i2c_map_lock:
                # get 2 bytes, combine them and append to reply data list
                for i in range(0, len(data), 2):
                    combined_data=(data[i] & 0x7f) + (data[i + 1] << 7)
                    reply_data.append(combined_data)

                # current_time = time.time()
                # reply_data.append(current_time)

                # place the data in the i2c map without storing the address byte or
                #  register byte (returned data only)
                map_entry=self.i2c_map.get(address)
                map_entry['value']=reply_data[3:]
                # map_entry['time_stamp'] = current_time
                self.i2c_map[address]=map_entry
                cb=map_entry.get('callback')
                if cb:
                    # send everything, including address and register bytes back
                    # to caller
                    # reply data will contain:
                    # [pin_type = 6, i2c_device address,
                    #                       raw data returned from i2c device, time-stamp]
                    cb(reply_data)
                else:
                    self.iic_async_lock.release()

    def _pin_state_response(self, data):
        """
        This is a private message handler method.
        It handles pin state query response messages.

        :param data: Pin state message

        """
        self.query_reply_data[PrivateConstants.PIN_STATE_RESPONSE]=data

    def _report_firmware(self, sysex_data):
        """
        This is a private message handler method.

        This method handles the sysex 'report firmware' command sent by
        Firmata (0x79).

        It assembles the firmware version by concatenating the major and
        minor version number components and the firmware identifier into
        a string.

        e.g. "2.3 StandardFirmata.ino"

        :param sysex_data: Sysex data sent from Firmata

        """
        # first byte after command is major number
        major=sysex_data[0]
        version_string=str(major)

        # next byte is minor number
        # minor = sysex_data[2]
        minor=sysex_data[1]

        # append a dot to major number
        version_string += '.'

        # append minor number
        version_string += str(minor)
        # add a space after the major and minor numbers
        version_string += ' '

        # slice the identifier - from the first byte after the minor
        #  number up until, but not including the END_SYSEX byte

        # name = sysex_data[3:-1]
        name=sysex_data[2:]

        firmware_name_iterator=iter(name)

        # convert each element from two 7-bit bytes into characters, then add each
        # character to the version string
        for e in firmware_name_iterator:
            version_string += chr(e + (next(firmware_name_iterator) << 7))

        # store the value
        self.query_reply_data[PrivateConstants.REPORT_FIRMWARE]=version_string

    def _report_version(self, data):
        """
        This is a private message handler method.

        This method reads the following 2 bytes after the report version
        command (0xF9 - non sysex).

        The first byte is the major number and the second byte is the
        minor number.

        """
        version_string=str(data[0]) + '.' + str(data[1])
        self.query_reply_data[PrivateConstants.REPORT_VERSION]=version_string

    def _send_command(self, command):
        """
        This is a private utility method.
        The method sends a non-sysex command to Firmata.

        :param command:  command data

        :returns: number of bytes sent
        """
        # send_message = ""
        # print(command)
        send_message=bytes(command)
        if not self.ip_address:
            try:
                if self.report:                                         #在有板载信息上报的，需要暂停上报发消息，然后再打开上报
                    stop_command = [0xF0, 0x0D, 0x20, 0x00, 0XF7]
                    stop_message = bytes(stop_command)
                    result=self.serial_port.write(stop_message)
                    self.serial_port.flush()
                    time.sleep(0.01)
                result=self.serial_port.write(send_message)
                self.serial_port.flush()
                # time.sleep(0.001)
                if self.report:
                    begin_command = [0xF0, 0x0D, 0x20, 0x01, 0XF7]
                    begin_message = bytes(begin_command)
                    result=self.serial_port.write(begin_message)
                    self.serial_port.flush()
                    time.sleep(0.01)
            except Exception as e:
                # if self.shutdown_on_exception:
                #    self.shutdown()
                # print("write fail in _send_command,quit process")
                # print("tuichu")
                self.quit()
                # raise RuntimeError('write fail in _send_command')
        else:
            result=self.sock.sendall(send_message)
            time.sleep(0.005)
            # time.sleep(0.008)
        return result

    def _send_keep_alive(self):
        """
        This is a the thread to continuously send keep alive messages
        """
        while True:
            if self.period:
                self._send_sysex(PrivateConstants.KEEP_ALIVE,
                                 self.keep_alive_interval)
                time.sleep(self.period - self.margin)
            else:
                break

    def _sonar_data(self, data):
        """
        This method handles the incoming sonar data message and stores
        the data in the response table.

        :param data: Message data from Firmata

        """

        pin_number=data[0]
        val=int((data[PrivateConstants.MSB] << 7) +
                  data[PrivateConstants.LSB])
        # initialize reply_data with SONAR pin type
        reply_data=[PrivateConstants.SONAR]
        # print("data=", data)
        with self.the_sonar_map_lock:
            sonar_pin_entry=self.active_sonar_map[pin_number]
            # if sonar_pin_entry[0] is not None:
            if True:
                # check if value changed since last reading
                if sonar_pin_entry[1] != val:
                    sonar_pin_entry[1]=val
                    # time_stamp = time.time()
                    # sonar_pin_entry[2] = time_stamp
                    self.active_sonar_map[pin_number]=sonar_pin_entry
                    # Do a callback if one is specified in the table
                    if sonar_pin_entry[0]:
                        reply_data.append(pin_number)
                        reply_data.append(val)
                        # reply_data.append(time_stamp)
                        if sonar_pin_entry[1]:
                            sonar_pin_entry[0](reply_data)
                # update the data in the table with latest value
                else:
                    sonar_pin_entry[1]=val
                    self.active_sonar_map[pin_number]=sonar_pin_entry
            time.sleep(self.sleep_tune)

    def _send_sysex(self, sysex_command, sysex_data=None):
        """
        This is a private utility method.
        This method sends a sysex command to Firmata.

        :param sysex_command: sysex command

        :param sysex_data: data for command

        """
        if not sysex_data:
            sysex_data=[]

        the_command=[PrivateConstants.START_SYSEX, sysex_command]
        if sysex_data:
            for d in sysex_data:
                the_command.append(d)
        the_command.append(PrivateConstants.END_SYSEX)
        with self.the_send_sysex_lock:
            self._send_command(the_command)

    # noinspection PyMethodMayBeStatic
    def _string_data(self, data):
        """
        This is a private message handler method.
        It is the message handler for String data messages that will be
        printed to the console.

        :param data:  message

        """
        reply=''
        for x in data:
            reply_data=x
            if reply_data:
                reply += chr(reply_data)
        print(reply)

    def _run_threads(self):
        self.run_event.set()

    def _is_running(self):
        return self.run_event.is_set()

    def _stop_threads(self):
        self.run_event.clear()

    def _reporter(self):
        """
        This is the reporter thread. It continuously pulls data from
        the deque. When a full message is detected, that message is
        processed.
        """
        self.run_event.wait()
        data=0
        # sysex commands are assembled into this list for processing
        # next_command_byte = None
        while self._is_running() and not self.shutdown_flag:
            if len(self.the_deque):

                # get next byte from the deque and process it
                data=self.the_deque.popleft()
                # this list will be populated with the received data for the command
                response_data=[]
                # print("data=", data)
                # process sysex commands
                if data == PrivateConstants.START_SYSEX:
                    # next char is the actual sysex command
                    # wait until we can get data from the deque
                    while len(self.the_deque) == 0:
                        pass
                    sysex_command=self.the_deque.popleft()
                    # retrieve the associated command_dispatch entry for this command
                    dispatch_entry=self.report_dispatch.get(sysex_command)

                    # get a "pointer" to the method that will process this command
                    try:
                        method=dispatch_entry[0]
                    except:
                        break

                    # now get the rest of the data excluding the END_SYSEX byte
                    end_of_sysex=False
                    while not end_of_sysex:
                        # wait for more data to arrive
                        while len(self.the_deque) == 0:
                            pass
                        data=self.the_deque.popleft()
                        if data != PrivateConstants.END_SYSEX:
                            response_data.append(data)
                        else:
                            end_of_sysex=True

                            # invoke the method to process the command
                            # print("response_data=",response_data)

                            method(response_data)
                            # go to the beginning of the loop to process the next command
                    continue

                # is this a command byte in the range of 0x80-0xff - these are the non-sysex messages

                elif 0x80 <= data <= 0xff:
                    # look up the method for the command in the command dispatch table
                    # for the digital reporting the command value is modified with port number
                    # the handler needs the port to properly process, so decode that from the command and
                    # place in response_data
                    if 0x90 <= data <= 0x9f:
                        port=data & 0xf

                        response_data.append(port)
                        data=0x90
                    # the pin number for analog data is embedded in the command so, decode it
                    elif 0xe0 <= data <= 0xef:
                        pin=data & 0xf
                        response_data.append(pin)
                        data=0xe0
                    else:
                        pass

                    dispatch_entry=self.report_dispatch.get(data)
                    # print("data = ",hex(data))
                    # print("dispatch_entry = ",dispatch_entry)
                    # this calls the method retrieved from the dispatch table
                    try:
                        method=dispatch_entry[0]
                    
                        # get the number of parameters that this command provides
                        num_args=dispatch_entry[1]

                        # look at the number of args that the selected method requires
                        # now get that number of bytes to pass to the called method
                        for i in range(num_args):
                            while len(self.the_deque) == 0:
                                pass
                            data=self.the_deque.popleft()
                            response_data.append(data)
                            # go execute the command with the argument list

                        method(response_data)
                    except:pass
                    # go to the beginning of the loop to process the next command
                    continue
            else:
                time.sleep(self.sleep_tune)

    def _serial_receiver(self):
        """
        Thread to continuously check for incoming data.
        When a byte comes in, place it onto the deque.
        """
        self.run_event.wait()

        while self._is_running() and not self.shutdown_flag:
            # we can get an OSError: [Errno9] Bad file descriptor when shutting down
            # just ignore it
            try:
                if self.serial_port.inWaiting():
                    c=self.serial_port.read(self.serial_port.in_waiting)  #解决主动上报太快，导致处理不过来的情况
                    if len(c) != 0:
                        for i in c:
                            self.the_deque.append(i)            
                else:
                    time.sleep(self.sleep_tune)
                    # continue
            except OSError:
                pass

    def __tcp_receiver(self):
        """
        Thread to continuously check for incoming data.
        When a byte comes in, place it onto the deque.
        """
        self.run_event.wait()

        while self._is_running() and not self.shutdown_flag:
            try:
                payload=self.sock.recv(1024)
                for i in payload:
                    self.the_deque.append(i)
            except Exception:
                pass
