/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __DRV_SPINLOCK_H__
#define __DRV_SPINLOCK_H__

enum SPINLOCK_FIELD {
	SPIN_UART,
	SPIN_LINUX_RTOS = 4, // this spinlock field is used for linux & rtos
	SPIN_MBOX = SPIN_LINUX_RTOS,
	SPIN_MAX = 7,
};

typedef struct hw_raw_spinlock {
	unsigned short locks;
	unsigned short hw_field;
} hw_raw_spinlock_t;

#define MAILBOX_LOCK_SUCCESS	1
#define MAILBOX_LOCK_FAILED		(-1)

#define __CVI_ARCH_SPIN_LOCK_UNLOCKED    { 0 }

#define __CVI_RAW_SPIN_LOCK_INITIALIZER(spinlock_hw_field)   \
	{                   \
		.locks = __CVI_ARCH_SPIN_LOCK_UNLOCKED,  \
		.hw_field = spinlock_hw_field, \
	}

#define DEFINE_CVI_SPINLOCK(x, y)  hw_raw_spinlock_t x = __CVI_RAW_SPIN_LOCK_INITIALIZER(y)

int _hw_raw_spin_lock_irqsave(hw_raw_spinlock_t *lock);
void _hw_raw_spin_unlock_irqrestore(hw_raw_spinlock_t *lock, int flag);

#define drv_spin_lock_irqsave(lock, flags)              \
	{	flags = _hw_raw_spin_lock_irqsave(lock);  }

#define drv_spin_unlock_irqrestore(lock, flags)     \
	_hw_raw_spin_unlock_irqrestore(lock, flags)

void spinlock_base(unsigned long mb_base);
void cvi_spinlock_init(void);

#endif // end of __DRV_SPINLOCK_H__

