# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bme680 import DFRobot_BME680

ip = "192.168.1.116"
port = 8081

Board(ip, port)

sensor = DFRobot_BME680()

while True:
    if sensor.get_sensor_data():
        print("-------------------")
        print("temperature: {} ℃".format(sensor.data.temperature))
        print("pressure: {} Pa".format(sensor.data.pressure))
        print("humidity: {} %rh".format(sensor.data.humidity))
        print("altitudu: {} m".format(sensor.data.altitudu))
        print("gas_resistance: {} ohm".format(sensor.data.gas_resistance))
        print("-------------------")
    time.sleep(1)