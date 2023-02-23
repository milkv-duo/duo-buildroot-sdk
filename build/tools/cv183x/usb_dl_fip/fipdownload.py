#!/usr/local/bin/python

from cv_usb_util.cv_usb_pyserial import cv_usb_pyserial
import os
import sys
import time
import cv_usb_util.cv_usb_pkt as pkt
from array import array


class USBDownload:
    def __init__(self, script_path, fip_path):
        self.script_path = script_path
        self.fip_path = fip_path

    def download(self):
        print("CV1835 USB download start... ")

        cv_usb_serial = cv_usb_pyserial()
        if len(sys.argv) > 1:
            cv_usb_serial.parse_arg()

        print("Connecting to ROM code... ")
        cv_usb_serial.serial_query([pkt.rom_vidpid])
        time.sleep(0.02)
        cv_usb_serial.usb_send_file('cv_dl_magic.bin', 0x4003000, 0)
        print("done")

        print("Send fip.bin...")
        cv_usb_serial.usb_send_file(self.fip_path, 0x0C040000, 0)
        print("done")

        # Set SRAM flag
        print("Send magic number for USB boot... ")
        flag = array('B')
        flag.append(ord('1'))
        flag.append(ord('N'))
        flag.append(ord('G'))
        flag.append(ord('M'))
        cv_usb_serial.usb_send_req_data(pkt.CV_USB_NONE, 0x0E00FC00, 12, flag)
        print("done")

        print("Send magic number for USB download... ")
        flag = array('B')
        flag.append(ord('3'))
        flag.append(ord('N'))
        flag.append(ord('G'))
        flag.append(ord('M'))
        cv_usb_serial.usb_send_req_data(pkt.CV_USB_NONE, 0x0E00FC08, 12, flag)
        print("done")

        cv_usb_serial.usb_send_req_data(pkt.CV_USB_BREAK, 0x04003000, 0, None)
        files = [self.script_path, self.fip_path, "reboot"]

        for i in range(len(files)):
            del cv_usb_serial
            cv_usb_serial = cv_usb_pyserial()
            if len(sys.argv) > 1:
                cv_usb_serial.parse_arg()
            print("Connecting to u-boot... ")
            cv_usb_serial.serial_query([pkt.uboot_vidpid])
            time.sleep(0.02)

            if files[i] == "reboot":
                # Set download flag to 0x03005D00
                fsize = array('B')
                flag.append(ord('U'))
                flag.append(ord('S'))
                flag.append(ord('B'))
                flag.append(ord('D'))
                cv_usb_serial.usb_send_req_data(pkt.CV_USB_NONE, 0x03005D00,
                                                12, fsize)

                cv_usb_serial.usb_send_req_data(pkt.CVI_USB_REBOOT, 0x04003000,
                                                0, None)
                print("reboot device done")
                break
            print("Send %s   " % files[i])
            if i == 0:
                cv_usb_serial.usb_send_file(files[i], 0x100080000, 0)
            else:
                cv_usb_serial.usb_send_file(files[i], 0x100090000, 0)

            content_size = os.path.getsize(files[i])
            fsize = array('B')
            fsize.append(content_size & 0xFF)
            fsize.append((content_size >> 8) & 0xFF)
            fsize.append((content_size >> 16) & 0xFF)
            fsize.append((content_size >> 24) & 0xFF)
            print("Send %s   " % files[i])
            cv_usb_serial.usb_send_req_data(pkt.CV_USB_NONE, 0x0E00FC10, 12,
                                            fsize)

            cv_usb_serial.usb_send_req_data(pkt.CV_USB_BREAK, 0x04003000, 0,
                                            None)
            print("done")


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='CVITEK usb download tool')
    parser.add_argument("script_path",
                        type=str,
                        help="path to usb download script")
    parser.add_argument("fip_path", type=str, help="path to fip image")
    args = parser.parse_args()
    dl = USBDownload(args.script_path, args.fip_path)
    dl.download()
