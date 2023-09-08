import time
from pinpong.board import gboard,Pin,ADC


class ANALOG_URM09():
    def __init__(self, board = None, pin_obj = None):
        if isinstance(board, Pin):
            pin_obj = board
            board = gboard
        elif board is None:
            board = gboard
        self.board = board
        self.pin_obj = pin_obj
        self.adc0 = ADC(self.pin_obj)
        if self.adc0.board.boardname == "UNIHIKER" or self.adc0.board.boardname == "HANDPY":
            self.map = 4095
            self.aref = 3.3
        elif self.adc0.board.boardname == "MICROBIT":
            self.map = 1024
            self.aref = 3.3
        else:
            self.map = 1023
            self.aref = 5.0

    def distance_cm(self):
        return int(self.adc0.read()*520/self.map)

        
