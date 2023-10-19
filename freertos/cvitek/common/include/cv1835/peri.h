/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/peri.h
 * Description:
 *   peripheral includes i2c for sns and reg for isp.
 */

#ifndef __PERI_H__
#define __PERI_H__

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

/* This function must be called to initialize the I2C master interface on the respective hardware pins.
 *
 * @i2c_bus: id of the i2c bus
 * @slave_addr: slave addr of the i2c device
 */
int i2cInit(uint8_t i2c_bus, uint8_t slave_addr);

/* This function must be called to release resource.
 *
 */
void i2cExit(void);

/* I2C Read
 *
 * @addr:        reg address
 * @addr_nbytes: number of bytes of reg address. 2 for 16-bit;1 for 8-bit address.
 * @buffer:      Pointer to the buffer where the data bytes that are received will be written to
 * @nbytes:      Number of bytes to receive
 *
 * @return:      Zero on success
 */
int i2cRead(uint16_t addr, unsigned int addr_nbytes, uint8_t *buffer,
	    unsigned int data_nbytes);

/* I2C Write
 *
 * @addr:        reg address
 * @addr_nbytes: number of bytes of reg address. 2 for 16-bit;1 for 8-bit address.
 * @buffer:      Pointer to the buffer that holds data bytes to send
 * @nbytes:      Number of bytes to send
 *
 * @return:      Zero on success
 */
int i2cWrite(uint16_t addr, unsigned int addr_nbytes, uint8_t *buffer,
	     unsigned int data_nbytes);

/* This function must be called to initialize the reg access.
 *
 */
int regInit(void);

/* This function must be called to release resource.
 *
 */
int regExit(void);

/* REG Read
 *
 * @addr:        reg address(32bit)
 *
 * @return:      Zero on success
 */
uint32_t regRead(uint32_t addr);

/* REG Write
 *
 * @addr:        reg address(32bit)
 * @data:        data to be writen.
 *
 */
void regWrite(uint32_t addr, uint32_t data);

/* REG Write with mask
 *
 * @addr:        reg address(32bit)
 * @data:        data to be writen.
 * @mask:        only masked bits will be updated.
 *
 */
void regWriteMask(uint32_t addr, uint32_t data, uint32_t mask);

/* REG Read
 *
 * @addr:        reg address(32bit)
 * @buffer:      Pointer to the buffer where the data bytes that are received will be written to
 * @ndws:        Number of DWORDs to receive
 *
 */
void regReadBurst(uint32_t addr, uint32_t *buffer, unsigned int ndws);

/* REG Write
 *
 * @addr:        reg address(32bit)
 * @buffer:      Pointer to the buffer that holds data bytes to send
 * @ndws:        Number of DWORDs to send
 *
 */
void regWriteBurst(uint32_t addr, uint32_t *buffer, unsigned int ndws);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */
#endif /*__PERI_H__ */
