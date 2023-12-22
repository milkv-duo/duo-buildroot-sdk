
struct mem_tuple{
	void *mem_ptr;
	unsigned long caller;
	unsigned long alloc_time;
};

struct mem_entry {
	struct mem_tuple mem;
	struct mem_entry *next;
	struct mem_entry *prev;
};

struct mem_list{
	struct mem_entry *mem_head;
	struct mem_entry *mem_tail;
	unsigned int entry_cnt;
};

INT mem_tracker_init();
INT mem_tracker_exit();
INT mem_tracker_add(void *mem, unsigned long caller, unsigned long time);
INT mem_tracker_del(void *mem);
INT mem_tracker_dump();

