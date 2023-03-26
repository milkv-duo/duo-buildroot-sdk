#ifndef __ROM_SPI_H__
#define __ROM_SPI_H__

enum rsa_size {
	RSA_2048_BITS,
	RSA_4096_BITS,
};

// called from BL2
int p_rom_api_load_image(void *buf, uint32_t offset, size_t image_size, int retry_num);
uint32_t p_rom_api_image_crc(const void *buf, int len);
int p_rom_api_flash_init(void);

enum boot_src p_rom_api_get_boot_src(void);
void p_rom_api_set_boot_src(enum boot_src src);
int p_rom_api_get_number_of_retries(void);

int p_rom_api_verify_rsa(void *message, size_t n, void *sig, enum rsa_size rsa_size);
int p_rom_api_cryptodma_aes_decrypt(void *plain, const void *encrypted, uint64_t len, uint8_t *key, uint8_t *iv);

#endif /* __ROM_SPI_H__ */
