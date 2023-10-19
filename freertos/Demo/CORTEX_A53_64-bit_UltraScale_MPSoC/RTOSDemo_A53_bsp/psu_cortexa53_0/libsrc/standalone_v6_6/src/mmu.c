#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "system.h"
#include "mmu.h"

#define __weak __attribute__((weak))

#define PAGE_TABLE_NUM 8
//#define __DEBUG__

#ifdef __DEBUG__
#define debug xil_printf
#else
#define debug(...) {}
#endif
#define printf xil_printf

#define min(x, y) ({                \
        typeof(x) _min1 = (x);          \
        typeof(y) _min2 = (y);          \
        (void) (&_min1 == &_min2);      \
        _min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({                \
                typeof(x) _max1 = (x);          \
                typeof(y) _max2 = (y);          \
                (void) (&_max1 == &_max2);      \
                _max1 > _max2 ? _max1 : _max2; })

typedef unsigned long ULONG;

#define panic(fmt, args...) do { \
	printf(fmt, ##args);	\
	printf("\n");	\
        asm volatile("b . ;"); \
        while(1); \
} while (0)

uint64_t g_tlb_addr;
uint64_t g_tlb_size;
uint64_t g_tlb_fillptr = 0;

// need to modify memory map and attributes later
static struct mm_region vexpress64_mem_map[] = {
	{

		.virt = 0x0UL,
		.phys = 0x0UL,
		.size = 0xc000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN

	}, {
		.virt = 0xc000000UL,
		.phys = 0xc000000UL,
		.size = 0x0100000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	}, {
		.virt = 0xe000000UL,
		.phys = 0xe000000UL,
		.size = 0x0010000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE

	}, {
		.virt = 0x110000000UL,
		.phys = 0x110000000UL,
		.size = 0x010000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	}, {
		.virt = 0x120000000UL,
		.phys = 0x120000000UL,
		.size = 0x001000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL_NC) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_OUTER_SHARE
	}, {

		/* List terminator */
		0,
	}
};

struct mm_region *mem_map = vexpress64_mem_map;


static inline unsigned int current_el(void)
{
	unsigned int el=1;
	asm volatile("mrs %0, CurrentEL" : "=r" (el) : : "cc");
	return el >> 2;
}

static inline unsigned int get_sctlr(void)
{
	unsigned int val=0;
        unsigned int el=0;
	el = current_el();
	if (el == 1)
		asm volatile("mrs %0, sctlr_el1" : "=r" (val) : : "cc");
	else if (el == 2)
		asm volatile("mrs %0, sctlr_el2" : "=r" (val) : : "cc");
	else
		asm volatile("mrs %0, sctlr_el3" : "=r" (val) : : "cc");
	return val;
}


static inline void set_sctlr(unsigned int val)
{
	unsigned int el;
	el = current_el();
	if (el == 1)
		asm volatile("msr sctlr_el1, %0" : : "r" (val) : "cc");
	else if (el == 2)
		asm volatile("msr sctlr_el2, %0" : : "r" (val) : "cc");
	else
		asm volatile("msr sctlr_el3, %0" : : "r" (val) : "cc");

	asm volatile("isb");
}

static inline unsigned long read_mpidr(void)
{
	unsigned long val=0;

	asm volatile("mrs %0, mpidr_el1" : "=r" (val));

	return val;
}


/*
 *  With 4k page granule, a virtual address is split into 4 lookup parts
 *  spanning 9 bits each:
 *
 *    _______________________________________________
 *   |       |       |       |       |       |       |
 *   |   0   |  Lv0  |  Lv1  |  Lv2  |  Lv3  |  off  |
 *   |_______|_______|_______|_______|_______|_______|
 *     63-48   47-39   38-30   29-21   20-12   11-00
 *
 *             mask        page size
 *
 *    Lv0: FF8000000000       --
 *    Lv1:   7FC0000000       1G
 *    Lv2:     3FE00000       2M
 *    Lv3:       1FF000       4K
 *    off:          FFF
 */

uint64_t get_tcr(int el, uint64_t *pips, uint64_t *pva_bits)
{
	uint64_t max_addr = 0;
	uint64_t ips, va_bits;
	uint64_t tcr;
	int i;

        debug("1mem_map[%d].virt=%x\n", 0, mem_map[0].virt);
        debug("1mem_map[%d].size=%x\n", 0, mem_map[0].size);
	/* Find the largest address we need to support */
	for (i = 0; mem_map[i].size || mem_map[i].attrs; i++)
        {
                debug("mem_map[%d].virt=%x\n", i, mem_map[i].virt);
                debug("mem_map[%d].size=%x\n", i, mem_map[i].size);
           
		max_addr = max(max_addr, mem_map[i].virt + mem_map[i].size);
	        debug("get_tcr max_addr=%x\n",max_addr);
        }
	/* Calculate the maximum physical (and thus virtual) address */
	if (max_addr > (1ULL << 44)) {
		ips = 5;
		va_bits = 48;
	} else  if (max_addr > (1ULL << 42)) {
		ips = 4;
		va_bits = 44;
	} else  if (max_addr > (1ULL << 40)) {
		ips = 3;
		va_bits = 42;
	} else  if (max_addr > (1ULL << 36)) {
		ips = 2;
		va_bits = 40;
	} else  if (max_addr > (1ULL << 32)) {
		ips = 1;
		va_bits = 36;
	} else {
		ips = 0;
		va_bits = 32;
	}

	if (el == 1) {
		tcr = TCR_EL1_RSVD | (ips << 32) | TCR_EPD1_DISABLE;
	} else if (el == 2) {
		tcr = TCR_EL2_RSVD | (ips << 16);
	} else {
		tcr = TCR_EL3_RSVD | (ips << 16);
	}

	/* PTWs cacheable, inner/outer WBWA and inner shareable */
	tcr |= TCR_TG0_4K | TCR_SHARED_INNER | TCR_ORGN_WBWA | TCR_IRGN_WBWA;
	tcr |= TCR_T0SZ(va_bits);

	if (pips)
		*pips = ips;
	if (pva_bits)
		*pva_bits = va_bits;

	return tcr;
}

#define MAX_PTE_ENTRIES 512

static int pte_type(uint64_t *pte)
{
	return *pte & PTE_TYPE_MASK;
}

/* Returns the LSB number for a PTE on level <level> */
static int level2shift(int level)
{
	/* Page is 12 bits wide, every level translates 9 bits */
	return (12 + 9 * (3 - level));
}

static uint64_t *find_pte(uint64_t addr, int level)
{
	int start_level = 0;
	uint64_t *pte;
	uint64_t idx;
	uint64_t va_bits;
	int i;

	debug("addr=%lx level=%d\n", addr, level);

	get_tcr(0, NULL, &va_bits);
	if (va_bits < 39)
		start_level = 1;
	debug("level=%d start_level=%d\n", level, start_level);
	if (level < start_level)
		return NULL;

	/* Walk through all page table levels to find our PTE */
	pte = (uint64_t*)g_tlb_addr;
        debug("pte=%x\n",pte);
	for (i = start_level; i < 4; i++) {
		idx = (addr >> level2shift(i)) & 0x1FF;
		pte += idx;
		debug("idx=%lx PTE %p at level %d: %lx\n", idx, pte, i, *pte);

		/* Found it */
		if (i == level) {
			return pte;
                }
		/* PTE is no table (either invalid or block), can't traverse */
		if (pte_type(pte) != PTE_TYPE_TABLE)
			return NULL;
		/* Off to the next level */
		pte = (uint64_t*)(*pte & 0x0000fffffffff000ULL);
	}

	/* Should never reach here */
	return NULL;
}

/* Returns and creates a new full table (512 entries) */
static uint64_t *create_table(void)
{
	uint64_t *new_table = (uint64_t*)g_tlb_fillptr;
	uint64_t pt_len = MAX_PTE_ENTRIES * sizeof(uint64_t);

	/* Allocate MAX_PTE_ENTRIES pte entries */
	g_tlb_fillptr += pt_len;

	if (g_tlb_fillptr - g_tlb_addr > g_tlb_size)
		panic("Insufficient RAM for page table: 0x%lx > 0x%lx. "
		      "Please increase the size in get_page_table_size()",
			g_tlb_fillptr - g_tlb_addr,
			g_tlb_size);

	/* Mark all entries as invalid */
	memset(new_table, 0, pt_len);

	return new_table;
}

static void set_pte_table(uint64_t *pte, uint64_t *table)
{
	*pte = PTE_TYPE_TABLE | (ULONG)table;
}

/* Splits a block PTE into table with subpages spanning the old block */
static void split_block(uint64_t *pte, int level)
{
	uint64_t old_pte = *pte;
	uint64_t *new_table;
	uint64_t i = 0;
	/* level describes the parent level, we need the child ones */
	int levelshift = level2shift(level + 1);

	if (pte_type(pte) != PTE_TYPE_BLOCK)
		panic("PTE %p (%lx) is not a block. Some driver code wants to "
		      "modify dcache settings for an range not covered in "
		      "mem_map.", pte, old_pte);

	new_table = create_table();
	debug("Splitting pte %p (%lx) into %p\n", pte, old_pte, new_table);

	for (i = 0; i < MAX_PTE_ENTRIES; i++) {
		new_table[i] = old_pte | (i << levelshift);

		/* Level 3 block PTEs have the table type */
		if ((level + 1) == 3)
			new_table[i] |= PTE_TYPE_TABLE;

		debug("Setting new_table[%ld] = %lx\n", i, new_table[i]);
	}

	/* Set the new table into effect */
	set_pte_table(pte, new_table);
}

/* Add one mm_region map entry to the page tables */
static void add_map(struct mm_region *map)
{
	uint64_t *pte;
	uint64_t virt = map->virt;
	uint64_t phys = map->phys;
	uint64_t size = map->size;
	uint64_t attrs = map->attrs | PTE_TYPE_BLOCK | PTE_BLOCK_AF;
	uint64_t blocksize;
	int level;
	uint64_t *new_table;

	while (size) {
		pte = find_pte(virt, 0);
		if (pte && (pte_type(pte) == PTE_TYPE_FAULT)) {
			debug("Creating table for virt 0x%lx\n", virt);
			new_table = create_table();
			set_pte_table(pte, new_table);
		}

		for (level = 1; level < 4; level++) {
			pte = find_pte(virt, level);
			if (!pte)
				panic("pte not found\n");

			blocksize = 1ULL << level2shift(level);
			debug("Checking if pte fits for virt=%lx size=%lx blocksize=%lx\n",
			      virt, size, blocksize);
			if (size >= blocksize && !(virt & (blocksize - 1))) {
				/* Page fits, create block PTE */
				debug("Setting PTE %p to block virt=%lx\n",
				      pte, virt);
                                if (level == 3)					
	  				*pte = phys | attrs | PTE_TYPE_TABLE;
				else
	  				*pte = phys | attrs;
				virt += blocksize;
				phys += blocksize;
				size -= blocksize;
				break;
			} else if (pte_type(pte) == PTE_TYPE_FAULT) {
				/* Page doesn't fit, create subpages */
				debug("Creating subtable for virt 0x%lx blksize=%lx\n",
				      virt, blocksize);
				new_table = create_table();
				set_pte_table(pte, new_table);
			} else if (pte_type(pte) == PTE_TYPE_BLOCK) {
				debug("Split block into subtable for virt 0x%lx blksize=0x%lx\n",
				      virt, blocksize);
				split_block(pte, level);
			} else {
                        	debug("pte_type(pte)= %d\n", pte_type(pte) );
        		 	//panic("pt");
                        }
		}
	}
}

enum pte_type {
	PTE_INVAL,
	PTE_BLOCK,
	PTE_LEVEL,
};

/*
 * This is a recursively called function to count the number of
 * page tables we need to cover a particular PTE range. If you
 * call this with level = -1 you basically get the full 48 bit
 * coverage.
 */
static int count_required_pts(uint64_t addr, int level, uint64_t maxaddr)
{
	int levelshift = level2shift(level);
	uint64_t levelsize = 1ULL << levelshift;
	uint64_t levelmask = levelsize - 1;
	uint64_t levelend = addr + levelsize;
	int r = 0;
	int i;
	enum pte_type pte_type = PTE_INVAL;

	for (i = 0; mem_map[i].size || mem_map[i].attrs; i++) {
		struct mm_region *map = &mem_map[i];
		uint64_t start = map->virt;
		uint64_t end = start + map->size;

		/* Check if the PTE would overlap with the map */
		if (max(addr, start) <= min(levelend, end)) {
			start = max(addr, start);
			end = min(levelend, end);

			/* We need a sub-pt for this level */
			if ((start & levelmask) || (end & levelmask)) {
				pte_type = PTE_LEVEL;
				break;
			}

			/* Lv0 can not do block PTEs, so do levels here too */
			if (level <= 0) {
				pte_type = PTE_LEVEL;
				break;
			}

			/* PTE is active, but fits into a block */
			pte_type = PTE_BLOCK;
		}
	}

	/*
	 * Block PTEs at this level are already covered by the parent page
	 * table, so we only need to count sub page tables.
	 */
	if (pte_type == PTE_LEVEL) {
		int sublevel = level + 1;
		uint64_t sublevelsize = 1ULL << level2shift(sublevel);

		/* Account for the new sub page table ... */
		r = 1;

		/* ... and for all child page tables that one might have */
		for (i = 0; i < MAX_PTE_ENTRIES; i++) {
			r += count_required_pts(addr, sublevel, maxaddr);
			addr += sublevelsize;

			if (addr >= maxaddr) {
				/*
				 * We reached the end of address space, no need
				 * to look any further.
				 */
				break;
			}
		}
	}

	return r;
}

/* Returns the estimated required size of all page tables */
__weak uint64_t get_page_table_size(void)
{
	uint64_t one_pt = MAX_PTE_ENTRIES * sizeof(uint64_t);
	uint64_t size = 0;
	uint64_t va_bits;
	int start_level = 0;

	get_tcr(0, NULL, &va_bits);
	if (va_bits < 39)
		start_level = 1;

	/* Account for all page tables we would need to cover our memory map */
	size = one_pt * count_required_pts(0, start_level - 1, 1ULL << va_bits);

	/*
	 * We need to duplicate our page table once to have an emergency pt to
	 * resort to when splitting page tables later on
	 */
	size *= 2;

	/*
	 * We may need to split page tables later on if dcache settings change,
	 * so reserve up to 4 (random pick) page tables for that.
	 */
	size += one_pt * 4;

	return size;
}

void setup_pgtables(void)
{
	int i;

	if (!g_tlb_fillptr || !g_tlb_addr)
		panic("Page table pointer not setup.");

	/*
	 * Allocate the first level we're on with invalidate entries.
	 * If the starting level is 0 (va_bits >= 39), then this is our
	 * Lv0 page table, otherwise it's the entry Lv1 page table.
	 */
	create_table();

	/* Now add all MMU table entries one after another to the table */
	for (i = 0; mem_map[i].size || mem_map[i].attrs; i++)
        {
                debug("add_map %d\n",i);
		add_map(&mem_map[i]);
        }
}

static void setup_all_pgtables(void)
{
	/* Reset the fill ptr */
	g_tlb_fillptr = g_tlb_addr;
	debug("g_tlb_fillptr=%lx\n",g_tlb_fillptr);
	/* Create normal system page tables */
	setup_pgtables();

}


/* to activate the MMU we need to set up virtual memory */
__weak void mmu_setup(void)
{
	int el;

	g_tlb_size = (MAX_PTE_ENTRIES * sizeof(uint64_t)) * PAGE_TABLE_NUM;
	g_tlb_addr = (uint64_t)memalign(4096, g_tlb_size);

	debug("g_tlb_size=%lx\n", g_tlb_size);
	debug("g_tlb_addr=%lx\n", g_tlb_addr);
	/* Set up page tables only once */
	if (!g_tlb_fillptr)
		setup_all_pgtables();

	el = current_el();
	set_ttbr_tcr_mair(el, g_tlb_addr, get_tcr(el, NULL, NULL),
			  MEMORY_ATTRIBUTES);
	/* enable the mmu */
	set_sctlr(get_sctlr() | CR_M);
}

/*
 * Performs a invalidation of the entire data cache at all levels
 */
void invalidate_dcache_all(void)
{
	__asm_invalidate_dcache_all();
}

/*
 * Performs a clean & invalidation of the entire data cache at all levels.
 * This function needs to be inline to avoid using stack.
 */
inline void flush_dcache_all(void)
{
	int ret;

	__asm_flush_dcache_all();
}

/*
 * Invalidates range in all levels of D-cache/unified cache
 */
void invalidate_dcache_range(unsigned long start, unsigned long stop)
{
	__asm_invalidate_dcache_range(start, stop);
}

/*
 * Flush range(clean & invalidate) from all levels of D-cache/unified cache
 */
void flush_dcache_range(unsigned long start, unsigned long stop)
{
	__asm_flush_dcache_range(start, stop);
}

void dcache_enable(void)
{
	/* The data cache is not active unless the mmu is enabled */
	if (!(get_sctlr() & CR_M)) {
		invalidate_dcache_all();
		__asm_invalidate_tlb_all();
		mmu_setup();
	}

	set_sctlr(get_sctlr() | CR_C);
	debug("\ndcache_setup done....\n");
}

void dcache_disable(void)
{
	uint32_t sctlr;

	sctlr = get_sctlr();

	/* if cache isn't enabled no need to disable */
	if (!(sctlr & CR_C))
		return;

	set_sctlr(sctlr & ~(CR_C|CR_M));

	flush_dcache_all();
	__asm_invalidate_tlb_all();
}

int dcache_status(void)
{
	return (get_sctlr() & CR_C) != 0;
}

void invalidate_icache_all(void)
{
	__asm_invalidate_icache_all();
}

void icache_enable(void)
{
	invalidate_icache_all();
	set_sctlr(get_sctlr() | CR_I);
}

void icache_disable(void)
{
	set_sctlr(get_sctlr() & ~CR_I);
}

int icache_status(void)
{
	return (get_sctlr() & CR_I) != 0;
}

void enable_caches(void)
{
	icache_enable();
	dcache_enable();
}
