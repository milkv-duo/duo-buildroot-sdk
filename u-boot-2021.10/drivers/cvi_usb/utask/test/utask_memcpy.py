#!/usr/bin/env python2
import os
import re
import usb.core
import usb.util
import sys
import time
import platform
from array import array
import random

# Define
SUCCESS = 0
FAIL = 1

HEADER_SIZE = 8

CVI_USB_NONE      = 0
CVI_USB_INFO      = 1
CVI_USB_VERBOSE   = 2
CVI_USB_JUMP      = 3
CVI_USB_BREAK     = 4
CVI_USB_KEEP_DL   = 5
CVI_USB_PRG_CMD   = 6
CVI_USB_RESET_ARM = 7
CVI_USB_TEST_THERMAL_SENSOR = 8
CVI_USB_TEST_EMMC = 9

# Global variables
header = array('B')
data = array('B')
ret = array('B')
bulk_command = array('B')
ser_cmd = array('B')
pkt_cnt = 0
filesize = 0
ioTime = 0
rom_vidpid = 'VID:PID=0559:1000'
prg_vidpid = 'VID:PID=30B1:1000'
verify_vidpid = 'VID:PID='
emmc_timeout = 0

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

def crc16_ccitt(HexData):
    cksum = int("0", 16)
    for i in range(len(HexData)):
        cksum = crc16_tab[((cksum >> 8) ^ HexData[i]) & 0xff] ^ ((cksum << 8) & 0xff00)
    return cksum

def close_device(device):
    progress_symbol = ['---', ' \\', ' \\', ' |', ' |', ' |', ' |', ' /', ' /']
    Found = -1
    i = 0
    while Found == -1:
        i = i + 1
        sys.stdout.write("Waiting for re-connect: %s    \r" % (progress_symbol[i%len(progress_symbol)]))
        sys.stdout.flush()
        if device is not None:
            try:
                active = device.is_kernel_driver_active(1)
                if active is False:
                    try:
                        device.attach_kernel_driver(1)
                        print("attach kernel driver\n")
                        return
                    except usb.USBError as e:
                        time.sleep(0.05)
                        continue
                else:
                    break
            except usb.USBError as e:
                if '[Errno 19]' in str(e):
                    # device is no longer connected
                    return
                else:
                    print("get active err %s\n" %(e))
                    time.sleep(0.05)
                    continue
        else:
            print("close a non-open device. \n")
            break

def get_vid(vid_pid):
    vid = vid_pid.replace('VID:PID=', "")
    vid = vid.split(':', 1)[0]
    return int(vid, 16)

def get_pid(vid_pid):
    pid = vid_pid.replace('VID:PID=', "")
    pid = pid.split(':', 1)[1]
    return int(pid, 16)

def serial_query(vid_pid_list, timeout=0):
    global device
    global epOut
    global epIn
    progress_symbol = ['---', ' \\', ' \\', ' |', ' |', ' |', ' |', ' /', ' /']
    found = -1
    device = None
    i = 0
    query_time = time.time()
    while(found == -1):
        i = i + 1
        sys.stdout.write("Waiting for CV1835 USB port: %s    \r" % (progress_symbol[i%len(progress_symbol)]))
        sys.stdout.flush()
        for vid_pid in vid_pid_list:
            device = usb.core.find(idVendor=get_vid(vid_pid), idProduct=get_pid(vid_pid))
            if device is not None:
                found = 1
                break
        if device is None:
            if timeout != 0 and ((time.time() - query_time) > timeout):
                sys.stdout.write("Query device timeout!       \n" )
                sys.stdout.flush()
                sys.exit(-1)
            time.sleep(0.05)
        else:
            break

    i = 0
    found = -1
    while(found == -1):
        i = i + 1
        try:
            cfg = device.get_active_configuration()
        except (NotImplementedError, usb.USBError):
            sys.stdout.write("Waiting for LibUSB hooked:   %s    \r" %  (progress_symbol[i%len(progress_symbol)]))
            sys.stdout.flush()
            time.sleep(0.05)
            continue
        except usb.USBError as e:
            continue
        found = 1
        # detach the linux kernel driver.
        osName = platform.system()
        if osName == 'Linux':
            if device.is_kernel_driver_active(1) is True:
                device.detach_kernel_driver(1)
        elif osName == 'Windows':
            # Set line state
            device.ctrl_transfer(0x21, 0x22, 0, 0, 0)
            # Set line code
            lineCode = [0x00, 0x10, 0x0E, 0x00, 0x00, 0x00, 0x08]
            assert device.ctrl_transfer(0x21, 0x20, 0, 0, lineCode) == len(lineCode)
        else:
            sys.stdout.write("Cannot support OS %s\n" %(osName))
            sys.stdout.flush()
            sys.exit(-1)

        intf = cfg[(1,0)]
        epOut = usb.util.find_descriptor(
                intf,
                # match the first OUT endpoint
                custom_match = \
                lambda e: \
                usb.util.endpoint_direction(e.bEndpointAddress) == \
                usb.util.ENDPOINT_OUT)
        epIn =  usb.util.find_descriptor(
                intf,
                # match the first OUT endpoint
                custom_match = \
                lambda e: \
                usb.util.endpoint_direction(e.bEndpointAddress) == \
                usb.util.ENDPOINT_IN)
    return device

def serial_write(command, recv_ack, delay_ms):
    global ret
    global pkt_cnt
    global filesize
    global ioTime

    start_time = time.time()
    try:
        #time.sleep(0.001 * delay_ms)
        #if pkt_cnt < 2 and filesize != 128:
        #    time.sleep(0.3)
        epOut.write(command)
    except usb.USBError as e:
        print ("Write data timeout %s\n" %e)

    ioTime = ioTime + (time.time() - start_time)

    if recv_ack == 1:
        cmd_crc = crc16_ccitt(command)
        # print ("cmd_crc %x" % cmd_crc)

        start_time = time.time()
        try:
            ret = epIn.read(16);
        except usb.USBError as e:
            print ("Read ACK timeout")
        ioTime = ioTime + (time.time() - start_time)
        ret_crc = (ret[2])*256 + (ret[3])
        #print ("ret_crc %x" % ret_crc)

        if ret_crc == cmd_crc:
            pkt_cnt += 1
            # sys.stdout.write("Packet count: %d   \r" % pkt_cnt)
            sys.stdout.write("[Working] %d%%    \r" % ((pkt_cnt * (512 - HEADER_SIZE) * 100) / filesize))
            sys.stdout.flush()
            return SUCCESS
        else:
            print ("ACK_CRC_ERROR")
            return FAIL
    else:
        return SUCCESS

def usb_send_file(filename, dest_addr, delay_ms):
    global header
    global data
    global bulk_command
    global pkt_cnt
    global filesize
    global ioTime

    ioTime = 0
    start_time = time.time()
    complete_cnt = 0
    pkt_cnt = 0
    filesize = 0
    while (complete_cnt < 1): # For stress test
        complete_cnt = complete_cnt + 1
        last_pos = 0
        tx_len = 512
        content_file = open(filename, 'rb')
        content_size = os.path.getsize(filename)
        filesize = content_size
        print ("%s is %d bytes" % (filename, content_size))
        print ("Send to address 0x%x" % dest_addr)

        while (content_size > 0):
            del header[:]
            del data[:]
            del bulk_command[:]

            content_file.seek(last_pos)
            if (content_size < tx_len - HEADER_SIZE):
                data.fromfile(content_file, content_size)
                tx_len = content_size + HEADER_SIZE
            else:
                data.fromfile(content_file, tx_len - HEADER_SIZE)
            last_pos = content_file.tell()

            #if filename == 'cvi_dl_magic.bin':
            #    header.append(CVI_USB_KEEP_DL)
            #else:
            header.append(CVI_USB_NONE)
            header.append((tx_len >> 8) & 0xFF)
            header.append((tx_len & 0xFF))
            header.append((dest_addr >> 32) & 0xFF)
            header.append((dest_addr >> 24) & 0xFF)
            header.append((dest_addr >> 16) & 0xFF)
            header.append((dest_addr >> 8)  & 0xFF)
            header.append(dest_addr & 0xFF)

            bulk_command = header + data
            send_ok = serial_write(bulk_command, 1, delay_ms)

            if send_ok == 0:
                dest_addr += tx_len - HEADER_SIZE
                content_size -= tx_len - HEADER_SIZE
            else:
                last_pos -= tx_len - HEADER_SIZE

        # print ("complete_cnt %d" % complete_cnt)
        print ("--- %s Seconds ---" % round(time.time() - start_time, 2 ))
        print ("--- %s Seconds ---" % str(ioTime))
    return

def _usb_send_req(command, recv_ack, delay_ms):
    global pkt_cnt
    global filesize
    global ioTime

    start_time = time.time()
    try:
        #time.sleep(0.001 * delay_ms)
        #if pkt_cnt < 2 and filesize != 128:
        #    time.sleep(0.3)
        epOut.write(command, timeout=0)
    except usb.USBError as e:
        print ("Write data timeout: %s\n", e)

    ioTime = ioTime + (time.time() - start_time)

    if recv_ack == 1:
        cmd_crc = crc16_ccitt(command)
        # print ("cmd_crc %x" % cmd_crc)

        start_time = time.time()
        try:
            rsp = epIn.read(16, timeout=0);
        except usb.USBError as e:
            print ("Read ACK timeout: %s\n", e)
        ioTime = ioTime + (time.time() - start_time)
        ret_crc = (rsp[2])*256 + (rsp[3])
        #print ("ret_crc %x" % ret_crc)

        if ret_crc == cmd_crc:
            return rsp
        else:
            print ("ACK_CRC_ERROR")
            return None
    else:
        return None

def usb_send_req_data(token, address, reqLen, ack, data=None):
    global ser_cmd
    del ser_cmd[:]
    ser_cmd.append(token) # command
    ser_cmd.append(0) # high byte of packet size
    ser_cmd.append(reqLen) # low byte of packet size
    ser_cmd.append((address >> 32) & 0xFF) # 5 bytes for destination address
    ser_cmd.append((address >> 24) & 0xFF)
    ser_cmd.append((address >> 16) & 0xFF)
    ser_cmd.append((address >> 8)  & 0xFF)
    ser_cmd.append((address & 0xFF))
    if data is not None:
        ser_cmd = ser_cmd + data
    else:
        print("data is empty!\n")
        return
    if ack != 0:
        return _usb_send_req(ser_cmd, 1, 0)
    else:
        return serial_write(ser_cmd, 0, 0)

def usb_send_req(token, address, reqLen, ack):
    del ser_cmd[:]
    ser_cmd.append(token) # command
    ser_cmd.append(0) # high byte of packet size
    ser_cmd.append(reqLen) # low byte of packet size
    ser_cmd.append((address >> 32) & 0xFF) # 5 bytes for destination address
    ser_cmd.append((address >> 24) & 0xFF)
    ser_cmd.append((address >> 16) & 0xFF)
    ser_cmd.append((address >> 8)  & 0xFF)
    ser_cmd.append((address & 0xFF))

    if ack != 0:
        return _usb_send_req(ser_cmd, 1, 0)
    else:
        return serial_write(ser_cmd, 0, 0)
#========================================================================
# New protocol of prg.bin
#========================================================================
USB_MSG_S2D_SIZE    = 16
USB_MSG_D2S_SIZE    = 16
CVI_USB_RUNTIME      = 0x80
CVI_USB_S2D          = 0x81
CVI_USB_D2S          = 0x82
USB_BULK_MAX_SIZE   = 0x400000

MSG_TOKEN_OFFSET    = 0

RSP_CRC16_HI_OFFSET = 2
RSP_CRC16_LO_OFFSET = 3
RSP_TOKEN_OFFSET    = 6

def protocol_msg_send(message, length, response):
    global ioTime

    start_time = time.time()
    try:
        #time.sleep(0.001 * delay_ms)
        #if pkt_cnt < 2 and filesize != 128:
        #    time.sleep(0.3)
        epOut.write(message, timeout=0)
    except usb.USBError as e:
        print ("Write data fail!")
        return FAIL

    ioTime = ioTime + (time.time() - start_time)

    if response == 1:
        start_time = time.time()
        try:
            ret = epIn.read(16);
        except usb.USBError as e:
            print ("message response fail %s\n" %e)
        ioTime = ioTime + (time.time() - start_time)

        cmd_crc = crc16_ccitt(message)
        # print ("cmd_crc %x" % cmd_crc)

        ret_crc = (ret[RSP_CRC16_HI_OFFSET])*256 + (ret[RSP_CRC16_LO_OFFSET])
        #print ("ret_crc %x" % ret_crc)

        if ret_crc == cmd_crc:
            if message[MSG_TOKEN_OFFSET] != ret[RSP_TOKEN_OFFSET]:
                sys.stdout.write("Token: exp 0x%x get 0x%x\n" % (message[MSG_TOKEN_OFFSET], ret[RSP_TOKEN_OFFSET]))
                sys.stdout.flush()
            else:
                # sys.stdout.write("Packet count: %d   \r" % pkt_cnt)
                #sys.stdout.write("[Working] %d%%    \r" % ((pkt_cnt * (512 - HEADER_SIZE) * 100) / filesize))
                #sys.stdout.flush()
                return SUCCESS
        else:
            sys.stdout.write("ACK_CRC_ERROR\n")
            sys.stdout.flush()
            return FAIL
    else:
        return SUCCESS

def protocol_msg_fill_header(message, token, addr, reqLen, dataSize):

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

def protocol_usb_write(dataBuf):
    try:
        epOut.write(dataBuf, timeout=0)
    except usb.USBError as e:
        print ("Write data Fail %s" %e)
        return FAIL
    return SUCCESS

def protocol_usb_read(length):
    try:
        return epIn.read(length, timeout=0)
    except usb.USBError as e:
        print ("Read data Fail %s" %e)
        return None

def protocol_msg_s2d_once(addr, dataBuf, length):
    msg = []
    protocol_msg_fill_header(msg, CVI_USB_S2D, addr, USB_MSG_S2D_SIZE, length)
    if protocol_msg_send(msg, USB_MSG_S2D_SIZE, 1) == FAIL:
        return FAIL
    else:
        return protocol_usb_write(dataBuf)

def protocol_msg_d2s_once(addr, length):
    msg = []
    protocol_msg_fill_header(msg, CVI_USB_D2S, addr, USB_MSG_D2S_SIZE, length)
    if protocol_msg_send(msg, USB_MSG_D2S_SIZE, 0) == FAIL:
        return FAIL
    else:
        return protocol_usb_read(length)

def protocol_msg_s2d(addr, dataBuf, length):
    left = length
    start = 0
    stop = 0
    tmpBuf = []
    while (left > 0):
        len_once = 0
        if left <= USB_BULK_MAX_SIZE:
            len_once = left
        else:
            len_once = USB_BULK_MAX_SIZE
        stop = start + len_once
        tmpBuf = dataBuf[start:stop]
        if protocol_msg_s2d_once(addr, tmpBuf, len_once) == FAIL:
            print("s2d failed at address:0x%x offset:0x%x len:%d\n" %(addr, start, len_once))
            return FAIL
        else:
            addr = addr + len_once
            start = start + len_once
            left = left - len_once
    return SUCCESS

def protocol_msg_d2s(addr, length):
    left = length
    tmpBuf = array('B', [])
    while (left > 0):
        len_once = 0
        if left <= USB_BULK_MAX_SIZE:
            len_once = left
        else:
            len_once = USB_BULK_MAX_SIZE
        oriLen = len(tmpBuf)
        tmpBuf = tmpBuf + protocol_msg_d2s_once(addr, len_once)
        newLen = len(tmpBuf)
        if (newLen-oriLen) != len_once:
            print("d2s size mismatch at address:0x%x oriLen=%d newLen=%d len_once=%d" %(addr, oriLen, newLen, len_once))
            return None
        else:
            addr = addr + len_once
            left = left - len_once
    return tmpBuf

def protocol_send_file(filename, dest_addr):
    global ioTime

    complete_cnt = 0
    while (complete_cnt < 1): # For stress test
        tmp_addr = dest_addr
        ioTime = 0
        start_time = time.time()

        complete_cnt = complete_cnt + 1
        last_pos = 0
        tx_len = USB_BULK_MAX_SIZE
        content_file = open(filename, 'rb')
        content_size = os.path.getsize(filename)
        file_size = content_size
        print ("%s is %d bytes" % (filename, content_size))
        print ("Send to address 0x%x" % tmp_addr)

        while (content_size > 0):
            #data = []
            del data[:]

            content_file.seek(last_pos)
            if (content_size < tx_len):
                data.fromfile(content_file, content_size)
                tx_len = content_size
            else:
                data.fromfile(content_file, tx_len)
            last_pos = content_file.tell()

            send_ok = protocol_msg_s2d_once(tmp_addr, data, tx_len)

            if send_ok == 0:
                tmp_addr += tx_len
                content_size -= tx_len
                sys.stdout.write("[Working] %d%%    \r" % (((file_size - content_size) * 100) / file_size))
                sys.stdout.flush()
            else:
                last_pos -= tx_len

        # print ("complete_cnt %d" % complete_cnt)
        print ("--- %s Seconds ---" % round(time.time() - start_time, 2 ))
        print ("--- %s Seconds ---" % str(ioTime))
    return

#==============================================================
# Main()
#===============================================================
def wait_for_reconnect(cnt_seconed):
    progress_symbol = ['---', ' \\', ' \\', ' |', ' |', ' |', ' |', ' /', ' /']
    while cnt_seconed > 0:
        cnt_seconed = cnt_seconed - 1
        sys.stdout.write("Waiting for re-connect: %s    \r" % (progress_symbol[cnt_seconed%len(progress_symbol)]))
        sys.stdout.flush()
        time.sleep(1)

def vidpid_string(vid_str, pid_str):
    global verify_vidpid

    vid_str = vid_str.replace("0x", "")
    pid_str = pid_str.replace("0x", "")
    while len(vid_str) < 4:
        vid_str = '0' + vid_str
    while len(pid_str) < 4:
        pid_str = '0' + pid_str
    verify_vidpid = verify_vidpid + vid_str + ':' + pid_str

def usb_emmc_dl_veirfy():
    print("veirfy id = %s" %verify_vidpid)
    vidpid_list = [verify_vidpid]
    serial_query(vidpid_list, emmc_timeout)

def show_usage():
    sys.stdout.write("usage- python [script] vvid=[vid] vpid=[pid]\n")
    sys.stdout.write("       [script] : %s \n" %__file__)
    sys.stdout.write("       [vvid]   : reconnect vid if emmc dl complete check enable \n")
    sys.stdout.write("       [vpid]   : reconnect pid if emmc dl complete check enable \n")
    sys.stdout.write("       [timeout]: verify timeout (second) from emmc programming to kerel start \n")
    sys.stdout.write("                  0 means wait forever (default)  \n")
    sys.stdout.flush()

def parse_arg():
    global emmc_timeout
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
            emmc_timeout = int(s)
            print("emmc timeout = %d s" %emmc_timeout)
        if 'usage' in sys.argv[i]:
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

if __name__ == '__main__':
    target_address = 0x110000000
    pattern_size = 16*1024*1024
    step = 64
    test_cnt = 100
    copy_size = 13
    # device enumeration
    vidpid_list = [prg_vidpid]
    serial_query(vidpid_list)
    # generate random list for write
    data_w = array('B',[random.randint(0, 255) for _ in range(pattern_size)])
    data_r = array('B',[])
    while test_cnt > 0:
        del data_r
        total_time = time.time()
        # send data
        if protocol_msg_s2d(target_address, data_w, copy_size) == FAIL:
            usb_send_req(CVI_USB_BREAK, 0, 8, 0)
            sys.exit(-1)
        # read data
        data_r = protocol_msg_d2s(target_address, copy_size)
        total_time = round(time.time() - total_time, 5 )
        # data verification
        if len(data_r) != copy_size:
            print("expect %d but receive %d\n" %(copy_size, len(data_r)))
            usb_send_req(CVI_USB_BREAK, 0, 8, 0)
            sys.exit(-2)
        if data_w[:copy_size] != data_r:
            print("data mismatched!\n")
            usb_send_req(CVI_USB_BREAK, 0, 8, 0)
            sys.exit(-3)
        test_cnt = test_cnt - 1
        copy_size = (copy_size + step) % pattern_size
        print ("Test Success!")
        print ("--- %s Seconds ---\n" % total_time)

    # leave cvi_usb
    usb_send_req(CVI_USB_BREAK, 0, 8, 0)
