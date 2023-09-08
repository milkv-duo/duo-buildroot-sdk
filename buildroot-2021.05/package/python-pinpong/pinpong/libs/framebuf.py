# -*- coding: utf-8 -*-
"""
The class of framebuf is using for modifying the data that
will write to the LCD-module.
"""

import json
import os
import freetype
from PIL import Image, ImageSequence



MVLSB     = 0
RGB565    = 1
GS2_HMSB  = 5
GS4_HMSB  = 2
GS8       = 6
MHLSB     = 3
MHMSB     = 4

enFontCode = {
    " ": [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
    "!": [0,0,24,60,60,60,24,24,24,0,24,24,0,0,0,0],
    "\"": [0,99,99,99,34,0,0,0,0,0,0,0,0,0,0,0],
    "#": [0,0,0,54,54,127,54,54,54,127,54,54,0,0,0,0],
    "$": [12,12,62,99,97,96,62,3,3,67,99,62,12,12,0,0],
    "%": [0,0,0,0,0,97,99,6,12,24,51,99,0,0,0,0],
    "&": [0,0,0,28,54,54,28,59,110,102,102,59,0,0,0,0],
    "'": [0,48,48,48,96,0,0,0,0,0,0,0,0,0,0,0],
    "`": [24,24,12,0,0,0,0,0,0,0,0,0,0,0,0,0],
    "(": [0,0,12,24,24,48,48,48,48,24,24,12,0,0,0,0],
    ")": [0,0,24,12,12,6,6,6,6,12,12,24,0,0,0,0],
    "*": [0,0,0,0,66,102,60,255,60,102,66,0,0,0,0,0],
    "+": [0,0,0,0,24,24,24,255,24,24,24,0,0,0,0,0],
    ",": [0,0,0,0,0,0,0,0,0,0,24,24,24,48,0,0],
    "-": [0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0],
    ".": [0,0,0,0,0,0,0,0,0,0,24,24,0,0,0,0],
    "/": [0,0,1,3,7,14,28,56,112,224,192,128,0,0,0,0],
    "0": [0,0,62,99,99,99,107,107,99,99,99,62,0,0,0,0],
    "1": [0,0,12,28,60,12,12,12,12,12,12,63,0,0,0,0],
    "2": [0,0,62,99,3,6,12,24,48,97,99,127,0,0,0,0],
    "3": [0,0,62,99,3,3,30,3,3,3,99,62,0,0,0,0],
    "4": [0,0,6,14,30,54,102,102,127,6,6,15,0,0,0,0],
    "5": [0,0,127,96,96,96,126,3,3,99,115,62,0,0,0,0],
    "6": [0,0,28,48,96,96,126,99,99,99,99,62,0,0,0,0],
    "7": [0,0,127,99,3,6,6,12,12,24,24,24,0,0,0,0],
    "8": [0,0,62,99,99,99,62,99,99,99,99,62,0,0,0,0],
    "9": [0,0,62,99,99,99,99,63,3,3,6,60,0,0,0,0],
    ":": [0,0,0,0,0,24,24,0,0,0,24,24,0,0,0,0],
    ";": [0,0,0,0,0,24,24,0,0,0,24,24,24,48,0,0],
    "<": [0,0,0,6,12,24,48,96,48,24,12,6,0,0,0,0],
    "=": [0,0,0,0,0,0,126,0,0,126,0,0,0,0,0,0],
    ">": [0,0,0,96,48,24,12,6,12,24,48,96,0,0,0,0],
    "?": [0,0,62,99,99,6,12,12,12,0,12,12,0,0,0,0],
    "@": [0,0,62,99,99,111,107,107,110,96,96,62,0,0,0,0],
    "A": [0,0,8,28,54,99,99,99,127,99,99,99,0,0,0,0],
    "B": [0,0,126,51,51,51,62,51,51,51,51,126,0,0,0,0],
    "C": [0,0,30,51,97,96,96,96,96,97,51,30,0,0,0,0],
    "D": [0,0,124,54,51,51,51,51,51,51,54,124,0,0,0,0],
    "E": [0,0,127,51,49,52,60,52,48,49,51,127,0,0,0,0],
    "F": [0,0,127,51,49,52,60,52,48,48,48,120,0,0,0,0],
    "G": [0,0,30,51,97,96,96,111,99,99,55,29,0,0,0,0],
    "H": [0,0,99,99,99,99,127,99,99,99,99,99,0,0,0,0],
    "I": [0,0,60,24,24,24,24,24,24,24,24,60,0,0,0,0],
    "J": [0,0,15,6,6,6,6,6,6,102,102,60,0,0,0,0],
    "K": [0,0,115,51,54,54,60,54,54,51,51,115,0,0,0,0],
    "L": [0,0,120,48,48,48,48,48,48,49,51,127,0,0,0,0],
    "M": [0,0,99,119,127,107,99,99,99,99,99,99,0,0,0,0],
    "N": [0,0,99,99,115,123,127,111,103,99,99,99,0,0,0,0],
    "O": [0,0,28,54,99,99,99,99,99,99,54,28,0,0,0,0],
    "P": [0,0,126,51,51,51,62,48,48,48,48,120,0,0,0,0],
    "Q": [0,0,62,99,99,99,99,99,99,107,111,62,6,7,0,0],
    "R": [0,0,126,51,51,51,62,54,54,51,51,115,0,0,0,0],
    "S": [0,0,62,99,99,48,28,6,3,99,99,62,0,0,0,0],
    "T": [0,0,255,219,153,24,24,24,24,24,24,60,0,0,0,0],
    "U": [0,0,99,99,99,99,99,99,99,99,99,62,0,0,0,0],
    "V": [0,0,99,99,99,99,99,99,99,54,28,8,0,0,0,0],
    "W": [0,0,99,99,99,99,99,107,107,127,54,54,0,0,0,0],
    "X": [0,0,195,195,102,60,24,24,60,102,195,195,0,0,0,0],
    "Y": [0,0,195,195,195,102,60,24,24,24,24,60,0,0,0,0],
    "Z": [0,0,127,99,67,6,12,24,48,97,99,127,0,0,0,0],
    "[": [0,0,60,48,48,48,48,48,48,48,48,60,0,0,0,0],
    "\\": [0,0,128,192,224,112,56,28,14,7,3,1,0,0,0,0],
    "]": [0,0,60,12,12,12,12,12,12,12,12,60,0,0,0,0],
    "^": [8,28,54,99,0,0,0,0,0,0,0,0,0,0,0,0],
    "_": [0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0],
    "a": [0,0,0,0,0,60,70,6,62,102,102,59,0,0,0,0],
    "b": [0,0,112,48,48,60,54,51,51,51,51,110,0,0,0,0],
    "c": [0,0,0,0,0,62,99,96,96,96,99,62,0,0,0,0],
    "d": [0,0,14,6,6,30,54,102,102,102,102,59,0,0,0,0],
    "e": [0,0,0,0,0,62,99,99,126,96,99,62,0,0,0,0],
    "f": [0,0,28,54,50,48,124,48,48,48,48,120,0,0,0,0],
    "g": [0,0,0,0,0,59,102,102,102,102,62,6,102,60,0,0],
    "h": [0,0,112,48,48,54,59,51,51,51,51,115,0,0,0,0],
    "i": [0,0,12,12,0,28,12,12,12,12,12,30,0,0,0,0],
    "j": [0,0,6,6,0,14,6,6,6,6,6,102,102,60,0,0],
    "k": [0,0,112,48,48,51,51,54,60,54,51,115,0,0,0,0],
    "l": [0,0,28,12,12,12,12,12,12,12,12,30,0,0,0,0],
    "m": [0,0,0,0,0,110,127,107,107,107,107,107,0,0,0,0],
    "n": [0,0,0,0,0,110,51,51,51,51,51,51,0,0,0,0],
    "o": [0,0,0,0,0,62,99,99,99,99,99,62,0,0,0,0],
    "p": [0,0,0,0,0,110,51,51,51,51,62,48,48,120,0,0],
    "q": [0,0,0,0,0,59,102,102,102,102,62,6,6,15,0,0],
    "r": [0,0,0,0,0,110,59,51,48,48,48,120,0,0,0,0],
    "s": [0,0,0,0,0,62,99,56,14,3,99,62,0,0,0,0],
    "t": [0,0,8,24,24,126,24,24,24,24,27,14,0,0,0,0],
    "u": [0,0,0,0,0,102,102,102,102,102,102,59,0,0,0,0],
    "v": [0,0,0,0,0,99,99,54,54,28,28,8,0,0,0,0],
    "w": [0,0,0,0,0,99,99,99,107,107,127,54,0,0,0,0],
    "x": [0,0,0,0,0,99,54,28,28,28,54,99,0,0,0,0],
    "y": [0,0,0,0,0,99,99,99,99,99,63,3,6,60,0,0],
    "z": [0,0,0,0,0,127,102,12,24,48,99,127,0,0,0,0],
    "{": [0,0,14,24,24,24,112,24,24,24,24,14,0,0,0,0],
    "|": [0,0,24,24,24,24,24,0,24,24,24,24,24,0,0,0],
    "}": [0,0,112,24,24,24,14,24,24,24,24,112,0,0,0,0],
    "~": [0,0,59,110,0,0,0,0,0,0,0,0,0,0,0,0]
}


class FrameBuffer(object):

    def __init__(self, buffer, width, height, mode):
        self.buffer = buffer
        self.width = width
        self.height = height
        self.stride = self.width
        self.mode = mode
        self.ch_text_json = None

    def set_pixel(self, x, y, col):
        if 0 <= x < self.width and 0 <= y < self.height:
            index = (y >> 3) * self.stride + x
            # print(x, y)
            offset = y & 0x07
            self.buffer[index] = (self.buffer[index] & ~(1 << offset)) | ((col != 0) << offset)

    def set_ascii(self, x, y, b, col):
        if not col:
            b = 255 - b
        b = bin(b)[2:].rjust(8, '0')
        count = 0
        for i in b:
            i = int(i)
            self.set_pixel(x+count, y, i)
            count += 1

    def get_pixel(self, x, y):
        if 0 <= x < self.width and 0 <= y <= self.height:
            index = (y >> 3) * self.stride + x
            return (self.buffer[index] >> (y & 0x07)) & 0x01

    def fill_rect(self, x, y, w, h, col):
        if h < 1 or w < 1 or x + w <= 0 or y + h <= 0 or y >= self.height or x >= self.width:
            return
        w = min(self.width, x + w) - x
        h = min(self.height, y + h) - y
        x = max(x, 0)
        y = max(y, 0)

        for y_axis in range(y, y+h):
            index = (y_axis >> 3) * self.stride + x
            offset = y_axis & 0x07
            for x_axis in range(x, x+w):
                self.buffer[index] = (self.buffer[index] & ~(1 << offset)) | ((col != 0) << offset)
                index += 1

    def fill(self, col):
        self.fill_rect(0, 0, self.width, self.height, col)

    def hline(self, x, y, width, col):
        self.fill_rect(x, y, width, 1, col)

    def vline(self, x, y, height, col):
        self.fill_rect(x, y, 1, height, col)

    def line(self, x1, y1, x2, y2, col):
        dx = x2 - x1
        sx = 1 if dx > 0 else -1
        dx = abs(dx)

        dy = y2 - y1
        sy = 1 if dy > 0 else -1
        dy = abs(dy)

        steep_flag = True
        if dy > dx:
            x1, y1 = y1, x1
            dx, dy = dy, dx
            sx, sy = sy, sx
        else:
            steep_flag = False

        e = 2 * dy - dx
        for i in range(dx):
            if steep_flag:
                if 0 <= x1 < self.height and 0 <= y1 < self.width:
                    self.set_pixel(y1, x1, col)
            else:
                if 0 <= x1 < self.width and 0 <= y1 < self.height:
                    self.set_pixel(x1, y1, col)
            while e >= 0:
                y1 += sy
                e -= (2 * dx)
            x1 += sx
            e += (2 * dy)

        if 0 <= x2 < self.width and 0 <= y2 < self.height:
            self.set_pixel(x2, y2, col)

    def rect(self, x, y, w, h, col):
        self.fill_rect(x, y, w, 1, col)
        self.fill_rect(x, y, 1, h, col)
        self.fill_rect(x+w-1, y, 1, h, col)
        self.fill_rect(x, y+h-1, w, 1, col)

    def circle(self, x, y, r, col):
        r = abs(r)
        if r == 1:
            r = 2
        x_axis = 0
        y_axis = r
        var = 3 - 2 * r
        while x_axis <= y_axis:
            self.set_pixel(x + x_axis, y + y_axis, col)
            self.set_pixel(x - x_axis, y + y_axis, col)
            self.set_pixel(x + x_axis, y - y_axis, col)
            self.set_pixel(x - x_axis, y - y_axis, col)
            self.set_pixel(x + y_axis, y + x_axis, col)
            self.set_pixel(x - y_axis, y + x_axis, col)
            self.set_pixel(x + y_axis, y - x_axis, col)
            self.set_pixel(x - y_axis, y - x_axis, col)
            if var < 0:
                var = var + 4 * x_axis + 6
            else:
                var = var + 4 * (x_axis - y_axis) + 10
                y_axis -= 1
            x_axis += 1
        # self.set_pixel(x + r - 1, y + r - 1, col)
        # self.set_pixel(x + r - 1, y - (r - 1), col)
        # self.set_pixel(x - (r - 1), y + r - 1, col)
        # self.set_pixel(x - (r - 1), y - (r - 1), col)

    def fill_circle(self, x, y, r, col):
        r = abs(r)
        if r == 1:
            r = 2
        x_axis = y_end = 0
        y_axis = r
        var = 3 - 2 * r
        while x_axis <= y_axis:
            self.vline(x + x_axis, y - y_axis, 2 * y_axis + 1, col)
            self.vline(x + y_axis, y - x_axis, 2 * x_axis + 1, col)
            self.vline(x - x_axis, y - y_axis, 2 * y_axis + 1, col)
            self.vline(x - y_axis, y - x_axis, 2 * x_axis + 1, col)
            if var < 0:
                var = var + 4 * x_axis + 6
            else:
                var = var + 4 * (x_axis - y_axis) + 10
                y_axis -= 1
            x_axis += 1

    def text(self, string, x, y, col, charbuf):
        if not isinstance(string, str):
            return
        n = 0
        for i in string:
            if ord(i) < 128:
                count = 0
                str_buf = enFontCode[i]
                for b in str_buf:
                    self.set_ascii(x, y+count, b, col)
                    count += 1
                x += 8
            # else:
            #     count = 0
            #     self.ch_text_json = json.load(open("Lib/fonts_msyh.ttf.json", 'r', encoding='utf-8'))
            #     if not self.ch_text_json['data'].get(i):
            #         i = " "
            #     str_buf = self.ch_text_json['data'].get(i)
            #     for b in str_buf:
            #         if count % 2:
            #             self.set_ascii(x + 8, y + count - 1, b, col)
            #         else:
            #             self.set_ascii(x, y + count, b, col)
            #         count += 1
            #     x += 16
            else:
                # buf = [0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x03,0xf8,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0xff,0xfe,0x00,0x00]
                buf = [[0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x00,0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x7f,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00]]
                p1, p2 = y // 8, y % 8 #（取整，取余）
                fontw, fonth =  16, 16 #（字体宽，字体高）
                ofdata = 0 # 中间运算值
                bufferlen = (self.height // 8) * self.width
                # if p2 == 0:
                #     for w in range(fontw):
                #         for h in range(fonth//8):
                #             index = w+x+(h+p1)*self.width
                #             if index < bufferlen:
                #                 self.buffer[index] = buf[w+h*16]
                for k in range(p2):
                    ofdata |= 1 << k
                for p in range(len(buf[n])):
                    indexl, indexh = x+(p1+p//fontw)*self.width+(p%fontw), x+(p1+1+p//fontw)*self.width+(p%fontw)
                    if x + p % fontw < self.width: #不支持换行
                        if indexl < bufferlen:
                            self.buffer[indexl] = buf[n][p] << p2 & 0xff | self.buffer[indexl] & ofdata
                        if indexh < bufferlen:
                            self.buffer[indexh] = buf[n][p] >> (8-p2) & 0xff | self.buffer[indexh] & ~ofdata
                x += 16
                # n += 1
