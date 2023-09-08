# -*- coding: utf-8 -*
import time
from pinpong.board import gboard,I2C
import math

I2C_MODE                  = 0x01
UART_MODE                 = 0x02
DEV_ADDRESS               = 0x0020
DEVICE_ADDRESS            = 0x20

class DFRobot_BloodOxygen():
  '''!
    @brief This is the base class of the heart rate and oximeter sensor.
  '''
  SPO2 = 0
  heartbeat = 0
  pSPO2 = 0
  pheartbeat = 0
  START_MODE = 2
  END_MODE = 3
  BAUT_RATE_1200 = 0 
  BAUT_RATE_2400 = 1
  BAUT_RATE_9600 = 3
  BAUT_RATE_19200 = 5
  BAUT_RATE_38400 = 6 
  BAUT_RATE_57600 = 7
  BAUT_RATE_115200 = 8   
    
  def __init__(self):
        pass  

  def begin(self):
    '''!
      @brief   Begin function, check sensor connection status
      @return  Return init status
      @retval True Init succeeded
      @retval False Init failed
    '''
    global DEV_ADDRESS
    rbuf = self.read_reg(0x04, 2)
    if (rbuf[0] & 0xff << 8 | rbuf[1]) == DEV_ADDRESS:
      return True
    else:
      return False

  def sensor_start_collect(self):
    '''!
      @brief   Sensor starts to collect data
    '''
    wbuf = [0x00,0x01]
    self.write_reg(0x20, wbuf)
    
  def sensor_end_collect(self):
    '''!
      @brief   Sensor ended collecting data
    '''
    wbuf = [0x00,0x02]
    self.write_reg(0x20, wbuf)

  def set_bautrate(self,bautrate):
    '''!
      @brief   Change serial baud rate
      @param bautrate
      @n     BAUT_RATE_1200 
      @n     BAUT_RATE_2400
      @n     BAUT_RATE_9600
      @n     BAUT_RATE_19200
      @n     BAUT_RATE_38400
      @n     BAUT_RATE_57600
      @n     BAUT_RATE_115200
    '''
    w_buf = [0,bautrate]
    self.write_reg(0x1C, w_buf)

  def get_heartbeat_SPO2(self):
    '''!
      @brief Get heart rate and oxygen saturation and store them into the struct  sHeartbeatSPO2
    '''
    rbuf = self.read_reg(0x0C,8)
    self.SPO2 = rbuf[0]
    if self.SPO2 == 0:
      self.SPO2 = -1
    self.heartbeat = int(rbuf[2]) << 24 | int(rbuf[3]) << 16 | int(rbuf[4]) << 8 | int(rbuf[5])
    if self.heartbeat == 0:
      self.heartbeat = -1

  def get_spo2(self):
    rbuf = self.read_reg(0x0C,8)
    self.SPO2 = rbuf[0]
    if self.SPO2 == 0:
      self.SPO2 = -1
    self.heartbeat = int(rbuf[2]) << 24 | int(rbuf[3]) << 16 | int(rbuf[4]) << 8 | int(rbuf[5])
    if self.heartbeat == 0:
      self.heartbeat = -1
    return self.SPO2
  
  def get_heartbeat(self):
    rbuf = self.read_reg(0x0C,8)
    self.SPO2 = rbuf[0]
    if self.SPO2 == 0:
      self.SPO2 = -1
    self.heartbeat = int(rbuf[2]) << 24 | int(rbuf[3]) << 16 | int(rbuf[4]) << 8 | int(rbuf[5])
    if self.heartbeat == 0:
      self.heartbeat = -1
    return self.heartbeat

  def get_temperature_c(self):
    '''!
      @brief   Get the sensor board temp
      @return  Return board temp
    '''
    temp_buf = self.read_reg(0x14, 2)
    Temperature = temp_buf[0] * 1.0 + temp_buf[0] / 100.0
    return Temperature

  def get_bautrate(self):
    '''!
      @brief   Get serial baud rate of the sensor
      @return  Return serial baud rate of the sensor
    '''
    r_buf = self.read_reg(0x1C,2)
    baudrate_type = int(r_buf[0]) << 8 | int(r_buf[1])
    if(baudrate_type == 0):
      return 1200
    elif(baudrate_type == 1):
      return 2400 
    elif(baudrate_type == 3):
      return 9600 
    elif(baudrate_type == 5):
      return 19200 
    elif(baudrate_type == 6):
      return 38400 
    elif(baudrate_type == 7):
      return 57600 
    elif(baudrate_type == 8):
      return 115200 
    else:
      return 9600
        
class DFRobot_BloodOxygen_S(DFRobot_BloodOxygen):
  '''
    @brief An example of an i2c interface module
  '''
  def __init__(self, board = None, i2c_addr = 0x57, bus_num=0):
    if isinstance(board, int):
        i2c_addr = board
        board = gboard
    elif board is None:
        board = gboard
    self.__addr = i2c_addr
    self._i2c = I2C(bus_num)
    super(DFRobot_BloodOxygen_S, self).__init__()    
    
  def write_reg(self, reg_addr, data_buf):
    '''
      @brief writes data to a register
      @param reg register address
      @param value written data
    '''
    self._i2c.writeto_mem(self.__addr ,reg_addr ,data_buf)

  def read_reg(self, reg_addr ,length):
    '''
      @brief read the data from the register
      @param reg register address
      @param value read data
    '''
    rslt = self._i2c.readfrom_mem(self.__addr ,reg_addr , length)
    return rslt