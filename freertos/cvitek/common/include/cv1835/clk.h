#ifndef __LINUX_CLK_H__
#define __LINUX_CLK_H__

#include <stdbool.h>
#include <linux/kernel.h>

struct clk;

bool __clk_is_enabled(struct clk *clk);
void clk_disable_unprepare(struct clk *clk);
void clk_disable(struct clk *clk);
int clk_prepare_enable(struct clk *clk);
int clk_prepare(struct clk *clk);
void clk_unprepare(struct clk *clk);

struct clk *devm_clk_get(struct device *dev, const char *id);

#endif //__LINUX_CLK_H__
