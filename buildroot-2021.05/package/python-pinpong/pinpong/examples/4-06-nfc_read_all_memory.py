# -*- coding: utf-8 -*-

#实验效果：NFC近场通讯模块 IIC读取卡片内信息
#接线：使用windows或linux电脑连接一块arduino主控板，NFC近场通讯模块接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_pn532 import PN532_I2C

Board("uno").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("uno","COM36").begin()  #windows下指定端口初始化
#Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

nfc = PN532_I2C()

def print_data(block):
  value = nfc.read_data(block)
  if  value != None:
    print(value)
  else:
    print_data(block)

while not nfc.begin():
  print("initial failure")
  time.sleep(1)
print("Waiting for a card......")

while True:
  if nfc.scan():
    card = nfc.get_information()
    if card != None:
      if card.length == 0x02 or card.length == 0x04:
        print("----------------Here is the card information to read-------------------")
        for i in range(card.num_block):
          if i == 0:
            print("Block %d:UID0-UID3/MANUFACTURER--------->"%(i), end="")
            print_data(i)
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