# -*- coding: utf-8 -*-
import time
from pinpong.board import gboard,I2C

class PAJ7620U2:
  PAJ7620_IIC_ADDR                    = 0x73
  PAJ7620_PARTID                      = 0x7620
  
  PAJ7620_REGITER_BANK_SEL            = 0xEF
  
  PAJ7620_ADDR_PART_ID_LOW            = 0x00
  PAJ7620_ADDR_PART_ID_HIGH           = 0x01
  PAJ7620_ADDR_VERSION_ID             = 0x01
  PAJ7620_ADDR_SUSPEND_CMD            = 0x03
  PAJ7620_ADDR_GES_PS_DET_MASK_0      = 0x41
  PAJ7620_ADDR_GES_PS_DET_MASK_1      = 0x42
  PAJ7620_ADDR_GES_PS_DET_FLAG_0      = 0x43
  PAJ7620_ADDR_GES_PS_DET_FLAG_1      = 0x44
  PAJ7620_ADDR_STATE_INDICATOR        = 0x45
  PAJ7620_ADDR_PS_HIGH_THRESHOLD      = 0x69
  PAJ7620_ADDR_PS_LOW_THRESHOLD       = 0x6A
  PAJ7620_ADDR_PS_APPROACH_STATE      = 0x6B
  PAJ7620_ADDR_PS_RAW_DATA            = 0x6C
  
  PAJ7620_ADDR_PS_GAIN                = 0x44
  PAJ7620_ADDR_IDLE_S1_STEP_0         = 0x67
  PAJ7620_ADDR_IDLE_S1_STEP_1         = 0x68
  PAJ7620_ADDR_IDLE_S2_STEP_0         = 0x69
  PAJ7620_ADDR_IDLE_S2_STEP_1         = 0x6A
  PAJ7620_ADDR_OP_TO_S1_STEP_0        = 0x6B
  PAJ7620_ADDR_OP_TO_S1_STEP_1        = 0x6C
  PAJ7620_ADDR_OP_TO_S2_STEP_0        = 0x6D
  PAJ7620_ADDR_OP_TO_S2_STEP_1        = 0x6E
  PAJ7620_ADDR_OPERATION_ENABLE       = 0x72
  
  PAJ7620_I2C_WAKEUP                  = 0x01
  PAJ7620_I2C_SUSPEND                 = 0x00
  
  PAJ7620_ENABLE                      = 0x01
  PAJ7620_DISABLE                     = 0x00
  
  gesture_none                         = 0x00
  gesture_right                        = 0x01 << 0
  gesture_left                         = 0x01 << 1
  gesture_up                           = 0x01 << 2
  gesture_down                         = 0x01 << 3
  gesture_forward                      = 0x01 << 4
  gesture_backward                     = 0x01 << 5
  gesture_clockwise                    = 0x01 << 6
  gesture_anti_clockwise               = 0x01 << 7
  gesture_wave                         = 0x01 << 8
  gesture_wave_slowly_disorder         = 0x01 << 9
  gesture_wave_slowly_left_right       = gesture_left + gesture_right
  gesture_wave_slowly_up_down          = gesture_up + gesture_down
  gesture_wave_slowly_forward_backward = gesture_forward + gesture_backward
  gesture_all = 0xff
  
  gesture_discription_table= {
  gesture_none:"None",
  gesture_right:"Right",
  gesture_left:"Left",
  gesture_up:"Up",
  gesture_down:"Down",
  gesture_forward:"Forward",
  gesture_backward:"Backward",
  gesture_clockwise:"Clockwise",
  gesture_anti_clockwise:"Anti-Clockwise",
  gesture_wave:"Wave",
  gesture_wave_slowly_disorder:"wave_slowly_disorder",
  gesture_wave_slowly_left_right:"wave_slowly_left_right",
  gesture_wave_slowly_up_down:"wave_slowly_up_down",
  gesture_wave_slowly_forward_backward:"wave_slowly_forward_backward"
  }
  
  init_register_array=[
    [0xEF,0x00],
    [0x32,0x29],
    [0x33,0x01],
    [0x34,0x00],
    [0x35,0x01],
    [0x36,0x00],
    [0x37,0x07],
    [0x38,0x17],
    [0x39,0x06],
    [0x3A,0x12],
    [0x3F,0x00],
    [0x40,0x02],
    [0x41,0xFF],
    [0x42,0x01],
    [0x46,0x2D],
    [0x47,0x0F],
    [0x48,0x3C],
    [0x49,0x00],
    [0x4A,0x1E],
    [0x4B,0x00],
    [0x4C,0x20],
    [0x4D,0x00],
    [0x4E,0x1A],
    [0x4F,0x14],
    [0x50,0x00],
    [0x51,0x10],
    [0x52,0x00],
    [0x5C,0x02],
    [0x5D,0x00],
    [0x5E,0x10],
    [0x5F,0x3F],
    [0x60,0x27],
    [0x61,0x28],
    [0x62,0x00],
    [0x63,0x03],
    [0x64,0xF7],
    [0x65,0x03],
    [0x66,0xD9],
    [0x67,0x03],
    [0x68,0x01],
    [0x69,0xC8],
    [0x6A,0x40],
    [0x6D,0x04],
    [0x6E,0x00],
    [0x6F,0x00],
    [0x70,0x80],
    [0x71,0x00],
    [0x72,0x00],
    [0x73,0x00],
    [0x74,0xF0],
    [0x75,0x00],
    [0x80,0x42],
    [0x81,0x44],
    [0x82,0x04],
    [0x83,0x20],
    [0x84,0x20],
    [0x85,0x00],
    [0x86,0x10],
    [0x87,0x00],
    [0x88,0x05],
    [0x89,0x18],
    [0x8A,0x10],
    [0x8B,0x01],
    [0x8C,0x37],
    [0x8D,0x00],
    [0x8E,0xF0],
    [0x8F,0x81],
    [0x90,0x06],
    [0x91,0x06],
    [0x92,0x1E],
    [0x93,0x0D],
    [0x94,0x0A],
    [0x95,0x0A],
    [0x96,0x0C],
    [0x97,0x05],
    [0x98,0x0A],
    [0x99,0x41],
    [0x9A,0x14],
    [0x9B,0x0A],
    [0x9C,0x3F],
    [0x9D,0x33],
    [0x9E,0xAE],
    [0x9F,0xF9],
    [0xA0,0x48],
    [0xA1,0x13],
    [0xA2,0x10],
    [0xA3,0x08],
    [0xA4,0x30],
    [0xA5,0x19],
    [0xA6,0x10],
    [0xA7,0x08],
    [0xA8,0x24],
    [0xA9,0x04],
    [0xAA,0x1E],
    [0xAB,0x1E],
    [0xCC,0x19],
    [0xCD,0x0B],
    [0xCE,0x13],
    [0xCF,0x64],
    [0xD0,0x21],
    [0xD1,0x0F],
    [0xD2,0x88],
    [0xE0,0x01],
    [0xE1,0x04],
    [0xE2,0x41],
    [0xE3,0xD6],
    [0xE4,0x00],
    [0xE5,0x0C],
    [0xE6,0x0A],
    [0xE7,0x00],
    [0xE8,0x00],
    [0xE9,0x00],
    [0xEE,0x07],
    [0xEF,0x01],
    [0x00,0x1E],
    [0x01,0x1E],
    [0x02,0x0F],
    [0x03,0x10],
    [0x04,0x02],
    [0x05,0x00],
    [0x06,0xB0],
    [0x07,0x04],
    [0x08,0x0D],
    [0x09,0x0E],
    [0x0A,0x9C],
    [0x0B,0x04],
    [0x0C,0x05],
    [0x0D,0x0F],
    [0x0E,0x02],
    [0x0F,0x12],
    [0x10,0x02],
    [0x11,0x02],
    [0x12,0x00],
    [0x13,0x01],
    [0x14,0x05],
    [0x15,0x07],
    [0x16,0x05],
    [0x17,0x07],
    [0x18,0x01],
    [0x19,0x04],
    [0x1A,0x05],
    [0x1B,0x0C],
    [0x1C,0x2A],
    [0x1D,0x01],
    [0x1E,0x00],
    [0x21,0x00],
    [0x22,0x00],
    [0x23,0x00],
    [0x25,0x01],
    [0x26,0x00],
    [0x27,0x39],
    [0x28,0x7F],
    [0x29,0x08],
    [0x30,0x03],
    [0x31,0x00],
    [0x32,0x1A],
    [0x33,0x1A],
    [0x34,0x07],
    [0x35,0x07],
    [0x36,0x01],
    [0x37,0xFF],
    [0x38,0x36],
    [0x39,0x07],
    [0x3A,0x00],
    [0x3E,0xFF],
    [0x3F,0x00],
    [0x40,0x77],
    [0x41,0x40],
    [0x42,0x00],
    [0x43,0x30],
    [0x44,0xA0],
    [0x45,0x5C],
    [0x46,0x00],
    [0x47,0x00],
    [0x48,0x58],
    [0x4A,0x1E],
    [0x4B,0x1E],
    [0x4C,0x00],
    [0x4D,0x00],
    [0x4E,0xA0],
    [0x4F,0x80],
    [0x50,0x00],
    [0x51,0x00],
    [0x52,0x00],
    [0x53,0x00],
    [0x54,0x00],
    [0x57,0x80],
    [0x59,0x10],
    [0x5A,0x08],
    [0x5B,0x94],
    [0x5C,0xE8],
    [0x5D,0x08],
    [0x5E,0x3D],
    [0x5F,0x99],
    [0x60,0x45],
    [0x61,0x40],
    [0x63,0x2D],
    [0x64,0x02],
    [0x65,0x96],
    [0x66,0x00],
    [0x67,0x97],
    [0x68,0x01],
    [0x69,0xCD],
    [0x6A,0x01],
    [0x6B,0xB0],
    [0x6C,0x04],
    [0x6D,0x2C],
    [0x6E,0x01],
    [0x6F,0x32],
    [0x71,0x00],
    [0x72,0x01],
    [0x73,0x35],
    [0x74,0x00],
    [0x75,0x33],
    [0x76,0x31],
    [0x77,0x01],
    [0x7C,0x84],
    [0x7D,0x03],
    [0x7E,0x01]
  ]
  
  def __init__(self, board=None, i2c_addr=0x73, bus_num=4):
    if isinstance(board, int):
      i2c_addr = board
      board = gboard
    elif board is None:
      board = gboard
    
    self.mark = True
    self.i2c_addr = i2c_addr
    self.board = board
    self.i2c = I2C(bus_num)
    
    self.write_reg(self.PAJ7620_REGITER_BANK_SEL, [0])
    while self.read_reg(self.PAJ7620_ADDR_PART_ID_LOW, 2) == 0:
      print("bus data access error")
      time.sleep(0.1)
    for li in self.init_register_array:
      self.write_reg(li[0],[li[1]])
    self.write_reg(self.PAJ7620_REGITER_BANK_SEL, [0])
  
  def get_gesture(self):
    data = self.read_reg(self.PAJ7620_ADDR_GES_PS_DET_FLAG_1, 1)
    gesture = data[0] << 8
    if gesture == self.gesture_wave:
      time.sleep(1)
    else:
      gesture = self.gesture_none
      data = self.read_reg(self.PAJ7620_ADDR_GES_PS_DET_FLAG_0, 1)
      gesture = data[0] & 0x00ff
      if not self.mark:
        time.sleep(1)
        data = self.read_reg(self.PAJ7620_ADDR_GES_PS_DET_FLAG_0, 1)
        gesture = gesture | data[0]
      if gesture != self.gesture_none:
        if gesture == self.gesture_right: pass
        elif gesture == self.gesture_left: pass
        elif gesture == self.gesture_up: pass
        elif gesture == self.gesture_down: pass
        elif gesture == self.gesture_forward:
          if not self.mark:
            time.sleep(1)
          else:
            time.sleep(0.2)
        elif gesture == self.gesture_backward:
          if not self.mark:
            time.sleep(1)
          else:
            time.sleep(0.2)
        elif gesture == self.gesture_clockwise: pass
        elif gesture == self.gesture_anti_clockwise: pass
        else:
          data = self.read_reg(self.PAJ7620_ADDR_GES_PS_DET_FLAG_1, 1)
          if data[0]:
            gesture = self.gesture_wave
          else:
            if gesture == self.gesture_wave_slowly_left_right: pass
            elif gesture == self.gesture_wave_slowly_up_down: pass
            elif gesture == self.gesture_wave_slowly_forward_backward: pass
            else:
              gesture = self.gesture_wave_slowly_disorder
    description = self.gesture_description(gesture)
    return description,gesture

  def gesture_description(self, gesture):
    if gesture in self.gesture_discription_table:
#      print(self.gesture_discription_table[gesture])
      str_gesture = self.gesture_discription_table[gesture]
    return str_gesture
    
  def set_gesture_high_rate(self):
    self.mark = True
  
  def set_gesture_low_rate(self):
    self.mark = False
  
  def write_reg(self, reg, value):
    self.i2c.writeto_mem(self.i2c_addr, reg, value)
    
  def read_reg(self, reg, lens):
    return self.i2c.readfrom_mem(self.i2c_addr, reg, lens)

class Gesture():
  none        = 0
  def __init__(self):
    pass
  def print_all():
    print("gesture = 0   description = None")
    print("gesture = 1   description = Right")
    print("gesture = 2   description = Left")
    print("gesture = 4   description = Up")
    print("gesture = 8   description = Down")
    print("gesture = 16  description = Forward")
    print("gesture = 32  description = Backward")
    print("gesture = 64  description = Clockwise")
    print("gesture = 128 description = Anti-Clockwise")
    print("gesture = 256 description = Wave")
    print("gesture = 512 description = wave_slowly_disorder")
    print("gesture = 3   description = wave_slowly_left_right")
    print("gesture = 12  description = wave_slowly_up_down")
    print("gesture = 48  description = wave_slowly_forward_backward")
