# -*- coding:utf-8 -*-
import time
from pinpong.board import Board, Pin
from pinpong.libs.dfrobot_as7341 import AS7341
from pinpong.libs.dfrobot_st7789 import ST7789_SPI

Board("milkv-duo").begin()

#设置demo所需的容器
prev_rect_num      = [0,0,0,0,0,0,0,0]
rect_num           = [0,0,0,0,0,0,0,0]
transform_rect_num = [0,0,0,0,0,0,0,0]
text_list          = ['F1','F2','F3','F4','F5','F6','F7','F8']
color_list         = [0x284B,0x194F,0x130D,0x266D,0x5F64,0xF548,0xF982,0x60A0]

#设置LCD屏dc和res引脚
dc = Pin(pin=15, mode=Pin.OUT)
res = Pin(pin=14, mode=Pin.OUT)

#构造外设对象
as7341 = AS7341()
lcd = ST7789_SPI(width=240, height=240, bus_num=0,device_num=0, dc=dc, res=res) 
lcd.reset()
#设置外设参数
mode  = as7341.e_ch_choose_t
#lcd.set_blend_mode(lcd.REPLACE)
lcd.set_font(font="msyh", size=30)

#在LCD上打印F1-F8
def text_print():
  for index,text in enumerate(text_list):
    lcd.text(text,0,(index)*30,color_list[index]) 

#获取as7341传感器的频谱数据
def get_spectrum_data_data():

  as7341.start_measure(0)
  data1 = as7341.read_spectral_data_one()
  as7341.start_measure(1)
  data2 = as7341.read_spectral_data_two()
  return  {data1['adf1'],data1['adf2'],data1['adf3'],data1['adf4'], data2['adf5']
  ,data2['adf6'],data2['adf7'],data2['adf8']}

#矩形变化
def transform_rects():
  global prev_rect_num,rect_num,transform_rect_num
  for index,transform in enumerate(transform_rect_num):
    if transform > 0:
      lcd.fill_rect(50+prev_rect_num[index]*2,(index)*30,transform*2,30,color_list[index])
    if transform < 0:
      lcd.fill_rect(50+(prev_rect_num[index]+transform)*2,(index)*30,abs(transform)*2,30,lcd.COLOR_BLACK)
    else:
      pass
  prev_rect_num = rect_num.copy()

#打印矩形
def print_rects(spectrum_data):
  global prev_rect_num,rect_num,transform_rect_num
  for index,data in enumerate(spectrum_data):
    if(data>999):
      rect_num[index] = 50
    else:
      if(data%20 !=0):
        rect_num[index] = data//20 + 1
      else:
        rect_num[index] = data//20
    transform_rect_num[index] = rect_num[index] - prev_rect_num[index]
  transform_rects()

#打印数据
def data_print(spectrum_data):
  lcd.fill_rect(180,0,60,240,lcd.COLOR_BLACK)
  for index,data in enumerate(spectrum_data):
    if data > 999:
      lcd.text('max',180,(index)*30,color_list[index])
    else:
      lcd.text(str(data),180,(index)*30,color_list[index])

#初始化外设
lcd.fill(lcd.COLOR_BLACK) #填充屏幕为黑色
while(as7341.begin() != True):
    lcd.text('AS7341 ERROR',0,0,lcd.COLOR_WHITE)
    time.sleep(0.5)
as7341.set_a_time(29)
as7341.set_a_step(400)
as7341.set_again(7)

#在LCD上打印F1-F8
text_print()

while True:
  #获取数据
  spectrum_data = get_spectrum_data_data()
  #{700:30,800:50,900:100,1500:200}
  #打印柱状图
  print_rects(spectrum_data) 
  #打印数据
  #data_print(spectrum_data)
