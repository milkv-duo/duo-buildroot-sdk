
#ifndef TEE_CV_PRIVATE_H
#define TEE_CV_PRIVATE_H

#include <linux/arm-smccc.h>

#define TEE_TPU_MAX_KEYFILE_SIZE 0x100000

extern struct device_attribute dev_attr_aimodel_keyfile_path;
extern struct device_attribute dev_attr_cv_debug;

enum {
	CV_DECRYPT_CMD = 1 << 0,
	CV_DECRYPT_WEIGHT = 1 << 1
};

/*
 * Cvitek's private SMC call
 */
#define OPTEE_SMC_FUNID_CV_DEBUG 0x01

#define OPTEE_SMC_FUNID_CV_TPU_SEND_KEYS 0x02
#define OPTEE_SMC_FUNID_CV_TPU_LOAD_MODEL 0x03
#define OPTEE_SMC_FUNID_CV_TPU_RUN 0x04
#define OPTEE_SMC_FUNID_CV_TPU_WAIT 0x05
#define OPTEE_SMC_FUNID_CV_TPU_SET_PROT_RANGE 0x08
#define OPTEE_SMC_FUNID_CV_TPU_CLEAR_PROT_RANGE 0x09

#define OPTEE_SMC_FUNID_CV_EFUSE_READ 0x06
#define OPTEE_SMC_FUNID_CV_EFUSE_WRITE 0x07

#define OPTEE_SMC_CALL_CV_DEBUG                                                \
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_STD_CALL, ARM_SMCCC_SMC_32,               \
			   ARM_SMCCC_OWNER_OEM, OPTEE_SMC_FUNID_CV_DEBUG)

#define OPTEE_SMC_CALL_CV_TPU_SEND_KEYS                                        \
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_STD_CALL, ARM_SMCCC_SMC_32,               \
			   ARM_SMCCC_OWNER_OEM,                                \
			   OPTEE_SMC_FUNID_CV_TPU_SEND_KEYS)

#define OPTEE_SMC_CALL_CV_TPU_LOAD_MODEL                                       \
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_STD_CALL, ARM_SMCCC_SMC_32,               \
			   ARM_SMCCC_OWNER_OEM,                                \
			   OPTEE_SMC_FUNID_CV_TPU_LOAD_MODEL)

#define OPTEE_SMC_CALL_CV_TPU_RUN                                              \
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_STD_CALL, ARM_SMCCC_SMC_32,               \
			   ARM_SMCCC_OWNER_OEM, OPTEE_SMC_FUNID_CV_TPU_RUN)

#define OPTEE_SMC_CALL_CV_TPU_WAIT                                             \
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_STD_CALL, ARM_SMCCC_SMC_32,               \
			   ARM_SMCCC_OWNER_OEM, OPTEE_SMC_FUNID_CV_TPU_WAIT)

#define OPTEE_SMC_CALL_CV_TPU_SET_PROT_RANGE                                   \
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_STD_CALL, ARM_SMCCC_SMC_32,               \
			   ARM_SMCCC_OWNER_OEM, OPTEE_SMC_FUNID_CV_TPU_SET_PROT_RANGE)

#define OPTEE_SMC_CALL_CV_TPU_CLEAR_PROT_RANGE                                 \
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_STD_CALL, ARM_SMCCC_SMC_32,               \
			   ARM_SMCCC_OWNER_OEM, OPTEE_SMC_FUNID_CV_TPU_CLEAR_PROT_RANGE)

#define OPTEE_SMC_CALL_CV_EFUSE_READ                                           \
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_STD_CALL, ARM_SMCCC_SMC_32,               \
			   ARM_SMCCC_OWNER_OEM, OPTEE_SMC_FUNID_CV_EFUSE_READ)

#define OPTEE_SMC_CALL_CV_EFUSE_WRITE                                          \
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_STD_CALL, ARM_SMCCC_SMC_32,               \
			   ARM_SMCCC_OWNER_OEM, OPTEE_SMC_FUNID_CV_EFUSE_WRITE)

int tee_cv_aimodel_load(phys_addr_t cmd_phys, size_t cmd_size,
			phys_addr_t weight_phys, size_t weight_size,
			phys_addr_t neuron_phys, bool decrypt_cmd,
			bool decrypt_weight, struct arm_smccc_res *res);
int tee_cv_aimodel_run(phys_addr_t dmabuf_phys, phys_addr_t gaddr_base2,
		       phys_addr_t gaddr_base3, phys_addr_t gaddr_base4,
		       phys_addr_t gaddr_base5, phys_addr_t gaddr_base6,
		       phys_addr_t gaddr_base7, struct arm_smccc_res *res);
int tee_cv_aimodel_wait(uint64_t arg, struct arm_smccc_res *res);
int tee_cv_aimodel_set_prot_range(phys_addr_t addr, uint32_t size, struct arm_smccc_res *res);
int tee_cv_aimodel_clear_prot_range(phys_addr_t addr, uint32_t size, struct arm_smccc_res *res);
int tee_cv_init(void);
void tee_cv_exit(void);

int tee_cv_efuse_read(uint32_t addr, size_t size, void *buf);
int tee_cv_efuse_write(uint32_t addr, uint32_t value);

#endif /* TEE_CV_PRIVATE_H */
