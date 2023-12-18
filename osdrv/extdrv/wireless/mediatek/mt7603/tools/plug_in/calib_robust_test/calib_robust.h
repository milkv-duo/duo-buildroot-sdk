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

	Module Name: calib_robust_test
	calib_robust.h
*/

#ifndef _CALIB_ROBUST_H
#define _CALIB_ROBUST_H

#include <linux/file.h>
#include "rt_config.h"
#include "rtmp_comm.h"
#include "rt_os_util.h"
#include "rt_os_net.h"


#define GSIZE 1024
#define CALIB_REG_END 0
#define CALIB_REG_TYPE_PHY  1
#define CALIB_REG_TYPE_MAC 2


enum {
	CALIB_F_END = 0,
	CALIB_F_START,
};

enum{
	CALIB_STATE_NOP,
	CALIB_STATE_START_TEST_MODE,
	CALIB_STATE_CALIB_START,
	CALIB_STATE_STOP_TEST_MODE
};


typedef enum {
	CALIB_TEST_START=0,
	CALIB_TEST_24GRCAL=0,
	CALIB_TEST_24GTMPSENS=1,
	CALIB_TEST_RXDCOC=2,
	CALIB_TEST_RC=3,
	CALIB_TEST_SXLOGEN=4,
	CALIB_TEST_TXLOFT=5,
	CALIB_TEST_TXIQ=6,
	CALIB_TEST_TSSICAL=7,
	CALIB_TEST_TSSICMP=8,
	CALIB_TEST_TXDPD=9,
	CALIB_TEST_RXIQC_FI=10,
	CALIB_TEST_RXIQC_FD=11,
	CALIB_TEST_TXLPFG=12,
	CALIB_TEST_RSSIDCOC,
	CALIB_TEST_END
}CALIB_TEST_T;


struct calibItem {
	unsigned int calibId;
	unsigned int mode;
	char name[16];
	unsigned int regs[64];
};

typedef struct calib_test_s{
	unsigned int curCalibId;
	unsigned int stat;
	unsigned int fstate;
	unsigned int times;
	unsigned int curCount;	
	spinlock_t lock;	
	unsigned int bw;
	struct work_struct *resultWork;
	char fname[64];	
}calib_test_t;

int calib_test_fileWrite(char *buf,unsigned int size);
void calib_test_fileClose(void);
int calib_test_fileOpen(char *fname);
int calib_test_proc_init(void);
int calib_test_proc_exit(void);


struct calibItem* calib_table_get(unsigned int calibId);


#endif
