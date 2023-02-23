/* Include files */
#ifndef _SSP_UNIT_TEST_H_
#define _SSP_UNIT_TEST_H_
#include <stdio.h>
#ifdef TYPE_ARM
#include <stdint.h>
#endif

int SSP_Algorithm(int test_mode, short *pinput, uint64_t pout);
int SSP_Algorithm2(int test_mode, short *pinput, uint64_t pout);
int SSP_Algorithm_Init(void);
#endif
