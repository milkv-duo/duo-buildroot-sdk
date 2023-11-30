#include <linux/init.h>
#include <linux/module.h>
#include <linux/irqchip/arm-gic.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/list.h>
#include <linux/time.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/of_reserved_mem.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/delay.h>

#include "rtos_cmdqu.h"
#include "cvi_mailbox.h"
#include "cvi_spinlock.h"

struct cvi_rtos_cmdqu_device {
	struct device *dev;
	struct miscdevice miscdev;
};

spinlock_t mailbox_queue_lock;
spinlock_t send_queue_lock;
static __u64  reg_base;
static int mailbox_irq;

static int cvi_rtos_cmdqu_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int cvi_rtos_cmdqu_release(struct inode *inode, struct file *file)
{
	return 0;
}

struct rtos_cmdqu_wait_list_t {
	struct list_head list;
	cmdqu_t cmdq;
	wait_queue_head_t wq;
	int condition;
};

static struct rtos_cmdqu_wait_list_t rtos_cmdqu_wait_head;

/* used for callback test*/
static void callback_rtos_irq_handler(int cmd_id, unsigned int ptr, void *dev_id)
{
	pr_info("callback_rtos_hander cmd_id = %x ptr=%x dev_id=%p\n", cmd_id, ptr, dev_id);
}

DEFINE_CVI_SPINLOCK(mailbox_lock, SPIN_MBOX);

irqreturn_t rtos_irq_handler(int irq, void *dev_id)
{
	char set_val, done_val;
	int i;
	int flags;
	cmdqu_t *cmdq;

	struct rtos_cmdqu_wait_list_t *wait_list;
	struct list_head *pos;

	drv_spin_lock_irqsave(&mailbox_lock, flags);
	if (flags == MAILBOX_LOCK_FAILED) {
		pr_err("drv_spin_lock_irqsave failed!\n");
		//must clear irq?
		return IRQ_HANDLED;
	}
	pr_info("rtos_irq_handler irq=%d\n", irq);
	set_val = mbox_reg->cpu_mbox_set[RECEIVE_CPU].cpu_mbox_int_int.mbox_int;
	done_val = mbox_done_reg->cpu_mbox_done[RECEIVE_CPU].cpu_mbox_int_int.mbox_int;
	pr_debug("set_val=%x\n", set_val);
	pr_debug("done_val=%x\n", done_val);

	for (i = 0; i < MAILBOX_MAX_NUM && set_val > 0; i++) {
		/* valid_val uses unsigned char because of mailbox register table
		 * ~valid_val will be 0xFF
		 */
		unsigned char valid_val = set_val & (1 << i);

		pr_debug("MAILBOX_MAX_NUM = %d\n", MAILBOX_MAX_NUM);
		pr_debug("valid_val = %d set_val=%d i = %d\n", valid_val, set_val, i);
		if (valid_val) {
			cmdqu_t linux_cmdq;

			cmdq = (cmdqu_t *)(mailbox_context) + i;
			/* mailbox buffer context is send from rtos, clear mailbox interrupt */
			mbox_reg->cpu_mbox_set[RECEIVE_CPU].cpu_mbox_int_clr.mbox_int_clr = valid_val;
			// need to disable enable bit
			mbox_reg->cpu_mbox_en[RECEIVE_CPU].mbox_info &= ~valid_val;
			// copy cmdq context (8 bytes) to buffer ASAP ??
			*((unsigned long long *) &linux_cmdq) = *((unsigned long long *)cmdq);
			/* need to clear mailbox interrupt before clear mailbox buffer ??*/
			*((unsigned long long *) cmdq) = 0;

			/* mailbox buffer context is send from rtos */
			pr_debug("cmdq=%p\n", cmdq);
			pr_debug("cmdq->ip_id =%d\n", linux_cmdq.ip_id);
			pr_debug("cmdq->cmd_id =%d\n", linux_cmdq.cmd_id);
			pr_debug("cmdq->param_ptr =%x\n", linux_cmdq.param_ptr);
			pr_debug("cmdq->block =%d\n", linux_cmdq.block);
			pr_debug("cmdq->linux_valid =%d\n", linux_cmdq.resv.valid.linux_valid);
			pr_debug("cmdq->rtos_valid =%x", linux_cmdq.resv.valid.rtos_valid);
			if (linux_cmdq.resv.valid.rtos_valid == 1 &&
				linux_cmdq.block == 1) {
				// dewait
				list_for_each(pos, &rtos_cmdqu_wait_head.list) {
					wait_list = list_entry(pos, struct rtos_cmdqu_wait_list_t, list);
					pr_debug("s wait_list->cmdq.ip_id=%d\n", wait_list->cmdq.ip_id);
					pr_debug("s wait_list->cmdq.cmd_id=%d\n", wait_list->cmdq.cmd_id);
					if (wait_list->cmdq.ip_id == linux_cmdq.ip_id &&
						wait_list->cmdq.cmd_id == linux_cmdq.cmd_id) {
						/* copy data to wait_list and return to user space */
						*((unsigned long long *) &wait_list->cmdq) =
							*((unsigned long long *) &linux_cmdq);
						pr_debug("wait_list->cmdq.ip_id=%d\n",
							wait_list->cmdq.ip_id);
						pr_debug("wait_list->cmdq.cmd_id=%d\n",
							wait_list->cmdq.cmd_id);
						pr_debug("wait_list->cmdq.param_ptr=%x\n",
							wait_list->cmdq.param_ptr);

						wait_list->condition = 1;
						wake_up_interruptible(&wait_list->wq);
						break;
					}
				}
			} else
				pr_err("error ip=%d , cmd=%d\n", linux_cmdq.ip_id, linux_cmdq.cmd_id);
		}
	}
	drv_spin_unlock_irqrestore(&mailbox_lock, flags);
	return IRQ_HANDLED;
}

long rtos_cmdqu_init(void)
{
	long ret = 0;
	int i;

	pr_debug("RTOS_CMDQU_INIT\n");
	spin_lock_init(&mailbox_queue_lock);
	spin_lock_init(&send_queue_lock);
	mbox_reg = (struct mailbox_set_register *) reg_base;
	mbox_done_reg = (struct mailbox_done_register *) (reg_base + MAILBOX_DONE_OFFSET);
	mailbox_context = (unsigned long *) (reg_base + MAILBOX_CONTEXT_OFFSET);//MAILBOX_CONTEXT;

	pr_debug("mbox_reg=%p\n", mbox_reg);
	pr_debug("mbox_done_reg=%p\n", mbox_done_reg);
	pr_debug("mailbox_context=%p\n", mailbox_context);

	// init mailbox_context
	for ( i=0;i < MAILBOX_MAX_NUM;i++)
		mailbox_context[i] = 0;
	/* init sqirq parameters*/
	return ret;
}

long rtos_cmdqu_deinit(void)
{
	long ret = 0;
	pr_debug("RTOS_CMDQU_DEINIT\n");
	//mailbox deinit
	return ret;
}

int rtos_cmdqu_send(cmdqu_t *cmdq)
{
	int ret = 0;
	int valid;
	unsigned long flags;
	int mb_flags;
	cmdqu_t *linux_cmdqu_t;

	pr_debug("RTOS_CMDQU_SEND\n");
	pr_debug("ip_id=%d cmd_id=%d param_ptr=%x\n", cmdq->ip_id, cmdq->cmd_id, (unsigned int)cmdq->param_ptr);

	spin_lock_irqsave(&mailbox_queue_lock, flags);
	// when linux and rtos send command at the same time, it might cause a problem.
	// might need to spinlock with rtos, do it later
	drv_spin_lock_irqsave(&mailbox_lock, mb_flags);
	if (mb_flags == MAILBOX_LOCK_FAILED) {
		pr_err("ip_id=%d cmd_id=%d param_ptr=%x\n", cmdq->ip_id, cmdq->cmd_id, (unsigned int)cmdq->param_ptr);
		spin_unlock_irqrestore(&mailbox_queue_lock, flags);
		return -ENOBUFS;
	}
	linux_cmdqu_t = (cmdqu_t *) mailbox_context;
	pr_debug("mailbox_context = %p\n", mailbox_context);
	pr_debug("linux_cmdqu_t = %p\n", linux_cmdqu_t);
	pr_debug("cmdq->ip_id = %d\n", cmdq->ip_id);
	pr_debug("cmdq->cmd_id = %d\n", cmdq->cmd_id);
	pr_debug("cmdq->block = %d\n", cmdq->block);
	pr_debug("cmdq->para_ptr = %d\n", cmdq->param_ptr);

	for (valid = 0; valid < MAILBOX_MAX_NUM; valid++) {
		if (linux_cmdqu_t->resv.valid.linux_valid == 0 && linux_cmdqu_t->resv.valid.rtos_valid == 0) {
			// mailbox buffer context is int (4 bytes) access
			int *ptr = (int *)linux_cmdqu_t;

			linux_cmdqu_t->resv.valid.linux_valid = 1;
			*ptr = ((cmdq->ip_id << 0) | (cmdq->cmd_id << 8) | (cmdq->block << 15) |
					(linux_cmdqu_t->resv.valid.linux_valid << 16) |
					(linux_cmdqu_t->resv.valid.rtos_valid << 24));
			linux_cmdqu_t->param_ptr = cmdq->param_ptr;
			pr_debug("linux_valid = %d\n", linux_cmdqu_t->resv.valid.linux_valid);
			pr_debug("rtos_valid = %d\n", linux_cmdqu_t->resv.valid.rtos_valid);
			pr_debug("ip_id = %d\n", linux_cmdqu_t->ip_id);
			pr_debug("cmd_id = %d\n", linux_cmdqu_t->cmd_id);
			pr_debug("block = %d\n", linux_cmdqu_t->block);
			pr_debug("param_ptr = %x\n", linux_cmdqu_t->param_ptr);
			pr_debug("*ptr = %x\n", *ptr);
			// clear mailbox
			mbox_reg->cpu_mbox_set[SEND_TO_CPU].cpu_mbox_int_clr.mbox_int_clr = (1 << valid);
			// trigger mailbox valid to rtos
			mbox_reg->cpu_mbox_en[SEND_TO_CPU].mbox_info |= (1 << valid);
			mbox_reg->mbox_set.mbox_set = (1 << valid);

			break;
		}
		linux_cmdqu_t++;
	}

	if (valid >= MAILBOX_MAX_NUM) {
		pr_err("No valid mailbox is available\n");
		drv_spin_unlock_irqrestore(&mailbox_lock, mb_flags);
		spin_unlock_irqrestore(&mailbox_queue_lock, flags);
		return -ENOBUFS;
	}
	drv_spin_unlock_irqrestore(&mailbox_lock, mb_flags);
	spin_unlock_irqrestore(&mailbox_queue_lock, flags);
    return ret;
}
EXPORT_SYMBOL(rtos_cmdqu_send);

int rtos_cmdqu_send_wait(cmdqu_t *cmdq, int wait_cmd_id)
{
	unsigned long flags;
	struct rtos_cmdqu_wait_list_t *wait_list;
	struct list_head *pos;
	int delaytime;
	int ret = 0;

	pr_debug("%s %d\n", __func__, __LINE__);

	spin_lock_irqsave(&send_queue_lock, flags);
	/* check list with same commands? if yes, ignore it */
	list_for_each(pos, &rtos_cmdqu_wait_head.list) {
		wait_list = list_entry(pos, struct rtos_cmdqu_wait_list_t, list);
		pr_debug("list->cmdq.ip_id=%d\n", wait_list->cmdq.ip_id);
		pr_debug("list->cmdq.cmd_id=%d\n", wait_list->cmdq.cmd_id);
		if (cmdq->ip_id == wait_list->cmdq.ip_id &&
			wait_cmd_id == wait_list->cmdq.cmd_id) {

			pr_debug("exist : wait_list->cmdq.ip_id=%d\n", wait_list->cmdq.ip_id);
			pr_debug("exist : wait_list->cmdq.cmd_id=%d\n", wait_list->cmdq.cmd_id);
			pr_debug("exist : wait_list->cmdq.param_ptr=%d\n", wait_list->cmdq.param_ptr);
			spin_unlock_irqrestore(&send_queue_lock, flags);
			return -EEXIST;

		}
	}
	cmdq->block = 1;

	wait_list = kzalloc(sizeof(struct rtos_cmdqu_wait_list_t), GFP_KERNEL);

	if (!wait_list) {
		spin_unlock_irqrestore(&send_queue_lock, flags);
		return -ENOMEM;
	}

	*((unsigned long long *) &wait_list->cmdq) = *((unsigned long long *) cmdq);
	wait_list->cmdq.cmd_id = wait_cmd_id;

	init_waitqueue_head(&wait_list->wq);
	list_add_tail(&wait_list->list, &rtos_cmdqu_wait_head.list);

	spin_unlock_irqrestore(&send_queue_lock, flags);
	/* check the delay ms
	 * if mstime is 65535 (-1), it will be blocked infinite (MAX_JIFFY_OFFSET)
	 */

	delaytime = wait_list->cmdq.resv.mstime;
	if (delaytime == 0xFFFF)
		delaytime = -1;

	ret = rtos_cmdqu_send(cmdq);

	ret = wait_event_interruptible_timeout(wait_list->wq,
		wait_list->condition != 0, msecs_to_jiffies(delaytime));
	spin_lock_irqsave(&send_queue_lock, flags);
	list_del_init(&wait_list->list);
	spin_unlock_irqrestore(&send_queue_lock, flags);
	if (!ret) {
		ret = -ETIME;
		kfree(wait_list);
		pr_err("RTOS_CMDQU_SEND_WAIT timeout\n");
		return ret;
	}
	pr_debug("RTOS_CMDQU_SEND_WAIT done\n");
	pr_debug("list wait_list->cmdq.ip_id=%d wait_list->cmdq.cmd_id=%d wait_list->cmdq.param_ptr=%x\n",
		wait_list->cmdq.ip_id, wait_list->cmdq.cmd_id, wait_list->cmdq.param_ptr);

	/* get context from interrupt and return to userspace */
	*((unsigned long long *) cmdq) = *((unsigned long long *) &wait_list->cmdq);
	kfree(wait_list);
	return 0;
}
EXPORT_SYMBOL(rtos_cmdqu_send_wait);

static long cvi_rtos_cmdqu_ioctl(struct file *filp, unsigned int cmd, unsigned long  arg)
{
	struct cvi_rtos_cmdqu_device *dev = filp->private_data;
	struct rtos_cmdqu_wait_list_t *wait_list;
	struct list_head *pos;
	long ret = 0;
	unsigned long flags;
	cmdqu_t cmdq;

	pr_debug("%s dev=%p\n", __func__, dev);
	switch (cmd) {
		case RTOS_CMDQU_SEND:
			pr_debug("RTOS_CMDQU_SEND\n");
			copy_from_user(&cmdq,
				(struct cmdqu_t __user *)arg,
				sizeof(struct cmdqu_t));
			ret = rtos_cmdqu_send(&cmdq);
			break;
		case RTOS_CMDQU_SEND_WAKEUP:
			pr_debug("RTOS_CMDQU_SEND_WAKEUP\n");
			copy_from_user(&cmdq,
				(struct cmdqu_t __user *)arg,
				sizeof(struct cmdqu_t));
			pr_debug("cmdq.ip_id=%d cmdq.cmd_id=%d\n", cmdq.ip_id, cmdq.cmd_id);

			spin_lock_irqsave(&send_queue_lock, flags);
			list_for_each(pos, &rtos_cmdqu_wait_head.list) {
				wait_list = list_entry(pos, struct rtos_cmdqu_wait_list_t, list);
				pr_debug("list->cmdq.ip_id=%d\n", wait_list->cmdq.ip_id);
				pr_debug("list->cmdq.cmd_id=%d\n", wait_list->cmdq.cmd_id);
				if (cmdq.ip_id == wait_list->cmdq.ip_id &&
					cmdq.cmd_id == wait_list->cmdq.cmd_id &&
					wait_list->cmdq.block == 1) {
					/* copy data to wait_list and return to user space */
					*((unsigned long long *) &wait_list->cmdq) = *((unsigned long long *) &cmdq);
					pr_debug("wait_list->cmdq.ip_id=%d\n", wait_list->cmdq.ip_id);
					pr_debug("wait_list->cmdq.cmd_id=%d\n", wait_list->cmdq.cmd_id);
					pr_debug("wait_list->cmdq.param_ptr=%d\n", wait_list->cmdq.param_ptr);

					wait_list->condition = 1;
					wake_up_interruptible(&wait_list->wq);
					break;
				}
			}
			spin_unlock_irqrestore(&send_queue_lock, flags);
			pr_debug("RTOS_CMDQU_SEND_WAKEUP done\n");
			break;

		case RTOS_CMDQU_SEND_WAIT:
			pr_debug("RTOS_CMDQU_SEND_WAIT\n");
			ret = copy_from_user(&cmdq,
				(struct cmdqu_t __user *)arg,
				sizeof(struct cmdqu_t));
			if (ret) {
				return -EFAULT;
			}
			rtos_cmdqu_send_wait(&cmdq, cmdq.cmd_id);
			ret = copy_to_user((struct cmdqu_t __user *)arg,
					&cmdq,
					sizeof(struct cmdqu_t));
			break;
		default:
			ret = -EFAULT;
			break;
	}
	return ret;
}

static const struct file_operations rtos_cmdqu_fops = {
	.owner = THIS_MODULE,
	.open = cvi_rtos_cmdqu_open,
	.release = cvi_rtos_cmdqu_release,
	.unlocked_ioctl = cvi_rtos_cmdqu_ioctl,
};

static int _register_dev(struct cvi_rtos_cmdqu_device *ndev)
{
	int rc;

	ndev->miscdev.minor = MISC_DYNAMIC_MINOR;
	ndev->miscdev.name = RTOS_CMDQU_DEV_NAME;
	ndev->miscdev.fops = &rtos_cmdqu_fops;

	rc = misc_register(&ndev->miscdev);
	if (rc) {
		dev_err(ndev->dev, "cvi_rtos_cmdqu: failed to register misc device.\n");
		return rc;
	}

	return 0;
}

static int cvi_rtos_cmdqu_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct cvi_rtos_cmdqu_device *ndev;
	struct resource *res;
	int ret = 0;
	int err = -1;

	pr_info("%s start ---\n", __func__);
	pr_info("name=%s\n", pdev->name);
	ndev = devm_kzalloc(&pdev->dev, sizeof(*ndev), GFP_KERNEL);
	if (!ndev)
		return -ENOMEM;

	ndev->dev = dev;

	ret = _register_dev(ndev);
	if (ret < 0) {
		pr_err("regsiter cvi_rtos_cmdqu chrdev error\n");
		return ret;
	}
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

//	void __iomem *regs;
//	regs = devm_ioremap_resource(dev, res);
//	if (IS_ERR(regs)) {
//		ret = PTR_ERR(regs);
//		goto ERROR_PROVE_DEVICE;
//	}
//	printk("regs=%x\n", regs);

	reg_base = (__u64)devm_ioremap(&pdev->dev, res->start,
		res->end - res->start);

	pr_info("res-reg: start: 0x%llx, end: 0x%llx, virt-addr(%llx).\n",
		res->start, res->end, le64_to_cpu(reg_base));
	// spinlock ip offset address (0xc0)
	spinlock_base(reg_base + 0xc0);
	mailbox_irq = platform_get_irq_byname(pdev, "mailbox");

	INIT_LIST_HEAD(&rtos_cmdqu_wait_head.list);
	/* init cmdqu*/
	rtos_cmdqu_init();
	platform_set_drvdata(pdev, ndev);

	err = request_irq(mailbox_irq, rtos_irq_handler, 0, "mailbox",
		(void *)ndev);

	if (err) {
		pr_err("fail to register interrupt handler\n");
		return -1;
	}

	pr_info("%s DONE\n", __func__);
	return 0;

//ERROR_PROVE_DEVICE:
//	return err;
}

static int cvi_rtos_cmdqu_remove(struct platform_device *pdev)
{
	struct cvi_rtos_cmdqu_device *ndev = platform_get_drvdata(pdev);

	misc_deregister(&ndev->miscdev);
	platform_set_drvdata(pdev, NULL);
	/* remove irq handler*/
	free_irq(mailbox_irq, ndev);
	rtos_cmdqu_deinit();
	pr_debug("%s DONE\n", __func__);

	return 0;
}

static const struct of_device_id cvi_rtos_cmdqu_match[] = {
	{ .compatible = "cvitek,rtos_cmdqu" },
	{},
};

static struct platform_driver cvi_rtos_cmdqu_driver = {
	.probe = cvi_rtos_cmdqu_probe,
	.remove = cvi_rtos_cmdqu_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = RTOS_CMDQU_DEV_NAME,
		.of_match_table = cvi_rtos_cmdqu_match,
	},
};

struct class *pbase_class;
static void cvi_rtos_cmdqu_exit(void)
{
	platform_driver_unregister(&cvi_rtos_cmdqu_driver);
	class_destroy(pbase_class);
	cvi_spinlock_uninit();
	pr_debug("%s DONE\n", __func__);
}

static int cvi_rtos_cmdqu_init(void)
{
	int rc;
	pr_info("cvi_rtos_cmdqu_init\n");
	pbase_class = class_create(THIS_MODULE, RTOS_CMDQU_DEV_NAME);
	if (IS_ERR(pbase_class)) {
		pr_err("create class failed\n");
		rc = PTR_ERR(pbase_class);
		goto cleanup;
	}

	platform_driver_register(&cvi_rtos_cmdqu_driver);
	pr_debug("%s done\n", __func__);
	cvi_spinlock_init();
	return 0;
cleanup:
	cvi_rtos_cmdqu_exit();
	return rc;
}

MODULE_DESCRIPTION("RTOS_CMD_QUEUE");
MODULE_LICENSE("GPL");
module_init(cvi_rtos_cmdqu_init);
module_exit(cvi_rtos_cmdqu_exit);
