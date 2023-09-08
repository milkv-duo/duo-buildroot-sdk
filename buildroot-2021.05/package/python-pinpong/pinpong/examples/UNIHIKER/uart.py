# -*- coding: utf-8 -*-
import time
from pinpong.board import Board, UART

Board("UNIHIKER").begin()  #初始化，选择板型，不输入板型则进行自动识别
#硬串口1 P0-RX P3-TX
uart1 = UART()   
#初始化串口 baud_rate 波特率, bits 数据位数(8/9) parity奇偶校验(0 无校验/1 奇校验/2 偶校验) stop 停止位(1/2)
# uart1.init(baud_rate = 115200, bits=8, parity=0, stop = 1) 
uart1.init() #默认波特率为9600
buf = [0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07]
#关闭硬串口1
#uart1.deinit() 
#返回可读取的字节数
#uart1.any() 
#串口写,buf为数列
#uart1.write(buf)
#读取串口字符,返回None或者数列
#uart1.read(n)
#读一行，以换行符结尾。读取行或 None 超时。(到换行键(0xa)结束，无则返回None)
#buf = uart1.readline()
#将字节读入buf。如果 nbytes 指定，则最多读取多个字节。否则，最多读取 len(buf) 字节数。
#uart1.readinto(buf, nbytes)
while True:
    uart1.write(buf)
    time.sleep(1)
