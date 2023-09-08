# -*- coding: utf-8 -*-

#实验效果：
# 将一些常用的哈士奇API集成在一个代码中。
# 切换到标签识别功能，对准标签后录入ID，
# 输入e或exit退出
# 输入f或forget遗忘
# 输入s或者switch根据ID来切换算法。（等待5s后会切换回标签识别）
# 

#接线：使用windows或linux电脑连接一块arduino主控板，哈士奇接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens

Board("PinPong Board").begin()  #初始化，选择板型和端口号，不输入端口号则进行自动识别
#Board("PinPong Board","COM36").begin()  #windows下指定端口初始化
#Board("PinPong Board","/dev/ttyACM0").begin()   #linux下指定端口初始化
#Board("PinPong Board","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

husky = Huskylens()

print("Switch to tag recognition mode...")
husky.command_request_algorthim("ALGORITHM_TAG_RECOGNITION")
time.sleep(.5)

id_list = []

while True:
    print("Align to a label and enter id (enter e to exit, enter f to forget the learned ID) :")
    v = input() 
    if v == 'e':
        break
    elif v == 'f':
        husky.command_request_forget()
        time.sleep(2)
        print("Forget the learned ID")
    elif v == 's':
        while True:
            # 获取数据
            data = husky.command_request()
            if (data):
                num_of_objects = int(len(data)/5)
                for i in range(num_of_objects):
                    #解析出ID
                    ID = data[5*i+4]
                    print("ID:",ID)
                    #当ID等于xx,切换至人脸识别
                    if ID == id_list[0]:
                        print("Get tag ID{}".format(ID))
                        print("Switch to the face recognition algorithm...")
                        husky.command_request_algorthim("ALGORITHM_FACE_RECOGNITION")
                        time.sleep(5)
                        print("According to the requirements, add the relevant code of face recognition, such as learning once, obtaining data, etc")
                        # 自定义代码xxx
                        #
                        #
                        #
                        # 切换回标签识别
                        print("Switch back to the tag recognition algorithm...")
                        husky.command_request_algorthim("ALGORITHM_TAG_RECOGNITION")
                        time.sleep(.5)
                        break
                    #当ID等于xx,切换至人脸识别
                    elif ID == id_list[1]:
                        print("Get tag ID{}".format(ID))
                        print("Switch to the object tracking algorithm...")
                        husky.command_request_algorthim("ALGORITHM_OBJECT_TRACKING")
                        time.sleep(5)
                        print("Add object tracking code as needed, such as learning once, fetching data, etc")

                        # 自定义代码xxx
                        #
                        #
                        #
                        #切换回标签识别
                        print("Switch back to the tag recognition algorithm...")
                        husky.command_request_algorthim("ALGORITHM_TAG_RECOGNITION")
                        time.sleep(.5)
                        break
    else:
        print("The tag ID {} has been learned".format(v))
        tag_id = int(v)
        id_list.append(tag_id)
        husky.command_request_learn_once(tag_id)
        time.sleep(2)



