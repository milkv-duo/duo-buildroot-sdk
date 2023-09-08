import time
from pinpong.board import Board,Pin

ip = "192.168.0.90" #插USB，运行get_ip_port.py文件获取到的ip，port
port = 8081

ip1 = "192.168.1.122"#插USB，运行get_ip_port.py文件获取到的ip，port
port1 = 8081

tcp = Board(ip, port)

tcp1 = Board(ip1, port1)

led = Pin(tcp1, Pin.D7, Pin.OUT)#"192.168.1.122"地址的板子
btn = Pin(tcp, Pin.D8, Pin.IN)#"192.168.0.90"地址的板子

while True:
  v = btn.value()  #读取地址为"192.168.0.90"的引脚电平  
  print(v)  #终端.打印读取的电平状态
  led.value(v)  #将按钮状态设置给"192.168.1.122"地址的led灯引脚
  time.sleep(0.1)
  