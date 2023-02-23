
/* Include files */
#include "mmse.h"
#include "abs.h"
#include "power.h"
#include "mmse_rtwutil.h"

/* Function Definitions */
void a_abs(const creal32_T a[321], float y[321], int N)
{
  int k;
  for (k = 0; k < N; k++) {
    y[k] = rt_hypotf(a[k].re, a[k].im);
  }
}


