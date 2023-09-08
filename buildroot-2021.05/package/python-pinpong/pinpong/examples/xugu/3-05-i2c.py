# -*- coding: utf-8 -*-

#实验效果：控制I2C 通信协议展示
#接线：连接一个i2c从机设备到arduino,假设i2c地址为0x33
import sys
import time
from pinpong.board import Board,I2C

Board("xugu").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别

i2c = I2C()
#i2c = I2C(bus_num=0)#bus_num 默认值为0，由于arduino uno只有一个编号为0的i2c口

l=i2c.scan()#当前版本未支持,返回[0x33]
print("i2c list:",l)
#i2c.writeto(0x33,[1,2,3,4,5]) #将[1,2,3,4,5]写入i2c设备
#i2c.readfrom(0x33,6)#从i2c设备读取6个数据
#i2c.readfrom_mem(0x33,2,6)#从i2c设备的寄存器2 读取6个数据
#i2c.writeto_mem(0x33,2,[1,2,3,4,5])#向i2c设备的寄存器2 写入[1,2,3,4,5]
while(1):
	time.sleep(1)

