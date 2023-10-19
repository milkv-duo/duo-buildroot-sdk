#ifndef __ARCH_CPU__
#define __ARCH_CPU__

#ifdef PXP_PLAT
#define configSYS_CLOCK_HZ              ( 1000000000 )
#elif defined RISCV_QEMU
#define configSYS_CLOCK_HZ              ( 100000000 )
#elif defined FPGA_PLAT
#define configSYS_CLOCK_HZ              ( 25000000 )
#else
#define configSYS_CLOCK_HZ              ( 25000000 )
#endif

#endif // end of __ARCH_CPU__
