# -*- coding: utf-8 -*-

#实验效果：PN532 NFC近场通讯模块 通过UART接口读写卡片信息
#接线：PN532 NFC近场通讯模块接到UART5（用户可自己配置）
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_pn532 import PN532_UART

Board("nezha").begin()

nfc = PN532_UART("/dev/ttyS5")

write_data = "NFC"
#write_data = [1, 2, 3, 4, 5, 6, 7, 8, 9]
#write_data = (10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

block_num = 2

while not nfc.begin():
  print("initial failure")
  time.sleep(1)
print("Waiting for a card......")

def parse_data(read_data):
  if read_data != None:
    print("read success! data is ", end=" ")
    print(read_data)
  else:
    print("read failure!")

while True:
  if nfc.scan():
    info = nfc.get_information()
    if info != None:
      if info.length == 0x02 or info.length == 0x04:
        if not nfc.write_data(block_num, write_data):
          print("write failure!")
        else:
          print("write success! data is", end=" ")
          print(write_data)
        read_data= nfc.read_data(block_num)
        parse_data(read_data)
      else:
        print("The card type is not mifareclassic...")
  time.sleep(2)
