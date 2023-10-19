#include "linux/printk.h"
#include "rtos_malloc.h"
#include "rtos_cmdqu.h"

#define HEADER_SIZE sizeof(Header)
#define NUNITS(bytes) bytes / HEADER_SIZE

static Header *increase_heap(unsigned int, Header *prev);
static Header *free_list = NULL;
static Header *base;
static size_t HeapBase = 0;
static size_t HeapLimit = 0;
static size_t offset = 0;
spinlock_t *rtos_memory_lock;

void* memory_alloc(size_t bytes)
{
	unsigned long flags;
	spin_lock_irqsave(rtos_memory_lock, flags);
	Header *block;
	Header *previous = base;
	unsigned int nunits = NUNITS(bytes);
	if(!nunits)
		nunits = 1;
	if(!free_list){
		// first block
		Header *block = increase_heap(nunits, base);
		block->next = NULL;
		block->next_vpa = NULL;
		base->size = 0;
		base->next = block;
		base->next_vpa = block + offset;
		free_list = base;
		spin_unlock_irqrestore(rtos_memory_lock, flags);
		return block + 1;
	}
	//search a memory block with enough size
	for(block = base->next; block != NULL ; previous = block, block = block->next ){
		/* the lastest block, so increase heap size directly */
		if(block->next == NULL) {
			previous = block;
			break;
		}
		if(nunits + 1 == block->next - block - (block->size + 1)) {
			/* need block is the same as reserved block*/
			block->next = block + 1 + block->size;
			block->next_vpa = block + 1 + block->size + offset;
			block->next->size = nunits;
			spin_unlock_irqrestore(rtos_memory_lock, flags);
			return block->next + 1;
		} else if( nunits + 1 < block->next - block - (block->size + 1)) {
			/* split large block into small block */
			Header *need_block = block + 1 + block->size;
			need_block->size = nunits;
			need_block->next = block->next;
			need_block->next_vpa = block->next_vpa;
			block->next = need_block;
			block->next_vpa = need_block + offset;
			spin_unlock_irqrestore(rtos_memory_lock, flags);
			return need_block + 1;
		}
	}
	/* increase more memory directly */
	block = increase_heap(nunits, previous);
	spin_unlock_irqrestore(rtos_memory_lock, flags);
	return block + 1;
}

void memory_free(void* ptr)
{
	unsigned long flags;
	spin_lock_irqsave(rtos_memory_lock, flags);
	Header *fblock = ((Header*)ptr) - 1;
	Header *block;
	Header *previous = base;
	for(block = base->next; block != NULL; previous = block, block = block->next){
		if(fblock == block) {
			if(fblock->next) {
				previous->next = fblock->next;
				previous->next_vpa = fblock->next_vpa;
			} else {
				previous->next = NULL;
				previous->next_vpa = NULL;
				/* set heap back to fblock address */
				base->heap_ptr -= (fblock->size + 1) * HEADER_SIZE;
				base->heap_ptr_vpa -= (fblock->size + 1) * HEADER_SIZE;
				/* set heap back to previois block + block size address */
				base->heap_ptr -= (fblock - previous - previous->size - 1) * HEADER_SIZE;
				base->heap_ptr_vpa -= (fblock - previous - previous->size - 1) * HEADER_SIZE;
				pr_debug("fblock=%lx previous=%lx size=%x\n",fblock, previous, previous->size );
				pr_debug("heap_ptr= %lx ptr_va=%lx\n", base->heap_ptr , base->heap_ptr_vpa);
			}
			spin_unlock_irqrestore(rtos_memory_lock, flags);
			return;
		}
	}
	printk("Double free or invalid pointer=%lx\n", ptr);
	spin_unlock_irqrestore(rtos_memory_lock, flags);
}
/* init heap with virtual address and size*/
void memory_init(void *ptr)
{
	rtos_shm_t *rtos_shm = (rtos_shm_t*) ptr;
    HeapBase = rtos_shm->addr + sizeof(rtos_shm_t);
	pr_debug("memory_init base=%lx\n", HeapBase);
    HeapLimit = rtos_shm->addr + rtos_shm->size;
	pr_debug("memory_init Limit=%lx\n", HeapLimit);
	/* need to get offset block size from linux*/
	offset = rtos_shm->virt_phys_offset / HEADER_SIZE;
	/* linux will init heap_ptr & free_list after booting */
	base = (Header*) &rtos_shm->mem_base;
	/* free_list is init in linux */
	free_list = base;
	/* do not need to init heap_ptr again*/
//	base->heap_ptr = 0;
//	base->heap_ptr_vpa = 0;
	rtos_memory_lock = &rtos_shm->rtos_memory_lock;
}

__attribute__((weak)) char *memory_sbrk (int nbytes)
{
	char *sbrk_base;
	/* linux will init heap_ptr after booting */
	if (!base->heap_ptr) {
		base->heap_ptr = HeapBase;
		base->heap_ptr_vpa = HeapBase + offset * HEADER_SIZE;
		pr_debug("HeapBase=%lx offset=%lx HEADER_SIZE=%x\n ", HeapBase, offset, HEADER_SIZE);
	}
	sbrk_base = (char *) base->heap_ptr;
	pr_debug("base->heap_ptr=%lx\n", base->heap_ptr);
	pr_debug("base->heap_ptr_vpa=%lx\n", base->heap_ptr_vpa);
	if((base->heap_ptr != NULL) && (base->heap_ptr + nbytes <= HeapLimit + 1)) {
		base->heap_ptr += nbytes;
		base->heap_ptr_vpa += nbytes;
		pr_debug("memory_sbrk =%lx\n", sbrk_base);
		pr_debug("memory_sbrk ptr =%lx\n", base->heap_ptr);
		pr_debug("memory_sbrk ptr_vpa =%lx\n", base->heap_ptr_vpa);
		return sbrk_base;
	} else {
    	return ((char *) -1);
	}
}

Header* increase_heap(unsigned int units, Header *prev)
{
	Header *block;
	if(units <= 1)
		block = (Header*) memory_sbrk(HEADER_SIZE * 2);
	else
		block = (Header*) memory_sbrk( (1 + units) * HEADER_SIZE);
	prev->next = block;
	prev->next_vpa = block + offset;
	block->size = units;
	block->next = NULL;
	block->next_vpa = NULL;
	return block;
}

#if 1
/* need to improve*/
void memory_set(char *ptr, char value, size_t size)
{
	int i = 0;
	for(i = 0; i < size; i++, ptr++)
	{
		*ptr = value;
	}
}
#endif
