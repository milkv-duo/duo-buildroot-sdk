# -*- coding: utf-8 -*-
import os
import time
import freetype
from PIL import Image, ImageSequence
from pinpong.board import gboard,SPI

class ST7789:
  COLOR_NAVY    =  0x000F  #  深蓝色  
  COLOR_DGREEN  =  0x03E0  #  深绿色  
  COLOR_DCYAN   =  0x03EF  #  深青色  
  COLOR_MAROON  =  0x7800  #  深红色  
  COLOR_PURPLE  =  0x780F  #  紫色  
  COLOR_OLIVE   =  0x7BE0  #  橄榄绿  
  COLOR_LGRAY   =  0xC618  #  灰白色
  COLOR_DGRAY   =  0x7BEF  #  深灰色  
  COLOR_BLUE    =  0x001F  #  蓝色    
  COLOR_GREEN   =  0x07E0  #  绿色    
  COLOR_CYAN    =  0x07FF  #  青色  
  COLOR_RED     =  0xF800  #  红色    
  COLOR_MAGENTA =  0xF81F  #  品红    
  COLOR_YELLOW  =  0xFFE0  #  黄色   
  COLOR_BLACK   = 0x0000   #  黑色    
  COLOR_BLUE    = 0x001F   #  蓝色  
  COLOR_GREEN   = 0x07E0   #  绿色 
  COLOR_WHITE   = 0xFFFF   #  白色 

  MERGE   = 0 #混合模式
  REPLACE = 1 #覆盖模式

  init_cmds = [
    #flag cmd 最高位为1表示后2位是延时，低7位表示参数的个数
    0x01, 0x01, 0x80, 0, 150,
    0x01, 0x11, 0x80, 0, 120,
    0x01, 0x3A, 1, 0x55,
    0x01, 0x36, 1, 0x00,
    0x01, 0x21, 0,
    0x01, 0x13, 0,
    0x01, 0x29, 0,
    0x00
  ]
  
  _ST7789_COLSET = 0x2A
  _ST7789_RAWSET = 0x2B
  _ST7789_RAMWR  = 0x2C

  
  COLOR_BLACK  = 0x0000   #  黑色    
  COLOR_BLUE   = 0x001F   #  深蓝色  
  COLOR_GREEN  = 0x07E0   #  深绿色 
  COLOR_RED    = 0xF800   #  深红色
  COLOR_WHITE  = 0xffff   #  白色 

  MERGE   = 0 #混合模式
  REPLACE = 1 #覆盖模式
  
  def __init__(self, width, height):
    self.width = width
    self.height = height

  def reset(self):
    offset = 0
    while self.init_cmds[offset]:
      offset += 1
      cmd = self.init_cmds[offset]
      offset += 1
      val = self.init_cmds[offset]
      offset += 1
      argsNum = val & 0x7F
      if val & 0x80:
        duration = self.init_cmds[offset]*255 + self.init_cmds[offset+1]
        time.sleep(duration/1000)
        offset += 2
      self.send_cmd(cmd, self.init_cmds[offset:offset+argsNum])
      offset += argsNum

  def flush(self,buf,x1,y1,x2,y2):
    self.send_cmd(self._ST7789_COLSET)
    self.send_data16(x1)#x起始坐标
    self.send_data16(x2)#x结束坐标
    self.send_cmd(self._ST7789_RAWSET)
    self.send_data16(y1)#y起始坐标
    self.send_data16(y2)#y结束坐标
    self.send_cmd(self._ST7789_RAMWR)
    self.send_buf(buf)

class ST7789_SPI(ST7789):
  def __init__(self, board=None, width=240, height=240, bus_num=1, device_num=0, dc=None, res=None, cs=None, font="msyh"):
    if board is None:
      board = gboard

    self.board = board
    self.dc=dc
    self.cs=cs
    self.res=res
    self.spi = SPI(bus_num=bus_num,device_num=device_num)
    #self.framebuf = FRAME_BUF(width,height,RGB565)
    self.framebuf = FRAME_BUF()
    super().__init__(width, height)

  def reset(self):
    if self.res:
      self.res.value(1)
    if self.dc:
      self.dc.value(1)

    if self.res:
      self.res.value(0)
      time.sleep(0.2)
      self.res.value(1)
      time.sleep(0.2)
    super().reset()

  def send_cmd(self, cmd, value=None):
    self.dc.value(0)
    self.spi.write([cmd])
    self.dc.value(1)
    if value:
      self.spi.write(value)

  def send_data16(self, data):
    self.spi.write([data>>8, data&0xff])

  def send_buf(self,buf):
    self.spi.write(buf)

  def blend_mode(self,mode):
    return self.framebuf.blend_mode(mode)
  
  def set_font(self,font,size):
    return self.framebuf.set_font(font,size)

  def fill(self,color):
    return self.framebuf.fill(color,self)

  def text(self,text,x,y,color):
    return self.framebuf.text(text,x,y,color,self)
    
  def rect(self,x1,y1,x2,y2,color):
    return self.framebuf.rect(x1,y1,x2,y2,color,self)
  
  def fill_rect(self,x1,y1,x2,y2,color):
    return self.framebuf.fill_rect(x1,y1,x2,y2,color,self)
  
  def circle(self,x,y,r,color):
    return self.framebuf.circle(x,y,r,color,self)
  
  def fill_circle(self,x,y,r,color):
    return self.framebuf.fill_circle(x,y,r,color,self)
  
  def line(self,x1,y1,x2,y2,color):
    return self.framebuf.line(x1,y1,x2,y2,color,self)
  
  def picture(self,filename,x,y,size):
    return self.framebuf.picture(filename,x,y,size,self)

class FRAME_BUF(object):
  blend_mode = 0
 
  font = "msyh"
  font_size   = 30
  
  bitmap_width = 0
  bitmap_height = 0
 
  send_pixel = []
  pixel = [[0 for col in range(2)] for row in range(240*240)]#定义一个二维列表
 
  def __init__(self):
    cwdpath,_ = os.path.split(os.path.realpath(__file__))
    self._face = freetype.Face(cwdpath+"/../base/"+self.font+".ttf")
 
  def text(self, text, x, y, color, obj):
   self.string(text, x, y, color, obj)
   self.flush(obj,x,y,len(text)*self.font_size,self.font_size)
 
  
  def string(self, text, x_pos, y_pos, color,obj):
   prev_char = 0
   pen       = freetype.Vector()
   pen.x     = x_pos << 6 # div 64
   pen.y     = y_pos << 6
   #这个矩阵控制字体旋转
   matrix    = freetype.Matrix(int(0.5 * 0x10000), int(0.4 * 0x10000), \
                               int(0.0 * 0x10000), int(1.0 * 0x10000))
   #这个向量控制字体平移
   pen_translate = freetype.Vector()
   cur_pen       = freetype.Vector()
   x = x_pos
   y = y_pos
   for cur_char in text:
     self._face.set_transform(matrix, pen_translate)
     #将字形图像装载到字形槽
     self._face.load_char(cur_char)
     kerning = self._face.get_kerning(prev_char, cur_char)
     pen.x += kerning.x
     #取字形槽
     glyph = self._face.glyph
     #取位图
     bitmap = glyph.bitmap
     cur_pen.x = pen.x
     cur_pen.y = pen.y - glyph.bitmap_top * 64
     cols = bitmap.width
 
     self.bitmap_width += cols 
     self.bitmap(bitmap, color, x, y,obj)
 
     x = x + self.font_size
     pen.x += glyph.advance.x
     prev_char = cur_char
 
  #freetype显示
  def bitmap(self, bitmap, color,x, y,obj):
   cols = bitmap.width
   rows = bitmap.rows
   glyph_pixels = bitmap.buffer
   temp = x
   for row in range(rows):
     for col in range(cols):
       if glyph_pixels[row * cols + col] != 0:
         self._set_pixel(x,y,color,obj)
         x = x + 1
       else:    
         x = x + 1 #该像素点没数据，跳过
     y = y + 1     #换行
     x = temp      #x轴方向的起始坐标
 
 
  def line(self,x1,y1,x2,y2,color,obj):
    delta_x = x2 - x1
    delta_y = y2 - y1
    if delta_x > 0:
      incx = 1
    elif delta_x == 0:
      incx = 0 
    else:
      incx = -1
      delta_x = -delta_x
    if delta_y > 0:
      incy = 1
    elif delta_y == 0:
      incy = 0
    else:
      incy = -1
      delta_y = -delta_y
    if delta_x > delta_y:
      distance = delta_x
    else:
      distance = delta_y
    x = x1
    y = y1
    x_temp = 0
    y_temp = 0
    for i in range(0,distance+2):
      self._set_pixel(x,y,color,obj)
      x_temp += delta_x
      if x_temp > distance:
        x_temp -= distance
        x += incx
      y_temp += delta_y
      if y_temp > distance:
        y_temp -= distance
        y += incy
    self.flush(obj,x1,y1,x2,y2)    
 
  def rect(self,x,y,w,h,color,obj):
    if((x+w)>obj.width): 
     w = obj.width - x
    if((y+h)>obj.height):
     h = obj.height - y
    self.line(x, y, x+w, y, color, obj)
    self.line(x+w, y, x+w, y+h, color, obj)
    self.line(x, y+h, x+w, y+h, color, obj)
    self.line(x, y, x, y+h, color, obj)
    self.flush(obj,x,y,x+w,y+h)
  
  def fill_rect(self,x,y,w,h,color,obj):
    if((x+w)>obj.width): 
     w = obj.width - x
    if((y+h)>obj.height):
     h = obj.height - y
    for width in range(x,x+w):
      for heigth in range(y,y+h):
        self._set_pixel(width,heigth,color,obj)
    self.flush(obj,x,y,x+w,y+h)
 
  def circle(self, x, y, r, color, obj):#画圆
    r = abs(r)
    if r == 1:
      r = 2
    x_axis = 0
    y_axis = r
    var = 3 - (r << 1)
    while (x_axis <= y_axis):
     #填充像素点信息
     self._set_pixel(x + x_axis, y + y_axis, color, obj)
     self._set_pixel(x - x_axis, y + y_axis, color, obj)
     self._set_pixel(x + x_axis, y - y_axis, color, obj)
     self._set_pixel(x - x_axis, y - y_axis, color, obj)
     self._set_pixel(x + y_axis, y + x_axis, color, obj)
     self._set_pixel(x - y_axis, y + x_axis, color, obj)
     self._set_pixel(x + y_axis, y - x_axis, color, obj)
     self._set_pixel(x - y_axis, y - x_axis, color, obj)
     if var < 0:
       var += 4 * x_axis + 6
     else:
       var += 10 + 4 * (x_axis - y_axis)
       y_axis -= 1
     x_axis += 1
    if(0<x-r<240)&(0<y-r<240)&(0<x+r<240)&(0<y+r<240):
     self.flush(obj,x-r,y-r,x+r,y+r)
    elif(x==0&y==0):
     self.flush(obj,x-r,y-r,x+r,y+r)
    else:
      self.cir_flush(obj,x-r,y-r,x+r,y+r)
  
  def fill_circle(self,x,y,r,color,obj):#画圆
    r = abs(r)
    if r == 1:
      r = 2
    x_axis = y_end =  0
    y_axis = r
    var = 3 - (r << 1)
    while (x_axis <= y_axis):
      #填充像素点信息
      self.fill_rect(x + x_axis, y - y_axis, 1, 2 * y_axis + 1, color, obj)
      self.fill_rect(x + y_axis, y - x_axis, 1, 2 * x_axis + 1, color, obj)
      self.fill_rect(x - x_axis, y - y_axis, 1, 2 * y_axis + 1, color, obj)
      self.fill_rect(x - y_axis, y - x_axis, 1, 2 * x_axis + 1, color, obj)
      if var < 0:
        var += 4 * x_axis + 6
      else:
        var += 10 + 4 * (x_axis - y_axis)
        y_axis -= 1
      x_axis += 1
    #self.flush(obj,x-r,y-r,x+r,y+r)
 
  def picture(self,filename,x,y,size,obj):
    img = Image.open(filename)
    if((filename[-3:-1:]+filename[-1]) == "gif" or (filename[-3:-1:]+filename[-1]) == "GIF"):
      for frame in ImageSequence.all_frames(img):
       frame = frame.convert("RGB")
       frame.thumbnail((size,size))
       for i in range(0,frame.size[1]):
         for m in range(0,frame.size[0]):
           R = frame.getpixel((m,i))[0] >> 3
           G = frame.getpixel((m,i))[1] >> 2
           B = frame.getpixel((m,i))[2] >> 3
           color = (R << 11)|(G << 5)|(B)
           self.pixel[(y+i)*240+(x+m)] = [color>>8,color]
       self.flush(obj,x,y,(x+size),(y+size))
    else:
      img = img.convert("RGB")
      img.thumbnail((size,size))
      for i in range(0,img.size[1]):
         for m in range(0,img.size[0]):
           R = img.getpixel((m,i))[0] >> 3
           G = img.getpixel((m,i))[1] >> 2
           B = img.getpixel((m,i))[2] >> 3
           color = (R << 11)|(G << 5)|(B)
           self.pixel[(y+i)*240+(x+m)] = [color>>8,color]
      self.flush(obj,x,y,(x+size),(y+size))
  
  def fill(self,color,obj):
    for i in range(obj.width*obj.height):
      self.pixel[i] = [color>>8,color]
    self.flush(obj,0,0,obj.width,obj.height) 
   
  def blend_mode(self,mode):
    self.blend_mode = mode
  
  def set_font(self,font,size):
    #   self._face.__del__()
    self.font      = font
    self.font_size = size
    cwdpath,_  = os.path.split(os.path.realpath(__file__))
    self._face = freetype.Face(cwdpath+"/../base/"+self.font+".ttf")
    self._face.set_char_size(self.font_size*64)
 
  def _set_pixel(self,x,y,color,obj):
    if(self.blend_mode == 0):
     if((0<=y<obj.height)&(0<=x<=obj.width)):
       self.pixel[y*obj.width+x] = [(color|self.pixel[y*obj.width+x][1])>>8,color|self.pixel[y*obj.width+x][1]]#这是二维列表
    else:
     if((0<=y<obj.height)&(0<=x<=obj.width)):
       self.pixel[y*obj.width+x] = [color>>8,color]#这是二维列表
  #两个flush需要合并
  def flush(self,obj,x1,y1,x2,y2):
    x1 = 0 if(x1 <= 0) else x1
    x2 = 0 if(x2 <= 0) else x2
    y1 = 0 if(y1 <= 0) else y1
    y2 = 0 if(y2 <= 0) else y2 
    x1 = obj.width  if(x1 > obj.width)  else x1
    x2 = obj.width  if(x2 > obj.width)  else x2
    y1 = obj.height if(y1 > obj.height) else y1
    y2 = obj.height if(y1 > obj.height) else y2
    buf=[]
    length_x =  abs(x1-x2) + 1
    length_y =  abs(y1-y2) + 1
    for y in range(0,length_y):
     buf += [n for a in self.pixel[((y1+y)*obj.width+x1):((y1+y)*obj.width+x1+length_x)] for n in a ]#展开成一维列表
    obj.flush(buf,x1,y1,x2,y2)
 
  def cir_flush(self,obj,x1,y1,x2,y2):
    x1 = 0 if(x1 <= 0) else x1
    x2 = 0 if(x2 <= 0) else x2
    y1 = 0 if(y1 <= 0) else y1
    y2 = 0 if(y2 <= 0) else y2 
    x1 = obj.width  if(x1 > obj.width)  else x1
    x2 = obj.width  if(x2 > obj.width)  else x2
    y1 = obj.height if(y1 > obj.height) else y1
    y2 = obj.height if(y1 > obj.height) else y2
    buf=[]
    length_x =  abs(x1-x2)
    length_y =  abs(y1-y2)
    for y in range(0,length_y):
     buf += [n for a in self.pixel[((y1+y)*obj.width+x1):((y1+y)*obj.width+x1+length_x)] for n in a ]#展开成一维列表
    obj.flush(buf,x1,y1,x2,y2)