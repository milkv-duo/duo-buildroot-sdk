#include "devmem.h"
#include "func.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))
#define PINMUX_BASE 0x03001000
#define INVALID_PIN 9999

struct pinlist {
	char name[32];
	uint32_t offset;
} pinlist_st;

struct pinlist cv180x_pin[] = {
	{ "GP0", 0x4c },				// IIC0_SCL
	{ "GP1", 0x50 },				// IIC0_SDA
	{ "GP2", 0x84 },				// SD1_GPIO1
	{ "GP3", 0x88 },				// SD1_GPIO0
	{ "GP4", 0x90 },				// SD1_D2
	{ "GP5", 0x94 },				// SD1_D1
	{ "GP6", 0xa0 },				// SD1_CLK
	{ "GP7", 0x9c },				// SD1_CMD
	{ "GP8", 0x98 },				// SD1_D0
	{ "GP9", 0x8c },				// SD1_D3
	{ "GP10", 0xf0 },				// PAD_MIPIRX1P
	{ "GP11", 0xf4 },				// PAD_MIPIRX0N
	{ "GP12", 0x24 },				// UART0_TX
	{ "GP13", 0x28 },				// UART0_RX
	{ "GP14", 0x1c },				// SD0_PWR_EN
	{ "GP15", 0x20 },				// SPK_EN
	{ "GP16", 0x3c },				// SPINOR_MISO
	{ "GP17", 0x40 },				// SPINOR_CS_X
	{ "GP18", 0x30 },				// SPINOR_SCK
	{ "GP19", 0x34 },				// SPINOR_MOSI
	{ "GP20", 0x38 },				// SPINOR_WP_X
	{ "GP21", 0x2c },				// SPINOR_HOLD_X
	{ "GP22", 0x68 },				// PWR_SEQ2
	{ "GP26", 0xa8 },				// ADC1
	{ "GP27", 0xac },				// USB_VBUS_DET

	{ "GP25", 0x12c },				// PAD_AUD_AOUTR
};

uint32_t convert_func_to_value(char *pin, char *func)
{
	uint32_t i = 0;
	uint32_t max_fun_num = NELEMS(cv180x_pin_func);
	char v;

	for (i = 0; i < max_fun_num; i++) {
		if (strcmp(cv180x_pin_func[i].func, func) == 0) {
			if (strncmp(cv180x_pin_func[i].name, pin, strlen(pin)) == 0) {
				v = cv180x_pin_func[i].name[strlen(cv180x_pin_func[i].name) - 1];
				break;
			}
		}
	}

	if (i == max_fun_num) {
		printf("ERROR: invalid pin or func\n");
		return INVALID_PIN;
	}

	return (v - 0x30);
}

void print_fun(char *name, uint32_t value)
{
	uint32_t i = 0;
	uint32_t max_fun_num = NELEMS(cv180x_pin_func);
	char pinname[128];

	sprintf(pinname, "%s%d", name, value);

	printf("%s function:\n", name);
	for (i = 0; i < max_fun_num; i++) {
		if (strlen(cv180x_pin_func[i].name) != (strlen(name) + 1)) {
			continue;
		}
		if (strncmp(pinname, cv180x_pin_func[i].name, strlen(name)) == 0) {
			if (strcmp(pinname, cv180x_pin_func[i].name) == 0)
				printf("[v] %s\n", cv180x_pin_func[i].func);
			else
				printf("[ ] %s\n", cv180x_pin_func[i].func);
			// break;
		}
	}
	printf("\n");
}

void print_usage(const char *prog)
{
	printf("pinmux for duo\n");
	printf("%s -p          <== List all pins\n", prog);
	printf("%s -l          <== List all pins and its func\n", prog);
	printf("%s -r pin      <== Get func from pin\n", prog);
	printf("%s -w pin/func <== Set func to pin\n", prog);
}

int main(int argc, char *argv[])
{
	int opt = 0;
	uint32_t i = 0;
	uint32_t value;
	char pin[32];
	char func[32];
	uint32_t f_val;

	if (argc == 1) {
		print_usage(argv[0]);
		return 1;
	}

	while ((opt = getopt(argc, argv, "hplr:w:")) != -1) {
		switch (opt) {
		case 'r':
			for (i = 0; i < NELEMS(cv180x_pin); i++) {
				if (strcmp(optarg, cv180x_pin[i].name) == 0)
					break;
			}
			if (i != NELEMS(cv180x_pin)) {
				value = devmem_readl(PINMUX_BASE + cv180x_pin[i].offset);
				// printf("value %d\n", value);
				print_fun(optarg, value);

				printf("register: 0x%x\n", PINMUX_BASE + cv180x_pin[i].offset);
				printf("value: %d\n", value);
			} else {
				printf("\nInvalid option: %s", optarg);
			}
			break;

		case 'w':
			// printf("optarg %s\n", optarg);
			if (sscanf(optarg, "%[^/]/%s", pin, func) != 2)
				print_usage(argv[0]);

			printf("pin %s\n", pin);
			printf("func %s\n", func);

			for (i = 0; i < NELEMS(cv180x_pin); i++) {
				if (strcmp(pin, cv180x_pin[i].name) == 0)
					break;
			}

			if (i != NELEMS(cv180x_pin)) {
				f_val = convert_func_to_value(pin, func);
				if (f_val == INVALID_PIN)
					return 1;
				devmem_writel(PINMUX_BASE + cv180x_pin[i].offset, f_val);

				printf("register: %x\n", PINMUX_BASE + cv180x_pin[i].offset);
				printf("value: %d\n", f_val);
				// printf("value %d\n", value);
			} else {
				printf("\nInvalid option: %s\n", optarg);
			}
			break;

		case 'p':
			printf("Pinlist:\n");
			for (i = 0; i < NELEMS(cv180x_pin); i++)
				printf("%s\n", cv180x_pin[i].name);
			break;

		case 'l':
			for (i = 0; i < NELEMS(cv180x_pin); i++) {
				value = devmem_readl(PINMUX_BASE + cv180x_pin[i].offset);
				// printf("value %d\n", value);
				print_fun(cv180x_pin[i].name, value);
			}
			break;

		case 'h':
			print_usage(argv[0]);
			break;

		case '?':
			print_usage(argv[0]);
			break;

		default:
			print_usage(argv[0]);
			break;
		}
	}

	return 0;
}
