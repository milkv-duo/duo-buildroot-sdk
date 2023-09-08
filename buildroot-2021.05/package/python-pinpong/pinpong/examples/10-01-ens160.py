# -*- coding: utf-8 -*-
import time
from pinpong.board import Board,Pin
from pinpong.libs.dfrobot_ens160 import Ens160

Board("uno").begin()#初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

ens160 = Ens160()

# ENS160_SLEEP_MODE: DEEP SLEEP mode (low power standby)
# ENS160_IDLE_MODE: IDLE mode (low-power)
# ENS160_STANDARD_MODE: STANDARD Gas Sensing Modes
ens160.set_pwr_mode(ens160.ENS160_STANDARD_MODE)

ens160.set_temp_hum(25.0, 50.0) # temperature & humidity

while True:
    print("-------------------------")
    print("Sensor operating status : %d" %ens160.get_status())
    print("Air quality index : %d" %ens160.get_aqi())
    print("Concentration of total volatile organic compounds : %d ppb" %ens160.get_tvoc())
    print("Carbon dioxide equivalent concentration : %d ppm" %ens160.get_eco2())
    time.sleep(1)
