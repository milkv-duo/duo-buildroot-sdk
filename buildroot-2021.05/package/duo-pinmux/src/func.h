struct funlist {
	char name[32];
	char func[32];
} funlist_st;

struct funlist cv180x_pin_func[] = {
	// GP0  IIC0_SCL
	{ "GP00", "JTAG_TDI"},
	{ "GP01", "UART1_TX"},
	{ "GP02", "UART2_TX"},
	{ "GP03", "GP0"},
	{ "GP04", "IIC0_SCL"},
	{ "GP05", "WG0_D0"},
	{ "GP07", "DBG_10"},

	// GP1  IIC0_SDA
	{ "GP10", "JTAG_TDO"},
	{ "GP11", "UART1_RX"},
	{ "GP12", "UART2_RX"},
	{ "GP13", "GP1"},
	{ "GP14", "IIC0_SDA"},
	{ "GP15", "WG0_D1"},
	{ "GP16", "WG1_D0"},
	{ "GP17", "DBG_11"},

	// GP2  SD1_GPIO1
	{ "GP21", "UART4_TX"},
	{ "GP23", "GP2"},
	{ "GP27", "PWM_10"},

	// GP3  SD1_GPIO0
	{ "GP31", "UART4_RX"},
	{ "GP33", "GP3"},
	{ "GP37", "PWM_11"},

	// GP4  SD1_D2
	{ "GP40", "PWR_SD1_D2"},
	{ "GP41", "IIC1_SCL"},
	{ "GP42", "UART2_TX"},
	{ "GP43", "GP4"},
	{ "GP44", "CAM_MCLK0"},
	{ "GP45", "UART3_TX"},
	{ "GP46", "PWR_SPINOR1_HOLD_X"},
	{ "GP47", "PWM_5"},

	// GP5  SD1_D1
	{ "GP50", "PWR_SD1_D1"},
	{ "GP51", "IIC1_SDA"},
	{ "GP52", "UART2_RX"},
	{ "GP53", "GP5"},
	{ "GP54", "CAM_MCLK1"},
	{ "GP55", "UART3_RX"},
	{ "GP56", "PWR_SPINOR1_WP_X"},
	{ "GP57", "PWM_6"},

	// GP6  SD1_CLK
	{ "GP60", "PWR_SD1_CLK"},
	{ "GP61", "SPI2_SCK"},
	{ "GP62", "IIC3_SDA"},
	{ "GP63", "GP6"},
	{ "GP64", "CAM_HS0"},
	{ "GP65", "EPHY_SPD_LED"},
	{ "GP66", "PWR_SPINOR1_SCK"},
	{ "GP67", "PWM_9"},

	// GP7  SD1_CMD
	{ "GP70", "PWR_SD1_CMD"},
	{ "GP71", "SPI2_SDO"},
	{ "GP72", "IIC3_SCL"},
	{ "GP73", "GP7"},
	{ "GP74", "CAM_VS0"},
	{ "GP75", "EPHY_LNK_LED"},
	{ "GP76", "PWR_SPINOR1_MOSI"},
	{ "GP77", "PWM_8"},

	// GP8  SD1_D0
	{ "GP80", "PWR_SD1_D0"},
	{ "GP81", "SPI2_SDI"},
	{ "GP82", "IIC1_SDA"},
	{ "GP83", "GP8"},
	{ "GP84", "CAM_MCLK1"},
	{ "GP85", "UART3_RTS"},
	{ "GP86", "PWR_SPINOR1_MISO"},
	{ "GP87", "PWM_7"},

	// GP9  SD1_D3
	{ "GP90", "PWR_SD1_D3"},
	{ "GP91", "SPI2_CS_X"},
	{ "GP92", "IIC1_SCL"},
	{ "GP93", "GP9"},
	{ "GP94", "CAM_MCLK0"},
	{ "GP95", "UART3_CTS"},
	{ "GP96", "PWR_SPINOR1_CS_X"},
	{ "GP97", "PWM_4"},

	// GP10  PAD_MIPIRX1P
	{ "GP101", "VI0_D_6"},
	{ "GP103", "GP10"},
	{ "GP104", "IIC1_SDA"},
	{ "GP106", "KEY_ROW2"},
	{ "GP107", "DBG_9"},

	// GP11  PAD_MIPIRX0N
	{ "GP111", "VI0_D_7"},
	{ "GP113", "GP11"},
	{ "GP114", "IIC1_SCL"},
	{ "GP115", "CAM_MCLK1"},
	{ "GP117", "DBG_10"},

	// GP12  UART0_TX
	{ "GP120", "UART0_TX"},
	{ "GP121", "CAM_MCLK1"},
	{ "GP122", "PWM_4"},
	{ "GP123", "GP12"},
	{ "GP124", "UART1_TX"},
	{ "GP125", "AUX1"},
	{ "GP126", "JTAG_TMS"},
	{ "GP127", "DBG_6"},

	// GP13  UART0_RX
	{ "GP130", "UART0_RX"},
	{ "GP131", "CAM_MCLK0"},
	{ "GP132", "PWM_5"},
	{ "GP133", "GP13"},
	{ "GP134", "UART1_RX"},
	{ "GP135", "AUX0"},
	{ "GP136", "JTAG_TCK"},
	{ "GP137", "DBG_7"},

	// GP14  SD0_PWR_EN
	{ "GP140", "SDIO0_PWR_EN"},
	{ "GP143", "GP14"},

	// GP15  SPK_EN
	{ "GP153", "GP15"},

	// GP16  SPINOR_MISO
	{ "GP161", "SPINOR_MISO"},
	{ "GP162", "SPINAND_MISO"},
	{ "GP163", "GP16"},

	// GP17  SPINOR_CS_X
	{ "GP171", "SPINOR_CS_X"},
	{ "GP172", "SPINAND_CS"},
	{ "GP173", "GP17"},

	// GP18  SPINOR_SCK
	{ "GP181", "SPINOR_SCK"},
	{ "GP182", "SPINAND_CLK"},
	{ "GP183", "GP18"},

	// GP19  SPINOR_MOSI
	{ "GP191", "SPINOR_MOSI"},
	{ "GP192", "SPINAND_MOSI"},
	{ "GP193", "GP19"},

	// GP20  SPINOR_WP_X
	{ "GP201", "SPINOR_WP_X"},
	{ "GP202", "SPINAND_WP"},
	{ "GP203", "GP20"},

	// GP21  SPINOR_HOLD_X
	{ "GP211", "SPINOR_HOLD_X"},
	{ "GP212", "SPINAND_HOLD"},
	{ "GP213", "GP21"},

	// GP22  PWR_SEQ2
	{ "GP220", "PWR_SEQ2"},
	{ "GP223", "GP22"},

	// GP25  PAD_AUD_AOUTR
	{ "GP253", "GP25"},
	{ "GP254", "IIS1_DI"},
	{ "GP255", "IIS2_DO"},
	{ "GP256", "IIS1_DO"},

	// GP26  ADC1
	{ "GP263", "GP26"},
	{ "GP264", "KEY_COL2"},
	{ "GP266", "PWM_3"},

	// GP27  USB_VBUS_DET
	{ "GP270", "USB_VBUS_DET"},
	{ "GP273", "GP27"},
	{ "GP274", "CAM_MCLK0"},
	{ "GP275", "CAM_MCLK1"},
	{ "GP276", "PWM_4"},
};
