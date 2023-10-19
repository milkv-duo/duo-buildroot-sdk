//#include <stdint.h>
#include "printf.h"
#include "hal_pinmux.h"
#include "hal_dw_i2c.h"
#include "i2c.h"

int i2c_write(uint8_t i2c_id, uint8_t dev, uint16_t addr, uint16_t alen, uint8_t *buffer, uint16_t len)
{
	return hal_i2c_write(i2c_id, dev, addr, alen, buffer, len);
}

int i2c_read(uint8_t i2c_id, uint8_t dev, uint16_t addr, uint16_t alen, uint8_t *buffer, uint16_t len)
{
	return hal_i2c_read(i2c_id, dev, addr, alen, buffer, len);
}

int i2c_set_interstop(uint8_t i2c_id, bool use_interstop)
{
	hal_i2c_set_interstop(i2c_id, use_interstop);
}

int i2c_xfer(uint8_t i2c_id, struct i2c_msg msgs[], int num)
{
	return hal_i2c_xfer(i2c_id, msgs, num);
}

void i2c_init(uint8_t i2c_id)
{
	// hal_pinmux_config(PINMUX_I2C3);
	hal_i2c_init(i2c_id);
}
