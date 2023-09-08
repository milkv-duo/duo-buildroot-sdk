import time
from pinpong.board import gboard,I2C

class DFRobot_IIC_Serial:
  ## Global control register, control sub UART clock 
  REG_WK2132_GENA = 0x00  
  ## Global sub UART reset register, reset a sub UART independently through software
  REG_WK2132_GRST = 0x01  
  ## Global main UART control register, and will be used only when the main UART is selected as UART, no need to be set here.
  REG_WK2132_GMUT = 0x02  
  ## Global interrupt register, control sub UART total interrupt.
  REG_WK2132_GIER = 0x10  
  ## Global interrupt flag register, only-read register: indicate if there is a interrupt occuring on a sub UART.
  REG_WK2132_GIFR = 0x11  
  ## sub UART page control register
  REG_WK2132_SPAGE = 0x03  
  ## Sub UART control register
  REG_WK2132_SCR  = 0x04 
  ## Sub UART configuration register 
  REG_WK2132_LCR  = 0x05  
  ## Sub UART FIFO control register
  REG_WK2132_FCR  = 0x06  
  ## Sub UART interrupt enable register
  REG_WK2132_SIER = 0x07 
  ## Sub UART interrupt flag register  
  REG_WK2132_SIFR = 0x08
  ## Sub UART transmit FIFO register, OR register  
  REG_WK2132_TFCNT= 0x09  
  ## Sub UART transmit FIFO register, OR register 
  REG_WK2132_RFCNT= 0x0A  
  ## Sub UART FIFO register, OR register 
  REG_WK2132_FSR  = 0x0B 
  ## Sub UART receive register, OR register  
  REG_WK2132_LSR  = 0x0C  
  ## Sub UART FIFO data register
  REG_WK2132_FDAT = 0x0D   
  ## Sub UART band rate configuration register high byte
  REG_WK2132_BAUD1 = 0x04  
  ## Sub UART band rate configuration register low byte
  REG_WK2132_BAUD0 = 0x05 
  ## Sub UART band rate configuration register decimal part 
  REG_WK2132_PRES  = 0x06  
  ## Sub UART receive FIFO interrupt trigger configuration register
  REG_WK2132_RFTL  = 0x07  
  ## Sub UART transmit FIFO interrupt trigger configuration register
  REG_WK2132_TFTL  = 0x08 
  ## Sub UART channel1 
  SUBUART_CHANNEL_1   = 0x00
  ## Sub UART channel2   
  SUBUART_CHANNEL_2   = 0x01
  ## All sub channels   
  SUBUART_CHANNEL_ALL = 0x11 
  ## The 4th and 3rd bits of IIC address are fixed, value 1 and 0 respectively   
  IIC_ADDR_FIXED      = 0x10   
  
  SERIAL_RX_BUFFER_SIZE = 32
  IIC_BUFFER_SIZE = 64
  
  ERR_OK           =    0
  ERR_REGDATA      =   -1
  ERR_READ         =   -2
  FOSC             =   14745600 #External cystal frequency 14.7456MHz
  OBJECT_REGISTER  =   0x00      #Register object 
  OBJECT_FIFO      =   0x01      #FIFO buffer object 
  
  
  '''
    Data format: N for no parity, Z for 0 parity, O for Odd parity, E for Even parity, F for 1 parity. 
    8 represents the number of data bit, 1 or 2 for the number of stop bit.
  '''
  IIC_Serial_8N1 = 0x00
  IIC_Serial_8N2 = 0x01
  IIC_Serial_8Z1 = 0x08
  IIC_Serial_8Z2 = 0x09
  IIC_Serial_8O1 = 0x0A
  IIC_Serial_8O2 = 0x0B
  IIC_Serial_8E1 = 0x0C
  IIC_Serial_8E2 = 0x0D
  IIC_Serial_8F1 = 0x0E
  IIC_Serial_8F2 = 0x0F
  
  
  eNormalMode = 0
  eNormal = 0
  
  _rx_buffer = [0]*SERIAL_RX_BUFFER_SIZE
  '''
    # LCR description of WK2132 sub UART configuration register:
    # -------------------------------------------------------------------------
    # |   b7   |   b6   |   b5   |   b4   |   b3   |   b2   |   b1   |   b0   |
    # -------------------------------------------------------------------------
    # |        RSV      |  BREAK |  IREN  |  PAEN  |      PAM        |  STPL  |
    # -------------------------------------------------------------------------
  '''
  ## Sub UART data format: PAEN sub UART check enable bit, 1-with parity bit(9-bits data), 0-no parity bit (8-bits data) PAM sub UART check mode selection bit, take effect when PAEN bit is 1, 00-0 parity(default), 01-Odd parity10-Even parity, 11-1 parity, STPL sub UART stop bit length, 0-1bit, 1-2bits, etc.
  sLcrReg_format =  0x0f 
  ## Sub UART IR enable bit, 0-normal mode, 1-IR mode   
  sLcrReg_irEn   =  1 << 4  
  ## Sub UART Line-Break output control bit, 0-output normally, 1-Line-Break output (TX force output 0)
  sLcrReg_lBreak =  1 << 5  
  ## Reserved bit
  sLcrReg_rsv    =  1 << 6  
  
  sIICAddr_type  = 1 << 0
  sIICAddr_uart  = 1 << 1
  sIICAddr_addrPre = 0x1F << 3
  ## Sub UART transmit TX busy flag bit, 0-sub UART transmit TX null, 1-sub UART transmit TX busy
  sFsrReg_tBusy = 1 << 0  
  ## Sub UART transmit TX full flag bit, 0-sub UART transmit FIFO not full, 1-sub UART transmit FIFO full
  sFsrReg_tFull = 1 << 1  
  ## Sub UART transmit FIFO null flag, 0-sub UART transmit FIFO null, 1-sub UART transmit FIFO not null
  sFsrReg_tDat = 1 << 2   
  ## Sub UART receive FIFO null flag, 0-sub UART receive FIFO null, 1-sub UART receive FIFO not null
  sFsrReg_rDat = 1 << 3   
  ## Sub UART receive FIFO data check error flag bit, 0- no PE error, 1-PE error
  sFsrReg_rFpe = 1 << 4   
  ## Sub UART receive FIFO data frame error flag bit, 0-no FE error, 1-FE error
  sFsrReg_rFfe = 1 << 5  
  ## Sub UART receive FIFO data Line-break error, 0-no Line-Break error, 1-Line-Break error 
  sFsrReg_rFbi = 1 << 6   
  ## Sub UART receive FIFO data overflow error flag bit, 0-no OE error, 1-OE error
  sFsrReg_rFoe = 1 << 7   
  
  
  STA_OK = 0x00
  STA_ERR = 0x01
  STA_ERR_DEVICE_NOT_DETECTED = 0x02
  STA_ERR_SOFT_VERSION = 0x03
  STA_ERR_PARAMETER = 0x04
  
  ## last operate status, users can use this variable to determine the result of a function call.
  last_operate_status = STA_OK
  
  def __init__(self, sub_uart_channel=0x00, IA1 = 1, IA0 =1, bus_num=0):
    self._sub_serial_channel = sub_uart_channel
    self._addr = (IA1 << 6) | (IA0 << 5) | self.IIC_ADDR_FIXED
    self._rx_buffer_head = 0
    self._rx_buffer_tail = 0
    self._i2c = I2C(bus_num)

  def begin(self, baud, format = IIC_Serial_8N1):
    '''!
      @brief Init function, set sub UART band rate, data format 
      @param baud: baud rate, it support: 9600, 57600, 115200, 2400, 4800, 7200,
      @n     14400, 19200, 28800,38400, 76800, 153600, 230400, 460800, 307200, 921600
      @param format: Data format, it support:
      @n     IIC_SERIAL_8N1, IIC_SERIAL_8N2, IIC_SERIAL_8Z1,IIC_SERIAL_8Z2
      @n     IIC_SERIAL_8O1, IIC_SERIAL_8O2, IIC_SERIAL_8E1, IIC_SERIAL_8E2
      @n     IIC_SERIAL_8F1, IIC_SERIAL_8F2
      @return Return 0 if it sucess, otherwise return non-zero
    '''
    return self._begin(baud, format, self.eNormalMode, self.eNormal)
  
  def end(self):
    '''!
      @brief Release sub UART to clean up all registers in Sub UART. Call function begin() again to make it work.
    '''
    self._sub_serial_global_reg_enable(self._sub_serial_channel, 1)
  
  def printf(self, *args, **kargs):
    '''!
      @brief The Prints the values to a stream, usage is the same as print function.
      @param args
      @param kargs
      @n sep: string inserted between values, default a space.
      @n end: string appended after the last value, default a newline.
    '''
    sep = kargs.pop('sep',' ')
    end = kargs.pop('end','\n')
    if kargs:raise TypeError('extra keywords:%s'%kargs)
    first = True
    output = ''
    for arg in args:
      output += ('' if first else sep) + str(arg)
      first = False
      #print(arg)
    output += end
    self.write(output)

  def available(self):
    '''!
      @brief Get the number of bytes in receive buffer, it should be the total number of bytes in FIFO
      @n receive buffer(256B) and self-defined _rx_buffer(31B).
      @return Return the number of bytes in receive buffer
    '''
    l = self._read_bytes(self.REG_WK2132_RFCNT, 1)
    index = 0
    if len(l) != 1:
      return 0
    index = int(l[0])
    if index == 0:
      fsr = self._read_fifo_state_reg()
      if (fsr & self.sFsrReg_rDat) > 0:
        index = 256
    
    return (index + (self.SERIAL_RX_BUFFER_SIZE + self._rx_buffer_head - self._rx_buffer_tail)% self.SERIAL_RX_BUFFER_SIZE)
    
  def peek(self):
    '''!
      @brief Return the data of 1 byte without deleting the data in the receive buffer
      @return Return the readings
    '''
    num = self.available() - ((self.SERIAL_RX_BUFFER_SIZE + self._rx_buffer_head - self._rx_buffer_tail) % self.SERIAL_RX_BUFFER_SIZE)
    i = 0
    while i < num:
      j = (self._rx_buffer_head + 1) % self.SERIAL_RX_BUFFER_SIZE
      if j != self._rx_buffer_tail:
        l = self._read_bytes(self.REG_WK2132_FDAT, 1)
        self._rx_buffer[self._rx_buffer_head] = chr(l[0])
        self._rx_buffer_head = j
        i += 1
      else:
        break
    if self._rx_buffer_head == self._rx_buffer_tail:
      return ''
    return self._rx_buffer[self._rx_buffer_tail]
        
  def read(self, size = 1):
    '''!
      @brief Read size bytes from the serial port, this operation will delete the data in the buffer.
      @param size: the bytes of read
      @return less characters as requested.
    '''
    num = self.available() - ((self.SERIAL_RX_BUFFER_SIZE + self._rx_buffer_head - self._rx_buffer_tail) % self.SERIAL_RX_BUFFER_SIZE)
    i = 0
    k = 0
    r = ""
    if size > self.available():
      size = self.available()
    
    while i < num or k < size:
      j = (self._rx_buffer_head + 1) % self.SERIAL_RX_BUFFER_SIZE
      if k < size and self._rx_buffer_tail != self._rx_buffer_head:
        r += self._rx_buffer[self._rx_buffer_tail]
        self._rx_buffer_tail = (self._rx_buffer_tail + 1) % self.SERIAL_RX_BUFFER_SIZE
        k += 1
        
      if i < num and j != self._rx_buffer_tail:
        l = self._read_bytes(self.REG_WK2132_FDAT, 1)
        if len(l) != 1:
          return -1
        self._rx_buffer[self._rx_buffer_head] = chr(l[0])
        self._rx_buffer_head = j
        i += 1
      if k >= size and j != self._rx_buffer_tail:
        #print("ok")
        break
    try:
      return ord(r)
    except:
      return str(r)

  def flush(self):
    '''!
      @brief Wait for the data to be transmited completely
    '''
    fsr = self._read_fifo_state_reg()
    while fsr & sFsrReg_tDat > 0:
      time.sleep(0.001)

  def write_reg(self, value):
    if isinstance(value, list):
      d = value
    else:
      d = 0
      try:
        d = to_bytes(value)
      except:
        d = str(value)
      d = [ord(x) for x in d]
    tx_len = length = len(d)
    n = 0
    while tx_len > 0:
      try:
        fsr = self._read_fifo_state_reg()
        if fsr & self.sFsrReg_tFull > 0:
          print("FIFO full")
          return length - len(d[n:])
        
        self._write_bytes(self.REG_WK2132_FDAT, [d[n]])
        n += 1
        tx_len -= 1
      except:
        pass
    return length - len(d[n:])

  def write(self, value):
    if isinstance(value, list):
      d = value
    else:
      d = 0
      try:
        d = to_bytes(value)
      except:
        d = str(value)
      d = [ord(x) for x in d]
    tx_len = length = len(d)
    n = 0
    fsr = self._read_fifo_state_reg()
    self.writeFIFO(d)

  def writeFIFO(self, buf):
    self.last_operate_status = self.STA_ERR_DEVICE_NOT_DETECTED
    self._addr = self._update_addr(self._addr, self._sub_serial_channel, self.OBJECT_FIFO)
    self._i2c.writeto(self._addr, buf)

  def _read_fifo_state_reg(self):
    l = self._read_bytes(self.REG_WK2132_FSR, 1)
    if len(l) != 1:
      return 0
    else:
      return l[0]

  def _begin(self, baud, format, mode, opt):
    '''!
      @brief Init function, set the band rate of sub UART, data format, communication mode, and Line-Break output
      @param baud: baud rate, it support: 9600, 57600, 115200, 2400, 4800, 7200,
      @n     14400, 19200, 28800,38400, 76800, 153600, 230400, 460800, 307200, 921600
      @param format: Data format, it support:
      @n     IIC_SERIAL_8N1, IIC_SERIAL_8N2, IIC_SERIAL_8Z1,IIC_SERIAL_8Z2
      @n     IIC_SERIAL_8O1, IIC_SERIAL_8O2, IIC_SERIAL_8E1, IIC_SERIAL_8E2
      @n     IIC_SERIAL_8F1, IIC_SERIAL_8F2
      @param mode: Sub UART communciation mode, can set to UART mode, all enumeration values in eCommunicationMode_t
      @param opt: Sub UART Line-Break output control bit, can set to normal output (0) and Line-Break output (1),
      @n all enumeration values in eLineBreakOutput_t or 0/1
      @return Return 0 if init succeeds, otherwise return non-zero 
    '''
    self._rx_buffer_head = 0
    self._rx_buffer_tail = 0
    val = 0
    channel = self._sub_serial_channel_switch(self.SUBUART_CHANNEL_1)
    l = self._read_bytes(self.REG_WK2132_GENA, 1)
    
    if len(l) != 1:
      print("READ BYTE ERROR!")
      return self.ERR_READ

    if l[0] & 0x80 == 0:
      print("Read REG_WK2132_GENA  ERROR!")
      return self.ERR_REGDATA
    self._sub_serial_channel_switch(channel)
    self._sub_serial_config(self._sub_serial_channel)
    self._set_sub_serial_baudrate(baud)
    self._set_sub_serial_config_reg(format, mode, opt)
    return self.ERR_OK

  def _sub_serial_config(self, sub_uart_channel):
    self._sub_serial_global_reg_enable(sub_uart_channel, 0)
    self._sub_serial_global_reg_enable(sub_uart_channel, 1)
    self._sub_serial_global_reg_enable(sub_uart_channel, 2)
    self._sub_serial_page_switch(0)
    sier = ((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (0 << 4) | (1 << 7)) & 0xff
    self._sub_serial_reg_config(self.REG_WK2132_SIER, sier)
    fcr = ((0 << 0) | (0 << 1) | (1 << 2) | (1 << 3) | (0 << 4) | (0 << 6)) & 0xff
    self._sub_serial_reg_config(self.REG_WK2132_FCR, fcr)
    scr = ((1 << 0) | (1 << 1) | (0 << 2) | (0 << 3)) & 0xff
    self._sub_serial_reg_config(self.REG_WK2132_SCR, scr)
    
  def _sub_serial_page_switch(self, page):
    if page > 1:
      return None
    l = self._read_bytes(self.REG_WK2132_SPAGE, 1)
    if len(l) != 1:
      print("READ BYTE ERROR!")
      return None
    if page == 0:
      l[0] &= 0xFE
    elif page == 1:
      l[0] |= 0x01
    self._write_bytes(self.REG_WK2132_SPAGE, l)
    l = self._read_bytes(self.REG_WK2132_SPAGE, 1)
  
  def _sub_serial_global_reg_enable(self, sub_uart_channel, type):
     if sub_uart_channel > self.SUBUART_CHANNEL_ALL:
       print("SUBSERIAL CHANNEL NUMBER ERROR!")
       return None
     reg_addr = self._get_global_reg_type(type)
     channel = self._sub_serial_channel_switch(self.SUBUART_CHANNEL_1)
     l = self._read_bytes(reg_addr, 1)
     if len(l) != 1:
       print("READ BYTE SIZE ERROR!")
       return None
     if sub_uart_channel == self.SUBUART_CHANNEL_1:
       l[0] |= 0x01
     elif sub_uart_channel == self.SUBUART_CHANNEL_2:
       l[0] |= 0x02
     else:
       l[0] |= 0x03
     self._write_bytes(reg_addr, l)
     l = self._read_bytes(reg_addr,1)
     self._sub_serial_channel_switch(channel)

  def _get_global_reg_type(self, type):
    if type < 0 or type > 2:
      print("Global Reg Type Error!")
      return 0
    reg_addr = 0
    if type == 0:
      reg_addr = self.REG_WK2132_GENA
    elif type == 1:
      reg_addr = self.REG_WK2132_GRST
    else:
      reg_addr = self.REG_WK2132_GIER
    return reg_addr

  def _sub_serial_reg_config(self, reg, val):
    l = self._read_bytes(reg, 1)
    if len(l) != 1:
      return None
    l[0] = l[0] | val
    self._write_bytes(reg, l)
    l = self._read_bytes(reg, 1)

  def _sub_serial_channel_switch(self, sub_uart_channel):
    channel = self._sub_serial_channel
    self._sub_serial_channel = sub_uart_channel
    return channel

  def _set_sub_serial_baudrate(self, baud):
    scr = self._read_bytes(self.REG_WK2132_SCR, 1)
    self._sub_serial_reg_config(self.REG_WK2132_SCR, 0)
    baud1 = 0
    baud0 = 0
    baud_pres = 0
    val_intger =  self.FOSC//(baud * 16) - 1
    val_decimal = (self.FOSC%(baud * 16))//(baud * 16)
    type(val_intger)
    type(val_decimal)
    baud1 = (val_intger >> 8)&0xff
    baud0 = (val_intger & 0x00ff)
    while val_decimal > 0x0A:
      val_decimal /= 0x0A
    baud_pres = val_decimal & 0xff
    self._sub_serial_page_switch(1)
    
    self._sub_serial_reg_config(self.REG_WK2132_BAUD1, int(baud1))
    self._sub_serial_reg_config(self.REG_WK2132_BAUD0, int(baud0))
    self._sub_serial_reg_config(self.REG_WK2132_PRES, int(baud_pres))
    
    self._sub_serial_page_switch(0)
    self._sub_serial_reg_config(self.REG_WK2132_SCR, scr[0])

  def _set_sub_serial_config_reg(self, format, mode, opt):
    self._addr = self._update_addr(self._addr, self._sub_serial_channel, self.OBJECT_REGISTER)
    l = self._read_bytes(self.REG_WK2132_LCR, 1)
    if len(l) != 1:
      print("READ BYTE ERROR!")
      return None
    
    lcr = l[0] & 0xC0
    lcr |= format
    lcr |= (mode << 4)
    lcr |= (opt << 5)
    self._write_bytes(self.REG_WK2132_LCR, [lcr])
    l = self._read_bytes(self.REG_WK2132_LCR, 1)

  def _update_addr(self, pre, sub_uart_channel, obj):
    addr = pre & 0xF8
    addr |= (obj & 0x01)
    addr |= (sub_uart_channel << 1) 
    addr &= 0xff
    return addr

  def _write_bytes(self, reg, buf):
    '''!
      @brief write bytes to register
      @param reg  Register address 8bits
      @param buf Store buffer list for the data to be write
    '''
    self.last_operate_status = self.STA_ERR_DEVICE_NOT_DETECTED
    self._addr = self._update_addr(self._addr, self._sub_serial_channel, self.OBJECT_REGISTER)
    try:
      self._i2c.writeto_mem(self._addr, reg, buf)
      self.last_operate_status = self.STA_OK
      return len(buf)
    except:
      #print("+++++++++++++")
      return 0

      

  def _read_bytes(self, reg, len1):
    '''!
      @brief Read bytes data from register.
      @param reg  Register address 8bits
      @param len1 Store buffer list for the data to be read
      @return Return list of data
    '''
    self.last_operate_status = self.STA_ERR_DEVICE_NOT_DETECTED
    self._addr = self._update_addr(self._addr, self._sub_serial_channel, self.OBJECT_REGISTER)
    self._addr = self._addr & 0xFE
    try:
      rslt = self._i2c.readfrom_mem(self._addr, reg, len1)
      self.last_operate_status = self.STA_OK
      return rslt
    except:
      return []
