 # -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_mcp4725 import MCP4725

ip = "192.168.1.116"
port = 8081

Board(ip, port)

ref_voltage    = 5000
output_voltage = 1000
dac = MCP4725()

#address0 -->0x60
#address1 -->0x61

dac.init(dac.address0, ref_voltage)

while True:
    print("DFRobot_MCP4725 write to EEPROM and output: {} mV".format(output_voltage))
    dac.output_voltage_EEPROM(output_voltage)
    time.sleep(0.2)