
/* Include files */
#include <math.h>
#include "mmse.h"
#include "log.h"
#include "power.h"
#include "mmse_rtwutil.h"

#ifdef FAST_MATH_CALC
inline float fast_log2 (float val)
 {
    int * const    exp_ptr =  (int *)(&val);
    int            x = *exp_ptr;
    const int      log_2 = ((x >> 23) & 255) - 128;
    x &= ~(255 << 23);
    x += 127 << 23;
    *exp_ptr = x;

    val = ((-1.0f/3) * val + 2) * val - 2.0f/3;   // (1)

    return (val + log_2);
 }

 inline float fast_log(const float val)
 {
    return (fast_log2 (val) * 0.69314718f);
 }
#endif

/* Function Definitions */
void b_log(float x[321])
{
  int k;
  for (k = 0; k < 321; k++) {
#ifdef FAST_MATH_CALC
    x[k] = (float)fast_log(x[k]);
#else
    x[k] = (float)log(x[k]);
#endif 
  }
}


