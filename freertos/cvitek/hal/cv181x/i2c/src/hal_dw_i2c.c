/*
 * (C) Copyright 2020 CVITEK
 */

#include <stdlib.h>
//#include <stdio.h>
#include <stdbool.h>
#include <string.h>
//#include <unistd.h>
#include "top_reg.h"
#include "intr_conf.h"
#include "hal_dw_i2c.h"
#include "printf.h"
#include "mmio.h"
#include "sleep.h"


static struct dw_i2c_dev dw_i2c[5];

static struct i2c_regs *get_i2c_base(uint8_t i2c_id)
{
    struct i2c_regs *i2c_base = NULL;

    switch (i2c_id) {
    case I2C0:
        i2c_base = (struct i2c_regs *)I2C0_BASE;
        break;
    case I2C1:
        i2c_base = (struct i2c_regs *)I2C1_BASE;
        break;
    case I2C2:
        i2c_base = (struct i2c_regs *)I2C2_BASE;
        break;
    case I2C3:
        i2c_base = (struct i2c_regs *)I2C3_BASE;
        break;
    case I2C4:
        i2c_base = (struct i2c_regs *)I2C4_BASE;
        break;
    }

    return i2c_base;
}

void i2c_write_cmd_data(struct i2c_regs *i2c, uint16_t value)
{
	mmio_write_32((uintptr_t)&i2c->ic_cmd_data, value);
}

static void i2c_enable(struct i2c_regs *i2c, bool enable)
{
	uint32_t ena = enable ? IC_ENABLE : 0;
	int timeout = 100;

	do {
		mmio_write_32((uintptr_t)&i2c->ic_enable, ena);
		if ((mmio_read_32((uintptr_t)&i2c->ic_enable_status) & IC_ENABLE) == ena)
			return;

		/*
		 * Wait 10 times the signaling period of the highest I2C
		 * transfer supported by the driver (for 400KHz this is
		 * 25us) as described in the DesignWare I2C databook.
		 */
		arch_usleep(25);
	} while (timeout--);

	printf("timeout in %sabling I2C adapter\n", enable ? "en" : "dis");
}

static void i2c_disable(struct dw_i2c_dev *dw_i2c)
{
	int timeout = 100;

	if (dw_i2c->wait_irq || dw_i2c->use_interstop)
		return; /* return due to i2c is still active */

	do {
		mmio_write_32((uintptr_t)&dw_i2c->i2c->ic_enable, 0x0);
		if ((mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_enable_status) & IC_ENABLE) == 0x0)
			return;

		/*
		 * Wait 10 times the signaling period of the highest I2C
		 * transfer supported by the driver (for 400KHz this is
		 * 25us) as described in the DesignWare I2C databook.
		 */
		arch_usleep(25);
	} while (timeout--);

	printf("timeout in disabling I2C adapter\n");
}

/*
 * i2c_flush_rxfifo - Flushes the i2c RX FIFO
 *
 * Flushes the i2c RX FIFO
 */
static void i2c_flush_rxfifo(struct i2c_regs *i2c)
{
	while (mmio_read_32((uintptr_t)&i2c->ic_status) & IC_STATUS_RFNE)
		mmio_read_32((uintptr_t)&i2c->ic_cmd_data);
}

/*
 * i2c_wait_for_bb - Waits for bus busy
 *
 * Waits for bus busy
 */
static int i2c_wait_for_bb(struct i2c_regs *i2c)
{
	uint16_t    timeout = 0;

	while ((mmio_read_32((uintptr_t)&i2c->ic_status) & IC_STATUS_MA) ||
	       !(mmio_read_32((uintptr_t)&i2c->ic_status) & IC_STATUS_TFE)) {

		/* Evaluate timeout */
		arch_usleep(5);
        timeout++;
        if (timeout > 200) /* exceed 1 ms */
            return 1;
	}

	return 0;
}

/*
 * i2c_setaddress - Sets the target slave address
 * @i2c_addr:	target i2c address
 *
 * Sets the target slave address.
 */
static void i2c_setaddress(struct i2c_regs *i2c, uint16_t i2c_addr)
{
	/* Disable i2c */
	i2c_enable(i2c, false);
	mmio_write_32((uintptr_t)&i2c->ic_tar, i2c_addr);
	/* Enable i2c */
	i2c_enable(i2c, true);
}


static int i2c_xfer_init(struct i2c_regs *i2c, uint16_t chip, uint16_t addr, uint16_t alen)
{
	if (i2c_wait_for_bb(i2c))
		return 1;

	i2c_setaddress(i2c, chip);

	while (alen) {
		alen--;
		/* high byte address going out first */
		i2c_write_cmd_data(i2c, (addr >> (alen * 8)) & 0xff);
		//mmio_write_32((uintptr_t)&i2c_base->ic_cmd_data, (addr >> (alen * 8)) & 0xff);
	}
	return 0;
}

static int i2c_xfer_finish(struct i2c_regs *i2c)
{
    uint16_t timeout = 0;
	while (1) {
		if ((mmio_read_32((uintptr_t)&i2c->ic_raw_intr_stat) & IC_STOP_DET)) {
			mmio_read_32((uintptr_t)&i2c->ic_clr_stop_det);
			break;
		} else {
            timeout++;
			arch_usleep(5);
            if (timeout > I2C_STOPDET_TO * 100) {
				printf("%s, tiemout\n", __func__);
			    break;
			}
		}
	}

	if (i2c_wait_for_bb(i2c))
		return 1;

	i2c_flush_rxfifo(i2c);

	return 0;
}

/*
 * i2c_read - Read from i2c memory
 * @chip:	target i2c address
 * @addr:	address to read from
 * @alen:
 * @buffer:	buffer for read data
 * @len:	no of bytes to be read
 *
 * Read from i2c memory.
 */
int hal_i2c_read(uint8_t i2c_id, uint8_t dev, uint16_t addr, uint16_t alen, uint8_t *buffer, uint16_t len)
{
	unsigned int active = 0;
	unsigned int time_count = 0;
	struct i2c_regs *i2c;
	int ret = 0;

	i2c = get_i2c_base(i2c_id);

	i2c_enable(i2c, true);

	if (i2c_xfer_init(i2c, dev, addr, alen))
		return 1;

	while (len) {
		if (!active) {
			/*
			 * Avoid writing to ic_cmd_data multiple times
			 * in case this loop spins too quickly and the
			 * ic_status RFNE bit isn't set after the first
			 * write. Subsequent writes to ic_cmd_data can
			 * trigger spurious i2c transfer.
			 */
			i2c_write_cmd_data(i2c, (dev <<1) | BIT_I2C_CMD_DATA_READ_BIT | BIT_I2C_CMD_DATA_STOP_BIT);
			//mmio_write_32((uintptr_t)&i2c_base->ic_cmd_data, (dev <<1) | BIT_I2C_CMD_DATA_READ_BIT | BIT_I2C_CMD_DATA_STOP_BIT);
			active = 1;
		}

		if (mmio_read_32((uintptr_t)&i2c->ic_raw_intr_stat) & BIT_I2C_INT_RX_FULL) {
			*buffer++ = (uint8_t)mmio_read_32((uintptr_t)&i2c->ic_cmd_data);
			len--;
			time_count = 0;
			active = 0;
		}
		else {
            arch_usleep(5);
            time_count++;
            if (time_count  >= I2C_BYTE_TO * 100)
				return 1;
		}
	}

	ret = i2c_xfer_finish(i2c);
	i2c_disable(&dw_i2c[i2c_id]);

	return ret;
}

/*
 * i2c_write - Write to i2c memory
 * @chip:	target i2c address
 * @addr:	address to read from
 * @alen:
 * @buffer:	buffer for read data
 * @len:	no of bytes to be read
 *
 * Write to i2c memory.
 */

int hal_i2c_write(uint8_t i2c_id, uint8_t dev, uint16_t addr, uint16_t alen, uint8_t *buffer, uint16_t len)
{
    struct i2c_regs *i2c;
	int ret = 0;
    i2c = get_i2c_base(i2c_id);

	i2c_enable(i2c, true);

	if (i2c_xfer_init(i2c, dev, addr, alen))
		return 1;

	while (len) {
		if (i2c->ic_status & IC_STATUS_TFNF) {
			if (--len == 0) {
				i2c_write_cmd_data(i2c, *buffer | IC_STOP);
				//mmio_write_32((uintptr_t)&i2c_base->ic_cmd_data, *buffer | IC_STOP);
			} else {
				i2c_write_cmd_data(i2c, *buffer);
				//mmio_write_32((uintptr_t)&i2c_base->ic_cmd_data, *buffer);
			}
			buffer++;
        } else
			printf("len=%d, ic status is not TFNF\n", len);
	}
	ret = i2c_xfer_finish(i2c);
	i2c_disable(&dw_i2c[i2c_id]);
	return ret;
}

/*
 * hal_i2c_set_bus_speed - Set the i2c speed
 * @speed:	required i2c speed
 *
 * Set the i2c speed.
 */
static void i2c_set_bus_speed(struct i2c_regs *i2c, unsigned int speed)
{
	unsigned int cntl;
	unsigned int hcnt, lcnt;
	int i2c_spd;

	if (speed > I2C_FAST_SPEED)
		i2c_spd = IC_SPEED_MODE_MAX;
	else if ((speed <= I2C_FAST_SPEED) && (speed > I2C_STANDARD_SPEED))
		i2c_spd = IC_SPEED_MODE_FAST;
	else
		i2c_spd = IC_SPEED_MODE_STANDARD;

	/* to set speed cltr must be disabled */
	i2c_enable(i2c, false);

	cntl = (mmio_read_32((uintptr_t)&i2c->ic_con) & (~IC_CON_SPD_MSK));

	switch (i2c_spd) {
	case IC_SPEED_MODE_MAX:
		cntl |= IC_CON_SPD_HS;
			//hcnt = (u16)(((IC_CLK * MIN_HS100pF_SCL_HIGHTIME) / 1000) - 8);
			/* 7 = 6+1 == MIN LEN +IC_FS_SPKLEN */
			//lcnt = (u16)(((IC_CLK * MIN_HS100pF_SCL_LOWTIME) / 1000) - 1);
			hcnt = 6;
			lcnt = 8;

		mmio_write_32((uintptr_t)&i2c->ic_hs_scl_hcnt, hcnt);
		mmio_write_32((uintptr_t)&i2c->ic_hs_scl_lcnt, lcnt);
		break;

	case IC_SPEED_MODE_STANDARD:
		cntl |= IC_CON_SPD_SS;

		hcnt = (uint16_t)(((IC_CLK * MIN_SS_SCL_HIGHTIME) / 1000) - 7);
		lcnt = (uint16_t)(((IC_CLK * MIN_SS_SCL_LOWTIME) / 1000) - 1);

		mmio_write_32((uintptr_t)&i2c->ic_ss_scl_hcnt, hcnt);
		mmio_write_32((uintptr_t)&i2c->ic_ss_scl_lcnt, lcnt);
		break;

	case IC_SPEED_MODE_FAST:
	default:
		cntl |= IC_CON_SPD_FS;
		hcnt = (uint16_t)(((IC_CLK * MIN_FS_SCL_HIGHTIME) / 1000) - 7);
		lcnt = (uint16_t)(((IC_CLK * MIN_FS_SCL_LOWTIME) / 1000) - 1);

		mmio_write_32((uintptr_t)&i2c->ic_fs_scl_hcnt, hcnt);
		mmio_write_32((uintptr_t)&i2c->ic_fs_scl_lcnt, lcnt);
		break;
	}

	mmio_write_32((uintptr_t)&i2c->ic_con, cntl);

	/* Enable back i2c now speed set */
	i2c_enable(i2c, true);
}


static void i2c_xfer_init_burst(struct i2c_regs *i2c, struct i2c_msg *msgs)
{
	uint32_t ic_con, ic_tar = 0, ic_intr = 0;

	/* Disable the i2c */
	i2c_enable(i2c, false);

	/* If the slave address is ten bit address, enable 10BITADDR */
	ic_con = mmio_read_32((uintptr_t)&i2c->ic_con);

	if (msgs[0].flags & I2C_M_TEN) {
		ic_con |= BIT_I2C_CON_10B_ADDR_MASTER;
		/*
		 * If I2C_DYNAMIC_TAR_UPDATE is set, the 10-bit addressing
		 * mode has to be enabled via bit 12 of IC_TAR register.
		 * We set it always as I2C_DYNAMIC_TAR_UPDATE can't be
		 * detected from registers.
		 */
		ic_tar = BIT_I2C_TAR_10B_ADDR_MASTER;
	} else {
		ic_con &= ~BIT_I2C_CON_10B_ADDR_MASTER;
	}

	mmio_write_32((uintptr_t)&i2c->ic_con, ic_con);

	/*
	 * Set the slave (target) address and enable 10-bit addressing mode
	 * if applicable.
	 */
	mmio_write_32((uintptr_t)&i2c->ic_tar, msgs[0].addr | ic_tar);
	/* Enforce disabled interrupts (due to HW issues) */
	//i2c_dw_disable_int(dev);

	/* Enable the adapter */
	i2c_enable(i2c, true);

	/* Clear and enable interrupts */
	mmio_read_32((uintptr_t)&i2c->ic_clr_intr);
	ic_intr = I2C_INTR_MASTER_MASK;
	mmio_write_32((uintptr_t)&i2c->ic_intr_mask, ic_intr);
}

int hal_i2c_set_interstop(uint8_t i2c_id, bool use_interstop)
{
	dw_i2c[i2c_id].use_interstop = use_interstop;

	return 0;
}

/*
 * Initiate (and continue) low level master read/write transaction.
 * This function is only called from i2c_dw_isr, and pumping i2c_msg
 * messages into the tx buffer.  Even if the size of i2c_msg data is
 * longer than the size of the tx buffer, it handles everything.
 */
static void i2c_dw_xfer_msg(struct dw_i2c_dev *dw_i2c, struct i2c_msg *msgs, uint32_t num)
{
	uint32_t intr_mask;
	uint16_t tx_limit, rx_limit;
	uint32_t addr = msgs[dw_i2c->msg_write_idx].addr;
	uint32_t buf_len = dw_i2c->tx_buf_len;
	uint8_t *buf = dw_i2c->tx_buf;
	bool need_restart = false;

	intr_mask = I2C_INTR_MASTER_MASK;
	//printf("%s, num=%d, write_idx=%d, flags=0x%x\n", __func__, num, dw_i2c->msg_write_idx, msgs[dw_i2c->msg_write_idx].flags);

	for (; dw_i2c->msg_write_idx < num; dw_i2c->msg_write_idx++) {
		uint32_t flags = msgs[dw_i2c->msg_write_idx].flags;

		/*
		 * If target address has changed, we need to
		 * reprogram the target address in the I2C
		 * adapter when we are done with this transfer.
		 */
		if (msgs[dw_i2c->msg_write_idx].addr != addr) {
			printf("%s: invalid target address\n", __func__);
			dw_i2c->msg_err = -1;
			break;
		}

		if (msgs[dw_i2c->msg_write_idx].len == 0) {
			printf("%s: invalid message length\n", __func__);
			dw_i2c->msg_err = -1;
			break;
		}

		/* new i2c_msg */
		buf = msgs[dw_i2c->msg_write_idx].buf;
		buf_len = msgs[dw_i2c->msg_write_idx].len;

		/* If both IC_EMPTYFIFO_HOLD_MASTER_EN and
		 * IC_RESTART_EN are set, we must manually
		 * set restart bit between messages.
		*/
		if (!(dw_i2c->status & STATUS_WRITE_IN_PROGRESS)) {
			/* new i2c_msg */
			buf = msgs[dw_i2c->msg_write_idx].buf;
			buf_len = msgs[dw_i2c->msg_write_idx].len;

			/* If both IC_EMPTYFIFO_HOLD_MASTER_EN and
			 * IC_RESTART_EN are set, we must manually
			 * set restart bit between messages.
			 */
			if (dw_i2c->msg_write_idx > 0)
				need_restart = true;
		}


		tx_limit = dw_i2c->tx_fifo_depth - mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_txflr);
		rx_limit = dw_i2c->rx_fifo_depth - mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_rxflr);

		while (buf_len > 0 && tx_limit > 0 && rx_limit > 0) {
			uint32_t cmd = 0;

			/*
			 * If IC_EMPTYFIFO_HOLD_MASTER_EN is set we must
			 * manually set the stop bit. However, it cannot be
			 * detected from the registers so we set it always
			 * when writing/reading the last byte.
			 */

			/*
			 * i2c-core always sets the buffer length of
			 * I2C_FUNC_SMBUS_BLOCK_DATA to 1. The length will
			 * be adjusted when receiving the first byte.
			 * Thus we can't stop the transaction here.
			 */
			if (((dw_i2c->msg_write_idx == num - 1) || (flags & I2C_M_WRSTOP)) &&
			    buf_len == 1 && !(flags & I2C_M_RECV_LEN)) {
				cmd |= BIT(9);
				if (flags & I2C_M_WRSTOP)
					dw_i2c->use_interstop = 1;
			}

			if (need_restart) {
				cmd |= BIT(10);
				need_restart = false;
			}

			if (msgs[dw_i2c->msg_write_idx].flags & I2C_M_RD) {

				/* Avoid rx buffer overrun */
				if (dw_i2c->rx_outstanding >= dw_i2c->rx_fifo_depth)
					break;
				mmio_write_32((uintptr_t)&dw_i2c->i2c->ic_cmd_data, cmd | 0x100);
				rx_limit--;
				dw_i2c->rx_outstanding++;
			} else
				mmio_write_32((uintptr_t)&dw_i2c->i2c->ic_cmd_data, cmd | *buf++);

			tx_limit--; buf_len--;
		}

		dw_i2c->tx_buf = buf;
		dw_i2c->tx_buf_len = buf_len;

		/*
		 * Because we don't know the buffer length in the
		 * I2C_FUNC_SMBUS_BLOCK_DATA case, we can't stop
		 * the transaction here.
		 */
		if (buf_len > 0 || flags & I2C_M_RECV_LEN) {
			/* more bytes to be written */
			dw_i2c->status |= STATUS_WRITE_IN_PROGRESS;
			break;
		}
		dw_i2c->status &= ~STATUS_WRITE_IN_PROGRESS;
	}

	/*
	 * If i2c_msg index search is completed, we don't need TX_EMPTY
	 * interrupt any more.
	 */
	if (dw_i2c->msg_write_idx == num)
		intr_mask &= ~BIT_I2C_INT_TX_EMPTY;

	if (dw_i2c->msg_err)
		intr_mask = 0;

	mmio_write_32((uintptr_t)&dw_i2c->i2c->ic_intr_mask, intr_mask);
}

static uint8_t i2c_dw_recv_len(struct dw_i2c_dev *dw_i2c, struct i2c_msg *msgs, uint8_t len)
{
	uint32_t flags = msgs[dw_i2c->msg_read_idx].flags;

	/*
	 * Adjust the buffer length and mask the flag
	 * after receiving the first byte.
	 */
	len += (flags & I2C_CLIENT_PEC) ? 2 : 1;
	dw_i2c->tx_buf_len = len - ((len <= dw_i2c->rx_outstanding) ? len : dw_i2c->rx_outstanding);
	msgs[dw_i2c->msg_read_idx].len = len;
	msgs[dw_i2c->msg_read_idx].flags &= ~I2C_M_RECV_LEN;

	return len;
}

static void i2c_dw_read(struct dw_i2c_dev *dw_i2c, struct i2c_msg *msgs, uint32_t num)
{
	int rx_valid;

	for (; dw_i2c->msg_read_idx < num; dw_i2c->msg_read_idx++) {
		uint32_t len;
		uint8_t *buf;

		if (!(msgs[dw_i2c->msg_read_idx].flags & I2C_M_RD))
			continue;

		if (!(dw_i2c->status & STATUS_READ_IN_PROGRESS)) {
			len = msgs[dw_i2c->msg_read_idx].len;
			buf = msgs[dw_i2c->msg_read_idx].buf;
		} else {
			len = dw_i2c->rx_buf_len;
			buf = dw_i2c->rx_buf;
		}

		rx_valid = mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_rxflr);

		for (; len > 0 && rx_valid > 0; len--, rx_valid--) {
			uint32_t flags = msgs[dw_i2c->msg_read_idx].flags;

			*buf = mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_cmd_data);
			/* Ensure length byte is a valid value */
			if (flags & I2C_M_RECV_LEN &&
				*buf <= I2C_SMBUS_BLOCK_MAX && *buf > 0) {
				len = i2c_dw_recv_len(dw_i2c, msgs, *buf);
			}
			buf++;
			dw_i2c->rx_outstanding--;
		}

		if (len > 0) {
			dw_i2c->status |= STATUS_READ_IN_PROGRESS;
			dw_i2c->rx_buf_len = len;
			dw_i2c->rx_buf = buf;
			return;
		}
		dw_i2c->status &= ~STATUS_READ_IN_PROGRESS;
	}
}

static uint32_t i2c_dw_read_clear_intrbits(struct dw_i2c_dev *dw_i2c)
{
	uint32_t stat;

	/*
	 * The IC_INTR_STAT register just indicates "enabled" interrupts.
	 * Ths unmasked raw version of interrupt status bits are available
	 * in the IC_RAW_INTR_STAT register.
	 *
	 * That is,
	 *   stat = dw_readl(IC_INTR_STAT);
	 * equals to,
	 *   stat = dw_readl(IC_RAW_INTR_STAT) & dw_readl(IC_INTR_MASK);
	 *
	 * The raw version might be useful for debugging purposes.
	 */
	stat = mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_intr_stat);

	/*
	 * Do not use the IC_CLR_INTR register to clear interrupts, or
	 * you'll miss some interrupts, triggered during the period from
	 * dw_readl(IC_INTR_STAT) to dw_readl(IC_CLR_INTR).
	 *
	 * Instead, use the separately-prepared IC_CLR_* registers.
	 */
	if (stat & BIT_I2C_INT_RX_UNDER)
		mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_clr_rx_under);
	if (stat & BIT_I2C_INT_RX_OVER)
		mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_clr_rx_over);
	if (stat & BIT_I2C_INT_TX_OVER)
		mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_clr_tx_over);
	if (stat & BIT_I2C_INT_RD_REQ)
		mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_clr_rd_req);
	if (stat & BIT_I2C_INT_TX_ABRT) {
		/*
		 * The IC_TX_ABRT_SOURCE register is cleared whenever
		 * the IC_CLR_TX_ABRT is read.  Preserve it beforehand.
		 */
		//dev->abort_source = mmio_read_32((uintptr_t)&dw_i2c->ic_tx_abrt_source);
		mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_clr_tx_abrt);
	}
	if (stat & BIT_I2C_INT_RX_DONE)
		mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_clr_rx_done);
	if (stat & BIT_I2C_INT_ACTIVITY)
		mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_clr_activity);
	if (stat & BIT_I2C_INT_STOP_DET)
		mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_clr_stop_det);
	if (stat & BIT_I2C_INT_START_DET)
		mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_clr_start_det);
	if (stat & BIT_I2C_INT_GEN_ALL)
		mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_clr_gen_call);

	return stat;
}

/*
 * Interrupt service routine. This gets called whenever an I2C master interrupt
 * occurs.
 */
static int i2c_dw_irq_handler_master(struct dw_i2c_dev *dw_i2c)
{
	uint32_t stat;

	stat = i2c_dw_read_clear_intrbits(dw_i2c);
	//printf("%s, i2c-%d, state=0x%x, mask=0x%x, enabled=0x%x\n", __func__, dw_i2c->i2c_id, stat,
	//		mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_intr_mask),
	//		mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_enable_status));

	if (stat & BIT_I2C_INT_TX_ABRT) {
		//dw_i2c->cmd_err |= DW_IC_ERR_TX_ABRT;
		dw_i2c->status = STATUS_IDLE;

		/*
		 * Anytime TX_ABRT is set, the contents of the tx/rx
		 * buffers are flushed. Make sure to skip them.
		 */
		mmio_write_32((uintptr_t)&dw_i2c->i2c->ic_intr_mask, 0);
		goto tx_aborted;
	}

	if (stat & BIT_I2C_INT_RX_FULL) {
		if (dw_i2c->msgs)
			i2c_dw_read(dw_i2c, dw_i2c->msgs, dw_i2c->msg_num);
		else
			i2c_disable(dw_i2c);
	}

	if (stat & BIT_I2C_INT_TX_EMPTY) {
		if (dw_i2c->msgs)
			i2c_dw_xfer_msg(dw_i2c, dw_i2c->msgs, dw_i2c->msg_num);
		else
			i2c_disable(dw_i2c);
	}
	/*
	 * No need to modify or disable the interrupt mask here.
	 * i2c_dw_xfer_msg() will take care of it according to
	 * the current transmit status.
	 */

tx_aborted:

	if ((stat & (BIT_I2C_INT_TX_ABRT | BIT_I2C_INT_STOP_DET))) {
		//printf("%s, completed\n", __func__);
		dw_i2c->wait_irq = false;
		if (!dw_i2c->use_interstop) {
			//printf("disable i2c-%d due to interstop is not set\n", i2c_id);
			i2c_disable(dw_i2c);
		}
		dw_i2c->use_interstop = 0;
		//complete(&dev->cmd_complete);

	} else if (dw_i2c->flags & ACCESS_INTR_MASK) {
		/* Workaround to trigger pending interrupt */
		stat = mmio_read_32((uintptr_t)&dw_i2c->i2c->ic_intr_mask);
		mmio_write_32((uintptr_t)&dw_i2c->i2c->ic_intr_mask, 0);
		mmio_write_32((uintptr_t)&dw_i2c->i2c->ic_intr_mask, stat);
	}

	return 0;
}

/*
 * Prepare controller for a transaction and call i2c_dw_xfer_msg.
 */
int hal_i2c_xfer(uint8_t i2c_id, struct i2c_msg msgs[], uint32_t num)
{
	//struct i2c_regs *i2c;
	int ret = 0;

	//Need to acquire lock here

    //dw_i2c[i2c_id].i2c = get_i2c_base(i2c_id);

	ret = i2c_wait_for_bb(dw_i2c[i2c_id].i2c);
	if (ret)
		goto done;

	i2c_xfer_init_burst(dw_i2c[i2c_id].i2c, msgs);

	dw_i2c[i2c_id].msgs = msgs;
	dw_i2c[i2c_id].msg_num = num;
	dw_i2c[i2c_id].wait_irq = true;

#if 0 /* due to freeRTOS doesn't support wait_for_completion_timeout, we don't know
		when to disable i2c in this function. So, disable action below */

	/* Not to stop the i2c since we don't know when the transfer completes */
	if (!dw_i2c[i2c_id].use_interstop) {
		//printf("stop i2c-%d due to interstop is false\n", i2c_id);
		i2c_enable(dw_i2c[i2c_id].i2c, false);
	}

	dw_i2c[i2c_id].use_interstop = 0;
#endif
done:
	//Need to release lock here

	return ret;
}

static int i2c_dw_isr(int irq, void *dev)
{
	struct dw_i2c_dev *i2c_dev = dev;
	uint32_t stat, enabled;

	(void) irq;

	enabled = mmio_read_32((uintptr_t)&i2c_dev->i2c->ic_enable);
	stat = mmio_read_32((uintptr_t)&i2c_dev->i2c->ic_intr_stat);

	if (!enabled || !(stat & ~BIT_I2C_INT_ACTIVITY) || !dev)
		return 0;

	i2c_dw_irq_handler_master(i2c_dev);

	return 0;
}


/*
 * __hal_i2c_init - Init function
 * @speed:	required i2c speed
 * @slaveaddr:	slave address for the device
 *
 * Initialization function.
 */
void hal_i2c_init(uint8_t i2c_id)
{
    struct i2c_regs *i2c;

    printf("%s, i2c-%d\n", __func__, i2c_id);
	/* Disable i2c */
    //Need to acquire lock here

	i2c = get_i2c_base(i2c_id);

	dw_i2c[i2c_id].tx_fifo_depth = 64;
	dw_i2c[i2c_id].rx_fifo_depth = 64;
	dw_i2c[i2c_id].status = STATUS_IDLE;
	dw_i2c[i2c_id].msg_err = 0;
	dw_i2c[i2c_id].i2c = i2c;
	dw_i2c[i2c_id].i2c_id = i2c_id;
	dw_i2c[i2c_id].msg_write_idx = 0;
	dw_i2c[i2c_id].msg_read_idx = 0;
	dw_i2c[i2c_id].use_interstop = 0;
	dw_i2c[i2c_id].wait_irq = false;

	request_irq(IC3_INTR, i2c_dw_isr, 0, "IC2_INTR int", &dw_i2c[i2c_id]);

	i2c_enable(i2c, false);
	mmio_write_32((uintptr_t)&i2c->ic_con, (IC_CON_SD | IC_CON_SPD_FS | IC_CON_MM | IC_CON_RE));
	mmio_write_32((uintptr_t)&i2c->ic_rx_tl, IC_RX_TL);
	mmio_write_32((uintptr_t)&i2c->ic_tx_tl, IC_TX_TL);
	mmio_write_32((uintptr_t)&i2c->ic_intr_mask, 0x0);
	i2c_set_bus_speed(i2c, I2C_SPEED);
	//mmio_write_32((uintptr_t)&i2c->ic_sar, slaveaddr);
	/* Enable i2c */
	i2c_enable(i2c, false);

	//Need to release lock here
}
