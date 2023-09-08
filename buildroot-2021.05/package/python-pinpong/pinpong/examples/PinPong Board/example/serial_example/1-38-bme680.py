# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bme680 import DFRobot_BME680

Board("PinPong board").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong board","COM36").begin()  #windows下指定端口初始化
#Board("PinPong board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

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