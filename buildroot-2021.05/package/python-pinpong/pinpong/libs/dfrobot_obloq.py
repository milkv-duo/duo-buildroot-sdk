from pinpong.board import gboard,I2C
import time
import threading

class Obloq:
  COMMAND_WIRE_ADDRESS = 0x16
  COMMAND_WIRE_REGTSTER = 0x1E
  COMMAND_WIRE_DATA = 0x22

  COMMAND_TYPE_READ = 0
  COMMAND_TYPE_WRITE = 1
  COMMAND_TYPE_EXECUTE = 2

  COMMAND_READTYPE_REG = 0
  COMMAND_READTYPE_DATA = 1

  RETURN_COMMAND_NONE            = 0
  RETURN_COMMAND_PING            = 1
  RETURN_COMMAND_WIFI            = 2
  RETURN_COMMAND_WIFI_IP         = 3
  RETURN_COMMAND_MQTT            = 4
  RETURN_COMMAND_MQTT_SUBSCRIBE  = 5
  RETURN_COMMAND_MQTT_TOPIC0     = 6
  RETURN_COMMAND_MQTT_TOPIC1     = 7
  RETURN_COMMAND_MQTT_TOPIC2     = 8
  RETURN_COMMAND_MQTT_TOPIC3     = 9
  RETURN_COMMAND_MQTT_TOPIC4     = 10
  RETURN_COMMAND_RESERVE_0x0B    = 11
  RETURN_COMMAND_RESERVE_0x0C    = 12
  RETURN_COMMAND_RESERVE_0x0D    = 13
  RETURN_COMMAND_RESERVE_0x0E    = 14
  RETURN_COMMAND_RESERVE_0x0F    = 15
  RETURN_COMMAND_HTTP            = 16
  RETURN_COMMAND_HTTP_ERROR      = 17
  RETURN_COMMAND_VERSION         = 18
  RETURN_COMMAND_MQTT_PUBLISH    = 19

  RETURN_STATUS_NONE             = 0
  RETURN_STATUS_PING_SUCCESS     = 1
  RETURN_STATUS_PING_FAILURE     = 2
  RETURN_STATUS_VERSION_SUCCESS  = 3
  RETURN_STATUS_WIFI_IDLE        = 4
  RETURN_STATUS_WIFI_CONNECTING  = 5
  RETURN_STATUS_WIFI_CONNECTED   = 6
  RETURN_STATUS_WIFI_GETIP_SUCCESS = 7
  RETURN_STATUS_MQTT_IDLE        = 8
  RETURN_STATUS_MQTT_CONNECT_SUCCESS = 9
  RETURN_STATUS_MQTT_CONNECT_FAILURE = 10
  RETURN_STATUS_MQTT_SUBSCRIBE_IDLE  = 11
  RETURN_STATUS_MQTT_SUBSCRIBE_SUCCESS = 12
  RETURN_STATUS_MQTT_SUBSCRIBE_LIMIT = 13
  RETURN_STATUS_MQTT_SUBSCRIBE_FAILURE = 14
  RETURN_STATUS_MQTT_RECEIVE_MESSAGE = 15
  RETURN_STATUS_MQTT_PUBLISH_SUCCESS = 16
  RETURN_STATUS_MQTT_PUBLISH_FAILURE = 17
  RETURN_STATUS_HTTP_SUCCESS = 18
  RETURN_STATUS_HTTP_FAILURE = 19
  RETURN_STATUS_UNKNOWN      = 20

  COMMAND_EXECUTE_NONE               = 0
  COMMAND_EXECUTE_PING               = 1
  COMMAND_EXECUTE_WIFI_CONNECT       = 2
  COMMAND_EXECUTE_WIFI_RECONNECT     = 3
  COMMAND_EXECUTE_WIFI_DISCONNECT    = 4
  COMMAND_EXECUTE_MQTT_CONNECT       = 5
  COMMAND_EXECUTE_MQTT_SUBSCRIBE0    = 6
  COMMAND_EXECUTE_MQTT_SUBSCRIBE1    = 7
  COMMAND_EXECUTE_MQTT_SUBSCRIBE2    = 8
  COMMAND_EXECUTE_MQTT_SUBSCRIBE3    = 9
  COMMAND_EXECUTE_MQTT_SUBSCRIBE4    = 10
  COMMAND_EXECUTE_MQTT_PUBLISH0      = 11
  COMMAND_EXECUTE_MQTT_PUBLISH1      = 12
  COMMAND_EXECUTE_MQTT_PUBLISH2      = 13
  COMMAND_EXECUTE_MQTT_PUBLISH3      = 14
  COMMAND_EXECUTE_MQTT_PUBLISH4      = 15
  COMMAND_EXECUTE_HTTP_GET           = 16
  COMMAND_EXECUTE_HTTP_POST          = 17
  COMMAND_EXECUTE_HTTP_PUT           = 18
  COMMAND_EXECUTE_VERSION            = 19
  COMMAND_EXECUTE_ONENETMQTT_CONNECT = 20

  RUNNING_NODE_NONE         = 0
  RUNNING_NODE_PING         = 1
  RUNNING_NODE_VERSION      = 2
  RUNNING_NODE_WIFI_CONNECT = 3
  RUNNING_NODE_WIFI_GETIP   = 4
  RUNNING_NODE_MQTT_CONNECT = 5
  RUNNING_NODE_RUNING       = 6

  COMMAND_WRITE_NONE           = 0
  COMMAND_WRITE_WIFI_NAME      = 1
  COMMAND_WRITE_WIFI_PASSWORD  = 2
  COMMAND_WRITE_MQTT_SERVER    = 3
  COMMAND_WRITE_MQTT_PORT      = 4
  COMMAND_WRITE_MQTT_ID        = 5
  COMMAND_WRITE_MQTT_PWD       = 6
  COMMAND_WRITE_HTTP_IP        = 7
  COMMAND_WRITE_HTTP_PORT      = 8
  COMMAND_WRITE_MQTT_CLIENTID  = 9

  def __init__(self, board=None, i2c_addr=0x00,bus_num=0):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard
      
    self.board = board
    self.i2c_addr = i2c_addr
    self.i2c = I2C(bus_num)

    self.tickLock = False
    self.timerStart = 0
    self.returnStatusList = [0 for i in range(self.RETURN_STATUS_UNKNOWN)]
    self.runningNode = self.RUNNING_NODE_NONE
    self.mqtt_message = ["", "", "", "", ""]
    self.handle = None
    self.mqtt_cb = False
    self.returnMessage = ""
    self.IPAddress = ""
    self.the_reporter_thread = threading.Thread(target=self.thread_handle)
    self.the_reporter_thread.daemon = True
    self.thread_lock = threading.Lock()

  def thread_handle(self):
    while True:
      self.MUTEX_TICK()
      time.sleep(0.01)

  def tick(self):
    result = self.read(2, self.COMMAND_READTYPE_REG)
    if len(result) != 2:
      print("Read reg error")
      return
    command = result[0]
    status = result[1]
    self.returnStatus = self.RETURN_STATUS_UNKNOWN
    if not self.CHECK_RETURN_COMMAND(command):
      print("Unknown command")
      return
    if command == self.RETURN_COMMAND_NONE:
      self.returnStatus = self.RETURN_STATUS_NONE
    elif command == self.RETURN_COMMAND_PING:
      if status == 0x00:
        self.returnStatus = self.RETURN_STATUS_PING_SUCCESS
      if status == 0x01:
        self.returnStatus = self.RETURN_STATUS_PING_SUCCESS
    elif command == self.RETURN_COMMAND_WIFI:
      if status == 0x00:
        self.returnStatus = self.RETURN_STATUS_WIFI_IDLE
      if status == 0x02:
        self.returnStatus = self.RETURN_STATUS_WIFI_CONNECTING
      if status == 0x03:
        self.returnStatus = self.RETURN_STATUS_WIFI_CONNECTED
    elif command == self.RETURN_COMMAND_VERSION:
      self.version = ""
      datalength = status
      databuffer = self.read(datalength, self.COMMAND_READTYPE_DATA)
      if len(databuffer) != datalength:
        print("Read data error")
        return
      for i in databuffer:
        self.version += chr(i)
      self.returnStatus = self.RETURN_STATUS_VERSION_SUCCESS
    elif command == self.RETURN_COMMAND_WIFI:
      if status == 0x00:
        self.returnStatus = self.RETURN_STATUS_WIFI_IDLE
      if status == 0x02:
        self.returnStatus = self.RETURN_STATUS_WIFI_CONNECTING
      if status == 0x03:
        self.returnStatus = self.RETURN_STATUS_WIFI_CONNECTED
    elif command == self.RETURN_COMMAND_WIFI_IP:
      datalength = status
      databuffer = self.read(datalength, self.COMMAND_READTYPE_DATA)
      if len(databuffer) != datalength:
        print("Read data error")
        return
      for i in databuffer:
        self.IPAddress += chr(i)
      self.returnStatus = self.RETURN_STATUS_WIFI_GETIP_SUCCESS
    elif command == self.RETURN_COMMAND_MQTT:
      if status == 0x00:
        self.returnStatus = self.RETURN_STATUS_MQTT_IDLE
      if status == 0x01:
        self.returnStatus = self.RETURN_STATUS_MQTT_CONNECT_SUCCESS
      if status == 0x02:
        self.returnStatus = self.RETURN_STATUS_MQTT_CONNECT_FAILURE
    elif command == self.RETURN_COMMAND_MQTT_SUBSCRIBE:
      if status == 0x00:
        self.returnStatus = self.RETURN_STATUS_MQTT_SUBSCRIBE_IDLE
      if status == 0x01:
        self.returnStatus = self.RETURN_STATUS_MQTT_SUBSCRIBE_SUCCESS
      if status == 0x02:
        self.returnStatus = self.RETURN_STATUS_MQTT_SUBSCRIBE_LIMIT
      if status == 0x03:
        self.returnStatus = self.RETURN_STATUS_MQTT_SUBSCRIBE_FAILURE
    elif command == self.RETURN_COMMAND_MQTT_TOPIC0 or \
          command == self.RETURN_COMMAND_MQTT_TOPIC1 or \
          command == self.RETURN_COMMAND_MQTT_TOPIC2 or \
          command == self.RETURN_COMMAND_MQTT_TOPIC3 or \
          command == self.RETURN_COMMAND_MQTT_TOPIC4:
      datalength = status
      databuffer = self.read(datalength, self.COMMAND_READTYPE_DATA)
      if len(databuffer) != datalength:
        return
      value = ""
      for i in databuffer:
        value += chr(i)
      self.mqtt_message[command - self.RETURN_COMMAND_MQTT_TOPIC0] = value
      self.RUN_TOPIC_STATIC(command-self.RETURN_COMMAND_MQTT_TOPIC0)
      self.returnStatus = self.RETURN_STATUS_MQTT_RECEIVE_MESSAGE

    elif command == self.RETURN_COMMAND_MQTT_PUBLISH:
      self.returnStatus = self.RETURN_STATUS_MQTT_PUBLISH_SUCCESS 
    elif command == self.RETURN_COMMAND_HTTP:
      val = ""
      datalength = status
      databuffer = self.read(datalength, self.COMMAND_READTYPE_DATA)
      if len(databuffer) != datalength:
        print("Read data error")
        return
      for i in databuffer:
        val += chr(i)
      self.returnMessage = val
      self.returnStatus = self.RETURN_STATUS_HTTP_SUCCESS
    elif command == self.RETURN_COMMAND_HTTP_ERROR:
      self.returnMessage = str(status)
      self.returnStatus = self.RETURN_STATUS_HTTP_FAILURE
    self.returnStatusList[self.returnStatus] = False
    return

  def RUN_TOPIC_STATIC(self, INDEX):
    if self.mqtt_cb:
      temp = INDEX + self.RETURN_COMMAND_MQTT_TOPIC0
      if temp == self.RETURN_COMMAND_MQTT_TOPIC0 or \
                      temp == self.RETURN_COMMAND_MQTT_TOPIC1 or \
                      temp == self.RETURN_COMMAND_MQTT_TOPIC2 or \
                      temp == self.RETURN_COMMAND_MQTT_TOPIC3 or \
                      temp == self.RETURN_COMMAND_MQTT_TOPIC4:
        self.handle(self.iotTopics[INDEX], self.mqtt_message[INDEX])


  def CHECK_RETURN_COMMAND(self, COMMAND):
    return \
    COMMAND >= self.RETURN_COMMAND_NONE and \
    COMMAND <= self.RETURN_COMMAND_MQTT_PUBLISH and \
    COMMAND != self.RETURN_COMMAND_RESERVE_0x0B and \
    COMMAND != self.RETURN_COMMAND_RESERVE_0x0C and \
    COMMAND != self.RETURN_COMMAND_RESERVE_0x0D and \
    COMMAND != self.RETURN_COMMAND_RESERVE_0x0E and \
    COMMAND != self.RETURN_COMMAND_RESERVE_0x0F

  def mqtt_handle(self, handle):
    self.handle = handle
    self.mqtt_cb = True
    self.the_reporter_thread.start()

  def MUTEX_TICK(self):
    if not self.tickLock:
      self.thread_lock.acquire()
      self.tickLock =True
      self.tick()
      self.tickLock = False
      self.thread_lock.release()

  def execute(self, command, data):
    if data == "":
      buffer = [self.COMMAND_TYPE_EXECUTE, command]
      self.writeReg(self.COMMAND_WIRE_ADDRESS, self.COMMAND_WIRE_REGTSTER, buffer)
    else:
      buffer = [self.COMMAND_TYPE_EXECUTE, command, len(data)]
      for i in data:
        buffer.append(ord(i))
      self.writeReg(self.COMMAND_WIRE_ADDRESS, self.COMMAND_WIRE_REGTSTER, buffer)

  def EXECUTE_CMD(self, COMMAND, STATUS_CODE, INTERVAL, TIMEOUT_MS, REPEAT_EXECUTE, data=""):
    self.returnStatus = self.RETURN_STATUS_NONE
    for i in range(len(self.returnStatusList)):
      self.returnStatusList[i] = True
    self.execute(COMMAND, data)
    time.sleep(0.1)
    self.timerInit(TIMEOUT_MS)
    while self.returnStatusList[STATUS_CODE]:
      if not self.timer_count():
        break
      self.MUTEX_TICK()
      if self.returnStatusList[STATUS_CODE]:
        time.sleep(INTERVAL)
        if REPEAT_EXECUTE:
          self.execute(COMMAND, data)
          time.sleep(0.1)

  def CHECK_RETURN_STATUS(self, STATUS, TRUE_TASK, FALSE_TASK):
    if not self.returnStatusList[STATUS]:
      print(TRUE_TASK)
    else:
      print(FALSE_TASK)

  def CHECK_RUNNING_NODE(self, NODE, TRUE_TASK, FALSE_TASK):
    if self.runningNode == NODE:
      print(TRUE_TASK)
    else:
      print(FALSE_TASK)
  
  def READ_DATA(self, STATUS_CODE, INTERVAL, TIMEOUT_MS):
    self.returnStatus = self.RETURN_STATUS_NONE
    self.timerInit(TIMEOUT_MS)
    while self.returnStatusList[STATUS_CODE]:
      if not self.timer_count():
        break
      self.MUTEX_TICK()
      if self.returnStatusList[STATUS_CODE]:
        time.sleep(INTERVAL)

  def WRITE_PARAMETER(self, COMMAND, DATA):
    self.write(COMMAND, DATA)

  def wifiConnect(self, ssid, pwd):
    print(">=====> wifi connect")
    self.CHECK_RUNNING_NODE(self.RUNNING_NODE_PING, "Wifi connect "+ssid+" "+pwd, "Please reinitialize")
    if not self.idle():
      return
    self.WRITE_PARAMETER(self.COMMAND_WRITE_WIFI_NAME, ssid)
    self.WRITE_PARAMETER(self.COMMAND_WRITE_WIFI_PASSWORD, pwd)
    self.EXECUTE_CMD(self.COMMAND_EXECUTE_WIFI_CONNECT, self.RETURN_STATUS_WIFI_CONNECTED, 0.2, 10, False)
    self.CHECK_RETURN_STATUS(self.RETURN_STATUS_WIFI_CONNECTED, "Wifi connect success", "Wifi connect timeout")
    self.READ_DATA(self.RETURN_STATUS_WIFI_GETIP_SUCCESS, 0.2, 2)
#    print(self.IPAddress)
    self.runningNode = self.RUNNING_NODE_WIFI_CONNECT

  def ip_address(self):
    return self.IPAddress

  def mqtt_connect(self, ssid, pwd, iotId, iotPwd, iotTopics, server, port ):
    #建立串口通讯
    self.ping()
    #WiFi连接
    self.wifiConnect(ssid, pwd)
    #Mqtt连接
    self.mqttConnect(iotId, iotPwd, iotTopics, server, port)
    # #订阅Topic
    self.subscribe()

  def http_connect(self, ssid, pwd, ip, port):
    self._startConnect(ssid, pwd, ip)
    self.WRITE_PARAMETER(self.COMMAND_WRITE_HTTP_PORT, port)
    self.runningNode = self.RUNNING_NODE_RUNING

  def _startConnect(self, ssid, pwd, host):
    self.ping()
    self.wifiConnect(ssid, pwd)
    self.WRITE_PARAMETER(self.COMMAND_WRITE_HTTP_IP, host)
    self.runningNode = self.RUNNING_NODE_RUNING

  def get(self, url, timeout):
    print(">=====> http get")
    self.CHECK_RUNNING_NODE(self.RUNNING_NODE_RUNING, "GET " + url, "Please reinitialize")
    self.EXECUTE_CMD(self.COMMAND_EXECUTE_HTTP_GET, self.RETURN_STATUS_HTTP_SUCCESS, 0.2, timeout//1000, False, url)
    self.CHECK_RETURN_STATUS(self.RETURN_STATUS_HTTP_SUCCESS, "GET success", "GET failure")
    return self.returnMessage
  
  def post(self, url, content, timeout):
    print(">=====> http post")
    self.CHECK_RUNNING_NODE(self.RUNNING_NODE_RUNING, "POST "+url+ " "+ content, \
                            "Please reinitialize")
    posturl = url + "," + content
    print(posturl)
    self.EXECUTE_CMD(self.COMMAND_EXECUTE_HTTP_POST, self.RETURN_STATUS_HTTP_SUCCESS, 0.2, timeout//1000, False, posturl)
    self.CHECK_RETURN_STATUS(self.RETURN_STATUS_HTTP_SUCCESS, "POST success", "POST failure")
    return self.returnMessage
  
  def put(self, url, content, timeout):
    print(">=====> http put")
    self.CHECK_RUNNING_NODE(self.RUNNING_NODE_RUNING, "PUT "+url+" "+ content, \
                            "Please reinitialize")
    puturl = url + "," + content
    self.EXECUTE_CMD(self.COMMAND_EXECUTE_HTTP_PUT, self.RETURN_STATUS_HTTP_SUCCESS, 0.2, timeout//1000, False, puturl)
    self.CHECK_RETURN_STATUS(self.RETURN_STATUS_HTTP_SUCCESS, "PUT success", "PUT failure")
    return self.returnMessage

  def publish(self, topic_num, message):
    print(">=====> publish")
    if not self.idle():
      return
    self.CHECK_RUNNING_NODE(self.RUNNING_NODE_RUNING, \
                            "Publish: " + message + " to " + topic_num, \
                            "Please reinitialize")
    if topic_num not in self.iotTopics:
      return 
    self.EXECUTE_CMD(self.COMMAND_EXECUTE_MQTT_PUBLISH0 + self.iotTopics.index(topic_num), self.RETURN_STATUS_MQTT_PUBLISH_SUCCESS, 0.2, 2, False, message)
    self.CHECK_RETURN_STATUS(self.RETURN_STATUS_MQTT_PUBLISH_SUCCESS, \
                            "Publish success", "Publish failure")
    # self.READ_DATA(self.RETURN_STATUS_MQTT_RECEIVE_MESSAGE, 0.1, 2)
    time.sleep(0.1)

  def subscribe(self):
    print(">=====> subscribe")
    self.CHECK_RUNNING_NODE(self.RUNNING_NODE_MQTT_CONNECT, "Subscribe", "Please reinitialize")
    if not self.idle():
      return
    for i in range(len(self.iotTopics)):
      self.EXECUTE_CMD(self.COMMAND_EXECUTE_MQTT_SUBSCRIBE0 + i, self.RETURN_STATUS_MQTT_SUBSCRIBE_SUCCESS, 0.1, 5, False, self.iotTopics[i])
      peek = ""
      if self.returnStatus == self.RETURN_STATUS_MQTT_SUBSCRIBE_IDLE:
        peek = "Subscribe " + self.iotTopics[i] + " no response"
      elif self.returnStatus == self.RETURN_STATUS_MQTT_SUBSCRIBE_LIMIT:
        peek = "Subscribe " + self.iotTopics[i] + " limit"
      elif self.returnStatus == self.RETURN_STATUS_MQTT_SUBSCRIBE_FAILURE:
        peek = "Subscribe " + self.iotTopics[i] + " failure"
      self.CHECK_RETURN_STATUS(self.RETURN_STATUS_MQTT_SUBSCRIBE_SUCCESS, "Subscribe "+ self.iotTopics[i]+ " success", peek)
      time.sleep(0.4)
    time.sleep(1)
    self.runningNode = self.RUNNING_NODE_RUNING

  def mqttConnect(self, iotId, iotPwd, iotTopics, server, port):
    print(">=====> mqtt connect")
    self.CHECK_RUNNING_NODE(self.RUNNING_NODE_WIFI_CONNECT, "MQTT connect "+server+ " "+str(port), "Please reinitialize")
    if not self.idle():
      return
    self.iotTopics = iotTopics
    self.WRITE_PARAMETER(self.COMMAND_WRITE_MQTT_SERVER, server)
    self.WRITE_PARAMETER(self.COMMAND_WRITE_MQTT_PORT, str(port))
    self.WRITE_PARAMETER(self.COMMAND_WRITE_MQTT_ID, iotId)
    self.WRITE_PARAMETER(self.COMMAND_WRITE_MQTT_PWD, iotPwd)
    self.EXECUTE_CMD(self.COMMAND_EXECUTE_MQTT_CONNECT, self.RETURN_STATUS_MQTT_CONNECT_SUCCESS, 0.2, 2, False)
    self.CHECK_RETURN_STATUS(self.RETURN_STATUS_MQTT_CONNECT_SUCCESS, "MQTT connect success", "MQTT connect failure")
    self.runningNode = self.RUNNING_NODE_MQTT_CONNECT

  def ping(self):
    if not self.idle():
      return
    print(">=====> ping")
    self.EXECUTE_CMD(self.COMMAND_EXECUTE_PING, self.RETURN_STATUS_PING_SUCCESS, 0.1, 20, True)
    self.CHECK_RETURN_STATUS(self.RETURN_STATUS_PING_SUCCESS, "Ping success", "Ping failure")
    if not self.idle():
      return
    print(">=====> get version")
    self.EXECUTE_CMD(self.COMMAND_EXECUTE_VERSION, self.RETURN_STATUS_VERSION_SUCCESS, 0.1, 20, False)
    self.CHECK_RETURN_STATUS(self.RETURN_STATUS_VERSION_SUCCESS, "Get version success", "Get version failure")
    self.runningNode = self.RUNNING_NODE_PING

  def idle(self, timeout=10):
    count = 0
    self.timerInit(timeout)
    while self.tickLock:
        time.sleep(0.001)
    self.tickLock = True
    while True:
        if not self.timer_count():
            print("idle timeout")
        result = self.read(2, self.COMMAND_READTYPE_REG)
        if len(result) != 2:
            print("idle error")
        if result[0] == self.RETURN_COMMAND_NONE and result[1] == self.RETURN_STATUS_NONE:
            count = count + 1
        else:
            count = 0
        if count > 5:
            self.tickLock = False
            return True
        
  def timerInit(self, times):
    self.timer = True
    self.timerDuration = times
    self.timerStart = time.time()

  def timer_count(self):
    if self.timer:
        if time.time() - self.timerStart >= self.timerDuration:
            self.timer = False
            return False
        return True
    return False
  
  def read(self, size, type):
    time.sleep(0.02)
    result = self.readReg(self.COMMAND_WIRE_ADDRESS, self.COMMAND_WIRE_REGTSTER  if type == self.COMMAND_READTYPE_REG  else self.COMMAND_WIRE_DATA, size)
    return result

  def write(self, command, data):
    buffer = [self.COMMAND_TYPE_WRITE, command, len(data)]
    for i in data:
      buffer.append(ord(i))
    self.writeReg(self.COMMAND_WIRE_ADDRESS, self.COMMAND_WIRE_REGTSTER, buffer)

  def readReg(self, address, reg, size):
    time.sleep(0.02)
    if reg == 0x1E:
      data = [0, 0x06]
      self.i2c.writeto_mem(address, reg, data)
      result = self.i2c.readfrom(address,size)
    else:
      result = self.i2c.readfrom_mem(address, reg, size)
    return result
  
  def writeReg(self, address, reg, data):
    self.i2c.writeto_mem(address, reg, data)