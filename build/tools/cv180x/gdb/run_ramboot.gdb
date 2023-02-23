echo Connetct to T-head debug server\n
target remote 172.26.16.1:1025

set architecture riscv:rv64
display/i $pc

echo $pc:
p/x $pc
set $mtvec=0x04400000

# echo Disable Cache\n
set $mhcr=0
set $mhint=0

# invalidate all memory for BTB,BHT,DCACHE,ICACHE
set $mcor=0x30013
# enable ICACHE,DCACHE,BHT,BTB,RAS,WA
set $mhcr=0x7f
# enable data_cache_prefetch, amr
set $mhint=0x610c
# enable fp
set $mstatus=(0x1 << 13)

maint flush dcache
shell sleep 0.5

echo Load fw_payload_uboot.bin\n
restore /home/usera/w/ws/cv-tip/install/soc_cv180x_fpga/fw_payload_uboot.bin binary 0x80000000
# symbol-file /home/usera/w/ws/cv-tip/opensbi/build/platform/generic/firmware/fw_payload.elf
x/16bx 0x80000000

echo Load ramboot.itb\n
restore /home/usera/w/ws/cv-tip/install/soc_cv180x_fpga/ramboot.itb binary 0x90000000
x/16bx 0x90000000

# Set PC to the entry point of OpenSBI
set $pc=0x80000000
echo $pc:
p/x $pc
