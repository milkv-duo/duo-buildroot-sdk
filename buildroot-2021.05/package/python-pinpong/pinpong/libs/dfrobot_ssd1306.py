# -*- coding: utf-8 -*-
"""
Import this file to initialize ssd-object to control LCD-module.
Only support I2C-connection with  LCD-module.
Able to draw line, rectangle, circle, pixel, english and chinese text,
fill_rect, fill_circle, fill screen, etc.
"""
from pinpong.board import gboard,I2C
import pinpong.libs.framebuf as framebuf
import time

SSD1306_JUMPTABLE_BYTES        = 4
SSD1306_JUMPTABLE_LSB          = 1
SSD1306_JUMPTABLE_SIZE         = 2
SSD1306_JUMPTABLE_WIDTH        = 3
SSD1306_JUMPTABLE_START        = 4
SSD1306_WIDTH_POS              = 0
SSD1306_HEIGHT_POS             = 1
SSD1306_FIRST_CHAR_POS         = 2
SSD1306_CHAR_NUM_POS           = 3

# display code
SSD1306_CHARGEPUMP             = 0x8D
SSD1306_COLUMNADDR             = 0x21
SSD1306_COMSCANDEC             = 0xC8
SSD1306_COMSCANINC             = 0xC0
SSD1306_DISPLAYALLON           = 0xA5
SSD1306_DISPLAYALLON_RESUME    = 0xA4
SSD1306_DISPLAYOFF             = 0xAE
SSD1306_DISPLAYON              = 0xAF
SSD1306_EXTERNALVCC            = 0x01
SSD1306_INVERTDISPLAY          = 0xA7
SSD1306_MEMORYMODE             = 0x20
SSD1306_NORMALDISPLAY          = 0xA6
SSD1306_PAGEADDR               = 0x22
SSD1306_SEGREMAP               = 0xA0
SSD1306_SETCOMPINS             = 0xDA
SSD1306_SETCONTRAST            = 0x81
SSD1306_SETDISPLAYCLOCKDIV     = 0xD5
SSD1306_SETDISPLAYOFFSET       = 0xD3
SSD1306_SETHIGHCOLUMN          = 0x10
SSD1306_SETLOWCOLUMN           = 0x00
SSD1306_SETMULTIPLEX           = 0xA8
SSD1306_SETPRECHARGE           = 0xD9
SSD1306_SETSEGMENTREMAP        = 0xA1
SSD1306_SETSTARTLINE           = 0x40
SSD1306_SETVCOMDETECT          = 0xDB
SSD1306_SWITCHCAPVCC           = 0x02

BEGIN_ERR_OK                   = 0
BEGIN_ERR_ERR                  = -1
BEGIN_WAR_NOTEST               = 1
DISPLAY_ERR_OK                 = 0
DISPLAY_ERR                    = -1
DISPLAY_ERR_PARAM              = -2
DISPLAY_ERR_NOTSUPPORT         = -3
DISPLAY_ERR_MEMOVER            = -4

EROTATION_0                    = 0
EROTATION_90                   = 90
EROTATION_180                  = 180
EROTATION_270                  = 270

class SSD1306(object):

    def __init__(self, width, height, external_vcc):
        self.width = width
        self.height = height
        self.external_vcc = external_vcc
        self.text_color = 1
        self.line_width = 1
        self.pages = self.height // 8
        self.buf_size = self.width * self.pages
        self.buffer = bytearray(self.buf_size)
        # self._buffer = None
        self.framebuf = framebuf.FrameBuffer(self.buffer, self.width, self.height, framebuf.MVLSB)
        self.power_on()
        self.init_display()

    def init_display(self):
        count = 0
        for cmd in (
            SSD1306_DISPLAYOFF,
            SSD1306_SETDISPLAYCLOCKDIV,
            0xF0,  # Increase speed of the display max ~96Hz
            SSD1306_SETMULTIPLEX,
            0x3F,
            SSD1306_SETDISPLAYOFFSET,
            0x00,
            SSD1306_SETSTARTLINE,
            SSD1306_CHARGEPUMP,
            0x14,
            SSD1306_MEMORYMODE,
            0x00,
            SSD1306_SEGREMAP,
            SSD1306_COMSCANINC,
            SSD1306_SETCOMPINS,
            0x12,
            SSD1306_SETCONTRAST,
            0xCF,
            SSD1306_SETPRECHARGE,
            0xF1,
            SSD1306_DISPLAYALLON_RESUME,
            SSD1306_NORMALDISPLAY,
            0x2E,  # stop scroll
            SSD1306_DISPLAYON,
        ):
            self.write_cmd(cmd)
            count += 1
            if count % 5:
                time.sleep(0.005)
        self.set_rotation(EROTATION_180)
        self.fill(0)
        # self.set_text_color(1)
        return BEGIN_WAR_NOTEST

    def set_rotation(self, direct):
        if direct == EROTATION_0:
            self.write_cmd(0xA0)
            self.write_cmd(0xC0)
        elif direct == EROTATION_180:
            self.write_cmd(0xA1)
            self.write_cmd(0xC8)
        else:
            return DISPLAY_ERR_NOTSUPPORT
        return DISPLAY_ERR_OK

    def power_off(self):
        self.write_cmd(SSD1306_DISPLAYOFF)

    def show(self, x0=0, y0=0):
        # x0 = 0
        # x1 = self.width - 1
        x1 = self.width - 1 - x0
        y1 = self.pages - 1 - y0
        # if self.width == 64:
        #     # displays with width of 64 pixels are shifted by 32
        #     x0 += 32
        #     x1 += 32
        self.write_cmd(SSD1306_COLUMNADDR)
        self.write_cmd(x0)
        self.write_cmd(x1)
        self.write_cmd(SSD1306_PAGEADDR)
        self.write_cmd(y0)
        self.write_cmd(y1)
        self.write_data(self.buffer)

    def set_textColor(self, col):
        self.text_color = col

    def fill(self, col):
        self.framebuf.fill(col)

    def pixel(self,x, y, col):
        self.framebuf.set_pixel(x, y, col)

    def hline(self, x, y, w, col):
        self.framebuf.hline(x, y, w, col)

    def vline(self, x, y, h, col):
        self.framebuf.vline(x, y, h, col)

    def line(self, x1, y1, x2, y2, col):
        self.framebuf.line(x1, y1, x2, y2, col)

    def rect(self, x, y, w, h, col):
        self.framebuf.rect(x, y, w, h, col)

    def fill_rect(self, x, y, w, h, col):
        self.framebuf.fill_rect(x, y, w, h, col)

    def circle(self, x, y, radius, col):
        self.framebuf.circle(x, y, radius, col)

    def fill_circle(self, x, y, radius, col):
        self.framebuf.fill_circle(x, y, radius, col)

    def text(self, string='Hello World!', x=20, y=22, col=1, charbuf = []):
        self.framebuf.text(str(string), x, y, col, charbuf)


class SSD1306_I2C(SSD1306):
  def __init__(self, board=None, width=128, height=64, bus_num=4, addr=0x3C, external_vcc=False):
    if isinstance(board, int):
      width = board
      board = gboard
    elif board is None:
      board = gboard

    self.board = board
    self.i2c = I2C(bus_num)
    self.addr = addr
    self.temp = [0x80, 0]
    super().__init__(width, height, external_vcc)

  def write_cmd(self, cmd):
    self.temp[1] = cmd
    self.i2c.writeto(self.addr, self.temp)

  def write_data(self, buf):
    count = 0
    while count < self.buf_size:
      temp = bytearray()
      temp.append(0x40)
      temp.extend(self.buffer[count:count+16])
      count += 16
      self.i2c.writeto(self.addr, temp)
      time.sleep(0.01)

  def power_on(self):
    pass
