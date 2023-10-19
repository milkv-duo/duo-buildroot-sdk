#ifdef ENV_CVITEST
#include <common.h>
#include <stdbool.h>
#include "system_common.h"
#include "timer.h"
#elif defined(ENV_EMU)
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "emu/command.h"
#else
#include "linux/types.h"
#include "linux/printk.h"
//#include <linux/delay.h>
#endif  // ENV_CVITEST

#include "reg.h"
#include "cmdq.h"

#define REG_CMDQ_INT_EVENT  0x00
#define REG_CMDQ_INT_EN     0x04
#define REG_CMDQ_DMA_ADDR_L 0x08
#define REG_CMDQ_DMA_ADDR_H 0x0C
#define REG_CMDQ_DMA_CNT    0x10
#define REG_CMDQ_DMA_CONFIG 0x14
#define REG_CMDQ_DMA_PARA   0x18
#define REG_CMDQ_JOB_CTL    0x1C
#define REG_CMDQ_STATUS     0x20
#define REG_CMDQ_APB_PARA   0x24
#define REG_CMDQ_SOFT_RST   0x28
#define REG_CMDQ_DEBUG      0x2c
#define REG_CMDQ_DUMMY      0x30

/**
 * cmdQ_set_package  - package reg_write to cmd_set.
 *
 */
void cmdQ_set_package(struct cmdq_set_reg *set, u32 addr, u32 data)
{
	set->data = data;
	set->addr = addr >> 2;
	set->byte_mask = 0xf;
	set->action = CMDQ_SET_REG;
}

/**
 * cmdQ_set_package  - package reg_write to cmd_set.
 *
 * @param set: the set to modify
 * @param is_timer: 1: wait_timer, 0: wait_flag
 * @param data: the data of wait condition.
 *		counter if timer and flag_num if flag
 * @param intr: the interrupt condition
 */
void cmdQ_set_wait(union cmdq_set *set, bool is_timer, u32 data, u8 intr)
{
	if (is_timer) {
		struct cmdq_set_wait_timer *wait = &(set->wait_timer);

		wait->counter = data;
		wait->intr_end = intr & 0x01;
		wait->intr_int = (intr >> 1) & 0x01;
		wait->intr_last = (intr >> 2) & 0x01;
		wait->action = CMDQ_SET_WAIT_TIMER;
	} else {
		struct cmdq_set_wait_flags *wait = &(set->wait_flags);

		wait->flag_num = data;
		wait->intr_end = intr & 0x01;
		wait->intr_int = (intr >> 1) & 0x01;
		wait->intr_last = (intr >> 2) & 0x01;
		wait->action = CMDQ_SET_WAIT_FLAG;
	}
}

/**
 * cmdQ_adma_package  - package adma entries.
 *
 * @param item: adma entry to modify
 * @param addr: address of link/cmd_set
 * @param size: size(bytes) if only this is cmd_set
 * @param is_link: 1: link descriptor, 2: cmd_set
 * @param is_end: true if this is last entry in adma-table.
 */
void cmdQ_adma_package(struct cmdq_adma *item, u64 addr, u32 size,
		       bool is_link, bool is_end)
{
	item->addr = addr;
	item->size = size;
	item->flags_end = is_end ? 1 : 0;
	item->flags_link = is_link ? 1 : 0;
}

/**
 * cmdQ_intr_ctrl - cmdQ's interrupt on(1)/off(0)
 *                 bit0: cmdQ intr, bit1: cmdQ end, bit2: cmdQ wait
 *
 * @param intr_mask: On/Off ctrl of the interrupt.
 */
void cmdQ_intr_ctrl(uintptr_t base, u8 intr_mask)
{
	_reg_write(base + REG_CMDQ_INT_EN, intr_mask);
}

/**
 * cmdQ_intr_ctrl - clear cmdQ's interrupt
 *                 bit0: cmdQ intr, bit1: cmdQ end, bit2: cmdQ wait
 *
 * @param base: base-address of cmdQ
 * @param intr_mask: the mask of the interrupt to clear.
 */
void cmdQ_intr_clr(uintptr_t base, u8 intr_mask)
{
	_reg_write(base + REG_CMDQ_INT_EVENT, intr_mask);
}

/**
 * cmdQ_intr_status - cmdQ's interrupt status
 *                 bit0: cmdQ intr, bit1: cmdQ end, bit2: cmdQ wait
 *
 * @param base: base-address of cmdQ
 */
u8 cmdQ_intr_status(uintptr_t base)
{
	return _reg_read(base + REG_CMDQ_INT_EVENT);
}

/**
 * cmdQ_engine - start cmdQ
 *
 * @param base: base-address of cmdQ
 * @param tbl_addr: adma/cmd_set table's address (32-byte alignment)
 * @param apb_base: relative IP's apb_base_address, whose MSB 10bits needed only
 * @param is_hw_restart: wait_flag is waiting for hw(1) or sw(0).
 * @param is_adma: 1 if adma table is used.
 * @param cnt: the number of entry in cmdset. only useful if adma
 */
void cmdQ_engine(uintptr_t base, uintptr_t tbl_addr, u16 apb_base,
		 bool is_hw_restart, bool is_adma, u16 cnt)
{
	u8 job_ctl = (is_hw_restart) ? 0x05 : 0x01;
	u8 dma_cfg = (is_adma) ? 0x02 : 0x00;

	// adma tbl addr
	_reg_write(base + REG_CMDQ_DMA_ADDR_L, tbl_addr);
	_reg_write(base + REG_CMDQ_DMA_ADDR_H, tbl_addr >> 32);
	if (!is_adma)
		_reg_write(base + REG_CMDQ_DMA_CNT, cnt<<3);

	_reg_write(base + REG_CMDQ_DMA_CONFIG, dma_cfg);
	_reg_write(base + REG_CMDQ_APB_PARA, apb_base);

	// job start
	_reg_write(base + REG_CMDQ_JOB_CTL, job_ctl);

#ifdef ENV_EMU
	u32 *cmdq_set = tbl_addr;

	for (u16 i = 0 ; i < cnt; ++i)
		printf("%3d: 0x%08x-%08x\n", i, cmdq_set[(i<<1) + 1],
		       cmdq_set[(i<<1)]);
#endif
}

/**
 * cmdQ_sw_restart - toggle sw_restart if cmdQ wait-flag is sw-toggle.
 *
 * @param base: base-address of cmdQ
 */
void cmdQ_sw_restart(uintptr_t base)
{
	_reg_write(base + REG_CMDQ_JOB_CTL, 0x02);
}

bool cmdQ_is_sw_restart(uintptr_t base)
{
	return (_reg_read(base + REG_CMDQ_JOB_CTL) & 0x04) ? false : true;
}
