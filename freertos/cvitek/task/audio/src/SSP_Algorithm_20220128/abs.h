
#ifndef ABS_H
#define ABS_H

/* Include files */
#include <stddef.h>
#ifndef PLATFORM_RTOS
#include <stdlib.h>
#endif
#include "tmwtypes.h"

/* Function Declarations */
extern void a_abs(const creal32_T a[321], float y[321], int N);

#endif


