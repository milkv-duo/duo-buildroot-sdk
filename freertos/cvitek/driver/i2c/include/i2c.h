/*
 * Copyright (C) 2020 CVITEK
 */

#ifndef __I2C_H_
#define __I2C_H_

#include <stdbool.h>
struct i2c_msg {
	uint16_t addr;	/* slave address			*/
	uint16_t flags;
#define I2C_M_TEN		0x0010	/* this is a ten bit chip address */
#define I2C_M_WRSTOP		0x0002  /* if allow stop between msg. */
#define I2C_M_RD		0x0001	/* read data, from slave to master */
#define I2C_M_STOP		0x8000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NOSTART		0x4000	/* if I2C_FUNC_NOSTART */
#define I2C_M_REV_DIR_ADDR	0x2000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK	0x1000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NO_RD_ACK		0x0800	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_RECV_LEN		0x0400	/* length will be first received byte */
	uint16_t len;		/* msg length				*/
	uint8_t *buf;		/* pointer to msg data			*/
};

int i2c_write(uint8_t i2c_id, uint8_t dev, uint16_t addr, uint16_t alen, uint8_t *buffer, uint16_t len);
int i2c_read(uint8_t i2c_id, uint8_t dev, uint16_t addr, uint16_t alen, uint8_t *buffer, uint16_t len);
int i2c_set_interstop(uint8_t i2c_id, bool use_interstop);
int i2c_xfer(uint8_t i2c_id, struct i2c_msg msgs[], int num);
void i2c_init(uint8_t i2c_id);

#endif /* __I2C_H_ */
