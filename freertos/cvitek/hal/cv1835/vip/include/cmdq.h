#ifndef _CVI_CMDQ_H_
#define _CVI_CMDQ_H_

enum {
	CMDQ_MODE_SDMA,
	CMDQ_MODE_ADMA,
};

struct cmdq_adma {
	u64 addr;
	u32 size;
	u32 flags_end : 1;
	u32 rsv : 2;
	u32 flags_link : 1;
	u32 rsv2 : 28;
};

enum {
	CMDQ_SET_REG,
	CMDQ_SET_WAIT_TIMER,
	CMDQ_SET_WAIT_FLAG,
};

struct cmdq_set_reg {
	u32 data;
	u32 addr : 20;
	u32 byte_mask : 4;
	u32 intr_end : 1;
	u32 intr_int : 1;
	u32 intr_last : 1;
	u32 intr_rsv : 1;
	u32 action : 4;  // 0 for this case
};

struct cmdq_set_wait_timer {
	u32 counter;
	u32 rsv : 24;
	u32 intr_end : 1;
	u32 intr_int : 1;
	u32 intr_last : 1;
	u32 intr_rsv : 1;
	u32 action : 4;  // 1 for this case
};

struct cmdq_set_wait_flags {
	u32 flag_num;   // 0 ~ 15, depending on each module
	u32 rsv : 24;
	u32 intr_end : 1;
	u32 intr_int : 1;
	u32 intr_last : 1;
	u32 intr_rsv : 1;
	u32 action : 4;  // 2 for this case
};

union cmdq_set {
	struct cmdq_set_reg reg;
	struct cmdq_set_wait_timer wait_timer;
	struct cmdq_set_wait_flags wait_flags;
};

/**
 * cmdQ_set_package  - package reg_write to cmd_set.
 *
 */
void cmdQ_set_package(struct cmdq_set_reg *set, u32 addr, u32 data);

/**
 * cmdQ_set_package  - package reg_write to cmd_set.
 *
 * @param set: the set to modify
 * @param is_timer: 1: wait_timer, 0: wait_flag
 * @param data: the data of wait condition.
 *		counter if timer and flag_num if flag
 * @param intr: the interrupt condition
 */
void cmdQ_set_wait(union cmdq_set *set, bool is_timer, u32 data, u8 intr);

/**
 * cmdQ_adma_package  - package adma entries.
 *
 * @param item: adma entry to modify
 * @param addr: address of link/cmd_set
 * @param size: size(bytes) if only this is cmd_set
 * @param is_link: 1: link descriptor, 2: cmd_set
 * @param is_end: true if this is last entry in adma-table.
 */
void cmdQ_adma_package(struct cmdq_adma *item, u64 addr, u32 size, bool is_link,
		       bool is_end);

/**
 * cmdQ_intr_ctrl - cmdQ's interrupt on(1)/off(0)
 *                 bit0: cmdQ intr, bit1: cmdQ end, bit2: cmdQ wait
 *
 * @param intr_mask: On/Off ctrl of the interrupt.
 */
void cmdQ_intr_ctrl(uintptr_t base, u8 intr_mask);

/**
 * cmdQ_intr_ctrl - clear cmdQ's interrupt
 *                 bit0: cmdQ intr, bit1: cmdQ end, bit2: cmdQ wait
 *
 * @param base: base-address of cmdQ
 * @param intr_mask: the mask of the interrupt to clear.
 */
void cmdQ_intr_clr(uintptr_t base, u8 intr_mask);

/**
 * cmdQ_intr_status - cmdQ's interrupt status
 *                 bit0: cmdQ intr, bit1: cmdQ end, bit2: cmdQ wait
 *
 * @param base: base-address of cmdQ
 */
u8 cmdQ_intr_status(uintptr_t base);

/**
 * cmdQ_engine - start cmdQ
 *
 * @param base: base-address of cmdQ
 * @param adma_addr: adma table's address
 * @param apb_base: relative IP's apb_base_address, whose MSB 10bits needed only
 * @param is_hw_restart: wait_flag is waiting for hw(1) or sw(0).
 * @param is_adma: 1 if adma table is used.
 * @param cnt: the number of entry in cmdset. only useful if adma
 */
void cmdQ_engine(uintptr_t base, uintptr_t tbl_addr, u16 apb_base,
		 bool is_hw_restart, bool is_adma, u16 cnt);

/**
 * cmdQ_sw_restart - toggle sw_restart if cmdQ wait-flag is sw-toggle.
 *
 * @param base: base-address of cmdQ
 */
void cmdQ_sw_restart(uintptr_t base);

bool cmdQ_is_sw_restart(uintptr_t base);

#endif  //_CVI_CMDQ_H_
