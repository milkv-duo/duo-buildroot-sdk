#ifndef DWC2_STDTYPES_H
#define DWC2_STDTYPES_H
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include "dwc2_stdint.h"

#define TYPEDEF typedef

/* Define NULL constant */
#ifndef NULL
#define	NULL	((void *)0)
#endif

/* Define size_t data type */
// typedef	uintptr_t	size_t;

/* Define bool data type */
#define bool	_Bool
#define	true	1
#define	false	0
#define	__bool_true_false_are_defined 1
#endif /* __KERNEL__ */
#endif	/* DWC2_STDTYPES_H */
