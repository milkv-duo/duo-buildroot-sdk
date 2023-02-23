
#ifndef DB2LINEAR_H
#define DB2LINEAR_H

/* Include files */
#include <stddef.h>
#include <stdlib.h>

/* Function Declarations */
extern short db_to_linear(short db_num);
extern void smooth16bit(short *state_p, short *in_p, short alpha, short size);

#endif


