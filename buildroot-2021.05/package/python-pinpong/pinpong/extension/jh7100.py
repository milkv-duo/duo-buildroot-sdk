# -*- coding: utf-8 -*- 

jh7_res = {
    "init" : init,
    "begin" : begin,
    "pin" : {
        type : "general",
        "class" : "SYSFSPin",
        "pinnum" : True,
        "pinmap" : [448,449,450,451,452,None,454,10000,#0
                   456,457,458,None,None,None,None,None,#8
                   None,465,None,467,468,469,470,None,#16
                   None,None,None,None,None,None,None,None,#24
                   None,None,None,None,None,None,None,None,#32
                   None,None,None,None,None,None,494,None,#40
                   None]
        },
    "pwm" : {
        type : "general",
        "class" : "SYSFSPWM",
        "GPIO" : [7],
        "config" : {
            7 : {"channel" : 0, "io" : 38}
            }
        }
  }

def init(board, boardname, port):
  printlogo()
  board.connected = True
  
def begin(board):
  version = sys.version.split(' ')[0]
  plat = platform.platform()
  print("[01] Python"+version+" "+plat+(" " if board.boardname == "" else " Board: "+ board.boardname))
  
