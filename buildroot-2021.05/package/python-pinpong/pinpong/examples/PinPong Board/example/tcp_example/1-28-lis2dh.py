# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_lis2dh import LIS2DH

ip = "192.168.0.15"
port = 8081

Board(ip, port)

lis = LIS2DH()

# Set measurement range
# Ga: LIS2DH12_RANGE_2GA
# Ga: LIS2DH12_RANGE_4GA
# Ga: LIS2DH12_RANGE_8GA
# Ga: LIS2DH12_RANGE_16GA
while not lis.init(lis.LIS2DH12_RANGE_8GA):
    print("No I2C devices found")
    time.sleep(1)

while True:
    lis.read_XYZ()
    print("x = %.0f mg, y = %.0f mg,z =%.0f mg"%(lis.x, lis.y, lis.z))
    time.sleep(0.1)