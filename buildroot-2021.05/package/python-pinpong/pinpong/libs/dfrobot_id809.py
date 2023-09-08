import time
import datetime
from pinpong.board import gboard, I2C
import math


class ID809():
    FINGERPRINT_CAPACITY      = 80    
    MODULE_SN_SIZE            = 16

    DELALL                    = 0xFF  

    CMD_PREFIX_CODE           = 0xAA55
    RCM_PREFIX_CODE           = 0x55AA
    CMD_DATA_PREFIX_CODE      = 0xA55A
    RCM_DATA_PREFIX_CODE      = 0x5AA5

    CMD_TYPE                  = 0xF0  
    RCM_TYPE                  = 0xF0  
    DATA_TYPE                 = 0x0F  

    CMD_TEST_CONNECTION       = 0X0001
    CMD_SET_PARAM             = 0X0002
    CMD_GET_PARAM             = 0X0003
    CMD_DEVICE_INFO           = 0X0004
    CMD_SET_MODULE_SN         = 0X0008
    CMD_GET_MODULE_SN         = 0X0009
    CMD_ENTER_STANDBY_STATE   = 0X000C
    CMD_GET_IMAGE             = 0X0020
    CMD_FINGER_DETECT         = 0X0021
    CMD_UP_IMAGE_CODE         = 0X0022
    CMD_DOWN_IMAGE            = 0X0023
    CMD_SLED_CTRL             = 0X0024
    CMD_STORE_CHAR            = 0X0040
    CMD_LOAD_CHAR             = 0X0041
    CMD_UP_CHAR               = 0X0042
    CMD_DOWN_CHAR             = 0X0043
    CMD_DEL_CHAR              = 0X0044
    CMD_GET_EMPTY_ID          = 0X0045
    CMD_GET_STATUS            = 0X0046
    CMD_GET_BROKEN_ID         = 0X0047
    CMD_GET_ENROLL_COUNT      = 0X0048
    CMD_GET_ENROLLED_ID_LIST  = 0X0049
    CMD_GENERATE              = 0X0060
    CMD_MERGE                 = 0X0061
    CMD_MATCH                 = 0X0062
    CMD_SEARCH                = 0X0063
    CMD_VERIFY                = 0X0064

    ERR_SUCCESS               = 0x00  
    error                 = 0xFF  

    ErrorSuccess            = 0x00
    ErrorFail               = 0x01
    ErrorVerify             = 0x10
    ErrorIdentify           = 0x11
    ErrorTmplEmpty          = 0x12
    ErrorTmplNotEmpty       = 0x13
    ErrorAllTmplEmpty       = 0x14
    ErrorEmptyIDNoexist     = 0x15
    ErrorBrokenIDNoexist    = 0x16
    ErrorInvalidTmplData    = 0x17
    ErrorDuplicationID      = 0x18
    ErrorBadQuality         = 0x19
    ErrorMergeFail          = 0x1A
    ErrorNotAuthorized      = 0x1B
    ErrorMemory             = 0x1C
    ErrorInvalidTmplNo      = 0x1D
    ErrorInvalidParam       = 0x22
    ErrorTimeOut            = 0x23
    ErrorGenCount           = 0x25
    ErrorInvalidBufferID    = 0x26
    ErrorFPNotDetected      = 0x28
    ErrorFPCancel           = 0x41
    ErrorRecvLength         = 0x42
    ErrorRecvCks            = 0x43
    ErrorGatherOut          = 0x45
    ErrorRecvTimeout        = 0x46

    breathing   = 1
    fast_blink   = 2
    keeps_on     = 3
    normal_close = 4
    fade_in      = 5
    fade_out     = 6
    slow_blink   = 7

    green      = 1
    red        = 2
    yellow     = 3
    blue       = 4
    cyan       = 5
    magenta    = 6
    white      = 7

    def __init__(self, board=None, i2c_addr=0x1F, bus_num=0):
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.i2c_addr = i2c_addr
        self._i2c = I2C(bus_num)
        self.ISIIC = True
        self.buf = [0 for i in range(20)]
        self._number = 0
        self._state = 0

    def get_deviceID(self):
        data = [0]
        data[0] = 0
        ret = self.get_param(data)
        return ret

    def get_security_level(self):
        data = [0]
        data[0] = 1
        ret = self.get_param(data)
        return ret

    def get_baudrate(self):
        data = [0]
        data[0] = 3
        ret = self.get_param(data)
        return ret

    def get_self_learn(self):
        data = [0]
        data[0] = 4
        ret = self.get_param(data)
        return ret

    def get_enroll_count(self):
        data = [0,0,0,0]
        data[0] = 1
        data[2] = self.FINGERPRINT_CAPACITY
        header = self.__pack(self.CMD_TYPE, self.CMD_GET_ENROLL_COUNT, data, 4)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.08)
        ret = self.__response_payload()
        if ret == self.ERR_SUCCESS:
            return self.buf[0]
        return ret

    def get_empty_id(self):
        data = [0,0,0,0]
        data[0] = 1
        data[2] = self.FINGERPRINT_CAPACITY
        header = self.__pack(self.CMD_TYPE, self.CMD_GET_EMPTY_ID, data, 4)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.1)
        ret = self.__response_payload()
        if ret == self.ERR_SUCCESS:
            ret = self.buf[0]
        return ret

    def get_brokenID(self):
        data = [0,0,0,0]
        data[0] = 1
        data[2] = self.FINGERPRINT_CAPACITY
        header = self.__pack(self.CMD_TYPE, self.CMD_GET_BROKEN_ID, data, 4)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.05)
        ret = self.__response_payload()
        if ret == self.ERR_SUCCESS:
            return self.buf[2]
        return ret

    def get_broken_quantity(self):
        data = [0,0,0,0]
        data[0] = 1
        data[2] = self.FINGERPRINT_CAPACITY
        header = self.__pack(self.CMD_TYPE, self.CMD_GET_BROKEN_ID, data, 4)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.05)
        ret = self.__response_payload()
        if ret == self.ERR_SUCCESS:
            return self.buf[0]
        return ret

    def ctrl_led(self, mode, color, blinkCount):
        data = [0,0,0,0]
        data[0] = mode
        data[1] = color
        data[2] = color
        data[3] = blinkCount
        header = self.__pack(self.CMD_TYPE, self.CMD_SLED_CTRL, data, 4)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.05)
        ret = self.__response_payload()
        return ret

    def get_enrolledID_list(self):
        header = self.__pack(self.CMD_TYPE, self.CMD_GET_ENROLLED_ID_LIST, [], 0)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.12)
        ret = self.__response_payload()
        time.sleep(0.2)
        datalen = self.buf[0] + (self.buf[1] << 8)
        data = [0 for i in range(datalen + 10)]
        ret = self.__response_payload()
        info = []
        for j in range(datalen*8):
            if self.get_ID(j) != 0:
                info.append(j)
        return info

    def get_ID(self, v):
        return ((self.buf[(0 + v // 8)]) & (0x01 << (v & 0x07)))

    def get_moduleSN(self):
        header = self.__pack(self.CMD_TYPE, self.CMD_GET_MODULE_SN, [], 0)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.1)
        ret = self.__response_payload()
        if ret != self.ERR_SUCCESS:
            return ""
        dataLen = self.buf[0] + (self.buf[1] << 8)
        result = self.__response_payload()
        data = ""
        for i in range(dataLen):
            data += chr(self.buf[i])
        return data

    def get_param(self, data):
        header = self.__pack(self.CMD_TYPE, self.CMD_GET_PARAM, data, 1)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.05)
        ret = self.__response_payload()
        if ret == self.ERR_SUCCESS:
            return self.buf[0]
        return ret

    def set_security_level(self, securityLevel=3):
        data = [0,0,0,0,0]
        if securityLevel > 255:
            securityLevel = securityLevel % 255
        data[0] = 1
        data[1] = securityLevel
        self.set_param(data)

    def set_baudrate(self, baudrate=5):
        if self.ISIIC == True:
            return
    
    def set_moduleSN(self, val):
        data = [0,0]
        data[0] = self.MODULE_SN_SIZE
        if len(val) > self.MODULE_SN_SIZE:
            print("The serial number exceeds 15 characters")
            return
        header = self.__pack(self.CMD_TYPE, self.CMD_SET_MODULE_SN, data, 2)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.1)
        ret = self.__response_payload()
        if ret != self.ERR_SUCCESS:
            print("error")
            return self.error
        SN = []
        if not isinstance(val, str):
            val = str(val)
        for i in val:
            SN.append((ord(i)))
        for i in range(16-len(val)):
            SN.append(0)
        head = self.__pack(self.DATA_TYPE, self.CMD_SET_MODULE_SN, SN, self.MODULE_SN_SIZE)
        self.__send_packet(head)
        if self.ISIIC == True:
            time.sleep(0.24)
        ret = self.__response_payload()
        if ret == self.ERR_SUCCESS:
            return self.buf[0]
        return ret

    def set_self_learn(self, selfLearn=1):
        data = [0,0,0,0,0]
        data[0] = 4
        data[1] = selfLearn
        self.set_param(data)

    def set_deviceID(self, deviceID=1):
        data = [0,0,0,0,0]
        if deviceID > 255:
            deviceID = deviceID % 255
        data[1] = deviceID
        self.set_param(data)

    def set_param(self, data):
        header = self.__pack(self.CMD_TYPE, self.CMD_SET_PARAM, data, 5)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.24)
        ret = self.__response_payload()
        return ret

    def collection_fingerprint(self, timeout):
        i = 0
        if self._number > 2:
            return self.error
        while not self.detect_finger():
            time.sleep(0.01)
            i += 1
            if i  > timeout * 10:
                self._state = 0
                return self.error
        ret = self.getImage()
        if ret != self.ERR_SUCCESS:
            self._state = 0
            return self.error
        ret = self.generate(self._number)
        if ret != self.ERR_SUCCESS:
            self._state = 0
            return self.error
        self._number += 1
        self._state = 1
        return ret

    def generate(self, RamBufferID):
        data = [0,0]
        data[0] = RamBufferID
        header = self.__pack(self.CMD_TYPE, self.CMD_GENERATE, data, 2)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.36)
        ret = self.__response_payload()
        return ret

    def del_fingerprint(self, id):
        data= [0,0,0,0]
        if id == self.DELALL:
            data[0] = 1
            data[2] = self.FINGERPRINT_CAPACITY
        else:
            data[0] = id
            data[2] = id
        header = self.__pack(self.CMD_TYPE, self.CMD_DEL_CHAR, data, 4)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.36)
        ret = self.__response_payload()
        return ret

    def getImage(self):
        header = self.__pack(self.CMD_TYPE, self.CMD_GET_IMAGE, [], 0)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.36)
        ret = self.__response_payload()
        return ret

    def get_status_id(self, id):
        data = [0,0]
        data[0] = id
        header = self.__pack(self.CMD_TYPE, self.CMD_GET_STATUS, data, 2)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.36)
        ret = self.__response_payload()
        if ret == self.ERR_SUCCESS:
            ret =  self.buf[0]
        return ret

    def verify(self, id):
        if self._state == 1:
            data = [0,0,0,0]
            data[0] = id
            self._number = 0
            header = self.__pack(self.CMD_TYPE, self.CMD_VERIFY, data, 4)
            self.__send_packet(header)
            if self.ISIIC == True:
                time.sleep(0.36)
            ret = self.__response_payload()
            if ret == self.ERR_SUCCESS:
                ret = True
            else:
                ret = False
            return ret
        return False
    
    def search(self):
        if self._state == 1:
            data = [0,0,0,0,0,0]
            data[2] = 1
            data[4] = self.FINGERPRINT_CAPACITY
            self._number = 0
            header = self.__pack(self.CMD_TYPE, self.CMD_SEARCH, data, 6)
            self.__send_packet(header)
            if self.ISIIC == True:
                time.sleep(0.36)
            ret = self.__response_payload()
            if ret == self.ERR_SUCCESS:
                return self.buf[0]
            return 0

    def store_fingerprint(self, id):
        data = [0,0,0,0]
        ret = self.merge()
        if ret != self.ERR_SUCCESS:
            return self.error
        self._number = 0
        data[0] = id
        header = self.__pack(self.CMD_TYPE, self.CMD_STORE_CHAR, data, 4)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.36)
        ret = self.__response_payload()
        return ret
    
    def merge(self):
        data = [0,0,0]
        data[2] = self._number
        header = self.__pack(self.CMD_TYPE, self.CMD_MERGE, data, 3)
        self.__send_packet(header)
        if self.ISIIC ==True:
            time.sleep(0.36)
        ret = self.__response_payload()
        return ret

    def detect_finger(self):
        header = self.__pack(self.CMD_TYPE, self.CMD_FINGER_DETECT, [], 0)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.24)
        ret = self.__response_payload()
        if ret == self.ERR_SUCCESS:
            return self.buf[0]
        return ret

    def connected(self):
        header = self.__pack(self.CMD_TYPE, self.CMD_TEST_CONNECTION, 0, 0)
        self.__send_packet(header)
        if self.ISIIC == True:
            time.sleep(0.05)
        ret = self.__response_payload()
        if ret == self.ERR_SUCCESS:
            return True
        else:
            return False

    def __response_payload(self , val=[]):
        header = [0 for i in range(10)]
        type = self.__read_prefix(header)
        if type == 1:
            self._error = self.ErrorRecvTimeout
            return self.error
        if type == self.RCM_TYPE:
            packet = [0 for i in range(26)]
            dataLen = 16
        else:
            lens = (header[7] << 8) | header[6]
            packet = [0 for i in range(lens + 10)]
            dataLen = lens
        packet[0:10] = header[0:10]
        data = self.__read_n(dataLen)
        packet[10:] = data
        dataCount = len(data)
        cks = data[dataLen - 2] + (data[dataLen - 1] << 8)
        RET1 = header[8] + (header[9] << 8)
        ret = RET1 & 0xff
        self._error = ret
        if ret != self.ERR_SUCCESS:
            ret = self.error
        elif dataLen != dataCount:
            self._error = self.ErrorRecvLength
            ret = self.error
        elif self.__get_Rcm_CKS(packet) != cks:
            self._error = self.ErrorRecvCks
            self.buf = data
            ret = self.error
        else:
            self.buf = data
            val = data
        return ret

    def __read_prefix(self, header):
        RECV_HEADER_INIT = 0
        RECV_HEADER_AA = 1
        RECV_HEADER_A5 = 2
        RECV_HEADER_OK = 3
        state = RECV_HEADER_INIT
        curr = time.time()
        while state != RECV_HEADER_OK:
            ch = self.__read_n(1)[0]
            if ch == 0xAA and (state == RECV_HEADER_INIT):
                state = RECV_HEADER_AA
                continue
            elif ch == 0xA5 and (state == RECV_HEADER_INIT):
                state = RECV_HEADER_A5
                continue
            elif ch == 0x55 and (state == RECV_HEADER_AA):
                state = RECV_HEADER_OK
                ret = self.RCM_TYPE
                continue
            elif ch == 0x5A and (state == RECV_HEADER_A5):
                state = RECV_HEADER_OK
                ret = self.DATA_TYPE
                continue
            else:
                state = RECV_HEADER_INIT
                if ch == 0xAA:
                    state = RECV_HEADER_AA
                elif ch == 0xA5:
                    state = RECV_HEADER_A5
                if time.time() - curr > 2:
                    return 1
        if ret == self.RCM_TYPE:
            header[0] = self.RCM_PREFIX_CODE & 0xff
            header[1] = self.RCM_PREFIX_CODE >> 8
        elif ret == self.DATA_TYPE:
            header[0] = self.RCM_DATA_PREFIX_CODE & 0xff
            header[1] = self.RCM_DATA_PREFIX_CODE >> 8
        header[2] = self.__read_n(1)[0]
        header[3] = self.__read_n(1)[0]
        RCM = self.__read_n(2)
        LEN = self.__read_n(2)
        RET = self.__read_n(2)
        header[4] = RCM[0]
        header[5] = RCM[1]
        header[6] = LEN[0]
        header[7] = LEN[1]
        header[8] = RET[0]
        header[9] = RET[1]
        return ret

    def __read_n(self, lens):
        return self.__read_to_addr(lens)

    def __send_packet(self, send_pack):
        while 0xee != self.__read_to_addr(1)[0]:
            time.sleep(0.01)
        self.__write_to_addr(send_pack)

    def __pack(self, type, cmd, payload, lens):
        if type == self.CMD_TYPE:
            header = [0 for i in range(26)]
            header[0] = self.CMD_PREFIX_CODE & 0xff
            header[1] = self.CMD_PREFIX_CODE >> 8
            dataLen = 16
        else:
            header = [0 for i in range(10+lens)]
            header[0] = self.CMD_DATA_PREFIX_CODE & 0xff
            header[1] = self.CMD_DATA_PREFIX_CODE >> 8
            dataLen = lens
        header[2] = 0
        header[3] = 0
        header[4] = cmd & 0xff
        header[5] = cmd >> 8
        header[6] = lens & 0xff
        header[7] = lens >> 8
        if lens:
            for i in range(lens):
                header[8+i] = payload[i]
        cks = self.__get_cmd_CKS(header)
        header[8+dataLen] = cks & 0xff
        header[9+dataLen] = cks >> 8
        self._PacketSize = 10 + dataLen
        return header
    
    def __get_Rcm_CKS(self, header):
        cks = 0xff
        cks += header[2]
        cks += header[3]
        cks += header[4]
        cks += header[5]
        cks += header[6]
        cks += header[7]
        cks += header[8]
        cks += header[9]
        lens = (header[7] << 8) | header[6]
        if lens > 0:
            for i in range(lens - 2):
                cks += header[10+i]
        return cks & 0xffff

    def __get_cmd_CKS(self, header):
        cks = 0xff
        cks += header[2]
        cks += header[3]
        cks += header[4]
        cks += header[5]
        cks += header[6]
        cks += header[7]
        LEN = (header[7] << 8) | header[6]
        if LEN > 0:
            for i in range(LEN):
                cks += header[8+i]
        return cks & 0xffff
    
    def __write_to_addr(self, value):
        if not isinstance(value, list):
            value = [value]
        self._i2c.writeto(self.i2c_addr, value)

    def __read_to_addr(self, lens):
        return self._i2c.readfrom(self.i2c_addr, lens)


    
