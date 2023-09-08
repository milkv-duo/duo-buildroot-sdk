# -*- coding: utf-8 -*-

#柱状图类
class HISTOGRAM(object):

  def __init__(self,obj,x,y,data_num):
    self.screen = obj
    self.pos_x  = x
    self.pos_y  = y
    self.rect_num = data_num
    self.prev_histogram      = [0 for num in range(self.rect_num)]
    self.histogram           = self.prev_histogram.copy()
    self.transform_histogram = self.histogram.copy()
  
#矩形变化
  def _transform_rects(self,color_list):
    for index,transform in enumerate(self.transform_histogram):
      if transform > 0:
        self.screen.fill_rect(self.pos_x+self.prev_histogram[index]*2,(index)*30+self.pos_y,transform*2,30,color_list[index])
      if transform < 0:
        self.screen.fill_rect(self.pos_x+(self.prev_histogram[index]+transform)*2,(index)*30+self.pos_y,abs(transform)*2,30,self.screen.COLOR_BLACK)
      else:
        pass
    self.prev_histogram = self.histogram.copy()

#打印数据
  def _print_sensor_data(self,sensor_data,color_list):
    self.screen.fill_rect(180,0,60,240,self.screen.COLOR_BLACK)
    for index,data in enumerate(sensor_data):
      if sensor_data[data]>999 :  self.screen.text('max',180,(index)*30,color_list[index])
      else: self.screen.text(str(sensor_data[data]),180,(index)*30,color_list[index])

#打印柱状图
  def print_histogram(self,sensor_data,color_list):
    for index,data in enumerate(sensor_data):
      if(sensor_data[data]>999):
        self.histogram[index] = 50
      else:
        if(sensor_data[data]%20 !=0):
          self.histogram[index] = sensor_data[data]//20 + 1
        else:
          self.histogram[index] = sensor_data[data]//20
      self.transform_histogram[index] = self.histogram[index] - self.prev_histogram[index]
    self._transform_rects(color_list)

#打印带数据的柱状图
  def histogram_with_data(self,sensor_data,color_list):
    self.print_histogram(sensor_data,color_list)
    self._print_sensor_data(sensor_data,color_list)
