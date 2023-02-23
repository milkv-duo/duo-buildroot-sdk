#!/usr/local/bin/python
from array import array
import cv_usb_util.cv_usb_pkt as pkt
import os
import serial.tools.list_ports
import sys
import time


class cv_usb_pyserial:
    # Table of CRC constants - implements x^16+x^12+x^5+1
    crc16_tab = [
        0x0000,
        0x1021,
        0x2042,
        0x3063,
        0x4084,
        0x50A5,
        0x60C6,
        0x70E7,
        0x8108,
        0x9129,
        0xA14A,
        0xB16B,
        0xC18C,
        0xD1AD,
        0xE1CE,
        0xF1EF,
        0x1231,
        0x0210,
        0x3273,
        0x2252,
        0x52B5,
        0x4294,
        0x72F7,
        0x62D6,
        0x9339,
        0x8318,
        0xB37B,
        0xA35A,
        0xD3BD,
        0xC39C,
        0xF3FF,
        0xE3DE,
        0x2462,
        0x3443,
        0x0420,
        0x1401,
        0x64E6,
        0x74C7,
        0x44A4,
        0x5485,
        0xA56A,
        0xB54B,
        0x8528,
        0x9509,
        0xE5EE,
        0xF5CF,
        0xC5AC,
        0xD58D,
        0x3653,
        0x2672,
        0x1611,
        0x0630,
        0x76D7,
        0x66F6,
        0x5695,
        0x46B4,
        0xB75B,
        0xA77A,
        0x9719,
        0x8738,
        0xF7DF,
        0xE7FE,
        0xD79D,
        0xC7BC,
        0x48C4,
        0x58E5,
        0x6886,
        0x78A7,
        0x0840,
        0x1861,
        0x2802,
        0x3823,
        0xC9CC,
        0xD9ED,
        0xE98E,
        0xF9AF,
        0x8948,
        0x9969,
        0xA90A,
        0xB92B,
        0x5AF5,
        0x4AD4,
        0x7AB7,
        0x6A96,
        0x1A71,
        0x0A50,
        0x3A33,
        0x2A12,
        0xDBFD,
        0xCBDC,
        0xFBBF,
        0xEB9E,
        0x9B79,
        0x8B58,
        0xBB3B,
        0xAB1A,
        0x6CA6,
        0x7C87,
        0x4CE4,
        0x5CC5,
        0x2C22,
        0x3C03,
        0x0C60,
        0x1C41,
        0xEDAE,
        0xFD8F,
        0xCDEC,
        0xDDCD,
        0xAD2A,
        0xBD0B,
        0x8D68,
        0x9D49,
        0x7E97,
        0x6EB6,
        0x5ED5,
        0x4EF4,
        0x3E13,
        0x2E32,
        0x1E51,
        0x0E70,
        0xFF9F,
        0xEFBE,
        0xDFDD,
        0xCFFC,
        0xBF1B,
        0xAF3A,
        0x9F59,
        0x8F78,
        0x9188,
        0x81A9,
        0xB1CA,
        0xA1EB,
        0xD10C,
        0xC12D,
        0xF14E,
        0xE16F,
        0x1080,
        0x00A1,
        0x30C2,
        0x20E3,
        0x5004,
        0x4025,
        0x7046,
        0x6067,
        0x83B9,
        0x9398,
        0xA3FB,
        0xB3DA,
        0xC33D,
        0xD31C,
        0xE37F,
        0xF35E,
        0x02B1,
        0x1290,
        0x22F3,
        0x32D2,
        0x4235,
        0x5214,
        0x6277,
        0x7256,
        0xB5EA,
        0xA5CB,
        0x95A8,
        0x8589,
        0xF56E,
        0xE54F,
        0xD52C,
        0xC50D,
        0x34E2,
        0x24C3,
        0x14A0,
        0x0481,
        0x7466,
        0x6447,
        0x5424,
        0x4405,
        0xA7DB,
        0xB7FA,
        0x8799,
        0x97B8,
        0xE75F,
        0xF77E,
        0xC71D,
        0xD73C,
        0x26D3,
        0x36F2,
        0x0691,
        0x16B0,
        0x6657,
        0x7676,
        0x4615,
        0x5634,
        0xD94C,
        0xC96D,
        0xF90E,
        0xE92F,
        0x99C8,
        0x89E9,
        0xB98A,
        0xA9AB,
        0x5844,
        0x4865,
        0x7806,
        0x6827,
        0x18C0,
        0x08E1,
        0x3882,
        0x28A3,
        0xCB7D,
        0xDB5C,
        0xEB3F,
        0xFB1E,
        0x8BF9,
        0x9BD8,
        0xABBB,
        0xBB9A,
        0x4A75,
        0x5A54,
        0x6A37,
        0x7A16,
        0x0AF1,
        0x1AD0,
        0x2AB3,
        0x3A92,
        0xFD2E,
        0xED0F,
        0xDD6C,
        0xCD4D,
        0xBDAA,
        0xAD8B,
        0x9DE8,
        0x8DC9,
        0x7C26,
        0x6C07,
        0x5C64,
        0x4C45,
        0x3CA2,
        0x2C83,
        0x1CE0,
        0x0CC1,
        0xEF1F,
        0xFF3E,
        0xCF5D,
        0xDF7C,
        0xAF9B,
        0xBFBA,
        0x8FD9,
        0x9FF8,
        0x6E17,
        0x7E36,
        0x4E55,
        0x5E74,
        0x2E93,
        0x3EB2,
        0x0ED1,
        0x1EF0,
    ]

    def crc16_ccitt(self, HexData):
        cksum = int("0", 16)
        for i in range(len(HexData)):
            cksum = self.crc16_tab[((cksum >> 8) ^ HexData[i]) & 0xFF] ^ (
                (cksum << 8) & 0xFF00
            )
        return cksum

    def serial_query(self, vid_pid_list, timeout=0, verify=0, location=None):
        if location is not None:
            self.location = location
        progress_symbol = ["---", " \\", " \\", " |", " |", " |", " |", " /", " /"]
        found = -1
        i = 0
        query_time = time.time()
        while found == -1:
            time.sleep(0.05)
            i = i + 1
            sys.stdout.write(
                "Waiting for USB port: %s    \r"
                % (progress_symbol[i % len(progress_symbol)])
            )
            sys.stdout.flush()
            comlist = serial.tools.list_ports.comports()
            for element in comlist:
                if self.location is not None:
                    # print(self.location + " " + element.hwid)
                    if element.hwid.find(self.location) == -1:
                        continue
                for vid_pid in vid_pid_list:
                    found = element.hwid.find(vid_pid)
                    if found != -1:
                        break
                if found != -1:
                    break

            if timeout != 0 and ((time.time() - query_time) > timeout):
                sys.stdout.write("Query pyserial device timeout!        \n")
                sys.stdout.flush()
                return pkt.TIMEOUT

        print(element.device)
        print(element.hwid)

        if verify == 1:
            return

        self.device = serial.Serial(timeout=10000, writeTimeout=0.5)
        self.device.port = element.device
        self.device.baudrate = 115200
        # self.device.set_buffer_size(rx_size = 16777216, tx_size = 16777216)
        # self.device.writeTimeout = 0.5
        time.sleep(0.1)
        self.device.close()
        connect = -1
        i = 0
        while connect == -1:
            i = i + 1
            try:
                self.device.open()
                connect = 1
                sys.stdout.write("\n")
                sys.stdout.flush()
            except serial.serialutil.SerialException as e:
                if timeout != 0 and ((time.time() - query_time) > timeout):
                    sys.stdout.write("Query device timeout!       \n")
                    sys.stdout.flush()
                    sys.exit(-1)
                sys.stdout.write(
                    "Waiting for USB connect: %s    \r"
                    % (progress_symbol[i % len(progress_symbol)])
                )
                sys.stdout.flush()
                time.sleep(0.05)
        self.device.flushOutput()

        return element.pid

    def serial_write(self, command, recv_ack, delay_ms):
        ret = array("B")

        try:
            # time.sleep(0.001 * delay_ms)
            # if self.pkt_cnt < 10 and self.filesize != 128:
            #     time.sleep(0.3)
            self.device.write(command)
            self.device.flushOutput()
        except serial.SerialTimeoutException as e:
            # print("Write data timeout")
            return pkt.FAIL

        cmd_crc = self.crc16_ccitt(command)
        # print ("cmd_crc %x" % cmd_crc)

        if recv_ack == 1:
            try:
                ret = self.device.read(16)
            except serial.SerialTimeoutException as e:
                print("Read ACK timeout")
            try:
                ret_crc = self.convert(ret[2]) * 256 + self.convert(ret[3])
            except IndexError:
                ret_crc = 0
            # print ("ret_crc %x" % ret_crc)

            if ret_crc == cmd_crc:
                self.pkt_cnt += 1
                # sys.stdout.write("Packet count: %d   \r" % self.pkt_cnt)
                sys.stdout.write(
                    "[Working] %d%%    \r"
                    % ((self.pkt_cnt * (512 - pkt.HEADER_SIZE) * 100) / self.filesize)
                )
                sys.stdout.flush()
                # print ("cmd_crc %x == ret_crc %x" % (cmd_crc, ret_crc))
                pkt.FIP_TX_OFFSET = self.convert(ret[8]) * (2 ** 24) + self.convert(ret[9]) * (2 ** 16) + self.convert(ret[10]) * (2 ** 8) + self.convert(ret[11])
                pkt.FIP_TX_SIZE = self.convert(ret[12]) * (2 ** 24) + self.convert(ret[13]) * (2 ** 16) + self.convert(ret[14]) * (2 ** 8) + self.convert(ret[15])
                # print("fip_tx_offset %d" % pkt.FIP_TX_OFFSET)
                # print("fip_tx_size %d" % pkt.FIP_TX_SIZE)

                return pkt.SUCCESS
            else:
                print("ACK_CRC_ERROR cmd_crc %x; ret_crc %x" % (cmd_crc, ret_crc))
                return pkt.FAIL
        else:
            return pkt.SUCCESS


    def usb_send_file(self, filename, dest_addr, delay_ms, last_pos=0):
        start_time = time.time()
        complete_cnt = 0
        self.pkt_cnt = 0
        self.filesize = 0
        while (complete_cnt < 1):  # For stress test
            complete_cnt = complete_cnt + 1
            tx_len = 512
            content_file = open(filename, 'rb')
            content_size = os.path.getsize(filename)
            self.filesize = content_size
            print("%s is %d bytes" % (filename, content_size))
            print("Send to address 0x%x" % dest_addr)

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
                self.header.append((dest_addr >> 8) & 0xFF)
                self.header.append(dest_addr & 0xFF)

                self.bulk_command = self.header + self.data
                send_ok = self.serial_write(self.bulk_command, 1, delay_ms)

                if send_ok == 0:
                    dest_addr += tx_len - pkt.HEADER_SIZE
                    content_size -= tx_len - pkt.HEADER_SIZE
                else:
                    last_pos -= tx_len - pkt.HEADER_SIZE

            # print ("complete_cnt %d" % complete_cnt)
            print("--- %s Seconds ---" % round(time.time() - start_time, 2))
        return


    def usb_send_chunk(self, chunk, size, dest_addr, delay_ms, Type="file"):
        start_time = time.time()
        complete_cnt = 0
        self.pkt_cnt = 0
        self.filesize = 0
        while complete_cnt < 1:  # For stress test
            complete_cnt = complete_cnt + 1
            # tx_len = 512
            tx_len = 256	# for usb fifo
            content_file = chunk
            content_size = size
            self.filesize = content_size
            last_pos = content_file.tell()
            # print("Send to address 0x%x" % dest_addr)

            while content_size > 0:
                del self.header[:]
                del self.data[:]
                del self.bulk_command[:]
                content_file.seek(last_pos)
                if content_size < tx_len - pkt.HEADER_SIZE:
                    self.data.fromfile(content_file, content_size)
                    tx_len = content_size + pkt.HEADER_SIZE
                else:
                    self.data.fromfile(content_file, tx_len - pkt.HEADER_SIZE)
                last_pos = content_file.tell()

                if Type == "magic":
                    self.header.append(pkt.CV_USB_KEEP_DL)
                else:
                    self.header.append(pkt.CVI_USB_TX_DATA_TO_RAM)

                self.header.append((tx_len >> 8) & 0xFF)
                self.header.append((tx_len & 0xFF))

                self.header.append((dest_addr >> 32) & 0xFF)
                self.header.append((dest_addr >> 24) & 0xFF)
                self.header.append((dest_addr >> 16) & 0xFF)
                self.header.append((dest_addr >> 8) & 0xFF)
                self.header.append(dest_addr & 0xFF)

                self.bulk_command = self.header + self.data
                send_ok = self.serial_write(self.bulk_command, 1, delay_ms)

                if send_ok == 0:
                    dest_addr += tx_len - pkt.HEADER_SIZE
                    content_size -= tx_len - pkt.HEADER_SIZE
                else:
                    last_pos -= tx_len - pkt.HEADER_SIZE

            # print ("complete_cnt %d" % complete_cnt)
            print("--- %s Seconds ---" % round(time.time() - start_time, 2))
        return

    def usb_send_req_kernel(self, token, reqLen, file_name, ack):
        del self.ser_cmd[:]
        self.ser_cmd.append(token)  # command
        # self.ser_cmd.append(0) # high byte of packet size
        self.ser_cmd.append((reqLen >> 32) & 0xFF)  # 5 bytes for destination address
        self.ser_cmd.append((reqLen >> 24) & 0xFF)
        self.ser_cmd.append((reqLen >> 16) & 0xFF)
        self.ser_cmd.append((reqLen >> 8) & 0xFF)
        self.ser_cmd.append((reqLen & 0xFF))
        # file_name_List = list(file_name)
        self.ser_cmd.fromstring(file_name)

        if ack != 0:
            return _usb_send_req(self.ser_cmd, 1, 0)
        else:
            return self.serial_write(self.ser_cmd, 0, 0)

    def protocol_msg_read(self, message, length):
        try:
            self.device.write(message)
            self.device.flushOutput()
        except serial.SerialTimeoutException as e:
            print("Write data fail!")
            return pkt.FAIL

        try:
            ret = self.device.read(length)
        except serial.SerialTimeoutException as e:
            print("Read data fail")
            return pkt.FAIL

        return ret

    def protocol_msg_send(self, message, length, response):
        start_time = time.time()
        try:
            self.device.write(message)
            self.device.flushOutput()
        except serial.SerialTimeoutException as e:
            print("Write data timeout")
            return pkt.FAIL

        self.ioTime = self.ioTime + (time.time() - start_time)

        if response == 1:
            start_time = time.time()
            try:
                ret = self.device.read(16)
            except serial.SerialTimeoutException as e:
                print("Read ACK timeout")

            self.ioTime = self.ioTime + (time.time() - start_time)

            cmd_crc = self.crc16_ccitt(message)
            # print ("cmd_crc %x" % cmd_crc)

            ret_crc = self.convert(ret[pkt.RSP_CRC16_HI_OFFSET]) * 256 + self.convert(
                ret[pkt.RSP_CRC16_LO_OFFSET]
            )
            # print ("ret_crc %x" % ret_crc)

            if ret_crc == cmd_crc:
                if message[pkt.MSG_TOKEN_OFFSET] != self.convert(
                    ret[pkt.RSP_TOKEN_OFFSET]
                ):
                    sys.stdout.write(
                        "Token: exp 0x%x get 0x%x\n"
                        % (
                            message[pkt.MSG_TOKEN_OFFSET],
                            ord(ret[pkt.RSP_TOKEN_OFFSET]),
                        )
                    )
                    sys.stdout.flush()
                else:
                    # sys.stdout.write("Packet count: %d   \r" % self.pkt_cnt)
                    # sys.stdout.write("[Working] %d%%    \r" % ((self.pkt_cnt * (512 - pkt.HEADER_SIZE) * 100) / self.filesize))
                    # sys.stdout.flush()
                    return pkt.SUCCESS
            else:
                sys.stdout.write("ACK_CRC_ERROR")
                sys.stdout.flush()
                return pkt.FAIL
        else:
            return pkt.SUCCESS

    def protocol_msg_fill_header(self, message, token, addr, reqLen, dataSize):

        message.append(token)  # command
        message.append((reqLen >> 8) & 0xFF)  # high byte of packet size
        message.append(reqLen & 0xFF)  # low byte of packet size
        message.append((addr >> 32) & 0xFF)  # 5 bytes for destination address
        message.append((addr >> 24) & 0xFF)
        message.append((addr >> 16) & 0xFF)
        message.append((addr >> 8) & 0xFF)
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
            print("Write data timeout")
            return pkt.FAIL

        return pkt.SUCCESS

    def protocol_msg_d2s_once(self, length):
        msg = []
        self.protocol_msg_fill_header(
            msg, pkt.CV_USB_D2S, 0, pkt.USB_MSG_D2S_SIZE, 0
        )
        return self.protocol_msg_read(msg, length)

    def protocol_msg_s2d_once(self, addr, dataBuf, length):
        msg = []
        self.protocol_msg_fill_header(
            msg, pkt.CV_USB_S2D, addr, pkt.USB_MSG_S2D_SIZE, length
        )
        if self.protocol_msg_send(msg, pkt.USB_MSG_S2D_SIZE, 1) == pkt.FAIL:
            return pkt.FAIL
        else:
            return self.protocol_usb_write(dataBuf)

    def protocol_send_file(self, filename, dest_addr):
        complete_cnt = 0
        while complete_cnt < 1:  # For stress test
            tmp_addr = dest_addr
            self.ioTime = 0
            start_time = time.time()
            complete_cnt = complete_cnt + 1
            last_pos = 0
            tx_len = pkt.USB_BULK_MAX_SIZE
            content_file = open(filename, "rb")
            content_size = os.path.getsize(filename)
            file_size = content_size
            print("%s is %d bytes" % (filename, content_size))
            print("Send to address 0x%x" % tmp_addr)

            while content_size > 0:
                del self.data[:]
                content_file.seek(last_pos)
                if content_size < tx_len:
                    self.data.fromfile(content_file, content_size)
                    tx_len = content_size
                else:
                    self.data.fromfile(content_file, tx_len)
                last_pos = content_file.tell()

                send_ok = self.protocol_msg_s2d_once(tmp_addr, self.data, tx_len)

                if send_ok == 0:
                    tmp_addr += tx_len
                    content_size -= tx_len
                    sys.stdout.write(
                        "[Working] %d%%    \r"
                        % (((file_size - content_size) * 100) / file_size)
                    )
                    sys.stdout.flush()
                else:
                    last_pos -= tx_len

            # print ("complete_cnt %d" % complete_cnt)
            print("--- %s Seconds ---" % round(time.time() - start_time, 2))
            # print ("--- %s Seconds ---" % str(self.ioTime))
        return

    def usb_send_req_data(self, token, address, reqLen, data=None):
        del self.ser_cmd[:]
        self.ser_cmd.append(token)  # command
        self.ser_cmd.append(0)  # high byte of packet size
        self.ser_cmd.append(reqLen)  # low byte of packet size
        self.ser_cmd.append((address >> 32) & 0xFF)  # 5 bytes for destination address
        self.ser_cmd.append((address >> 24) & 0xFF)
        self.ser_cmd.append((address >> 16) & 0xFF)
        self.ser_cmd.append((address >> 8) & 0xFF)
        self.ser_cmd.append((address & 0xFF))
        if data is not None:
            self.ser_cmd = self.ser_cmd + data
        # else:
        #     print("data is empty!\n")
        #     return
        return self.serial_write(self.ser_cmd, 0, 0)

    def wait_for_reconnect(self, cnt_seconed):
        progress_symbol = ["---", " \\", " \\", " |", " |", " |", " |", " /", " /"]
        while cnt_seconed > 0:
            cnt_seconed = cnt_seconed - 1
            sys.stdout.write(
                "Waiting for re-connect: %s    \r"
                % (progress_symbol[cnt_seconed % len(progress_symbol)])
            )
            sys.stdout.flush()
            time.sleep(0.5)

    def vidpid_string(self, vid_str, pid_str):
        vid_str = vid_str.replace("0x", "")
        pid_str = pid_str.replace("0x", "")
        while len(vid_str) < 4:
            vid_str = "0" + vid_str
        while len(pid_str) < 4:
            pid_str = "0" + pid_str
        verify_vidpid = verify_vidpid + vid_str + ":" + pid_str

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
        sys.stdout.write("       [script] : %s \n" % __file__)
        sys.stdout.write(
            "       [vvid]   : reconnect vid if emmc dl complete check enable \n"
        )
        sys.stdout.write(
            "       [vpid]   : reconnect pid if emmc dl complete check enable \n"
        )
        sys.stdout.write(
            "       [timeout]: verify timeout (second) from emmc programming to kerel start \n"
        )
        sys.stdout.write("                  0 means wait forever (default)  \n")
        sys.stdout.flush()

    def parse_arg(self):
        vid = ""
        pid = ""
        for i in range(1, len(sys.argv)):
            if "vvid" in sys.argv[i]:
                vid = sys.argv[i]
                vid = vid.replace("vvid=", "")
            if "vpid" in sys.argv[i]:
                pid = sys.argv[i]
                pid = pid.replace("vpid=", "")
            if "timeout" in sys.argv[i]:
                s = sys.argv[i]
                s = s.replace("timeout=", "")
                self.emmc_timeout = int(s)
                print("emmc timeout = %d s" % self.emmc_timeout)
            if "location" in sys.argv[i]:
                self.location = sys.argv[i]
                self.location = self.location.replace("location=", "")
                print("bus location = " + self.location)
            if "stdout" in sys.argv[i]:
                self.stdout = sys.argv[i]
                self.stdout = self.stdout.replace("stdout=", "")
                print("stdout = " + self.stdout)
                sys.stdout = open(self.stdout, "a")
            if "usage" in sys.argv[i]:
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
        self.header = array("B")
        self.data = array("B")
        self.bulk_command = array("B")
        self.ser_cmd = array("B")
        self.ioTime = 0
        self.pkt_cnt = 0
        self.filesize = 0
        self.emmc_timeout = 0
        self.location = None
        self.stdout = None
        self.python_version = 3
        if sys.version_info[0] < 3:
            self.python_version = 2
