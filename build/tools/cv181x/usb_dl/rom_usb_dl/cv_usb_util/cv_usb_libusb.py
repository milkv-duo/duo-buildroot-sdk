#!/usr/local/bin/python
from array import array
import cv_usb_util.cv_usb_pkt as pkt
import os
import platform
import re
import sys
import time
import usb.core
import usb.util
import warnings
from usb.core import USBError

warnings.filterwarnings("ignore", category=DeprecationWarning)

FAIL = 1
USB_MSG_S2D_SIZE = 16
USB_MSG_D2S_SIZE = 16
CV_USB_RUNTIME = 0x80
CV_USB_S2D = 0x81
CV_USB_D2S = 0x82
USB_BULK_MAX_SIZE = 0x80000


class cv_usb_libusb:
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

    def dev_release(self):
        usb.util.dispose_resources(self.device)

    def crc16_ccitt(self, HexData):
        cksum = int("0", 16)
        for i in range(len(HexData)):
            cksum = self.crc16_tab[((cksum >> 8) ^ HexData[i]) & 0xFF] ^ (
                (cksum << 8) & 0xFF00
            )
        return cksum

    def close_device(self):
        progress_symbol = ["---", " \\", " \\", " |", " |", " |", " |", " /", " /"]
        Found = -1
        i = 0
        while Found == -1:
            i = i + 1
            sys.stdout.write(
                "Waiting for re-connect: %s    \r"
                % (progress_symbol[i % len(progress_symbol)])
            )
            sys.stdout.flush()
            if self.device is not None:
                try:
                    active = self.device.is_kernel_driver_active(1)
                    if active is False:
                        try:
                            self.device.attach_kernel_driver(1)
                            print("attach kernel driver\n")
                            return
                        except usb.USBError as e:
                            time.sleep(0.05)
                            continue
                    else:
                        break
                except usb.USBError as e:
                    if "[Errno 19]" in str(e):
                        # device is no longer connected
                        return
                    else:
                        print("get active err %s\n" % (e))
                        time.sleep(0.05)
                        continue
            else:
                print("close a non-open device. \n")
                break

    def get_vid(self, vid_pid):
        vid = vid_pid.replace("VID:PID=", "")
        vid = vid.split(":", 1)[0]
        return int(vid, 16)

    def get_pid(self, vid_pid):
        pid = vid_pid.replace("VID:PID=", "")
        pid = pid.split(":", 1)[1]
        return int(pid, 16)

    def libusb_query(self, vid_pid_list, timeout=0, location=None):
        if location is not None:
            self.location = location
        progress_symbol = ["---", " \\", " \\", " |", " |", " |", " |", " /", " /"]
        found = -1
        self.device = None
        i = 0
        query_time = time.time()
        while found == -1:
            i = i + 1
            sys.stdout.write(
                "Waiting for u-boot USB port: %s    \r"
                % (progress_symbol[i % len(progress_symbol)])
            )
            sys.stdout.flush()
            t_bus = None
            t_port_numbers = None
            if self.location is not None:
                t_bus = int(self.location.split("-")[0])
                t_port_numbers = self.location.split("-")[-1].split(".")
                t_port_numbers = tuple([int(j) for j in t_port_numbers])
            for vid_pid in vid_pid_list:
                if self.location is not None:
                    self.device = usb.core.find(
                        idVendor=self.get_vid(vid_pid),
                        idProduct=self.get_pid(vid_pid),
                        bus=t_bus,
                        port_numbers=t_port_numbers,
                    )
                else:
                    self.device = usb.core.find(
                        idVendor=self.get_vid(vid_pid), idProduct=self.get_pid(vid_pid)
                    )
                if self.device is not None:
                    # found = 1
                    time.sleep(1)
                    self.dev_backend = self.device.backend
                    break
            if self.device is None:
                if timeout != 0 and ((time.time() - query_time) > timeout):
                    sys.stdout.write("Query device timeout!\n")
                    sys.stdout.flush()
                    sys.exit(-1)
                time.sleep(0.05)
            else:
                try:
                    cfg = self.device.get_active_configuration()
                except (NotImplementedError, usb.USBError):
                    if timeout != 0 and ((time.time() - query_time) > timeout):
                        sys.stdout.write("Query device timeout!\n")
                        sys.stdout.flush()
                        sys.exit(-1)
                    else:
                        sys.stdout.write(
                            "Waiting for LibUSB hooked:   %s    \r"
                            % (progress_symbol[i % len(progress_symbol)])
                        )
                        sys.stdout.flush()
                        time.sleep(0.05)
                        continue
                except usb.USBError as e:
                    continue
                found = 1
                # detach the linux kernel driver.
                osName = platform.system()
                if osName == "Linux":
                    if self.device.is_kernel_driver_active(1) is True:
                        self.device.detach_kernel_driver(1)
                elif osName == "Windows":
                    # mimic serial device:
                    sys.stdout.write("LIBUSB on %s                       \n" % (osName))
                    sys.stdout.flush()
                else:
                    sys.stdout.write("Cannot support OS %s\n" % (osName))
                    sys.stdout.flush()
                    sys.exit(-1)
                for i in range(2):
                    intf = cfg[(i, 0)]
                    self.epOut = usb.util.find_descriptor(
                        intf,
                        # match the first OUT endpoint
                        custom_match=lambda e: usb.util.endpoint_direction(
                            e.bEndpointAddress
                        )
                        == usb.util.ENDPOINT_OUT
                        and usb.util.endpoint_type(e.bmAttributes)
                        == usb.util.ENDPOINT_TYPE_BULK,
                    )
                    self.epIn = usb.util.find_descriptor(
                        intf,
                        # match the first OUT endpoint
                        custom_match=lambda e: usb.util.endpoint_direction(
                            e.bEndpointAddress
                        )
                        == usb.util.ENDPOINT_IN
                        and usb.util.endpoint_type(e.bmAttributes)
                        == usb.util.ENDPOINT_TYPE_BULK,
                    )
        return self.device

    def serial_write(self, command, recv_ack, delay_ms):
        ret = array("B")
        # print("Token %d" % command[0])
        try:
            # time.sleep(0.001 * delay_ms)
            # if self.pkt_cnt < 10 and self.filesize != 128:
            #     time.sleep(0.3)
            self.epOut.write(command, 5000)
        except usb.USBError as e:
            print("Write error")

        cmd_crc = self.crc16_ccitt(command)
        # print ("cmd_crc %x" % cmd_crc)

        if recv_ack == 1:
            try:
                ret = self.epIn.read(16, timeout=0)
            except usb.USBError as e:
                print("Read ACK timeout" + e)

            ret_crc = self.convert(ret[2]) * 256 + self.convert(ret[3])
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
                return pkt.SUCCESS
            else:
                print("ACK_CRC_ERROR cmd_crc %x; ret_crc %x" % (cmd_crc, ret_crc))
                return pkt.FAIL
        else:
            return pkt.SUCCESS

    def serial_send_file(self, filename, dest_addr, delay_ms, type="file"):
        start_time = time.time()
        complete_cnt = 0
        self.pkt_cnt = 0
        self.filesize = 0
        while complete_cnt < 1:  # For stress test
            complete_cnt = complete_cnt + 1
            last_pos = 0
            tx_len = 512
            content_file = open(filename, "rb")
            content_size = os.path.getsize(filename)
            self.filesize = content_size
            print("%s is %d bytes" % (filename, content_size))
            print("Send to address 0x%x" % dest_addr)

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

                if filename == "cv_dl_magic.bin":
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

    def libusb_write(self, command, recv_ack, delay_ms):
        start_time = time.time()
        write_len = -1
        try:
            write_len = self.epOut.write(command, 5000)
        except usb.USBError as e:
            print("Write error")

        return write_len

    def usb_send_file(self, filename, dest_addr, delay_ms):
        self.ioTime = 0
        start_time = time.time()
        complete_cnt = 0
        while complete_cnt < 1:  # For stress test
            complete_cnt = complete_cnt + 1
            last_pos = 0
            tx_len = 1048576
            content_file = open(filename, "rb")
            content_size = os.path.getsize(filename)
            print("%s is %d bytes" % (filename, content_size))
            print("Send to address 0x%x" % dest_addr)

            while content_size > 0:
                del self.data[:]
                content_file.seek(last_pos)
                if content_size < tx_len:
                    self.data.fromfile(content_file, content_size)
                    tx_len = content_size
                else:
                    self.data.fromfile(content_file, tx_len)
                last_pos = content_file.tell()

                send_len = self.libusb_write(self.data, 1, delay_ms)

                if send_len >= 0:
                    dest_addr += send_len
                    content_size -= send_len
                    last_pos = last_pos - tx_len + send_len
                else:
                    print("IO error, stop send file")
                    break
            # print ("complete_cnt %d" % complete_cnt)
            print("--- %s Seconds ---" % round(time.time() - start_time, 2))
            # print ("--- %s Seconds ---" % str(self.ioTime))
        return

    def serial_send_chunk(
        self, content_file, content_size, dest_addr, delay_ms, Type="file"
    ):
        start_time = time.time()
        complete_cnt = 0
        self.pkt_cnt = 0
        self.filesize = 0
        while complete_cnt < 1:  # For stress test
            complete_cnt = complete_cnt + 1
            tx_len = 512
            self.filesize = content_size
            last_pos = content_file.tell()
            print("Send to address 0x%x" % dest_addr)

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

    def _usb_send_req(self, command, recv_ack, delay_ms):
        start_time = time.time()
        try:
            self.epOut.write(command, timeout=0)
        except usb.USBError as e:
            print("Write data timeout: %s\n", e)

        self.ioTime = self.ioTime + (time.time() - start_time)

        if recv_ack == 1:
            cmd_crc = self.crc16_ccitt(command)
            # print ("cmd_crc %x" % cmd_crc)

            start_time = time.time()
            try:
                rsp = self.epIn.read(16, timeout=0)
            except usb.USBError as e:
                print("Read ACK timeout")
            self.ioTime = self.ioTime + (time.time() - start_time)
            ret_crc = (rsp[2]) * 256 + (rsp[3])
            # print ("ret_crc %x" % ret_crc)

            if command[0] == pkt.CVI_USB_PROGRAM:
                return
            if command[0] == pkt.CVI_USB_REBOOT:
                return

            if ret_crc == cmd_crc:
                return rsp
            else:
                print("ACK_CRC_ERROR")
                return
        else:
            return

    def usb_send_req_data(self, token, address, reqLen, ack, data=None):
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
        if ack != 0:
            return self._usb_send_req(self.ser_cmd, 1, 0)
        else:
            return self.libusb_write(self.ser_cmd, 0, 0)

    def usb_send_req_kernel(self, token, reqLen, file_name, ack):
        del self.ser_cmd[:]
        self.ser_cmd.append(token)  # command
        # ser_cmd.append(0) # high byte of packet size
        self.ser_cmd.append((reqLen >> 32) & 0xFF)  # 5 bytes for destination address
        self.ser_cmd.append((reqLen >> 24) & 0xFF)
        self.ser_cmd.append((reqLen >> 16) & 0xFF)
        self.ser_cmd.append((reqLen >> 8) & 0xFF)
        self.ser_cmd.append((reqLen & 0xFF))
        # file_name_List = list(file_name)
        self.ser_cmd.fromstring(file_name)
        if ack != 0:
            return self._usb_send_req(self.ser_cmd, 1, 0)
        else:
            return self.libusb_write(self.ser_cmd, 0, 0)

    def protocol_msg_read(self, message, length, timeout_ms=0):
        try:
            self.epOut.write(message, timeout_ms)
        except usb.USBError as e:
            print("Write data fail!")
            return pkt.FAIL

        try:
            ret = self.epIn.read(length, timeout_ms)
        except usb.USBError as e:
            print("Read data fail")
            return pkt.FAIL

        return ret

    def protocol_msg_send(self, message, length, response):
        start_time = time.time()
        try:
            self.epOut.write(message, timeout=0)
        except usb.USBError as e:
            print("Write data fail!")
            return pkt.FAIL
        self.ioTime = self.ioTime + (time.time() - start_time)

        if response == 1:
            start_time = time.time()
            try:
                ret = self.epIn.read(16)
            except usb.USBError as e:
                print("message response fail")
            self.ioTime = self.ioTime + (time.time() - start_time)

            cmd_crc = self.crc16_ccitt(message)
            # print ("cmd_crc %x" % cmd_crc)

            ret_crc = (ret[pkt.RSP_CRC16_HI_OFFSET]) * 256 + (
                ret[pkt.RSP_CRC16_LO_OFFSET]
            )
            # print ("ret_crc %x" % ret_crc)

            if ret_crc == cmd_crc:
                if message[pkt.MSG_TOKEN_OFFSET] != ret[pkt.RSP_TOKEN_OFFSET]:
                    sys.stdout.write(
                        "Token: exp 0x%x get 0x%x\n"
                        % (message[pkt.MSG_TOKEN_OFFSET], ret[pkt.RSP_TOKEN_OFFSET])
                    )
                    sys.stdout.flush()
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
            self.epOut.write(dataBuf, timeout=0)
        except usb.USBError as e:
            print("Write data Fail %s" % e)
            return pkt.FAIL
        return pkt.SUCCESS

    def protocol_msg_d2s_once(self, addr, length, timeout_ms=0):
        msg = []
        self.protocol_msg_fill_header(msg, CV_USB_D2S, addr, USB_MSG_D2S_SIZE, length)
        return self.protocol_msg_read(msg, length, timeout_ms)

    def protocol_msg_s2d_once(self, addr, dataBuf, length):
        msg = []
        self.protocol_msg_fill_header(msg, CV_USB_S2D, addr, USB_MSG_S2D_SIZE, length)
        if self.protocol_msg_send(msg, USB_MSG_S2D_SIZE, 1) == FAIL:
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
            tx_len = USB_BULK_MAX_SIZE
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
            print("--- %s Seconds ---" % str(self.ioTime))
        return

    def protocol_send_chunk(self, chunk, size, dest_addr):
        complete_cnt = 0
        while complete_cnt < 1:  # For stress test
            tmp_addr = dest_addr
            self.ioTime = 0
            start_time = time.time()
            complete_cnt = complete_cnt + 1
            tx_len = USB_BULK_MAX_SIZE
            content_file = chunk
            content_size = size
            file_size = content_size
            last_pos = content_file.tell()

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
            print("--- size %d %s Seconds ---" % (size, round(time.time() - start_time, 5)))
            # print("--- %s Seconds ioTime ---" % str(self.ioTime))
        return

    def ctrl_transfer(
        self,
        bmRequestType,
        bRequest,
        wValue=0,
        wIndex=0,
        data_or_wLength=None,
        timeout=None,
    ):
        return self.device.ctrl_transfer(
            bmRequestType, bRequest, wValue, wIndex, data_or_wLength, timeout
        )

    def config_serial(self):
        # send line state
        try:
            send_len = self.ctrl_transfer(0x21, 0x22, 0, 0, 0, 5000)
        except USBError as e:
            print("set line state fail!" + str(e))
            sys.exit(-1)
        # send line code
        data = array("B")
        data.append(0)
        data.append(0x10)
        data.append(0x0E)
        data.append(0)
        data.append(0)
        data.append(0)
        data.append(8)
        try:
            self.ctrl_transfer(0x21, 0x20, 0, 0, data, 5000)
        except USBError as e:
            print("set line code fail!" + str(e))

            sys.exit(-1)

    def wait_for_reconnect(self, cnt_seconed):
        progress_symbol = ["---", " \\", " \\", " |", " |", " |", " |", " /", " /"]
        while cnt_seconed > 0:
            cnt_seconed = cnt_seconed - 1
            sys.stdout.write(
                "Waiting for re-connect: %s    \r"
                % (progress_symbol[cnt_seconed % len(progress_symbol)])
            )
            sys.stdout.flush()
            time.sleep(1)

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
        self.libusb_query(vid_pid_list, query_timeout)

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
            if "storage" in sys.argv[i]:
                self.storage = sys.argv[i]
                self.storage = self.storage.replace("storage=", "")
                print("storage = " + self.storage)
            if "usage" in sys.argv[i]:
                show_usage()
                sys.exit(0)
        if len(vid) != 0 and len(pid) != 0:
            vidpid_string(vid, pid)
        else:
            if len(vid) != 0:
                sys.stdout.write("pid is not appointed! \n")
                sys.stdout.flush()
                show_usage()
                sys.exit(-1)
            if len(pid) != 0:
                sys.stdout.write("vid is not appointed! \n")
                sys.stdout.flush()
                show_usage()
                sys.exit(-1)

    def convert(self, byte):
        return byte

    def __init__(self):
        self.device = 0
        self.dev_backend = 0
        self.epOut = 0
        self.epIn = 0
        self.data = array("B")
        self.ser_cmd = array("B")
        self.ioTime = 0
        self.emmc_timeout = 0
        self.python_version = 3
        self.location = None
        self.stdout = None
        self.header = array("B")
        self.bulk_command = array("B")
        self.storage = "emmc"
        if sys.version_info[0] < 3:
            self.python_version = 2
