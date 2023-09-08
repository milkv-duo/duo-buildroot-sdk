# -*- coding: utf-8 -*-
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_gravity_pm25 import DFRobot_GravityPM25

Board("milkv-duo").begin()

pm = DFRobot_GravityPM25(bus_num=1)

version = pm.gain_version()
print("version is : " + str(version))

# pm.set_lowpower()   # Control the sensor to enter low-power mode
# pm.awake()          # Wake up sensor

while True:
    '''
    @brief Get PM concentration in the air: parameters available
    @n     PARTICLE_PM1_0_STANDARD   
    @n     PARTICLE_PM2_5_STANDARD  
    @n     PARTICLE_PM10_STANDARD  
    @n     PARTICLE_PM1_0_ATMOSPHERE 
    @n     PARTICLE_PM2_5_ATMOSPHERE
    @n     PARTICLE_PM10_ATMOSPHERE   
    '''
    concentration = pm.gain_particle_concentration_ugm3(pm.PARTICLE_PM1_0_STANDARD)
    print("PM1.0 concentration:" + str(concentration) + " mg/m3")
    '''
    @n     PARTICLENUM_0_3_UM_EVERY0_1L_AIR 
    @n     PARTICLENUM_0_5_UM_EVERY0_1L_AIR 
    @n     PARTICLENUM_1_0_UM_EVERY0_1L_AIR 
    @n     PARTICLENUM_2_5_UM_EVERY0_1L_AIR 
    @n     PARTICLENUM_5_0_UM_EVERY0_1L_AIR 
    @n     PARTICLENUM_10_UM_EVERY0_1L_AIR
    '''
    num = pm.gain_particlenum_every0_1l(pm.PARTICLENUM_0_3_UM_EVERY0_1L_AIR)
    print("The number of particles with a diameter of 0.3um per 0.1 in lift-off is:" + str(num))
    time.sleep(1)
