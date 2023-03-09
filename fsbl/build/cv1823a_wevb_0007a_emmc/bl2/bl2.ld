OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(bl2_entrypoint)
MEMORY {
    RAM (rwx): ORIGIN = (0x40100000), LENGTH = (0x37000)
}
SECTIONS
{
    . = (0x40100000);
    ASSERT(. == ALIGN(4096),
           "BL2_BASE address is not aligned on a page boundary.")
    ro . : {
        __RO_START__ = .;
        *bl2_entrypoint.o(.text*)
        *(.vectors)
        *(.text*)
        *(.rodata*)
        __RO_END__ = .;
    } >RAM
    .data . : {
        . = ALIGN(16);
        __DATA_START__ = .;
        *(.data*)
        . = ALIGN(16);
        __DATA_END__ = .;
    } >RAM
    stacks (NOLOAD) : {
        . = ALIGN(64);
        __STACKS_START__ = .;
        . += 0x2000;
        . = ALIGN(64);
        __STACKS_END__ = .;
    } >RAM
    .bss : ALIGN(16) {
        . = ALIGN(16);
        __BSS_START__ = .;
        *(SORT_BY_ALIGNMENT(.bss*))
        *(COMMON)
        . = ALIGN(16);
        __BSS_END__ = .;
    } >RAM
    __BL2_END__ = .;
    __BSS_SIZE__ = SIZEOF(.bss);
    ASSERT(. <= ((0x40100000) + (0x37000)), "BL2 image has exceeded its limit.")
p_rom_api_cryptodma_aes_decrypt = 0x0000000040000100;
p_rom_api_flash_init = 0x0000000040000080;
p_rom_api_get_boot_src = 0x0000000040000020;
p_rom_api_get_number_of_retries = 0x00000000400000c0;
p_rom_api_image_crc = 0x00000000400000a0;
p_rom_api_load_image = 0x0000000040000060;
p_rom_api_set_boot_src = 0x0000000040000040;
p_rom_api_verify_rsa = 0x00000000400000e0;
}
