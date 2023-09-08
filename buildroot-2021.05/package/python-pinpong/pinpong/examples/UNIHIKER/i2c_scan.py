import time
from pinpong.board import Board,I2C

Board("UINIHIKER").begin()
i2c = I2C(0)

l=i2c.scan()
print("i2c list:",l)

while(1):
	time.sleep(1)
    