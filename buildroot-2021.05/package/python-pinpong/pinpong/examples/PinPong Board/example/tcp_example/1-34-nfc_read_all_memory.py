# -*- coding: utf-8 -*-

#实验效果：NFC近场通讯模块 IIC读取卡片内信息
#接线：使用windows或linux电脑连接一块arduino主控板，NFC近场通讯模块接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_pn532 import PN532

ip = "192.168.0.90"
port = 8081

Board(ip, port)

nfc = PN532()

def print_data(block):
  value = nfc.read_data(block)
  if  value != None:
    for i in value:
      print("0x%x "%(i), end="")
    print("")
  else:
    print_data(block)

while not nfc.begin():
  print("initial failure")
  time.sleep(1)
print("Waiting for a card......")

while True:
  if nfc.scan():
    NFC = nfc.get_information()
    if NFC != None:
      if NFC.lenght == 0x02 or NFC.lenght == 0x04:
        print("----------------Here is the card information to read-------------------")
        for i in range(NFC.num_block):
          if i == 0:
            print("Block %d:UID0-UID3/MANUFACTURER--------->"%(i), end="")
            print_data(i);
          elif (i+1)%4==0 and i < 128:
            print("Block %d:KEYA/ACCESS/KEYB--------------->"%(i), end="")
            print_data(i)
          elif (i+1)%16==0 and i > 127:
            print("Block %d:KEYA/ACCESS/KEYB--------------->"%(i), end="")
            print_data(i)
          else:
            print("Block %d:DATA   ------------------------>"%(i), end="")
            print_data(i)
      else:
        print("The card type is not mifareclassic...")
  time.sleep(3)