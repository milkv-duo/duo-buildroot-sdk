# -*- coding: utf-8 -*-

# 实验效果：获取方框的的x,y坐标及长宽。
# 接线：使用windows或linux电脑连接一块arduino主控板，哈士奇接到I2C口SCL SDA
import time
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylens import Huskylens, Parameter

# from dfrobot_huskylens import Huskylens

Board("uno").begin()  # 初始化，选择板型和端口号，不输入端口号则进行自动识别
# Board("uno","COM36").begin()  #windows下指定端口初始化
# Board("uno","/dev/ttyACM0").begin()   #linux下指定端口初始化
# Board("uno","/dev/cu.usbmodem14101").begin()   #mac下指定端口初始化

husky = Huskylens()

# # 切换算法
parame = ["ALGORITHM_FACE_RECOGNITION", "ALGORITHM_OBJECT_TRACKING", "ALGORITHM_OBJECT_RECOGNITION",
          "ALGORITHM_LINE_TRACKING", "ALGORITHM_COLOR_RECOGNITION", "ALGORITHM_TAG_RECOGNITION",
          "ALGORITHM_OBJECT_CLASSIFICATION"]
# for i in range(5):
#     for j in range(7):    
#         husky.command_request_algorthim(parame[j])
#         time.sleep(0.5)

husky.command_request_algorthim(parame[4])

while True:
    # # 获取请求结果
    # data = husky.command_request()
    # print(data)
    # if (data):

    #     num_of_objects = int(len(data)/5)
    #     for i in range(num_of_objects):
    #         x_center = data[5*i]
    #         y_center = data[5*i+1]
    #         width = data[5*i+2]
    #         height = data[5*i+3]
    #         ID = data[5*i+4]
    #         print("X_center:",x_center)
    #         print("y_center:",y_center)
    #         print("width:",width)
    #         print("height:",height)
    #         print("ID:",ID)

    # 获取方框参数
    # data_blocks = husky.command_request_blocks()
    # print(data_blocks)

    # 获取箭头参数
    # data = husky.command_request_arrows()
    # print(data)

    # 获取已学习的参数
    # data = husky.command_request_learned()
    # print(data)

    # # 获取已学习的方框参数
    # data = husky.command_request_blocks_learned(1)

    # # 获取已学习的箭头参数
    # data = husky.command_request_arrows_learned(1)

    # # 读取IDx 的参数
    # data = husky.command_request_by_id(1)

    # # 读取IDx blocks的参数
    # data = husky.command_request_blocks_by_id(1)

    # # 读取IDx arrows的参数
    # data = husky.command_request_arrows_by_id(1)

    # # 显示文字
    # data = husky.command_request_custom_text("Test",20,30)

    # # 清除文字
    # data = husky.command_request_clear_text()

    # # 拍照
    # data = husky.command_request_photo()

    # # 设置IDx名为xxxx
    # data = husky.command_request_customnames(1, "Test")

    # # 截屏
    # data = husky.command_request_screenshot()

    # # 自动学习一次IDx
    # data = husky.command_request_learn_once(2)

    # # 忘记当前算法所有学习数据
    # data = husky.command_request_forget()

    # # 保存算法到SD卡x号模型
    # data = husky.command_request_save_model_to_SD_card(1)

    # # 从SD卡中加载x号模型算法
    # data = husky.command_request_load_model_from_SD_card(1)

    # # 查找blocks/arrows
    # data = husky.select_blocks_arrows("blocks")

    # # 已学ID总数
    # data = husky.read_learned_id_count()

    # # blocks/arrows是否在画面中
    # data = husky.is_appear_direct("blocks")

    # # IDx是否已学习
    # data = husky.is_learned(3)

    # # ID为x的blocks/arrows是否在画面中
    # data = husky.is_appear(1, "blocks")

    # #读取IDx的方框/箭头总数
    # data = husky.read_count("blocks",2)

    # # 读取第x个方框/箭头的参数
    # data = husky.read_blocks_arrows_parameter_direct(Parameter.xCenter, "blocks",1)

    # # 读取IDx第1个 blocks/arrows的参数
    data = husky.read_blocks_arrows_parameter((2, 1), Parameter.xCenter,"blocks")

    # # 读取靠近中心的blocks/arrows参数
    # data = husky.read_block_center_parameter_direct(Parameter.xCenter, "blocks")

    print(data)

    time.sleep(1)
