int cvi_board_init(void)
{
#if defined(CONFIG_MMC_SDHCI_CVITEK_WIFI)
	PINMUX_CONFIG(JTAG_CPU_TCK, XGPIOA_18);
	PINMUX_CONFIG(PWR_WAKEUP1, PWR_GPIO_7);

        /* Pull WIFI_CHIP_EN to high */
	writel(readl(0x03020004) | WIFI_CHIP_EN_BGA, 0x03020004);
	writel(readl(0x03020000) | WIFI_CHIP_EN_BGA, 0x03020000);
#endif
        return 0;
}
