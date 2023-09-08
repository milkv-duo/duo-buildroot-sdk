# -*- coding: utf-8 -*-

#实验效果：读取I2C TCS34725颜色传感器数值
#接线：使用windows或linux电脑连接一块arduino主控板，TCS34725颜色传感器接到I2C口SCL SDA
import time
from pinpong.board import Board,I2C
from pinpong.libs.dfrobot_tcs34725 import TCS34725 #从libs导入tcs34725库

Board("xugu").begin()
i2c = I2C(bus_num=0)
tcs = TCS34725(i2c) #传感器初始化

print("Color View Test!")
while True:
  if tcs.begin(): #查找传感器，读取到则返回True
    print("Found sensor")
    break #找到 跳出循环
  else:
    print("No TCS34725 found ... check your connections")
    time.sleep(1)

while True:
  r,g,b,c = tcs.get_rgbc() #获取rgbc数据
  print(r,g,b,c)
  print("C=%d\tR=%d\tG=%d\tB=%d\t"%(c,r,g,b))
  #数据转换
  if c:
    r /= c 
    g /= c
    b /= c
    r *= 256
    g *= 256
    b *= 256;
    print("------C=%d\tR=%d\tG=%d\tB=%d\t"%(c,r,g,b))

  time.sleep(1)