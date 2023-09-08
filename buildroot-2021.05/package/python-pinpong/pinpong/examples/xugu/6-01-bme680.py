# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bme680 import DFRobot_BME680

Board("xugu").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("xugu","COM36").begin()  #windows下指定端口初始化
#Board("xugu","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("xugu","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

sensor = DFRobot_BME680()

sensor.set_humidity_oversample(sensor.OS_2X) #Oversampling value: OS_NONE, OS_1X, OS_2X, OS_4X, OS_8X, OS_16X
sensor.set_pressure_oversample(sensor.OS_4X) #Oversampling value: OS_NONE, OS_1X, OS_2X, OS_4X, OS_8X, OS_16X
sensor.set_temperature_oversample(sensor.OS_8X) #Oversampling value: OS_NONE, OS_1X, OS_2X, OS_4X, OS_8X, OS_16X
sensor.set_filter(sensor.FILTER_SIZE_3) #increasing resolution but reducing bandwidth
sensor.set_gas_status(sensor.ENABLE_GAS_MEAS) #1 for enable and 0 for disable

sensor.set_gas_heater_temperature(320) #value:target temperature in degrees celsius, between 200 ~ 400
sensor.set_gas_heater_duration(150) #value:target duration in milliseconds, between 1 and 4032
sensor.select_gas_heater_profile(0) #value:current gas sensor conversion profile: 0 to 9

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