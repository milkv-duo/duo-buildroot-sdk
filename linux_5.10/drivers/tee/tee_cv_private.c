#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/buffer_head.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/ioctl.h>
#include <linux/kdev_t.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sizes.h>
#include <linux/slab.h>
#include <linux/tee_drv.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/arm-smccc.h>
#include <asm/cacheflush.h>
#include <linux/kernel_read_file.h>
#include <linux/uaccess.h>
#include <linux/clk.h>

#include "tee_cv_private.h"

#define TEE_CV_DEBUG 0

#ifdef CONFIG_ARM
#define FLUSH_DCACHE __cpuc_flush_dcache_area
#else
#define FLUSH_DCACHE __flush_dcache_area
#endif
static struct clk *clk_efuse;

static char aimodel_keyfile_path[PAGE_SIZE] = "/mnt/data/aimodel.keys";
static char debug_info[PAGE_SIZE] = "(nothing)";

#if TEE_CV_DEBUG
/*
 * /memreserve/	0x0000000138000000 0x0000000008000000; // TEE buffer 896MB ~ 1024MB (128MB)
 */
static phys_addr_t large_buf_phys = 0x0000000138000000;
static const size_t large_buf_maxsize = 0x04000000;
static size_t large_buf_size;
static void *large_buf;
#endif

static int64_t plat_efuse_read_safe(uint32_t addr);
static int plat_efuse_write_safe(uint32_t addr, uint32_t value);

static inline void mmio_write_32(void __iomem *addr, uint32_t value)
{
	iowrite32(value, addr);
}

static inline void mmio_setbits_32(void __iomem *addr, uint32_t set)
{
	mmio_write_32(addr, ioread32(addr) | set);
}

static void do_smccc_smc(unsigned long a0, unsigned long a1, unsigned long a2,
			 unsigned long a3, unsigned long a4, unsigned long a5,
			 unsigned long a6, unsigned long a7,
			 struct arm_smccc_res *res)
{
	int err;

	err = clk_prepare_enable(clk_efuse);
	if (err) {
		pr_err("clk_efuse: clock failed to prepare+enable: %d\n", err);
		return;
	}
	pr_info("do_smccc_smc: a=0x%lx 0x%lx 0x%lx\n", a0, a1, a2);
	arm_smccc_smc(a0, a1, a2, a3, a4, a5, a6, a7, res);
	pr_info("do_smccc_smc: r=0x%lx 0x%lx\n", res->a0, res->a1);
	clk_disable_unprepare(clk_efuse);
}

int tee_cv_aimodel_keyfile_send(const char *path)
{
	int err = 0;
	struct arm_smccc_res res = { 0 };

	void *key_buf = 0;
	void *key_cont = 0;
	phys_addr_t key_cont_phy = 0;
	size_t key_size = 0;

	pr_info("aimodel_keyfile_path=%s\n", path);
	err = kernel_read_file_from_path(aimodel_keyfile_path, 0, &key_buf,
					  TEE_TPU_MAX_KEYFILE_SIZE, 
					  &key_size, READING_FIRMWARE);
	if (err < 0)
		return err;

	// Copy to contiguous physical memory
	key_cont = kzalloc(key_size, GFP_KERNEL);
	if (!key_cont) {
		vfree(key_buf);
		return -ENOMEM;
	}

	memcpy(key_cont, key_buf, key_size);
	vfree(key_buf);

	key_cont_phy = virt_to_phys(key_cont);
	FLUSH_DCACHE(key_cont, key_size);

	pr_info("key_size=%zu %pa |%16ph|\n", key_size, &key_cont_phy,
		key_cont);
	do_smccc_smc(OPTEE_SMC_CALL_CV_TPU_SEND_KEYS, key_cont_phy, key_size, 0,
		     0, 0, 0, 0, &res);
	kfree(key_cont);

	return res.a0;
}
EXPORT_SYMBOL(tee_cv_aimodel_keyfile_send);

/*
 * Flush cmd_phys and weight_phys before calling
 */
int tee_cv_aimodel_load(phys_addr_t cmd_phys, size_t cmd_size,
			phys_addr_t weight_phys, size_t weight_size,
			phys_addr_t neuron_phys, bool decrypt_cmd,
			bool decrypt_weight, struct arm_smccc_res *res)
{
	uint32_t flag = 0;

	pr_info("%s(): cmd_phys=0x%llx cmd_size=%zu weight_phys=0x%llx weight_size=%zu neuron_phys=0x%llx\n",
		__func__, cmd_phys, cmd_size, weight_phys, weight_size,
		neuron_phys);

	if (decrypt_cmd)
		flag |= CV_DECRYPT_CMD;

	if (decrypt_weight)
		flag |= CV_DECRYPT_WEIGHT;

	tee_cv_aimodel_keyfile_send(aimodel_keyfile_path);
	do_smccc_smc(OPTEE_SMC_CALL_CV_TPU_LOAD_MODEL, cmd_phys, cmd_size,
		     weight_phys, weight_size, neuron_phys, flag, 0, res);

	return res->a0;
}
EXPORT_SYMBOL(tee_cv_aimodel_load);

int tee_cv_aimodel_run(phys_addr_t dmabuf_phys, phys_addr_t gaddr_base2,
		       phys_addr_t gaddr_base3, phys_addr_t gaddr_base4,
		       phys_addr_t gaddr_base5, phys_addr_t gaddr_base6,
		       phys_addr_t gaddr_base7, struct arm_smccc_res *res)
{
	pr_info("%s():\n", __func__);
	do_smccc_smc(OPTEE_SMC_CALL_CV_TPU_RUN, dmabuf_phys, gaddr_base2,
		     gaddr_base3, gaddr_base4, gaddr_base5, gaddr_base6, gaddr_base7, res);

	return res->a0;
}
EXPORT_SYMBOL(tee_cv_aimodel_run);

int tee_cv_aimodel_wait(uint64_t arg, struct arm_smccc_res *res)
{
	pr_info("%s():\n", __func__);
	do_smccc_smc(OPTEE_SMC_CALL_CV_TPU_WAIT, arg, 0, 0, 0, 0, 0, 0, res);

	return res->a0;
}
EXPORT_SYMBOL(tee_cv_aimodel_wait);

int tee_cv_aimodel_set_prot_range(phys_addr_t addr, uint32_t size, struct arm_smccc_res *res)
{
	pr_info("%s(): addr=0x%llx size=0x%x\n", __func__, addr, size);

	do_smccc_smc(OPTEE_SMC_CALL_CV_TPU_SET_PROT_RANGE, addr, size, 0, 0, 0, 0, 0, res);

	return res->a0;
}
EXPORT_SYMBOL(tee_cv_aimodel_set_prot_range);

int tee_cv_aimodel_clear_prot_range(phys_addr_t addr, uint32_t size, struct arm_smccc_res *res)
{
	pr_info("%s(): addr=0x%llx size=0x%x\n", __func__, addr, size);

	do_smccc_smc(OPTEE_SMC_CALL_CV_TPU_CLEAR_PROT_RANGE, addr, size, 0, 0, 0, 0, 0, res);

	return res->a0;
}
EXPORT_SYMBOL(tee_cv_aimodel_clear_prot_range);

static ssize_t aimodel_keyfile_path_show(struct device *dev,
					 struct device_attribute *attr,
					 char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%s\n", aimodel_keyfile_path);
}

static ssize_t aimodel_keyfile_path_store(struct device *dev,
					  struct device_attribute *attr,
					  const char *buf, size_t count)
{
	char *p = 0;

	strlcpy(aimodel_keyfile_path, buf, PAGE_SIZE - 1);
	p = strpbrk(aimodel_keyfile_path, "\n\r");
	if (p)
		*p = '\0';

	tee_cv_aimodel_keyfile_send(aimodel_keyfile_path);

	return count;
}

#if TEE_CV_DEBUG
static int read_file(char *path, void *buf, size_t *size)
{
	int err = 0;
	void *vbuf;
	loff_t read_size = 0;

	err = kernel_read_file_from_path(path, &vbuf, &read_size,
					 large_buf_size, READING_FIRMWARE);
	if (err < 0)
		return err;

	if (read_size <= *size)
		*size = read_size;

	memcpy(buf, vbuf, *size);
	vfree(vbuf);

	return err;
}

static void tpu_load_debug(void)
{
	void *cmd, *weight;
	size_t cmd_size, weight_size;
	struct arm_smccc_res res = { 0 };

	pr_info("tpu_load_debug:\n");

	large_buf = memremap(large_buf_phys, large_buf_maxsize, MEMREMAP_WB);
	if (!large_buf) {
		pr_err("[%s] memremap fail.\n", __func__);
		return;
	}
	large_buf_size = large_buf_maxsize;

	cmd = large_buf;
	cmd_size = large_buf_size;
	read_file("/mnt/data/cmd.bin.enc", cmd, &cmd_size);
	pr_info("cmd=%p(%lu) |%32ph|\n", cmd, cmd_size, cmd);

	large_buf += roundup(cmd_size, 16);
	large_buf_size -= roundup(cmd_size, 16);

	weight = large_buf;
	weight_size = large_buf_size;
	read_file("/mnt/data/weight.bin.enc", weight, &weight_size);
	pr_info("weight=%p(%lu) |%32ph|\n", weight, weight_size, weight);

	FLUSH_DCACHE(cmd, cmd_size);
	FLUSH_DCACHE(weight, weight_size);

	tee_cv_aimodel_load(virt_to_phys(cmd), cmd_size, virt_to_phys(weight),
			    weight_size, virt_to_phys(weight), &res);
}

static void tpu_set_prot_debug(void)
{
	phys_addr_t cmd, weight;
	size_t cmd_size, weight_size;
	struct arm_smccc_res res = { 0 };

	pr_info("%s()\n", __func__);

	cmd = large_buf_phys;
	cmd_size = 0x1000;
	weight = cmd + roundup(cmd_size, 16);
	weight_size = 0x1400000;

	tee_cv_aimodel_set_prot_range(weight, weight_size, &res);
}

static void tpu_clear_prot_debug(void)
{
	phys_addr_t cmd, weight;
	size_t cmd_size, weight_size;
	struct arm_smccc_res res = { 0 };

	pr_info("%s()\n", __func__);

	cmd = large_buf_phys;
	cmd_size = 0x1000;
	weight = cmd + roundup(cmd_size, 16);
	weight_size = 0x2000;

	tee_cv_aimodel_clear_prot_range(weight, weight_size, &res);
}

#else /* TEE_CV_DEBUG */
static void tpu_load_debug(void)
{
}

static void tpu_clear_prot_debug(void)
{
}

static void tpu_set_prot_debug(void)
{
}
#endif /* TEE_CV_DEBUG */

static ssize_t cv_debug_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%s\n", debug_info);
}

static ssize_t cv_debug_store(struct device *dev, struct device_attribute *attr,
			      const char *buf, size_t count)
{
	char *p = 0;
	struct arm_smccc_res res = { 0 };
	phys_addr_t debug_info_phy = 0;

	memset(debug_info, 0, sizeof(debug_info));
	memcpy(debug_info, buf, count);
	debug_info[sizeof(debug_info) - 1] = '\0';
	p = strpbrk(debug_info, "\n\r");
	if (p)
		*p = '\0';

	pr_info("cv_debug_info=0x%09llx(%zu) |%16ph|\n", debug_info_phy,
		sizeof(debug_info), debug_info);

	if (strncmp("tpu_load", debug_info, sizeof(debug_info)) == 0) {
		tpu_load_debug();
	} else if (strncmp("tpu_set_prot", debug_info, sizeof(debug_info)) == 0) {
		tpu_set_prot_debug();
	} else if (strncmp("tpu_clear_prot", debug_info, sizeof(debug_info)) == 0) {
		tpu_clear_prot_debug();
	} else if (strncmp("console_on", debug_info, sizeof(debug_info)) == 0) {
		pr_info("Enable BL32 console output\n");
		do_smccc_smc(OPTEE_SMC_CALL_CV_DEBUG, 0x7F000001, 2, 0, 0, 0, 0, 0, &res);
	} else {
		debug_info_phy = virt_to_phys(debug_info);
		FLUSH_DCACHE(debug_info, sizeof(debug_info));

		do_smccc_smc(OPTEE_SMC_CALL_CV_DEBUG, 0x1F000000,
			     debug_info_phy, sizeof(debug_info), 0, 0, 0, 0,
			     &res);
	}

	return count;
}

/*
 * /sys/devices/virtual/tee/tee0/aimodel_keyfile_path
 * /sys/devices/virtual/tee/tee0/cv_debug
 */
DEVICE_ATTR_RW(aimodel_keyfile_path);
DEVICE_ATTR_RW(cv_debug);

#define EFUSE_BASE (0x03000000 + 0x00050000)
#define EFUSE_REG_BANK_SIZE 0x1000
#define EFUSE_SIZE 0x100

#define EFUSE_SHADOW_REG (efuse_base + 0x100)
#define EFUSE_MODE (efuse_base + 0x0)
#define EFUSE_ADR (efuse_base + 0x4)
#define EFUSE_DIR_CMD (efuse_base + 0x8)
#define EFUSE_RD_DATA (efuse_base + 0xC)
#define EFUSE_STATUS (efuse_base + 0x10)
#define EFUSE_ONE_WAY (efuse_base + 0x14)

#define BIT_AREAD (1 << 0)
#define BIT_MREAD (1 << 1)
#define BIT_PRG (1 << 2)
#define BIT_PWR_DN (1 << 3)
#define BIT_CMD (1 << 4)
#define BIT_BUSY (1 << 0)
#define CMD_REFRESH (0x30)

enum EFUSE_READ_TYPE { AREAD, MREAD };

static void __iomem *efuse_base;

int tee_cv_init(void)
{
	pr_info("%s\n", __func__);

	clk_efuse = clk_get_sys(NULL, "clk_efuse");
	if (IS_ERR(clk_efuse)) {
		pr_err("clk_efuse: clock not found %ld\n", PTR_ERR(clk_efuse));
		return PTR_ERR(clk_efuse);
	}

	efuse_base = ioremap(EFUSE_BASE, EFUSE_REG_BANK_SIZE);

	return 0;
}

void tee_cv_exit(void)
{
	iounmap(efuse_base);
}

int tee_cv_efuse_read(uint32_t addr, size_t size, void *buf)
{
	long ret = -1;
	struct arm_smccc_res res = { 0 };

	void *efuse_buf;
	phys_addr_t efuse_buf_phys;

	size_t i;
	uint32_t v;

	if (addr % 4 || size % 4 || !size)
		return -EFAULT;

	efuse_buf = kzalloc(size, GFP_KERNEL);
	if (!efuse_buf)
		return -ENOMEM;

	FLUSH_DCACHE(efuse_buf, size);
	efuse_buf_phys = virt_to_phys(efuse_buf);

	pr_debug("%s(): 0x%x(%zu) to %p(0x%llx)\n", __func__, addr, size,
		 efuse_buf, efuse_buf_phys);
	do_smccc_smc(OPTEE_SMC_CALL_CV_EFUSE_READ, addr, size, efuse_buf_phys,
		     0, 0, 0, 0, &res);
	ret = (typeof(ret))res.a0;
	pr_debug("%s(): ret=0x%lx\n", __func__, ret);

	if (ret >= 0)
		memcpy(buf, efuse_buf, min((typeof(res.a0))size, res.a0));

	kfree(efuse_buf);

	if (ret < 0) {
		for (i = 0; i < size && (addr + i) < EFUSE_SIZE; i += 4) {
			v = plat_efuse_read_safe(addr + i);
			memcpy(buf + i, &v, sizeof(v));
		}

		return i;
	}

	return ret;
}
EXPORT_SYMBOL(tee_cv_efuse_read);

int tee_cv_efuse_write(uint32_t addr, uint32_t value)
{
	long ret = -1;
	struct arm_smccc_res res = { 0 };

	pr_debug("%s(): 0x%4x=0x%08x\n", __func__, addr, value);
	do_smccc_smc(OPTEE_SMC_CALL_CV_EFUSE_WRITE, addr, value, 0, 0, 0, 0, 0,
		     &res);

	ret = (typeof(ret))res.a0;
	if (ret < 0)
		return plat_efuse_write_safe(addr, value);

	return ret;
}
EXPORT_SYMBOL(tee_cv_efuse_write);

static int64_t plat_efuse_read_safe(uint32_t addr)
{
	int err;
	uint32_t v;

	if (addr >= EFUSE_SIZE)
		return -EFAULT;

	if (addr % 4 != 0)
		return -EFAULT;

	err = clk_prepare_enable(clk_efuse);
	if (err) {
		pr_err("clk_efuse: clock failed to prepare+enable: %d\n", err);
		return -EIO;
	}

	v = ioread32(EFUSE_SHADOW_REG + addr);
	clk_disable_unprepare(clk_efuse);
	return v;
}

static void efuse_wait_for_ready(void)
{
	while (ioread32(EFUSE_STATUS) & BIT_BUSY)
		;
}

static void efuse_power_on(uint32_t on)
{
	if (on)
		mmio_setbits_32(EFUSE_MODE, BIT_CMD);
	else
		mmio_setbits_32(EFUSE_MODE, BIT_PWR_DN | BIT_CMD);
}

static void efuse_refresh(void)
{
	mmio_write_32(EFUSE_MODE, CMD_REFRESH);
}

static void efuse_prog_bit(uint32_t word_addr, uint32_t bit_addr,
			   uint32_t high_row)
{
	uint32_t phy_addr;

	// word_addr: virtual addr, take "lower 6-bits" from 7-bits (0-127)
	// bit_addr: virtual addr, 5-bits (0-31)

	// composite physical addr[11:0] = [11:7]bit_addr + [6:0]word_addr
	phy_addr =
		((bit_addr & 0x1F) << 7) | ((word_addr & 0x3F) << 1) | high_row;

	efuse_wait_for_ready();

	// send efuse program cmd
	mmio_write_32(EFUSE_ADR, phy_addr);
	mmio_write_32(EFUSE_MODE, BIT_PRG | BIT_CMD);
}

static uint32_t efuse_read_word(uint32_t phy_word_addr,
				enum EFUSE_READ_TYPE type)
{
	// power on efuse macro
	efuse_power_on(1);

	efuse_wait_for_ready();

	mmio_write_32(EFUSE_ADR, phy_word_addr);

	if (type == AREAD) // array read
		mmio_write_32(EFUSE_MODE, BIT_AREAD | BIT_CMD);
	else if (type == MREAD) // margin read
		mmio_write_32(EFUSE_MODE, BIT_MREAD | BIT_CMD);
	else
		pr_err("EFUSE: Unsupported read type!");

	efuse_wait_for_ready();
	return ioread32(EFUSE_RD_DATA);
}

static int plat_bm_efuse_write(uint32_t vir_word_addr, uint32_t val)
{
	uint32_t i, j, row_val, zero_bit;
	uint32_t new_value;
	int err_cnt = 0;

	for (j = 0; j < 2; j++) {
		pr_debug("EFUSE: Program physical word addr #%d\n",
			 (vir_word_addr << 1) | j);

		// array read by word address
		row_val = efuse_read_word((vir_word_addr << 1) | j,
					  AREAD); // read low word of word_addr
		zero_bit = val & (~row_val); // only program zero bit

		// program row which bit is zero
		for (i = 0; i < 32; i++) {
			if ((zero_bit >> i) & 1)
				efuse_prog_bit(vir_word_addr, i, j);
		}

		// check by margin read
		new_value = efuse_read_word((vir_word_addr << 1) | j, MREAD);
		pr_debug("%s(): val=0x%x new_value=0x%x\n", __func__, val,
			 new_value);
		if ((val & new_value) != val) {
			err_cnt += 1;
			pr_err("EFUSE: Program bits check failed (%d)!\n",
			       err_cnt);
		}
	}

	efuse_refresh();

	return err_cnt >= 2 ? -EIO : 0;
}

static void plat_bm_efuse_init(void)
{
	// power on efuse macro
	efuse_power_on(1);

	// send refresh cmd to reload all eFuse values to shadow registers
	efuse_refresh();

	// efuse macro will be auto powered off after refresh cmd, so don't
	// need to turn it off manually
}

static int plat_efuse_write_safe(uint32_t addr, uint32_t value)
{
	int ret;

	pr_debug("%s(): 0x%x = 0x%x\n", __func__, addr, value);

	if (addr >= EFUSE_SIZE)
		return -EFAULT;

	if (addr % 4 != 0)
		return -EFAULT;

	ret = clk_prepare_enable(clk_efuse);
	if (ret) {
		pr_err("clk_efuse: clock failed to prepare+enable: %d\n", ret);
		return -EIO;
	}

	ret = plat_bm_efuse_write(addr / 4, value);
	pr_debug("%s(): ret=%d\n", __func__, ret);
	plat_bm_efuse_init();
	efuse_wait_for_ready();

	clk_disable_unprepare(clk_efuse);

	return ret;
}
