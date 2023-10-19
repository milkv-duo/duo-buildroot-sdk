#include "xil_sleep.h"

/*****************************************************************************
*
* This API gives delay in sec
*
* @param            seconds - delay time in seconds
*
* @return           none
*
* @note             none
*
*****************************************************************************/
void arch_sleep(unsigned int seconds)
{
	sleep_A53(seconds);
}

/****************************************************************************/
/**
*
* This API gives delay in usec
*
* @param            useconds - delay time in useconds
*
* @return           none
*
* @note             none
*
*****************************************************************************/
void arch_usleep(unsigned long useconds)
{
	usleep_A53(useconds);
}
