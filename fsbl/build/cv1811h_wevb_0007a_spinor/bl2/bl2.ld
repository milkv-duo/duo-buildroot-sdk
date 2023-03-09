OUTPUT_FORMAT("elf64-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(bl2_entrypoint)
MEMORY {
    RAM (rwx): ORIGIN = (0x0C000000), LENGTH = (0x37000)
}
SECTIONS
{
    . = (0x0C000000);
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
    ASSERT(. <= ((0x0C000000) + (0x37000)), "BL2 image has exceeded its limit.")
p_rom_api_cryptodma_aes_decrypt = 0x0000000004418100;
p_rom_api_flash_init = 0x0000000004418080;
p_rom_api_get_boot_src = 0x0000000004418020;
p_rom_api_get_number_of_retries = 0x00000000044180c0;
p_rom_api_image_crc = 0x00000000044180a0;
p_rom_api_load_image = 0x0000000004418060;
p_rom_api_set_boot_src = 0x0000000004418040;
p_rom_api_verify_rsa = 0x00000000044180e0;
}
