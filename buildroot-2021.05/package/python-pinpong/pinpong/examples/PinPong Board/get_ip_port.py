# -*- coding: utf-8 -*-

from pinpong.board import Board,Pin,WIFI,oled12864

def func():
  obj.connect_wifi("dfrobotOffice", "dfrobot2011")
  print("Waiting for WIFI connection...")
  while True:
    ip = obj.get_ip_port()
    if ip != None:
      break
  oled.text(1, ip)

Board("PinPong Board").begin()
oled=oled12864()
obj = WIFI()
func()
print(obj.get_ip_port())