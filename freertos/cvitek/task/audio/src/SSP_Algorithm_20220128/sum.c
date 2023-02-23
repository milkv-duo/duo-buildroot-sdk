
/* Include files */
#include "mmse.h"
#include "sum.h"

/* Function Definitions */
creal32_T b_sum(const creal32_T x_data[], const int x_size[1])
{
  creal32_T y;
  int k;
  if (x_size[0] == 0) {
    y.re = 0.0F;
    y.im = 0.0F;
  } else {
    y = x_data[0];
    for (k = 2; k <= x_size[0]; k++) {
      y.re += x_data[k - 1].re;
      y.im += x_data[k - 1].im;
    }
  }

  return y;
}

float c_sum(const float x[], int N)
{
  float y;
  int k;
  y = x[0];
  for (k = 0; k < N-1; k++) {
    y += x[k + 1];
  }

  return y;
}


