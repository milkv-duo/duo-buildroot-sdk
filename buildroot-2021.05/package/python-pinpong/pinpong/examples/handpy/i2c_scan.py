import time
from pinpong.board import Board,I2C

Board("handpy").begin()
i2c = I2C(1)

l=i2c.scan()
print("i2c list:",l)

while(1):
	time.sleep(1)
    