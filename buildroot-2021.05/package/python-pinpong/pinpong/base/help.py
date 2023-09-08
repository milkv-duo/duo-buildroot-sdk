# -*- coding: utf-8 -*-
import sys, getopt
import serial
import os
import json
import platform
import serial.tools.list_ports
from pinpong.base.comm import *

def main():
  argc = len(sys.argv)
  cwdpath,_ = os.path.split(os.path.realpath(__file__))

  with open(cwdpath+'/../libs/libs.json', 'r', encoding='UTF-8') as f:
    descs = json.loads(f.read())
  if argc == 1:
    argc = 2
    sys.argv.append("help")
  cmd = sys.argv[1]
  if cmd == "help" and argc == 2:
    printlogo_big()
    version = sys.version.split(' ')[0]
    plat = platform.platform()
    print("[1]Environment information: Python"+version+"  "+plat+"\n")
    print("[2]Document URL: "+"\n")
    print("Main Station:https://pinpong.readthedocs.io"+"\n")
    print("Mirror station:https://pinpong.gitee.io"+"\n")
    print("[3]Commands:")
    print("   pinpong              Help information for pinpong library")
    print("   pinpong libs list    List of pinpong libraries")
    print("   pinpong libs xxx     How to use the xxx library\n")
    print("[4]Serial ports list:")
    plist = list(serial.tools.list_ports.comports())
    for port in plist:
      print("  ",port)
  elif cmd == "libs" and argc == 3:
    arg = sys.argv[2]
    if arg == "list":
      print("\n[-] Libs list:")
      items = descs.items()
      for key,_ in items:
        print(str(key).lower())
    else:
      if arg.upper() in descs:
        print("\n[-] How to import?: ")
        print(descs[arg.upper()]["import"])
        print("\n[-] API list ")
        print(descs[arg.upper()]["api"])
      else:
        print("[Err] Unknown lib: ",arg)
  else:
    print("\n[Err] Unknown command:", sys.argv[1])
    print("\n[-] Available commands")
    print("  pinpong              Help information for pinpong library")
    print("  pinpong libs list    List of modules currently supported by the pinpong library")
    print("  pinpong libs xxx     How to use the xxx module")
