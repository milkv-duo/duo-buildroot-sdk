# -*- coding: utf_8 -*-

import time
from pinpong.board import gboard,UART

class STEP_DRIVER_4460():
  CW=0 #顺时针
  CCW=1 #逆时针
  
  CMD_STATUS = 1 #查询和设置状态
  CMD_RUN = 2    #启动或停止运行
  CMD_MAINTENANCE = 4 #维护
  
  ERR_NONE           = 0 #无错误
  ERR_UART_COMM      = 1 #通讯错误
  ERR_TIME_OUT       = 2 #通讯超时
  ERR_PARAMETER      = 3 #参数错误
  ERR_INVALID_ADDR   = 4 #无效地址
  ERR_INVALID_CMD    = 5 #无效命令
  ERR_INVALID_REQ    = 6 #无效请求
  ERR_MOT_BUSY       = 7 #电机忙碌
  ERR_MOT_TSD        = 8 #过热停机
  ERR_MOT_POWER_DOWN = 9 #电机掉电错误
  

  def __init__(self, board = None, port=None, addr=0xC0):
    self.uart = UART(board=board, tty_name=port,baud_rate=9600)
    self.addr = addr
  def ConvertFixedIntegerToComplement(self,fixedInterger) :#浮点数整数部分转换成补码(整数全部为正)
    return bin(fixedInterger)[2:]
  
  def ConvertFixedDecimalToComplement(self,fixedDecimal) :#浮点数小数部分转换成补码
    fixedpoint = int(fixedDecimal) / (10.0**len(fixedDecimal))
    s = ''
    while fixedDecimal != 1.0 and len(s) < 23 :
      fixedpoint = fixedpoint * 2.0
      s += str(fixedpoint)[0]
      fixedpoint = fixedpoint if str(fixedpoint)[0] == '0' else fixedpoint - 1.0
    return s
  
  def ConvertToExponentMarker(self,number) : #阶码生成
    return bin(number + 127)[2:].zfill(8)
  
  
  def float_to_hex(self,floatingPoint) :#转换成IEEE754标准的数
    floatingPoint = float(floatingPoint)
    floatingPointString = str(floatingPoint)
    if floatingPointString.find('-') != -1 :#判断符号位
      sign = '1'
      floatingPointString = floatingPointString[1:]
    else :
      sign = '0'
    l = floatingPointString.split('.')#将整数和小数分离
    front = self.ConvertFixedIntegerToComplement(int(l[0]))#返回整数补码
    rear  = self.ConvertFixedDecimalToComplement(l[1])#返回小数补码
    floatingPointString = front + '.' + rear #整合
    relativePos =   floatingPointString.find('.') - floatingPointString.find('1')#获得字符1的开始位置
    if relativePos > 0 :#若小数点在第一个1之后
      exponet = self.ConvertToExponentMarker(relativePos-1)#获得阶码
      mantissa =  floatingPointString[floatingPointString.find('1')+1 : floatingPointString.find('.')]  + floatingPointString[floatingPointString.find('.') + 1 :] # 获得尾数
    else :
      exponet = self.ConvertToExponentMarker(relativePos)#获得阶码
      mantissa = floatingPointString[floatingPointString.find('1') + 1: ]  # 获得尾数
    mantissa =  mantissa[:23] + '0' * (23 - len(mantissa))
    floatingPointString = '0b' + sign + exponet + mantissa
    rslt = int( floatingPointString , 2 )
    return rslt
  
  def hex_to_float(self,v):
    sign = -1 if v&0x80000000 else 1
    exp = ((v&0x7F800000) >> 23)-127
    frac0 = v&0x007FFFFF
    frac0_str = str(bin(frac0)).replace("0b","")
    frac0_str = '0'*(23-len(frac0_str))+frac0_str
    if exp >= 0:
      v=['1'+frac0_str[0:exp],frac0_str[exp:]]
    else:
      v=['0','0'*(abs(exp)-1)+'1'+frac0_str]
    rslt = int(v[0],2)
    fact = 0.5
    for i in v[1]:
      rslt = rslt + fact*int(i)
      fact = fact/2
    return rslt*sign

  def checksum(self,data):
    cs = 0
    for i in range(len(data)-2):
      cs = cs + data[i+2]
    return cs&0xFF

  def pack(self, addr, cmd, data_len, data):
    packet = [0xFE, 0xFE, 0x68, addr, 0x68, cmd, data_len]+data
    packet.append(self.checksum(packet))
    packet.append(0x16)
    return packet

  def ndelay(self, n, s):
    count = 0
    for i in range(int(s/0.01)):
      if self.uart.any() != n:
        time.sleep(0.01)
        count = count + 1
        if count == 100:
          return
      else:
        return

  def get_status(self):
    packet = self.pack(self.addr, self.CMD_STATUS, data_len = 2, data=[self.addr, 3])
    self.uart.write(packet)
    self.ndelay(12,2)
    rslt = self.uart.readall()
    return rslt[9]

  def get_error(self):
    packet = self.pack(self.addr, self.CMD_STATUS, data_len = 3, data = [self.addr, 1, 0])
    self.uart.write(packet)
    self.ndelay(12,2)
    rslt = self.uart.readall()
    return rslt[9]

  '''
  def set_motor_dir(self,dir=CW):
    self.dir = dir
    return 0
  '''

  def get_motor_dir(self):
    packet = self.pack(self.addr, self.CMD_STATUS, data_len = 2, data=[self.addr, 5])
    self.uart.write(packet)
    self.ndelay(12,2)
    #print("self.uart.any()=",self.uart.any())
    rslt = self.uart.readall()
    return rslt[9]

  #获取电机位置
  def get_motor_pos(self):
    packet = self.pack(self.addr, self.CMD_STATUS, data_len = 2, data=[self.addr, 7])
    self.uart.write(packet)
    self.ndelay(15,2)
    rslt = self.uart.readall()
    return rslt[9],rslt[10],rslt[11],rslt[12]

  #设置当前未知为起点
  def set_origin(self):
    packet = self.pack(self.addr, self.CMD_STATUS, data_len = 2, data=[self.addr, 9])
    self.uart.write(packet)
    self.ndelay(11,2)
    rslt = self.uart.readall()
    return rslt[8] == 0xA

  def get_speed(self):
    packet = self.pack(self.addr, self.CMD_STATUS, data_len = 2, data=[self.addr, 0xB])
    self.uart.write(packet)
    self.ndelay(15,2)
    rslt = self.uart.readall()
    return rslt[9],rslt[10],rslt[11],rslt[12]

  #FE FE 68 C0 68 02     0E        C0    01      01 00   
  #               00 00 80 3F    00 00 C8 42     05 00      30 16
  def set_speed(self, speed, direction):
    if speed == 0:
      return self.stop()
    speed_hex = self.float_to_hex(speed)
    self.speed_hex = speed_hex
    packet = self.pack(self.addr, self.CMD_RUN, data_len = 0xE, data=[self.addr, 0x1, 0x1, direction,
    0x00,0x00,0x80,0x3F,    (self.speed_hex)&0xFF,(self.speed_hex>>8)&0xFF,(self.speed_hex>>16)&0xFF,(self.speed_hex>>24), 5, 0x00])
    self.uart.write(packet)
    self.ndelay(12, 1)
    #print("self.uart.any()=",self.uart.any())
    rslt = self.uart.readall()
    return rslt[9]

  def set_steps(self, steps, direction):
    packet = self.pack(self.addr, self.CMD_RUN, data_len = 0x13, data=[self.addr, 0x3, 0x1, direction,
      0xC8,0x00,0x00,0x00,    0x00,0x00,0x80,0x3F, 
      (self.speed_hex)&0xFF,(self.speed_hex>>8)&0xFF,(self.speed_hex>>16)&0xFF,(self.speed_hex>>24), 0x1, 0x1, 0x00])
    self.uart.write(packet)
    time.sleep(0.05)
    rslt = self.uart.readall()
    return rslt[9]

  def stop(self):
    packet = self.pack(self.addr, self.CMD_RUN, data_len = 2, data=[self.addr, 7])
    self.uart.write(packet)
    self.ndelay(11,2)
    rslt = self.uart.readall()
    return rslt[8] == 0x8

  def maintenance(self):
    return 0

  def set_max_speed(self,max_speed):
    packet = self.pack(self.addr, self.CMD_MAINTENANCE, data_len = 2, data = [self.addr, 0x19])
    self.uart.write(packet)
    self.ndelay(15, 2)
    return self.uart.readall()

    self.max_speed = max_speed
    return 0

  def get_max_speed(self):
    packet = self.pack(self.addr, self.CMD_MAINTENANCE, data_len = 2, data = [self.addr, 0x19])
    self.uart.write(packet)
    self.ndelay(15,2)
    #print("self.uart.any()=",self.uart.any())
    rslt = self.uart.readall()
    return rslt[9],rslt[10],rslt[11],rslt[12]

  def set_addr(self):
    return 0

  def get_version(self):
    packet = self.pack(self.addr, self.CMD_MAINTENANCE, data_len = 2, data = [self.addr, 0x11])
    self.uart.write(packet)
    self.ndelay(14,2)
    #print("self.uart.any()=",self.uart.any())
    rslt = self.uart.readall()
    return rslt[9],rslt[10],rslt[11]

  def set_default(self):
    return 0
