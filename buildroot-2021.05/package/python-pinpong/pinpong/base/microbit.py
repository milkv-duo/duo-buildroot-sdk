# -*- coding: utf-8 -*-
import os
import re
import sys
import subprocess
import time

from pinpong.base.programmer import *

class Microbit(Programmer):
  def __init__(self, port="/dev/ttyS1", baudrate=115200):
    super().__init__(port, baudrate)

  def setup(self):
    print("Microbit setup %s"%self.port)
    
  def initialize(self):
    global logger
    print("initialize")

  def display(self):
    if sys.platform == 'win32':
      try:
        disks = subprocess.Popen(
        "wmic logicaldisk get deviceid, description", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read().decode('utf-8').split("\n")
      except Exception:
        disks = subprocess.Popen(
        "wmic logicaldisk get deviceid, description", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read().decode('gbk').split("\n")
      for disk in disks:
        if 'Removable' in disk or '可移动磁盘' in disk:
          d=re.search(r'\w:', disk).group()
          diskname = subprocess.Popen(
      "wmic logicaldisk where name='%s' get volumename"%(d), shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read().decode('utf-8').split("\n")
          if "MICROBIT" in diskname[1]:
            self.mount_point = d+"/"
    elif sys.platform == 'linux':
      message=""
      with open('/proc/mounts', 'r') as f:
        while True:
          l = f.readline()
          if l == "":
            break
          elif "MICROBIT" in l:
            message=l
        if message != "":
          self.mount_point = message.split(" ")[1]+"/"
    elif sys.platform == 'darwin':
      result = subprocess.Popen(
      "ls /Volumes", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read().decode('utf-8').split()
      if 'MICROBIT' in result:
        self.mount_point = "/Volumes/MICROBIT/"
      else:
        self.mount_point = None
    return True if self.mount_point else False

  def read_sig_bytes(self):
    pass

  def enable(self):
    pass

  def program_enable(self):
    global logger
    pass

  def burn(self):
    if self.mount_point is not None:
      if sys.platform == "win32":
        self.filename = self.filename.replace("\\","/")
        filename = self.filename[self.filename.rfind("/")+1:]
        path = self.filename[0:self.filename.rfind("/")+1]
        cmd = "robocopy " + path + " " + self.mount_point + " " + filename + " /NFL /NDL /NJH /NJS /nc /ns /np"  #debug 不显示复制信息
        os.system(cmd)
        
      else:
        if 'V2' in self.filename:
          os.system("cp "+ self.filename +" " + self.mount_point)     #debug 
          time.sleep(14)
        else:
          os.system("cp "+ self.filename +" " + self.mount_point)
          time.sleep(9)
  def disable(self):
    pass