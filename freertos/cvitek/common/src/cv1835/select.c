#ifdef __LINUX__
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/export.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/personality.h> /* for STICKY_TIMEOUTS */
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/rcupdate.h>
#include <linux/hrtimer.h>
#include <linux/sched/rt.h>
#include <linux/freezer.h>
#include <net/busy_poll.h>
#include <linux/vmalloc.h>

#include <asm/uaccess.h>
#else
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#endif

struct poll_table_page {
	struct poll_table_page * next;
	struct poll_table_entry * entry;
	struct poll_table_entry entries[0];
};

static void __pollwait(struct file *filp, wait_queue_head_t *wait_address,
                       poll_table *p);

void poll_initwait(struct poll_wqueues *pwq)
{
	init_poll_funcptr(&pwq->pt, __pollwait);
//	pwq->polling_task = current; // TODO
	pwq->triggered = 0;
	pwq->error = 0;
	pwq->table = NULL;
	pwq->inline_index = 0;
}

static void free_poll_entry(struct poll_table_entry *entry)
{
	remove_wait_queue(entry->wait_address, &entry->wait);
	fput(entry->filp);
}

void poll_freewait(struct poll_wqueues *pwq)
{
	struct poll_table_page * p = pwq->table;
	int i;
	for (i = 0; i < pwq->inline_index; i++)
		free_poll_entry(pwq->inline_entries + i);
	while (p) {
		struct poll_table_entry * entry;
		struct poll_table_page *old;

		entry = p->entry;
		do {
			entry--;
			free_poll_entry(entry);
		} while (entry > p->entries);
		old = p;
		p = p->next;
		free_page((unsigned long) old);
	}
}

static int __pollwake(wait_queue_t *wait, unsigned mode, int sync, void *key)
{
        struct poll_wqueues *pwq = wait->private;
        DECLARE_WAITQUEUE(dummy_wait, pwq->polling_task);

        /*
         * Although this function is called under waitqueue lock, LOCK
         * doesn't imply write barrier and the users expect write
         * barrier semantics on wakeup functions.  The following
         * smp_wmb() is equivalent to smp_wmb() in try_to_wake_up()
         * and is paired with smp_store_mb() in poll_schedule_timeout.
         */
        smp_wmb();
        pwq->triggered = 1;

        /*
         * Perform the default wake up operation using a dummy
         * waitqueue.
         *
         * TODO: This is hacky but there currently is no interface to
         * pass in @sync.  @sync is scheduled to be removed and once
         * that happens, wake_up_process() can be used directly.
         */
        //return default_wake_function(&dummy_wait, mode, sync, key); // TODO
}

static int pollwake(wait_queue_t *wait, unsigned mode, int sync, void *key)
{
        struct poll_table_entry *entry;

        entry = container_of(wait, struct poll_table_entry, wait);
        if (key && !((unsigned long)key & entry->key))
                return 0;
        return __pollwake(wait, mode, sync, key);
}

/* Add a new entry */
static void __pollwait(struct file *filp, wait_queue_head_t *wait_address,
                                poll_table *p)
{
        struct poll_wqueues *pwq = container_of(p, struct poll_wqueues, pt);
        struct poll_table_entry *entry = poll_get_entry(pwq);
        if (!entry)
                return;
        entry->filp = get_file(filp);
        entry->wait_address = wait_address;
        entry->key = p->_key;
        init_waitqueue_func_entry(&entry->wait, pollwake);
        entry->wait.private = pwq;
        add_wait_queue(wait_address, &entry->wait);
}
