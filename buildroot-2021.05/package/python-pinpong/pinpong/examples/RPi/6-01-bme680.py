# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bme680 import DFRobot_BME680

Board("RPI").begin()#初始化

sensor = DFRobot_BME680(bus_num=1) #根据支持i2c设备填写

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