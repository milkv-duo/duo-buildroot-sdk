#include <debug.h>
#include <console.h>
#include <platform.h>
#include <delay_timer.h>

struct fw_dynamic_info {
	/** Info magic */
	unsigned long magic;
	/** Info version */
	unsigned long version;
	/** Next booting stage address */
	unsigned long next_addr;
	/** Next booting stage mode */
	unsigned long next_mode;
	/** Options for OpenSBI library */
	unsigned long options;
	/**
	 * Preferred boot HART id
	 *
	 * It is possible that the previous booting stage uses same link
	 * address as the FW_DYNAMIC firmware. In this case, the relocation
	 * lottery mechanism can potentially overwrite the previous booting
	 * stage while other HARTs are still running in the previous booting
	 * stage leading to boot-time crash. To avoid this boot-time crash,
	 * the previous booting stage can specify last HART that will jump
	 * to the FW_DYNAMIC firmware as the preferred boot HART.
	 *
	 * To avoid specifying a preferred boot HART, the previous booting
	 * stage can set it to -1UL which will force the FW_DYNAMIC firmware
	 * to use the relocation lottery mechanism.
	 */
	unsigned long boot_hart;
} __packed;

/** Expected value of info magic ('OSBI' ascii string in hex) */
#define FW_DYNAMIC_INFO_MAGIC_VALUE 0x4942534f

/** Maximum supported info version */
#define FW_DYNAMIC_INFO_VERSION_MAX 0x2

/** Possible next mode values */
#define FW_DYNAMIC_INFO_NEXT_MODE_U 0x0
#define FW_DYNAMIC_INFO_NEXT_MODE_S 0x1
#define FW_DYNAMIC_INFO_NEXT_MODE_M 0x3

static struct fw_dynamic_info fw_dynamic_info;

void jump_to_monitor(uintptr_t monitor_entry, uintptr_t next_addr)
{
	void (*entry)(unsigned long arg0, unsigned long arg1, struct fw_dynamic_info *) = (void *)monitor_entry;

	unsigned long arg0 = 0, arg1 = OPENSBI_FDT_ADDR;

	fw_dynamic_info.magic = FW_DYNAMIC_INFO_MAGIC_VALUE;
	fw_dynamic_info.next_addr = next_addr;
	fw_dynamic_info.next_mode = FW_DYNAMIC_INFO_NEXT_MODE_S;
	fw_dynamic_info.boot_hart = -1;

	NOTICE("OPENSBI: next_addr=0x%lx arg1=0x%lx", next_addr, arg1);

	flush_dcache_range((uintptr_t)&fw_dynamic_info, sizeof(struct fw_dynamic_info));

	time_records->fsbl_exit = read_time_ms();
	entry(arg0, arg1, &fw_dynamic_info);
}

void jump_to_loader_2nd(uintptr_t loader_2nd_entry)
{
}
