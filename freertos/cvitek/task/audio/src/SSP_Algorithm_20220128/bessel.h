
#ifndef BESSEL_H
#define BESSEL_H

/* Include files */
#include <stddef.h>
#ifndef PLATFORM_RTOS
#include <stdlib.h>
#endif
#include "tmwtypes.h"

/* Function Declarations */
extern void bessel_function(float v, const float x[321], float bessel[321]);

#endif


