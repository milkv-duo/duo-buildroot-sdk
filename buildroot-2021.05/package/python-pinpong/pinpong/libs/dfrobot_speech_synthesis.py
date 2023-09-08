# -*- coding: utf-8 -*
import time
from pinpong.board import I2C

class DFRobot_SpeechSynthesis(object):
    V1 = 1
    V2 = 2
    
    I2C_ADDR = 0x40  # i2c address
    INQUIRYSTATUS = 0x21  # Check status
    ENTERSAVEELETRI = 0x88  #
    WAKEUP = 0xFF  # Wake-up command

    START_SYNTHESIS = 0x01  # start synthesis command 0
    START_SYNTHESIS1 = 0x05  # Start synthesis command 1
    STOP_SYNTHESIS = 0x02  # End speech synthesis
    PAUSE_SYNTHESIS = 0x03  # pause speech synthesis command
    RECOVER_SYNTHESIS = 0x04  # Resume speech synthesis commands
    I2C_MODE = 0x01
    UART_MODE = 0x02

    MALE = 0x04  # Male voice
    FEMALE = 0x03  # female voice
    MALE2 = 0x08  
    FEMALE2 = 0x09
    DONALDDUCK = 0x05  # Voice of Donald Duck
    FEMALE3 = 0x10

    ALPHABET = 0X06  # Spell
    WORD = 0X07  # word


    CHINESE = 0XA0  # Chinese
    ENGLISH = 0XA1  # English
    NONE = 0XA2   #
    CATON = 0XA3  # Word by word
    SMOOTH = 0XA4  # Fluently
    PINYIN_ENABLE = 0XA5  # Use Pinyin to read
    PINYIN_DISABLE = 0XA6  # don't use pinyin pronunciation
    CHINESEL = 0XA7   #
    ENGLISHL = 0XA8   #
    AUTOJUDGEL = 0XA9  # Auto Judge
    NUMBER = 0XAA  # Telephone number
    NUMERIC = 0XAB   #
    AUTOJUDGED = 0XAC  # Auto Judge
    ZREO = 0XAD  # Read as 'zero'
    OU = 0XAE  # Read as'ou'
    YAO = 0XAF  # Read as 'yao'
    CHONE = 0XB0  # Read as 'yi'
    NAME = 0XB1   #
    AUTOJUDGEDN = 0XB2   #
    ERROR = -1

    def __init__(self):
        self._is_V1 = False
        self._index = 0
        self._len = 0

    def begin(self, version):
        init = [0xAA]
        if version == self.V1:
            self._is_V1 = True
            self.write_cmd(init)
            time.sleep(0.05)
            self.speak_lish("[n1]")
            self.set_voice(5)
            self.set_speed(5)
            self.set_tone(5)
            self.set_sound_type(self.FEMALE)
            self.set_english_pron(self.WORD)
        else:
            for i in range(40):
                self.write_cmd(init)
                time.sleep(0.05)
                check = [0xFD, 0x00, 0x01, 0x21]
                self.write_cmd(check)
                if self.read_ack(1)[0] == 0x4F:
                    break
            self.set_voice(1)

    def set_voice(self, voc):
        str = "[v3]"
        if voc > 9:
            voc = 9
        str = str[:2] + chr(48 + voc) + str[3:]
        self.speak_lish(str)

    def speak(self, string):
        str = string.encode('gb2312')
        head = [0xfd, 0x00, 0x00, 0x01, 0x00]
        data = list(str)
        data2 = (list(data))
        lenght = len(data2) + 2
        head[1] = lenght >> 8
        head[2] = lenght & 0xff
        data1 = head + data2
        # self.read_ack(1)
        # self.read_ack(1)
        self.write_cmd(data1)
        self.wait()
        return

    def wait(self):
        if self._is_V1:
            if self.board.boardname == "MICROBIT":
                while True:
                    if self.readACK1() == 0x4F:
                        break
                    time.sleep(0.2)
            else:
                while True:
                    if self.readACK() == 0x41:
                        break
                    time.sleep(0.2)
                while True:
                    if self.readACK() == 0x4F:
                        break
                    time.sleep(0.2)
        else:
            if self.board.boardname == "MICROBIT":
                while True:
                    if self.readACK1() == 0x41:
                        break
                    time.sleep(0.2)
                time.sleep(0.1)
                while True:
                    check = [0xFD, 0x00, 0x01, 0x21]
                    self.write_cmd(check)
                    if self.readACK1() == 0x4f:
                        break
                    time.sleep(0.02)
            else:    
                while True:
                    if self.readACK() == 0x41:
                        break
                    time.sleep(0.2)
                time.sleep(0.1)
                while True:
                    check = [0xFD, 0x00, 0x01, 0x21]
                    self.write_cmd(check)
                    if self.readACK() == 0x4f:
                        break
                    time.sleep(0.02)
                
    def readACK(self):
        data = self.read_ack(1)
        return data[0]

    def readACK1(self):
        data = self.read_ack(2)
        return 0x4F if data[0] == 0x4F or data[1] == 0x4F else data[0]

    def speak_lish(self, word):
        self._len = len(word)
        point = 0
        _unicode = [0 for i in range(self._len)]
        while self._index < self._len:
            _unicode[point] = ord((word[self._index])) & 0x7f
            self._index += 1
            point += 1
        self.send_pack(self.START_SYNTHESIS1, _unicode, self._len)
        self.wait()
        self._index = 0
        self._len = 0

    def send_pack(self, cmd, data, len):
        length = 0
        head = [0, 0, 0, 0, 0]
        head[0] = 0xfd
        if cmd == self.START_SYNTHESIS:
            pass
        elif cmd == self.START_SYNTHESIS1:
            length = 2 + len
            head[1] = length >> 8
            head[2] = length & 0xff
            head[3] = self.START_SYNTHESIS
            if self._is_V1:
                head[4] = 0x00
            else:
                head[4] = 0x04
            self.send_command(head, data, len)

    def send_command(self, head, data, len):
        self.write_cmd(head)
        self.write_cmd(data)

    def set_speed(self, speed):
        str = "[s5]"
        if speed > 9:
            speed = 9
        str = str[:2] + chr(48 + speed) + str[3:]
        self.speak_lish(str)

    def set_sound_type(self, type):
        if(type == self.MALE):
            str = "[m51]"
            self.speak_lish(str)
        elif(type == self.MALE2):
            str = "[m52]"
            self.speak_lish(str)
        elif(type == self.FEMALE):
            str = "[m3]"
            self.speak_lish(str)
        elif(type == self.FEMALE2):
            str = "[m53]"
            self.speak_lish(str)
        elif(type == self.DONALDDUCK):
            str = "[m54]"
            self.speak_lish(str)
        elif(type == self.FEMALE3):
            str = "[m55]"
            self.speak_lish(str)
        else:
            print("no that type")

    def set_tone(self, tone):
        str = "[t5]"
        if tone > 9:
            tone = 9
        str = str[:2] + chr(48 + tone) + str[3:]
        self.speak_lish(str)

    def set_english_pron(self, pron):
        if(pron == self.ALPHABET):
            str = "[h1]"
            self.speak_lish(str)
        elif(pron == self.WORD):
            str = "[h2]"
            self.speak_lish(str)

    def enable_rhythm(self, enable):
        if(enable == True):
            str1 = "[z1]"
        elif(enable == False):
            str1 = "[z0]"
        self.speak_lish(str1)

    def set_digital_pron(self, pron):
        if(pron == self.NUMBER):
            str1 = "[n1]"
        elif(pron == self.NUMERIC):
            str1 = "[n2]"
        elif(pron == self.AUTOJUDGED):
            str1 = "[n0]"
        self.speak_lish(str1)

    def enable_pinyin(self, enable):
        if(enable == True):
            str1 = "[i1]"
        elif(enable == False):
            str1 = "[i0]"
        self.speak_lish(str1)

    def set_speech_style(self, style):
        if(style == self.CATON):
            str1 = "[f0]"
        elif(style == self.SMOOTH):
            str1 = "[f1]"
        else:
            str1 = "[f0]"
        self.speak_lish(str1)

    def set_language(self, style):
        if(style == self.CHINESEL):
            str1 = "[g1]"
        elif(style == self.ENGLISHL):
            str1 = "[g2]"
        elif(style == self.AUTOJUDGEL):
            str1 = "[g0]"
        else:
            str1 = "[g1]"
        self.speak_lish(str1)

    def set_zero_pron(self, pron):
        if(pron == self.ZREO):
            str1 = "[o0]"
        elif(pron == self.OU):
            str1 = "[o1]"
        else:
            str1 = "[o0]"
        self.speak_lish(str1)

    def set_one_pron(self, pron):
        if(pron == self.YAO):
            str1 = "[y0]"
        elif(pron == self.CHONE):
            str1 = "[y1]"
        else:
            str1 = "[y0]"
        self.speak_lish(str1)

    def set_name_pron(self, pron):
        if(pron == self.NAME):
            str1 = "[r1]"
        elif(pron == self.AUTOJUDGEDN):
            str1 = "[r0]"
        else:
            str1 = "[r1]"
        self.speak_lish(str1)

    def reset(self):
        self.speak_lish("[d]")

class DFRobot_SpeechSynthesis_I2C(DFRobot_SpeechSynthesis):
    def __init__(self, board=None, i2c_addr=0x40, bus_num=0):
        from pinpong.board import gboard
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard
        self.board = board
        self.__addr = i2c_addr
        self._i2c = I2C(bus_num)
        super(DFRobot_SpeechSynthesis_I2C, self).__init__()

    def write_cmd(self, data):
        self._i2c.writeto(self.__addr, data)

    def read_ack(self, len):
        rslt = self._i2c.readfrom(self.__addr, len)
        return rslt

