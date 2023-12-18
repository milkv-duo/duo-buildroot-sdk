/*
 ***************************************************************************
 * MediaTek Inc. 
 *
 * All rights reserved. source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of MediaTek. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of MediaTek, Inc. is obtained.
 ***************************************************************************

	Module Name: softq_stat
	softq_stat.h
*/

#ifndef _SOFTQ_STAT_H
#define _SOFTQ_STAT_H

#include <linux/file.h>


#define GSIZE 10240


typedef struct softq_stat_s {

unsigned int isSoftqStat;
unsigned int counter;
unsigned int swDropPacket[15];
unsigned int swQueueCounter[15];
unsigned int swQueueTotal[15];
unsigned int txRingCounter[15];
unsigned int txRingTotal[15];
unsigned int apSendPacket[15];
unsigned int apDequeuePacket[15];
unsigned int swPreDropPacket[15];
unsigned int apPreSendPacket[15];
unsigned int apPreDequeuePacket[15];
unsigned int times;
unsigned int staId;
unsigned int fstate;
char   fname[64];

} softq_stat_t;

enum { 
	SOFTQ_STATE_NOP,
	SOFTQ_STATE_CLEAN,
	SOFTQ_STATE_REPORT,
	SOFTQ_STATE_REPORT_INTR_PERIOD,
	SOFTQ_STATE_REPORT_INTR_EACHTIME,
};

enum {
	SOFTQ_F_END = 0,
	SOFTQ_F_START,
};



int softq_stat_fileWrite(char *buf,unsigned int size);
void softq_stat_fileClose(void);
int softq_stat_fileOpen(char *fname);
int softq_stat_proc_init(void);
int softq_stat_proc_exit(void);





#endif
