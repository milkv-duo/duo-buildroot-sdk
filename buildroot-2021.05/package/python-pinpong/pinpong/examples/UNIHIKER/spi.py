import time
from pinpong.board import Board, Pin, SPI

Board("UNIHIKER").begin()  #初始化，选择板型，不输入板型则进行自动识别

#0代表SPI0(P1 SCK, P10 MISO, P2 MOSI), 1代表SPI1(P13 SCK, P14 MISO, P15 MOSI)
#cs为选片引脚, 只能做主机
spi0 = SPI(0, cs = Pin.P3)

w_buf = [0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07]

while True:
    # r_buf = spi0.read(8)    #向从机读取8个字节
    # print(r_buf)
    #spi0.write(0x88)         #spi0发送单字节
    spi0.write(w_buf)         #spi0发送buf
    time.sleep(1)


