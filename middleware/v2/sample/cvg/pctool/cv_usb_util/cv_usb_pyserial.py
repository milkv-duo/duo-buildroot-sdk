#!/usr/local/bin/python
from array import array
import cv_usb_util.cv_usb_pkt as pkt
import os
import re
import serial.tools.list_ports
import sys
import time

class cv_usb_pyserial:
    # Table of CRC constants - implements x^16+x^12+x^5+1
    crc16_tab = [
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
    ]

    def crc16_ccitt(self, HexData):
        cksum = int("0", 16)
        for i in range(len(HexData)):
            cksum = self.crc16_tab[((cksum >> 8) ^ HexData[i]) & 0xff] ^ ((cksum << 8) & 0xff00)
        return cksum

    def serial_query(self, vid_pid_list, timeout=0, verify=0, location=None):
        if location is not None:
            self.location = location
        progress_symbol = ['---', ' \\', ' \\', ' |', ' |', ' |', ' |', ' /', ' /']
        found = -1
        i = 0
        query_time = time.time()
        while(found == -1):
            time.sleep(0.05)
            i = i + 1
            sys.stdout.write("Waiting for cv1835 USB port: %s    \r" % (progress_symbol[i%len(progress_symbol)]))
            sys.stdout.flush()
            comlist = serial.tools.list_ports.comports()
            for element in comlist:
                if self.location is not None:
                    #print(self.location + " " + element.hwid)
                    if element.hwid.find(self.location) == -1:
                        continue
                for vid_pid in vid_pid_list:
                    found = element.hwid.find(vid_pid)
                    if found != -1:
                        break
                if found != -1:
                    break;

            if timeout != 0 and ((time.time() - query_time) > timeout):
                sys.stdout.write("Query device timeout!       \n" )
                sys.stdout.flush()
                sys.exit(-1)

        print (element.device)
        print (element.hwid)

        if verify == 1:
            return

        self.device = serial.Serial(timeout=10000, writeTimeout = 0.5)
        self.device.port = element.device
        self.device.baudrate = 115200
        #self.device.set_buffer_size(rx_size = 16777216, tx_size = 16777216)
        #self.device.writeTimeout = 0.5
        time.sleep(0.1)
        self.device.close()
        connect = -1
        i = 0
        while (connect == -1):
            i = i + 1
            try:
                self.device.open()
                connect = 1
                sys.stdout.write("\n")
                sys.stdout.flush()
            except serial.serialutil.SerialException as e:
                if timeout != 0 and ((time.time() - query_time) > timeout):
                    sys.stdout.write("Query device timeout!       \n" )
                    sys.stdout.flush()
                    sys.exit(-1)
                sys.stdout.write("Waiting for cv1835 USB connect: %s    \r" % (progress_symbol[i%len(progress_symbol)]))
                sys.stdout.flush()
                time.sleep(0.05)
        self.device.flushOutput()

        return

    def serial_write(self, command, recv_ack, delay_ms):
        ret = array('B')
        # print("Token %d" % command[0])
        if command[0] == pkt.CV_USB_JUMP_ACK:
            recv_ack = 1
            print("CV_USB_JUMP_ACK")

        try:
            #time.sleep(0.001 * delay_ms)
            # if self.pkt_cnt < 10 and self.filesize != 128:
            #     time.sleep(0.3)
            self.device.write(command)
            self.device.flushOutput()
        except serial.SerialTimeoutException as e:
            print ("Write data timeout")

        cmd_crc = self.crc16_ccitt(command)
        # print ("cmd_crc %x" % cmd_crc)

        if recv_ack == 1:
            try:
                ret = self.device.read(16);
            except serial.SerialTimeoutException as e:
                print ("Read ACK timeout")

            ret_crc = self.convert(ret[2])*256 + self.convert(ret[3])
            # print ("ret_crc %x" % ret_crc)

            if ret_crc == cmd_crc:
                self.pkt_cnt += 1
                # sys.stdout.write("Packet count: %d   \r" % self.pkt_cnt)
                sys.stdout.write("[Working] %d%%    \r" % ((self.pkt_cnt * (512 - pkt.HEADER_SIZE) * 100) / self.filesize))
                sys.stdout.flush()
                #print ("cmd_crc %x == ret_crc %x" % (cmd_crc, ret_crc))
                return pkt.SUCCESS
            else:
                print ("ACK_CRC_ERROR cmd_crc %x; ret_crc %x" % (cmd_crc, ret_crc))
                return pkt.FAIL
        else:
            return pkt.SUCCESS

    def usb_send_file(self, filename, dest_addr, delay_ms):
        start_time = time.time()
        complete_cnt = 0
        self.pkt_cnt = 0
        self.filesize = 0
        while (complete_cnt < 1): # For stress test
            complete_cnt = complete_cnt + 1
            last_pos = 0
            tx_len = 512
            content_file = open(filename, 'rb')
            content_size = os.path.getsize(filename)
            self.filesize = content_size
            print ("%s is %d bytes" % (filename, content_size))
            print ("Send to address 0x%x" % dest_addr)

            while (content_size > 0):
                del self.header[:]
                del self.data[:]
                del self.bulk_command[:]

                content_file.seek(last_pos)
                if (content_size < tx_len - pkt.HEADER_SIZE):
                    self.data.fromfile(content_file, content_size)
                    tx_len = content_size + pkt.HEADER_SIZE
                else:
                    self.data.fromfile(content_file, tx_len - pkt.HEADER_SIZE)
                last_pos = content_file.tell()

                if filename == 'cv_dl_magic.bin':
                    self.header.append(pkt.CV_USB_KEEP_DL)
                else:
                    self.header.append(pkt.CV_USB_NONE)

                self.header.append((tx_len >> 8) & 0xFF)
                self.header.append((tx_len & 0xFF))

                self.header.append((dest_addr >> 32) & 0xFF)
                self.header.append((dest_addr >> 24) & 0xFF)
                self.header.append((dest_addr >> 16) & 0xFF)
                self.header.append((dest_addr >> 8)  & 0xFF)
                self.header.append(dest_addr & 0xFF)

                self.bulk_command = self.header + self.data
                send_ok = self.serial_write(self.bulk_command, 1, delay_ms)

                if send_ok == 0:
                    dest_addr += tx_len - pkt.HEADER_SIZE
                    content_size -= tx_len - pkt.HEADER_SIZE
                else:
                    last_pos -= tx_len - pkt.HEADER_SIZE

            # print ("complete_cnt %d" % complete_cnt)
            print ("--- %s Seconds ---" % round(time.time() - start_time, 2 ))
        return

    def usb_send_req_kernel(self, token, reqLen, file_name, ack):
        del self.ser_cmd[:]
        self.ser_cmd.append(token) # command
        #self.ser_cmd.append(0) # high byte of packet size
        self.ser_cmd.append((reqLen >> 32) & 0xFF) # 5 bytes for destination address
        self.ser_cmd.append((reqLen >> 24) & 0xFF)
        self.ser_cmd.append((reqLen >> 16) & 0xFF)
        self.ser_cmd.append((reqLen >> 8)  & 0xFF)
        self.ser_cmd.append((reqLen & 0xFF))
        #file_name_List = list(file_name)
        self.ser_cmd.fromstring(file_name)

        if ack != 0:
            return _usb_send_req(self.ser_cmd, 1, 0)
        else:
            return self.serial_write(self.ser_cmd, 0, 0)

    def protocol_msg_send(self, message, length, response):
        start_time = time.time()
        try:
            self.device.write(message)
            self.device.flushOutput()
        except serial.SerialTimeoutException as e:
            print ("Write data timeout")

        self.ioTime = self.ioTime + (time.time() - start_time)

        if response == 1:
            start_time = time.time()
            try:
                ret = self.device.read(16);
            except serial.SerialTimeoutException as e:
                print ("Read ACK timeout")

            self.ioTime = self.ioTime + (time.time() - start_time)

            cmd_crc = self.crc16_ccitt(message)
            #print ("cmd_crc %x" % cmd_crc)

            ret_crc = self.convert(ret[pkt.RSP_CRC16_HI_OFFSET])*256 + self.convert(ret[pkt.RSP_CRC16_LO_OFFSET])
            #print ("ret_crc %x" % ret_crc)

            if ret_crc == cmd_crc:
                if message[pkt.MSG_TOKEN_OFFSET] != self.convert(ret[pkt.RSP_TOKEN_OFFSET]):
                    sys.stdout.write("Token: exp 0x%x get 0x%x\n" % (message[pkt.MSG_TOKEN_OFFSET], ord(ret[pkt.RSP_TOKEN_OFFSET])))
                    sys.stdout.flush()
                else:
                    # sys.stdout.write("Packet count: %d   \r" % self.pkt_cnt)
                    #sys.stdout.write("[Working] %d%%    \r" % ((self.pkt_cnt * (512 - pkt.HEADER_SIZE) * 100) / self.filesize))
                    #sys.stdout.flush()
                    return pkt.SUCCESS
            else:
                sys.stdout.write("ACK_CRC_ERROR")
                sys.stdout.flush()
                return pkt.FAIL
        else:
            return pkt.SUCCESS

    def protocol_msg_fill_header(self, message, token, addr, reqLen, dataSize):

        message.append(token) # command
        message.append((reqLen >> 8) & 0xFF) # high byte of packet size
        message.append(reqLen & 0xFF) # low byte of packet size
        message.append((addr >> 32) & 0xFF) # 5 bytes for destination address
        message.append((addr >> 24) & 0xFF)
        message.append((addr >> 16) & 0xFF)
        message.append((addr >> 8)  & 0xFF)
        message.append((addr & 0xFF))
        if dataSize != 0:
            message.append((dataSize & 0xFF))
            message.append((dataSize >> 8 & 0xFF))
            message.append((dataSize >> 16 & 0xFF))
            message.append((dataSize >> 24 & 0xFF))
            message.append((dataSize >> 32 & 0xFF))
            message.append((dataSize >> 40 & 0xFF))
            message.append((dataSize >> 48 & 0xFF))
            message.append((dataSize >> 56 & 0xFF))
        return

    def protocol_usb_write(self, dataBuf):
        try:
            self.device.write(dataBuf)
            self.device.flushOutput()
        except serial.SerialTimeoutException as e:
            print ("Write data timeout")
            return pkt.FAIL

        return pkt.SUCCESS

    def protocol_msg_s2d_once(self, addr, dataBuf, length):
        msg = []
        self.protocol_msg_fill_header(msg, pkt.CV_USB_S2D, addr, pkt.USB_MSG_S2D_SIZE, length)
        if self.protocol_msg_send(msg, pkt.USB_MSG_S2D_SIZE, 1) == pkt.FAIL:
            return pkt.FAIL
        else:
            return self.protocol_usb_write(dataBuf)

    def protocol_send_file(self, filename, dest_addr):
        complete_cnt = 0
        while (complete_cnt < 1): # For stress test
            tmp_addr = dest_addr
            self.ioTime = 0
            start_time = time.time()
            complete_cnt = complete_cnt + 1
            last_pos = 0
            tx_len = pkt.USB_BULK_MAX_SIZE
            content_file = open(filename, 'rb')
            content_size = os.path.getsize(filename)
            file_size = content_size
            print ("%s is %d bytes" % (filename, content_size))
            print ("Send to address 0x%x" % tmp_addr)

            while (content_size > 0):
                del self.data[:]
                content_file.seek(last_pos)
                if (content_size < tx_len):
                    self.data.fromfile(content_file, content_size)
                    tx_len = content_size
                else:
                    self.data.fromfile(content_file, tx_len)
                last_pos = content_file.tell()

                send_ok = self.protocol_msg_s2d_once(tmp_addr, self.data, tx_len)

                if send_ok == 0:
                    tmp_addr += tx_len
                    content_size -= tx_len
                    sys.stdout.write("[Working] %d%%    \r" % (((file_size - content_size) * 100) / file_size))
                    sys.stdout.flush()
                else:
                    last_pos -= tx_len

            # print ("complete_cnt %d" % complete_cnt)
            print ("--- %s Seconds ---" % round(time.time() - start_time, 2 ))
            # print ("--- %s Seconds ---" % str(self.ioTime))
        return

    def usb_send_req_data(self, token, address, reqLen, data=None):
        del self.ser_cmd[:]
        self.ser_cmd.append(token) # command
        self.ser_cmd.append(0) # high byte of packet size
        self.ser_cmd.append(reqLen) # low byte of packet size
        self.ser_cmd.append((address >> 32) & 0xFF) # 5 bytes for destination address
        self.ser_cmd.append((address >> 24) & 0xFF)
        self.ser_cmd.append((address >> 16) & 0xFF)
        self.ser_cmd.append((address >> 8)  & 0xFF)
        self.ser_cmd.append((address & 0xFF))
        if data is not None:
            self.ser_cmd = self.ser_cmd + data
        # else:
        #     print("data is empty!\n")
        #     return
        return self.serial_write(self.ser_cmd, 0, 0)

    def wait_for_reconnect(self, cnt_seconed):
        progress_symbol = ['---', ' \\', ' \\', ' |', ' |', ' |', ' |', ' /', ' /']
        while cnt_seconed > 0:
            cnt_seconed = cnt_seconed - 1
            sys.stdout.write("Waiting for re-connect: %s    \r" % (progress_symbol[cnt_seconed%len(progress_symbol)]))
            sys.stdout.flush()
            time.sleep(0.5)

    def vidpid_string(self, vid_str, pid_str):
        vid_str = vid_str.replace("0x", "")
        pid_str = pid_str.replace("0x", "")
        while len(vid_str) < 4:
            vid_str = '0' + vid_str
        while len(pid_str) < 4:
            pid_str = '0' + pid_str
        verify_vidpid = verify_vidpid + vid_str + ':' + pid_str

    def usb_emmc_dl_verify(self, vid_pid_list, timeout):
        print("verify id = %s" % vid_pid_list)
        if self.emmc_timeout == 0:
            query_timeout = timeout
        else:
            query_timeout = self.emmc_timeout
        # print("emmc_timeout = %s s" % self.emmc_timeout)
        # print("query_timeout = %s s" % query_timeout)
        self.serial_query(vid_pid_list, query_timeout, 1)

    def show_usage(self):
        sys.stdout.write("usage- python [script] vvid=[vid] vpid=[pid]\n")
        sys.stdout.write("       [script] : %s \n" %__file__)
        sys.stdout.write("       [vvid]   : reconnect vid if emmc dl complete check enable \n")
        sys.stdout.write("       [vpid]   : reconnect pid if emmc dl complete check enable \n")
        sys.stdout.write("       [timeout]: verify timeout (second) from emmc programming to kerel start \n")
        sys.stdout.write("                  0 means wait forever (default)  \n")
        sys.stdout.flush()

    def parse_arg(self):
        vid = ''
        pid = ''
        for i in range(1, len(sys.argv)):
            if 'vvid' in sys.argv[i]:
                vid = sys.argv[i]
                vid = vid.replace('vvid=', '')
            if 'vpid' in sys.argv[i]:
                pid = sys.argv[i]
                pid = pid.replace('vpid=', '')
            if 'timeout' in sys.argv[i]:
                s = sys.argv[i]
                s = s.replace('timeout=', "")
                self.emmc_timeout = int(s)
                print("emmc timeout = %d s" % self.emmc_timeout)
            if 'location' in sys.argv[i]:
                self.location = sys.argv[i]
                self.location = self.location.replace('location=', '')
                print("bus location = " + self.location)
            if 'stdout' in sys.argv[i]:
                self.stdout = sys.argv[i]
                self.stdout = self.stdout.replace('stdout=', '')
                print("stdout = " + self.stdout)
                sys.stdout=open(self.stdout, "a")
            if 'storage' in sys.argv[i]:
                self.storage = sys.argv[i]
                self.storage = self.storage.replace('storage=', '')
                print("storage = " + self.storage)
            if 'usage' in sys.argv[i]:
                self.show_usage()
                sys.exit(0)

        if len(vid) != 0 and len(pid) != 0:
            self.vidpid_string(vid, pid)
        else:
            if len(vid) != 0:
                sys.stdout.write("pid is not appointed! \n")
                sys.stdout.flush()
                self.show_usage()
                sys.exit(-1)
            if len(pid) != 0:
                sys.stdout.write("vid is not appointed! \n")
                sys.stdout.flush()
                self.show_usage()
                sys.exit(-1)

    def convert(self, byte):
        if self.python_version == 2:
            return ord(byte)
        else:
            return byte

    def __init__(self):
        self.device = 0
        self.header = array('B')
        self.data = array('B')
        self.bulk_command = array('B')
        self.ser_cmd = array('B')
        self.ioTime = 0
        self.pkt_cnt = 0
        self.filesize = 0
        self.emmc_timeout = 0
        self.location = None
        self.stdout = None
        self.python_version = 3
        self.storage="emmc"
        if sys.version_info[0] < 3:
            self.python_version = 2;
