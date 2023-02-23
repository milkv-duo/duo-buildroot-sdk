int cvi_board_init(void)
{
#if defined(CONFIG_MMC_SDHCI_CVITEK_WIFI)
	PINMUX_CONFIG(JTAG_CPU_TCK, XGPIOA_18);
	PINMUX_CONFIG(PWR_WAKEUP1, PWR_GPIO_7);
#endif
        return 0;
}
