#ifndef __DDR_H__
#define __DDR_H__

struct ddr_param {
	uint8_t data[1024 * 16];
};

int ddr_init(const struct ddr_param *ddr_param);


#endif /* __DDR_H__ */
