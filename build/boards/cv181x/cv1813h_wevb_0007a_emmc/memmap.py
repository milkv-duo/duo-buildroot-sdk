SIZE_1M = 0x100000
SIZE_1K = 1024


# Only attributes in class MemoryMap are generated to .h
class MemoryMap:
    # No prefix "CVIMMAP_" for the items in _no_prefix[]
    _no_prefix = [
        "CONFIG_SYS_TEXT_BASE"  # u-boot's CONFIG_SYS_TEXT_BASE is used without CPP.
    ]

    DRAM_BASE = 0x80000000
    DRAM_SIZE = 512 * SIZE_1M

    # ==============
    # C906L FreeRTOS
    # ==============
    FREERTOS_SIZE = 2 * SIZE_1M
    # FreeRTOS is at the end of DRAM
    FREERTOS_ADDR = DRAM_BASE + DRAM_SIZE - FREERTOS_SIZE
    FSBL_C906L_START_ADDR = FREERTOS_ADDR

    # ==============================
    # OpenSBI | arm-trusted-firmware
    # ==============================
    # Monitor is at the begining of DRAM
    MONITOR_ADDR = DRAM_BASE

    ATF_SIZE = 512 * SIZE_1K
    OPENSBI_SIZE = 512 * SIZE_1K
    OPENSBI_FDT_ADDR = MONITOR_ADDR + OPENSBI_SIZE

    # =========================
    # memory@DRAM_BASE in .dts.
    # =========================
    # Ignore the area of FreeRTOS in u-boot and kernel
    KERNEL_MEMORY_ADDR = DRAM_BASE
    KERNEL_MEMORY_SIZE = DRAM_SIZE - FREERTOS_SIZE

    # =================
    # Multimedia buffer. Used by u-boot/kernel/FreeRTOS
    # =================
    ION_SIZE = 170 * SIZE_1M
    H26X_BITSTREAM_SIZE = 2 * SIZE_1M
    H26X_ENC_BUFF_SIZE = 0
    ISP_MEM_BASE_SIZE = 20 * SIZE_1M
    FREERTOS_RESERVED_ION_SIZE = H26X_BITSTREAM_SIZE + H26X_ENC_BUFF_SIZE + ISP_MEM_BASE_SIZE

    # ION after FreeRTOS
    ION_ADDR = FREERTOS_ADDR - ION_SIZE

    # Buffers of the fast image are inside the ION buffer
    H26X_BITSTREAM_ADDR = ION_ADDR
    H26X_ENC_BUFF_ADDR = H26X_BITSTREAM_ADDR + H26X_BITSTREAM_SIZE
    ISP_MEM_BASE_ADDR = H26X_ENC_BUFF_ADDR + H26X_ENC_BUFF_SIZE

    assert ISP_MEM_BASE_ADDR + ISP_MEM_BASE_SIZE <= ION_ADDR + ION_SIZE

    # Boot logo is after the ION buffer
    # Framebuffer uses boot logo's reserved memory
    BOOTLOGO_SIZE = 1800 * SIZE_1K
    BOOTLOGO_ADDR = ION_ADDR - BOOTLOGO_SIZE
    FRAMEBUFFER_SIZE = BOOTLOGO_SIZE
    FRAMEBUFFER_ADDR = BOOTLOGO_ADDR

    # ===================
    # FSBL and u-boot-2021
    # ===================
    CVI_UPDATE_HEADER_SIZE = SIZE_1K
    UIMAG_SIZE = 16 * SIZE_1M

    # kernel image loading buffer
    UIMAG_ADDR = DRAM_BASE + 24 * SIZE_1M
    CVI_UPDATE_HEADER_ADDR = UIMAG_ADDR - CVI_UPDATE_HEADER_SIZE

    # FSBL decompress buffer
    FSBL_UNZIP_ADDR = UIMAG_ADDR
    FSBL_UNZIP_SIZE = UIMAG_SIZE

    assert UIMAG_ADDR + UIMAG_SIZE <= BOOTLOGO_ADDR

    # u-boot's run address and entry point
    CONFIG_SYS_TEXT_BASE = DRAM_BASE + 2 * SIZE_1M
    # u-boot's init stack point is only used before board_init_f()
    CONFIG_SYS_INIT_SP_ADDR = UIMAG_ADDR + UIMAG_SIZE
