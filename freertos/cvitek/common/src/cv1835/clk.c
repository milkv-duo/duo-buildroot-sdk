#include <stdbool.h>
#include <linux/kernel.h>

struct clk {

};

bool __clk_is_enabled(struct clk *clk)
{
	return 0;
}

/* clk_disable_unprepare helps cases using clk_disable in non-atomic context. */
void clk_disable_unprepare(struct clk *clk)
{
	// clk_disable(clk);
	// clk_unprepare(clk);
}

clk_disable(struct clk *clk)
{
}

/* clk_prepare_enable helps cases using clk_enable in non-atomic context. */
int clk_prepare_enable(struct clk *clk)
{
	return 0;
}

int clk_prepare(struct clk *clk)
{
	might_sleep();
	return 0;
}

void clk_unprepare(struct clk *clk)
{
	might_sleep();
}

struct clk *devm_clk_get(struct device *dev, const char *id)
{
	return 0;
}
