# -*- coding: utf-8 -*-

#实验效果：读取I2C 10DOF绝对定向传感器
#接线：使用windows或linux电脑连接一块arduino主控板，绝对定向传感器接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_bno055 import BNO055

Board("uno").begin()               #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()      #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin() #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化
bno = BNO055()

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


























