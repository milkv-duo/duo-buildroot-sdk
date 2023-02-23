int cvi_board_init(void)
{
#if defined(CONFIG_MMC_SDHCI_CVITEK_WIFI)
	PINMUX_CONFIG(PWR_GPIO2, PWR_GPIO_2);
	PINMUX_CONFIG(PWR_WAKEUP0, PWR_GPIO_6);
#endif
        return 0;
}
