# -*- coding: utf-8 -*-

#Nezha
#实验效果：使用LIS331HH传感器读取三轴加速度值

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_lis import LIS331HH_I2C


Board("nezha").begin()

#ADDRESS_0       = 0x18            #Sensor address0
ADDRESS_1       = 0x19            #Sensor address1
acce = LIS331HH_I2C(addr = ADDRESS_1)

#Chip initialization
acce.begin()
#Get chip id
print('chip id :0x%x'%acce.get_id())

'''
set range:Range(g)
          LIS331HH_6G  = 6   #±6G
          LIS331HH_12G = 12  #±12G
          LIS331HH_24G = 24  #±24G
'''
acce.set_range(acce.LIS331HH_6G)

'''
Set data measurement rate
         POWERDOWN_0HZ 
         LOWPOWER_HALFHZ 
         LOWPOWER_1HZ 
         LOWPOWER_2HZ 
         LOWPOWER_5HZ 
         LOWPOWER_10HZ 
         NORMAL_50HZ 
         NORMAL_100HZ 
         NORMAL_400HZ 
         NORMAL_1000HZ 
'''
acce.set_acquire_rate(acce.NORMAL_50HZ)
time.sleep(0.1)

while True:
    #Get the acceleration in the three directions of xyz
    #The measurement range can be ±6g, ±12g or ±24g, set by set_range() function
    x,y,z = acce.read_acce_xyz()
    print("Acceleration [X = %.2d mg,Y = %.2d mg,Z = %.2d mg]"%(x,y,z))
    time.sleep(1)
