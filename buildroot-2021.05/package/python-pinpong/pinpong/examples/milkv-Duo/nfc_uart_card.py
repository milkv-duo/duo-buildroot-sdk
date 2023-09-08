# -*- coding: utf-8 -*-

#实验效果：NFC近场通讯模块 IIC读取卡片信息

import time
from pinpong.board import Board
from pinpong.libs.dfrobot_pn532 import PN532_UART

Board("MILKV-DUO").begin()  #初始化
nfc = PN532_UART("/dev/ttyS4")

while not nfc.begin():
  print("initial failure")
  time.sleep(1)
print("Please place the info card/tag on module..... ")

while True:
  if nfc.scan():
    info = nfc.get_information()
    if info != None:
      print("----------------NFC card/tag information-------------------")
      print("UID Lenght: %d"%info.length)
      print("UID: %x %x %x %x"%(info.uid[0],info.uid[1],info.uid[2],info.uid[3] ))
      print("AQTA: %x %x"%(info.AQTA[0], info.AQTA[0]))
      print("SAK: 0x%x"%(info.sak))
      print("Type: %s"%(info.types))
      print("Manu facturer: %s"%(info.manu))
      print("RF Technology: %s"%(info.RF))
      print("Memory Size: %d bytes(total)/%d bytes(available)"%(info.size_total, info.size_available))
      print("Block/Page Size: %d bytes"%(info.block))
      print("Number of Blocks/pages: %d"%(info.num_block))
  time.sleep(1)