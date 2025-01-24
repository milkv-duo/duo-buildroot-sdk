#include <debug.h>
#include <string.h>
#include <platform.h>
#include <mmio.h>
#include <utils.h>
#include <errno.h>
#include <delay_timer.h>
#include <security/efuse.h>

#include <security/security.h>
#include <security/cryptodma.h>

#include <bigdigits.h>
#include <tomcrypt.h>

static DIGIT_T rsa_e[RSA_N_BYTES / sizeof(DIGIT_T)] = { 0x10001 };
static DIGIT_T current_pk[RSA_N_BYTES / sizeof(DIGIT_T)];
static DIGIT_T current_sig[RSA_N_BYTES / sizeof(DIGIT_T)];
static DIGIT_T decrypted_sig[RSA_N_BYTES / sizeof(DIGIT_T)];
static uint8_t current_digest[SHA256_SIZE];

int cryptodma_aes_decrypt(const void *plain, const void *encrypted, uint64_t len, uint8_t *key, uint8_t *iv)
{
	__aligned(32) uint32_t dma_descriptor[22] = { 0 };

	uint32_t status;
	uint32_t ts;

	uint64_t dest = phys_to_dma((uintptr_t)plain);
	uint64_t src = phys_to_dma((uintptr_t)encrypted);

	INFO("AES/0x%lx/0x%lx/0x%lx\n", src, dest, len);

	// Prepare descriptor
	dma_descriptor[CRYPTODMA_CTRL] = DES_USE_DESCRIPTOR_IV | DES_USE_DESCRIPTOR_KEY | DES_USE_AES | 0xF;
	dma_descriptor[CRYPTODMA_CIPHER] = AES_KEY_MODE << 3 | CBC_ENABLE << 1 | DECRYPT_ENABLE;

	dma_descriptor[CRYPTODMA_SRC_ADDR_L] = (uint32_t)(src & 0xFFFFFFFF);
	dma_descriptor[CRYPTODMA_SRC_ADDR_H] = (uint32_t)(src >> 32);

	dma_descriptor[CRYPTODMA_DST_ADDR_L] = (uint32_t)(dest & 0xFFFFFFFF);
	dma_descriptor[CRYPTODMA_DST_ADDR_H] = (uint32_t)(dest >> 32);

	dma_descriptor[CRYPTODMA_DATA_AMOUNT_L] = (uint32_t)(len & 0xFFFFFFFF);
	dma_descriptor[CRYPTODMA_DATA_AMOUNT_H] = (uint32_t)(len >> 32);

	memcpy(&dma_descriptor[CRYPTODMA_KEY], key, 16);
	memcpy(&dma_descriptor[CRYPTODMA_IV], iv, 16);

	// Set cryptodma control
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_INT_MASK, 0x3);
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DES_BASE_L,
		      (uint32_t)(phys_to_dma((uintptr_t)dma_descriptor) & 0xFFFFFFFF));
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DES_BASE_H,
		      (uint32_t)(phys_to_dma((uintptr_t)dma_descriptor) >> 32));

	status = mmio_read_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DMA_CTRL);

	flush_dcache_range((unsigned long)dma_descriptor, sizeof(dma_descriptor));
	flush_dcache_range((uintptr_t)plain, len);
	flush_dcache_range((uintptr_t)encrypted, len);

	// Clear interrupt
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_WR_INT, 0x3);
	// Trigger cryptodma engine
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DMA_CTRL,
		      DMA_WRITE_MAX_BURST << 24 | DMA_READ_MAX_BURST << 16 | DMA_DESCRIPTOR_MODE << 1 | DMA_ENABLE);

	ts = get_timer(0);
	do {
		status = mmio_read_32(SEC_CRYPTODMA_BASE + CRYPTODMA_WR_INT);
		INFO("INT status 0x%x\n", status);
		if (get_timer(ts) >= 1000) {
			ERROR("Decryption timeout\n");
			return -EIO;
		}
	} while (status == 0);

	return 0;
}

int verify_rsa(const void *message, size_t n, const void *sig, void *rsa_n, size_t rsa_nbytes)
{
	hash_state md;

	NOTICE("VI/0x%lx/0x%lx\n", (uintptr_t)message, n);

	mpConvFromOctets(current_pk, rsa_nbytes / sizeof(DIGIT_T), rsa_n, rsa_nbytes);
	mpConvFromOctets(current_sig, rsa_nbytes / sizeof(DIGIT_T), sig, rsa_nbytes);
	/** Computes y = x^e mod m */
	mpModExp(decrypted_sig, current_sig, rsa_e, current_pk, rsa_nbytes / sizeof(DIGIT_T));

	sha256_init(&md);
	sha256_process(&md, message, n);
	sha256_done(&md, current_digest);

	bytes_reverse(current_digest, sizeof(current_digest));

	if (memcmp(current_digest, decrypted_sig, SHA256_SIZE))
		return -EFAULT;

	return 0;
}

/*
 * image (size) = body (size - RSA_N_BYTES) + sig (RSA_N_BYTES)
 */
int dec_verify_image(const void *image, size_t size, size_t dec_skip, struct fip_param1 *fip_param1)
{
	const void *sig = image + size - RSA_N_BYTES;
	uint8_t iv[AES128_SIZE] = { 0 };

	if (size <= RSA_N_BYTES) {
		ERROR("image size <= signature size\n");
		return -EFAULT;
	}

	if (!security_is_tee_enabled())
		return 0;

	if (security_is_tee_encrypted()) {
		NOTICE("DI/0x%lx/0x%lx\n", (uintptr_t)image, size);
		cryptodma_aes_decrypt(image + dec_skip, image + dec_skip, size - dec_skip, fip_param1->bl_ek, iv);
	}

	size -= RSA_N_BYTES;

	return verify_rsa(image, size, sig, fip_param1->bl_pk, RSA_N_BYTES);
}

int efuse_wait_idle(void)
{
	uint32_t status;
	uint32_t time_count = 0;
	int ret = 0;

	do {
		status = mmio_read_32(EFUSE_BASE + EFUSE_STATUS);
		time_count++;

		if (time_count > 0x1000) {
			NOTICE("wait idle timeout\n");
			return -1;
		}
	} while ((status & 0x1) != 0);

	return ret;
}

int efuse_power_on(void)
{
	int ret = 0;

	if (efuse_wait_idle() != 0) {
		return -1;
	}

	mmio_write_32(EFUSE_BASE + EFUSE_MODE, 0x10);

	return ret;
}

int efuse_program_bit(uint32_t addr, const uint32_t bit)
{
	uint16_t w_addr = (bit << 7) | ((addr & 0x3F) << 1);

	if (efuse_wait_idle() != 0) {
		return -1;
	}

	mmio_write_32(EFUSE_BASE + EFUSE_ADR, (w_addr & 0xFFF));
	mmio_write_32(EFUSE_BASE + EFUSE_MODE, 0x14);

	if (efuse_wait_idle() != 0) {
		return -1;
	}

	w_addr |= 0x1;
	mmio_write_32(EFUSE_BASE + EFUSE_ADR, (w_addr & 0xFFF));
	mmio_write_32(EFUSE_BASE + EFUSE_MODE, 0x14);

	return 0;
}

int efuse_refresh_shadow(void)
{
	int ret = 0;

	if (efuse_wait_idle() != 0) {
		return -1;
	}

	mmio_write_32(EFUSE_BASE + EFUSE_MODE, 0x30);

	if (efuse_wait_idle() != 0) {
		return -1;
	}

	return ret;
}

int efuse_power_off(void)
{
	int ret = 0;

	if (efuse_wait_idle() != 0) {
		return -1;
	}

	mmio_write_32(EFUSE_BASE + EFUSE_MODE, 0x18);

	return ret;
}
