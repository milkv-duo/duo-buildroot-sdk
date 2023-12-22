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
	calib_robust_file.c
*/

#include <linux/fs.h>
#include <linux/file.h>
#include <linux/types.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>
#include "calib_robust.h"

struct file *filp = NULL;

int calib_test_fileOpen(char *fname)
{
	int result = 0;
	if(filp == NULL) 
	{
		filp = filp_open(fname, O_RDWR | O_CREAT, 0);
	}else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("[calib] file is exist!!\n"));
		result = -1;
	}
	if (IS_ERR(filp)) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("[calib] error occurred while opening file %s, exiting...\n", fname));
		result = -1;
	}
	return result;
}

int calib_test_fileWrite(char *buf,unsigned int size)
{	
	if (!IS_ERR(filp) && filp->f_op && filp->f_op->write) 
	{
		filp->f_op->write(filp, buf, size, &filp->f_pos);
	}else
	{
		return -1;
	}
	return 0;
}

void calib_test_fileClose(void)
{
	if(filp != NULL)
		filp_close(filp, NULL);
}


