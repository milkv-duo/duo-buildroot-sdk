# -*- coding: utf-8 -*-

#实验效果：读取I2C 10DOF绝对定向传感器

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bno055 import BNO055

Board("RPI").begin()   #初始化

bno = BNO055(bus_num=1) #根据支持的i2c设备填写

while not bno.begin():#等待传感器初始化完成
  print("bno begin faild")
  time.sleep(2)
print("bno begin success")

while True:
  acc_analog = bno.get_axis_acc()
  mag_analog = bno.get_axis_mag()
  gyr_analog = bno.get_axis_gyr()
  lia_analog = bno.get_axis_lia()
  grv_analog = bno.get_axis_grv()
  eul_analog = bno.get_eul()
  qua_analog = bno.get_qua()
  print("==================== analog data print start ====================")
  print("acc analog: (unit mg)      x:%.2f   y:%.2f   z:%.2f\t"%(acc_analog.x,acc_analog.y,acc_analog.z))
  print("mag analog: (unit ut)      x:%.2f   y:%.2f   z:%.2f\t"%(mag_analog.x,mag_analog.y,mag_analog.z))
  print("gyr analog: (unit dps)     x:%.2f   y:%.2f   z:%.2f\t"%(gyr_analog.x,gyr_analog.y,gyr_analog.z))
  print("lia analog: (unit mg)      x:%.2f   y:%.2f   z:%.2f\t"%(lia_analog.x,lia_analog.y,lia_analog.z))
  print("grv analog: (unit mg)      x:%.2f   y:%.2f   z:%.2f\t"%(grv_analog.x,grv_analog.y,grv_analog.z))
  print("eul analog: (unit degree)  head:%.2f   roll:%.2f   pitch:%.2f"%(eul_analog.head,eul_analog.roll,eul_analog.pitch))
  print("qua analog: (no unit)      w:%0.2f   x:%.2f   y:%.2f   z:%.2f"%(qua_analog.w,qua_analog.x,qua_analog.y,qua_analog.z))
  print("====================  analog data print end  ====================");
  time.sleep(1)


























