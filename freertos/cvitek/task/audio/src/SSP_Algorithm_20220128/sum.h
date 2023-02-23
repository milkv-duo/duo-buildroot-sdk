
#ifndef SUM_H
#define SUM_H

/* Include files */
#include <stddef.h>
#ifndef PLATFORM_RTOS
#include <stdlib.h>
#endif
#include "tmwtypes.h"

/* Function Declarations */
extern creal32_T b_sum(const creal32_T x_data[], const int x_size[1]);
extern float c_sum(const float x[], int N);

#endif


