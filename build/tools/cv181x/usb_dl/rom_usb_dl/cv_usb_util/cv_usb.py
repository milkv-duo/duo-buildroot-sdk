from importlib import import_module
import logging

FORMAT = "%(levelname)s: %(message)s"
logging.basicConfig(level=logging.INFO, format=FORMAT)


class cv_usb:
    def __init__(self, driver="pyserial"):
        if driver == "pyserial":
            self.pyserial = import_module("cv_usb_util.cv_usb_pyserial")
            self.usb_dev = self.pyserial.cv_usb_pyserial()
        else:
            self.libusb = import_module("cv_usb_util.cv_usb_libusb")
            self.usb_dev = self.libusb.cv_usb_libusb()
        self.driver = driver

    def query(self, vid_pid_list, timeout=0, verify=0, location=None):
        if self.driver == "pyserial":
            self.usb_dev.serial_query(vid_pid_list, timeout, verify, location)
        else:
            dev = self.usb_dev.libusb_query(vid_pid_list, timeout, location)
            if dev is None:
                logging.error("Hook LIBUSB failed")
                raise IOError
            self.usb_dev.config_serial()

    def send_file(self, filename, dest_addr, delay_ms):
        if self.driver == "pyserial":
            self.usb_dev.usb_send_file(filename, dest_addr, delay_ms)
        else:
            self.usb_dev.serial_send_file(filename, dest_addr, delay_ms)

    def send_chunk(self, chunk, size, dest_addr, delay_ms, Type="file"):
        if self.driver == "pyserial":
            self.usb_dev.usb_send_chunk(chunk, size, dest_addr, delay_ms, Type)
        else:
            if Type == "file":
                self.usb_dev.protocol_send_chunk(chunk, size, dest_addr)
            else:
                self.usb_dev.serial_send_chunk(chunk, size, dest_addr, delay_ms, Type)

    def recive_data(self, addr, length, timeout_ms=0):
        if self.driver == "pyserial":
            return self.usb_dev.protocol_msg_d2s_once(addr, length)
        else:
            return self.usb_dev.protocol_msg_d2s_once(addr, length, timeout_ms)

    def send_req_data(self, token, address, reqLen, data=None, ack=0):
        if self.driver == "pyserial":
            return self.usb_dev.usb_send_req_data(token, address, reqLen, data)
        else:
            return self.usb_dev.usb_send_req_data(token, address, reqLen, ack, data)

    def restart(self):
        del self.usb_dev
        if self.driver == "pyserial":
            self.usb_dev = self.pyserial.cv_usb_pyserial()
        else:
            self.usb_dev = self.libusb.cv_usb_libusb()
