#include <stdio.h>

void *memset(void *dest, int value, unsigned long size)
{
    char *pd = (char *) dest;
//	printf("memset size=%x\n", size);
    for(int i = 0 ; i < size; i++) {
        *pd++ = (char) value;
    }
}

