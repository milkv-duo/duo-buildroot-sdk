from pinpong.board import gboard,I2C


algorthimsByteID = {
    "ALGORITHM_FACE_RECOGNITION": [0x00,0x00],
    "ALGORITHM_OBJECT_TRACKING": [0x01,0x00],
    "ALGORITHM_OBJECT_RECOGNITION": [0x02,0x00],
    "ALGORITHM_LINE_TRACKING": [0x03,0x00],
    "ALGORITHM_COLOR_RECOGNITION": [0x04,0x00],
    "ALGORITHM_TAG_RECOGNITION": [0x05,0x00],
    "ALGORITHM_OBJECT_CLASSIFICATION": [0x06,0x00],
    "ALGORITHM_QR_RECOGNITION": [0x07,0x00],
    "ALGORITHM_BARCODE_RECOGNITION": [0x08,0x00]
}

class Parameter:
    xCenter = 0
    yCenter = 1
    width = 2
    height = 3

    xOrigin = 0
    yOrigin = 1
    xTarget = 2
    yTarget = 3

    id = 4

class Huskylens:

    #command
    COMMAND_REQUEST = 0x20
    COMMAND_REQUEST_BLOCKS = 0x21
    COMMAND_REQUEST_ARROWS = 0x22
    COMMAND_REQUEST_LEARNED = 0x23
    COMMAND_REQUEST_BLOCKS_LEARNED = 0x24 
    COMMAND_REQUEST_ARROWS_LEARNED = 0x25
    COMMAND_REQUEST_BY_ID = 0x26
    COMMAND_REQUEST_BLOCKS_BY_ID = 0x27
    COMMAND_REQUEST_ARROWS_BY_ID = 0x28
    COMMAND_RETURN_INFO = 0x29
    COMMAND_RETURN_BLOCK = 0x2a
    COMMAND_RETURN_ARROW = 0x2b
    COMMAND_REQUEST_KNOCK = 0x2c
    COMMAND_REQUEST_ALGORITHM = 0x2d
    COMMAND_RETURN_OK = 0x2e
    COMMAND_REQUEST_CUSTOMNAMES= 0x2f
    #COMMAND_REQUEST_FORGET = 0x30
    #COMMAND_REQUEST_SENSOR = 0x31  
    COMMAND_REQUEST_TAKE_PHOTO_TO_SD_CARD = 0x30
    COMMAND_REQUEST_SAVE_MODEL_TO_SD_CARD = 0x32
    COMMAND_REQUEST_LOAD_MODEL_FROM_SD_CARD = 0x33
    COMMAND_REQUEST_CUSTOM_TEXT = 0x34
    COMMAND_REQUEST_CLEAR_TEXT = 0x35
    COMMAND_REQUEST_LEARN_ONECE = 0x36
    COMMAND_REQUEST_FORGET = 0x37
    COMMAND_REQUEST_SCREENSHOT_TO_SD_CARD = 0x39
    COMMAND_REQUEST_FIRMWARE_VERSION = 0x3C



    commandHeaderAndAddress = [0x55,0xAA,0x11]
    valid_data = [0, 0, []]

    HEADER_0_INDEX   =   0
    HEADER_1_INDEX   =   1
    ADDRESS_INDEX    =   2
    CONTENT_SIZE_INDEX = 3
    COMMAND_INDEX     =  4
    CONTENT_INDEX     =  5
    PROTOCOL_SIZE     =  6

    Huskylens_IIC_ADDR = 0x32
    

    def __init__(self, board=None, i2c_addr=Huskylens_IIC_ADDR,bus_num=0):
        ''' The i2c default device address is 0x32 '''
        if isinstance(board, int):
            i2c_addr = board
            board = gboard
        elif board is None:
            board = gboard

        self.board = board
        self.i2c_addr = i2c_addr
        self.i2c = I2C(bus_num)
        self.lastCmdSent = ""

    def _calculateChecksum(self, cmd):       
        total = 0        
        for i in cmd:            
            total += i        
        return total % 256
		#return bytearray.fromhex('{:0192x}'.format(total))[-1]

    def _splitCommandToParts(self, dataArray):
        headers = dataArray[0:2]
        address = dataArray[2]
        data_length = dataArray[3]
        command = dataArray[4]
        if(data_length > 0):
            data = dataArray[5:-1]
        else:
            data = []
        checkSum = dataArray[-1]
        #print(f"RESPONSE SPLIT INTO PARTS -> {[headers, address, data_length, command, data, checkSum]}")
        return [headers, address, data_length, command, data, checkSum]


    def _parseResponse(self, resonseData):
        #print(f"resonseData array {resonseData}")
        commandSplit = self._splitCommandToParts(resonseData)
        returnData = []
        if(commandSplit[3] == 0x2e):
            return "KNOCK RECEIVED"
        else:
            numberOfBlocksOrArrow = commandSplit[4][0]
            # print("number of objects detected:", numberOfBlocksOrArrow)
            if(commandSplit[4][1] > 0):
                numberOfBlocksOrArrow = 255+commandSplit[4][1]
            for i in range(numberOfBlocksOrArrow):
                blockOrArrowResp=self._read_response()
                splitIntoParts=self._splitCommandToParts(blockOrArrowResp)
                returnData.append(splitIntoParts[4])
        return returnData

        
    def _dataProcess(self, returnData):
        if(returnData != "KNOCK RECEIVED"):  
            data=[]
            for index, q in enumerate(returnData):
                #print("index:", index)
                for i in range(0,len(q)-1,2):
                    val=q[i]
                    if(q[i+1]>0):
                        val=256+q[i+1]
                    data.append(val)
                #data.append(q[-1])
            return data

    def _read_response(self):
        x = 0
        resonseData = []
        datalen = 0
        keep_reading = True
        while keep_reading:
            d = self.i2c.readfrom_mem(0x32, x, 1)
            if(d[0] == 0x55):
                # recv 0x55
                resonseData.append(d[0])
                # recv 0xAA
                for i in range(4):
                    x += 1
                    tmp = self.i2c.readfrom_mem(0x32, x, 1)
                    resonseData.append(tmp[0])
                datalen = resonseData[3]
                for i in range(datalen+1):
                    x += 1
                    tmp = self.i2c.readfrom_mem(0x32, x, 1)
                    resonseData.append(tmp[0])
                keep_reading = False
                break
        return resonseData
 
    def _processReturnData(self):
        resonseData = self._read_response()
        # print("_processReturnData: ", resonseData)
        self.valid_data[0] = resonseData[5] + resonseData[6]*256
        self.valid_data[1] = resonseData[7] + resonseData[8]*256
        
        parsed_data = self._parseResponse(resonseData)
        # print(parsed_data)
        self.valid_data[2] = self._dataProcess(parsed_data)
        # print("valid_data: ", self.valid_data)
        return self.valid_data[2]

    #write the command to huskylens
    def _write_to_huskyLens(self, command):
        self.i2c.writeto(self.i2c_addr, command)


    # def command_request_knock(self):
    #     #print("command_request_knock")
        
    #     cmd = self.commandHeaderAndAddress[:]
    #     #data length 0x00
    #     cmd.append(0x00)
    #     #COMMAND_REQUEST_KNOCK 0x2c
    #     cmd.append(self.COMMAND_REQUEST_KNOCK)
    #     #checksum 
    #     cmd.append(0x3C)
    #     self._write_to_huskyLens(cmd)
    #     #print("command_request_knock: ", cmd)
    #     return self._processReturnData()

    def command_request(self):
        #print("command_request")
        
        cmd = self.commandHeaderAndAddress[:]
        #data length 0x00
        cmd.append(0x00)
        #COMMAND_REQUEST 0x20
        cmd.append(self.COMMAND_REQUEST)
        #checksum 
        cmd.append(0x30)
        self._write_to_huskyLens(cmd)
        #print("command_request: ", cmd)
        return self._processReturnData()

    def command_request_blocks(self):
        #print("command_request_blocks")
         
        cmd = self.commandHeaderAndAddress[:]
        #data length 0x00
        cmd.append(0x00)
        #COMMAND_REQUEST_BLOCKS  0x21
        cmd.append(self.COMMAND_REQUEST_BLOCKS)
        #checksum 
        cmd.append(0x31)
        self._write_to_huskyLens(cmd)
        return self._processReturnData()

    def command_request_arrows(self):
        #print("command_request_arrows")
        
        cmd = self.commandHeaderAndAddress[:]
        #data length 0x00
        cmd.append(0x00)
        #COMMAND_REQUEST_ARROWS   0x22
        cmd.append(self.COMMAND_REQUEST_ARROWS)
        #checksum 
        cmd.append(0x32)
        #print("command_request_arrows:", cmd)
        self._write_to_huskyLens(cmd)
        return self._processReturnData()

    def command_request_learned(self):
        #print("command_request_learned")
         
        cmd = self.commandHeaderAndAddress[:]
        #data length 0x00
        cmd.append(0x00)
        #COMMAND_REQUEST_LEARNED    0x23
        cmd.append(self.COMMAND_REQUEST_LEARNED )
        #checksum 
        cmd.append(0x33)
        self._write_to_huskyLens(cmd)
        return self._processReturnData()

    def command_request_blocks_learned(self):
        #print("command_request_blocks_learned")
        
        cmd = self.commandHeaderAndAddress[:]
        #data length 0x00
        cmd.append(0x00)
        #COMMAND_REQUEST_BLOCKS_LEARNED     0x24
        cmd.append(self.COMMAND_REQUEST_BLOCKS_LEARNED)
        #checksum 
        cmd.append(0x34)
        self._write_to_huskyLens(cmd)
        return self._processReturnData()

    def command_request_arrows_learned(self):
        #print("command_request_arrows_learned")
        
        cmd = self.commandHeaderAndAddress[:]
        #data length 0x00
        cmd.append(0x00)
        #COMMAND_REQUEST_ARROWS_LEARNED      0x25
        cmd.append(self.COMMAND_REQUEST_ARROWS_LEARNED)
        #checksum 
        cmd.append(0x35)
        self._write_to_huskyLens(cmd)
        return self._processReturnData()

    def command_request_by_id(self, idVal):
        #print("command_request_by_id")
        
        cmd = self.commandHeaderAndAddress[:]

        #data length 0x00
        cmd.append(0x02)
        #COMMAND_REQUEST_BY_ID       0x26
        cmd.append(self.COMMAND_REQUEST_BY_ID)
        #Data 0x01 0x00
        cmd.append(idVal)
        cmd.append(0x00)
        #checksum 
        cmd.append(self._calculateChecksum(cmd))

        self._write_to_huskyLens(cmd)
        return self._processReturnData()

    def command_request_blocks_by_id(self, idVal):
        #print("command_request_blocks_by_id")
        
        cmd = self.commandHeaderAndAddress[:]

        #data length 0x00
        cmd.append(0x02)
        #COMMAND_REQUEST_BLOCKS_BY_ID        0x27
        cmd.append(self.COMMAND_REQUEST_BY_ID)
        #Data 0x01 0x00
        cmd.append(idVal)
        cmd.append(0x00)
        #checksum 
        cmd.append(self._calculateChecksum(cmd))

        self._write_to_huskyLens(cmd)
        return self._processReturnData()

    def command_request_arrows_by_id(self, idVal):
        #print("command_request_arrows_by_id")
        
        cmd = self.commandHeaderAndAddress[:]

        #data length 0x00
        cmd.append(0x02)
        #COMMAND_REQUEST_ARROWS_BY_ID         0x28
        cmd.append(self.COMMAND_REQUEST_ARROWS_BY_ID )
        #Data 0x01 0x00
        cmd.append(idVal)
        cmd.append(0x00)
        #checksum 
        cmd.append(self._calculateChecksum(cmd))

        self._write_to_huskyLens(cmd)
        return self._processReturnData()



    def command_request_algorthim(self, algorithm):
        #print("command_request_algorthim")
        if algorithm in algorthimsByteID:
            #0x02 Data Length
            #COMMAND_REQUEST_ALGORITHM(0x2D):
            #print("self.commandHeaderAndAddress:", self.commandHeaderAndAddress)
            
            cmd = self.commandHeaderAndAddress[:]
            cmd.append(0x02)
            cmd.append(0x2D)
            cmd.append(algorthimsByteID[algorithm][0])
            #print("algorthimsByteID[algorithm][0]",algorthimsByteID[algorithm][0])
            cmd.append(algorthimsByteID[algorithm][1])
            #print("algorthimsByteID[algorithm][1]",algorthimsByteID[algorithm][1])
            cmd.append(self._calculateChecksum(cmd))
            self._write_to_huskyLens(cmd)
            #process return
        else:
            print("INCORRECT ALGORITHIM NAME")

    def command_request_custom_text(self, text,x,y):
        if x > 319 or x < 0 or y > 239 or y < 0:
           return None
        textLength = len(text)
        dataLength = textLength+4
        cmd = self.commandHeaderAndAddress[:] #[0x55,0xAA,0x11] [85, 170, 17]
        cmd.append(dataLength)#length of data [4+len(msg)=6]
        cmd.append(self.COMMAND_REQUEST_CUSTOM_TEXT)#COMMAND_REQUEST_CUSTOM_TEXT = 0x34,[52]


        data = [0]*(textLength+4)
        data[:4] =[textLength,0,x,y]#first 4 digits, len,cor_x1,cor_x1,cor_y

        if x > 255:
            data[1] = 0xff
            data[2] = x % 256
        
        for index, char in enumerate(text):
            data[index+4] = ord(char)      
        cmd.extend(data)
        cmd.append(self._calculateChecksum(cmd))
        #print("cmd:",cmd)
        self._write_to_huskyLens(cmd)
        
    def command_request_clear_text(self):
        cmd = self.commandHeaderAndAddress[:] #[0x55,0xAA,0x11] [85, 170, 17]
        dataLength = 0
        cmd.append(dataLength)
        cmd.append(self.COMMAND_REQUEST_CLEAR_TEXT)
        cmd.append(self._calculateChecksum(cmd))
        #print("cmd:",cmd)
        self._write_to_huskyLens(cmd)

    def command_request_photo(self):
        cmd = self.commandHeaderAndAddress[:] #[0x55,0xAA,0x11] [85, 170, 17]
        dataLength = 0
        cmd.append(dataLength)
        cmd.append(self.COMMAND_REQUEST_TAKE_PHOTO_TO_SD_CARD)
        cmd.append(self._calculateChecksum(cmd))
        #print("cmd:",cmd)
        self._write_to_huskyLens(cmd)


    def command_request_customnames(self, id, name):
        if id <= 0:
            return None
        nameLength = len(name)
        dataLength = nameLength+3

        cmd = self.commandHeaderAndAddress[:] #[0x55,0xAA,0x11] [85, 170, 17]
        cmd.append(dataLength)#length of data [4+len(msg)=6]
        cmd.append(self.COMMAND_REQUEST_CUSTOMNAMES)

        data = [0]*(nameLength+2)
        data[:2] =[id,nameLength+1]#first 2 digits, id,nameLength+1

        for index, char in enumerate(name):
            data[index+2] = ord(char)      
        cmd.extend(data)
        cmd.append(0)#end of name
        cmd.append(self._calculateChecksum(cmd))
        #print("cmd:",cmd)
        self._write_to_huskyLens(cmd)

    def command_request_screenshot(self):
        cmd = self.commandHeaderAndAddress[:] #[0x55,0xAA,0x11] [85, 170, 17]
        dataLength = 0
        cmd.append(dataLength)
        cmd.append(self.COMMAND_REQUEST_SCREENSHOT_TO_SD_CARD)
        cmd.append(self._calculateChecksum(cmd))
        #print("cmd:",cmd)
        self._write_to_huskyLens(cmd)

    def command_request_learn_once(self,id):
        if id <= 0:
            return None
        cmd = self.commandHeaderAndAddress[:] #[0x55,0xAA,0x11] [85, 170, 17]
        dataLength = 2
        cmd.append(dataLength)
        cmd.append(self.COMMAND_REQUEST_LEARN_ONECE)
        id = [id & 0xff, (id >> 8) & 0xff]
        cmd.append(id[0])
        cmd.append(id[1])
        cmd.append(self._calculateChecksum(cmd))
        #print("cmd:",cmd)
        self._write_to_huskyLens(cmd)

    def command_request_forget(self):
        cmd = self.commandHeaderAndAddress[:] #[0x55,0xAA,0x11] [85, 170, 17]
        dataLength = 0
        cmd.append(dataLength)
        cmd.append(self.COMMAND_REQUEST_FORGET)
        cmd.append(self._calculateChecksum(cmd))
        #print("cmd:",cmd)
        self._write_to_huskyLens(cmd)


    def command_request_save_model_to_SD_card(self, index):
        cmd = self.commandHeaderAndAddress[:] #[0x55,0xAA,0x11] [85, 170, 17]
        dataLength = 2
        cmd.append(dataLength)
        cmd.append(self.COMMAND_REQUEST_SAVE_MODEL_TO_SD_CARD)
        index = [index & 0xff, (index >> 8) & 0xff]
        cmd.append(index[0])
        cmd.append(index[1])
        cmd.append(self._calculateChecksum(cmd))
        #print("cmd:",cmd)
        self._write_to_huskyLens(cmd)

    def command_request_load_model_from_SD_card(self, index):
        cmd = self.commandHeaderAndAddress[:] #[0x55,0xAA,0x11] [85, 170, 17]
        dataLength = 2
        cmd.append(dataLength)
        cmd.append(self.COMMAND_REQUEST_LOAD_MODEL_FROM_SD_CARD)
        index = [index & 0xff, (index >> 8) & 0xff]
        cmd.append(index[0])
        cmd.append(index[1])
        cmd.append(self._calculateChecksum(cmd))
        #print("cmd:",cmd)
        self._write_to_huskyLens(cmd)




# *****************************************************************************************

    def select_blocks_arrows(self, Huskylens_type):      
        if Huskylens_type == "blocks":
            self.command_request_blocks()
        elif Huskylens_type == "arrows":
            self.command_request_arrows()
        else:
            self.command_request_arrows()
    
    def read_learned_id_count(self):
        return self.valid_data[1]

    def is_appear_direct(self, Huskylens_type):
        self.select_blocks_arrows(Huskylens_type)
        return True if self.valid_data[0] else False

    def is_learned(self, id):
        return True if (id <= self.valid_data[1] and id >= 0) else False
  
    def is_appear(self, id, Huskylens_type):
        self.select_blocks_arrows(Huskylens_type)
        id_list = self.valid_data[2][4:len(self.valid_data[2]):5]
        return True if id in id_list else False
        
    def read_count(self, Huskylens_type, id=None):
        self.select_blocks_arrows(Huskylens_type)
        if id is None:
            return int(len(self.valid_data[2])/5)
        id_list = self.valid_data[2][4:len(self.valid_data[2]):5]
        return id_list.count(id)
    
    def read_blocks_arrows_parameter_direct(self, parameter, Huskylens_type, index):
        self.select_blocks_arrows(Huskylens_type)
        if len(self.valid_data[2])==0 or len(self.valid_data[2])/5 < index or index <= 0:
            return -1
        return self.valid_data[2][(index-1)*5:index*5][parameter]
     
    def read_blocks_arrows_parameter(self, id, parameter, Huskylens_type):
        self.select_blocks_arrows(Huskylens_type)
        index = int(len(self.valid_data[2])/5)
        if index == 0:
            return -1
        para_list = []
        for i in range(index):
            para_list.append(self.valid_data[2][i*5:(i+1)*5])
        id_list = []
        for x in para_list:
            if x[4] == id[0]:
                id_list.append(x)
        if len(id_list)==0 or id[1] > len(id_list) or id[1]==0:
            return -1
        return id_list[id[1]-1][parameter]
        
    def read_block_center_parameter_direct(self, parameter, Huskylens_type):
        self.select_blocks_arrows(Huskylens_type)
        index = int(len(self.valid_data[2])/5)
        if index == 0:
            return -1
        para_list = []
        for i in range(index):
            para_list.append(self.valid_data[2][i*5:(i+1)*5])
        distance_min = 2147483647
        for p in para_list:
            if Huskylens_type == "blocks":
                distance = (p[0] - 320 / 2)**2 + (p[1] - 240 / 2)**2
            elif Huskylens_type == "arrows":
                distance = ((p[0]+p[2])/2 - 320 / 2)**2 + ((p[1]+p[3])/2 - 240 / 2)**2
            if distance < distance_min:
                distance_min = distance
                distance_min_index = para_list.index(p)
        return para_list[distance_min_index][parameter]
