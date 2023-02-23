
/* Include files */
#include <math.h>
#include "mmse.h"
#include "power.h"
#include "mmse_rtwutil.h"

/* Function Definitions */
void c_power(const float a[], float y[], int N)
{
  int k;
  for (k = 0; k < N; k++) {
    y[k] = a[k] * a[k];
  }
}

void d_power(const float a[321], float y[321])
{
  int k;
  for (k = 0; k < 321; k++) {
    y[k] = (float)sqrt(a[k]);
  }
}


