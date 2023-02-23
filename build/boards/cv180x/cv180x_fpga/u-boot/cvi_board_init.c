int cvi_board_init(void)
{
	PINMUX_CONFIG(SD1_CMD, IIC3_SCL);
	PINMUX_CONFIG(SD1_CLK, IIC3_SDA);

	return 0;
}
