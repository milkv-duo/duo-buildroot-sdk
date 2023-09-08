# -*- coding: utf-8 -*-

#实验效果：NFC近场通讯模块 通过UART接口读取卡片信息
#接线：PN532 NFC近场通讯模块接到UART
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_pn532 import PN532_UART

Board("nezha").begin()

nfc = PN532_UART("/dev/ttyS5")

while not nfc.begin():
  print("initial failure")
  time.sleep(1)

while True:
  print("Please place the info card/tag on module..... ")
  if nfc.scan():
    info = nfc.get_information()
    if info != None:
      print("----------------NFC card/tag information-------------------")
      print("UID Length: %d"%info.length)
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