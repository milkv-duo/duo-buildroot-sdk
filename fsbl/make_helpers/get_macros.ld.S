#include <platform.h>
#include <cdefs.h>

#define MAKE_MACRO_TO_SYMBOL(macro) DEF_##macro = macro;

SECTIONS
{
	.text : {
		LONG(0xDEADBEEF);
		*(*)
	}

	MAKE_MACRO_TO_SYMBOL(DRAM_BASE)
	MAKE_MACRO_TO_SYMBOL(MONITOR_RUNADDR)
	MAKE_MACRO_TO_SYMBOL(BLCP_2ND_RUNADDR)
}
