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
	softq_proc.c
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ctype.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include "softq_stat.h"

static struct proc_dir_entry *softq_proc=NULL;
static struct proc_dir_entry *state_entry=NULL, *times_entry=NULL,  *staId_entry=NULL, *file_entry=NULL;
extern  softq_stat_t softq_statistic;
extern unsigned int glen;
extern char *gBuffer;


 static int softq_state_show(void)
{
	int i;	
	unsigned int swq = 0, ring = 0;

	for(i=0;i<4;i++)
	{
		swq = softq_statistic.swQueueTotal[i] ? softq_statistic.swQueueTotal[i]/softq_statistic.swQueueCounter[i]:0;
		ring = softq_statistic.swQueueTotal[i] ? softq_statistic.txRingTotal[i]/softq_statistic.txRingCounter[i]:0;
		
		 DBGPRINT(RT_DEBUG_OFF, ("\tAC%d Drop=%d, SW_Q=%d, Ring=%d, APSendPacket=%d, DequeuePacket=%d\n", i,
		 softq_statistic.swDropPacket[i],
		swq,
		ring,
		 softq_statistic.apSendPacket[i],
		 softq_statistic.apDequeuePacket[i]));
	}
	return 0;
 }

static  int softq_state_clean(void)
{
	int i;	
	for(i=0;i<4;i++)
	{
		 softq_statistic.swDropPacket[i] = 0;
		 softq_statistic.swQueueCounter[i] = 0;
		 softq_statistic.swQueueTotal[i] = 0;
		 softq_statistic.txRingCounter[i] = 0;
		 softq_statistic.txRingTotal[i] = 0;
		 softq_statistic.apSendPacket[i] = 0;
		 softq_statistic.apDequeuePacket[i] = 0;
		 softq_statistic.apPreSendPacket[i] = 0;
		 softq_statistic.apPreDequeuePacket[i] = 0;
	}	
	softq_statistic.counter = 0;

	return 0;
 }


static int softq_file_state_run(unsigned int fstate)
{
	switch(fstate){
	case SOFTQ_F_START:		
		softq_stat_fileOpen(softq_statistic.fname);		
	break;
	case SOFTQ_F_END:
		softq_stat_fileClose();		
		glen = 0;
		memset(gBuffer,0,GSIZE);		
	break;
	default:
	break;
	}
	return 0;
}




static int softq_state_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len=0;
	len+=sprintf(page,"softq_state: %s\n", 
	softq_statistic.isSoftqStat == SOFTQ_STATE_CLEAN ? "clean":
	softq_statistic.isSoftqStat == SOFTQ_STATE_REPORT ? "report":
	softq_statistic.isSoftqStat == SOFTQ_STATE_REPORT_INTR_PERIOD? "report periodical":
	softq_statistic.isSoftqStat == SOFTQ_STATE_REPORT_INTR_EACHTIME? "report each": "no operation");
	
	if (len <= off+count) *eof = 1;

	*start = page + off;

	len -= off;

	if (len>count) len = count;

	if (len<0) len = 0;

	return len;
}

static int softq_state_proc_write( struct file *filp, const char *buff,unsigned long len1, void *data )
{
	char value[64], *end;
	
	if (buff && !copy_from_user(value, buff, len1)) {

		 softq_statistic.isSoftqStat = simple_strtoul(value, &end, 10);

		switch(softq_statistic.isSoftqStat){
		case SOFTQ_STATE_REPORT:
		{
			softq_state_show();			
			DBGPRINT(RT_DEBUG_OFF, ("\nshow don...\n"));
		}
		break;
		case SOFTQ_STATE_CLEAN:
		{
			softq_state_clean();
			DBGPRINT(RT_DEBUG_OFF, ("\nclean don...\n"));
		}
		break;
		case  SOFTQ_STATE_NOP:			
			DBGPRINT(RT_DEBUG_OFF, ("\nchange to no operation...\n"));
		break;
		case SOFTQ_STATE_REPORT_INTR_PERIOD:			
			DBGPRINT(RT_DEBUG_OFF, ("\nchange to report periodic...\n"));
		break;
		case SOFTQ_STATE_REPORT_INTR_EACHTIME:			
			DBGPRINT(RT_DEBUG_OFF, ("\nchange to report each time...\n"));
		break;
		default:
			DBGPRINT(RT_DEBUG_OFF, ("\nplease echo: [ clean: %d  | report: %d  | report-period: %d  | report-each: %d  | nop: %d]\n",
			SOFTQ_STATE_CLEAN,
			SOFTQ_STATE_REPORT,
			SOFTQ_STATE_REPORT_INTR_PERIOD,
			SOFTQ_STATE_REPORT_INTR_EACHTIME,
			SOFTQ_STATE_NOP));
		break;
		}
	
		 
	}		 
	return len1;
}

static int softq_times_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{

	int len=0;

	len+=sprintf(page,"softq_times: %d\n",softq_statistic.times);
	
	if (len <= off+count) *eof = 1;

	*start = page + off;

	len -= off;

	if (len>count) len = count;

	if (len<0) len = 0;

	return len;
}

static int softq_times_proc_write( struct file *filp, const char *buff,unsigned long len1, void *data )
{
	char value[64], *end;
	
	if (buff && !copy_from_user(value, buff, len1)) {

		 softq_statistic.times = simple_strtoul(value, &end, 10);
		 DBGPRINT(RT_DEBUG_OFF, ("change periodic time to %d\n", softq_statistic.times));
		 
	}		 
	return len1;
}


static int softq_staId_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{

	int len=0;

	len+=sprintf(page,"softq_staId: %d\n",softq_statistic.staId);
	
	if (len <= off+count) *eof = 1;

	*start = page + off;

	len -= off;

	if (len>count) len = count;

	if (len<0) len = 0;

	return len;
}

static int softq_staId_proc_write( struct file *filp, const char *buff,unsigned long len1, void *data )
{
	char value[64], *end;
	
	if (buff && !copy_from_user(value, buff, len1)) {

		 softq_statistic.staId = simple_strtoul(value, &end, 10);
		 DBGPRINT(RT_DEBUG_OFF, ("change  staId to %d\n", softq_statistic.staId));
		 
	}		 
	return len1;
}


static int softq_file_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{

	int len=0;

	len+=sprintf(page,"softq_filename: %s, state: %d\n",softq_statistic.fname, softq_statistic.fstate);
	
	if (len <= off+count) *eof = 1;

	*start = page + off;

	len -= off;

	if (len>count) len = count;

	if (len<0) len = 0;

	return len;
}


static int softq_file_proc_write( struct file *filp, const char *buff,unsigned long len1, void *data )

{
	char value[64], *end;
	
	if (buff && !copy_from_user(value, buff, len1)) {
		 softq_statistic.fstate = simple_strtoul(value, &end, 10);
		 DBGPRINT(RT_DEBUG_OFF, ("change file stat  to %d\n", softq_statistic.fstate));
		 softq_file_state_run(softq_statistic.fstate);
	}		 
	return len1;
}


int softq_stat_proc_init(void)
{
	softq_proc = proc_mkdir("softq_stat",NULL) ;

	if (!softq_proc) 
	{
		DBGPRINT(RT_DEBUG_ERROR, (KERN_INFO "Create softq_stat dir failed!!!\n"));
		return -1;
	}

	state_entry = create_proc_entry("stat", 0, softq_proc);

	if (!state_entry) 
	{
		DBGPRINT(RT_DEBUG_ERROR, (KERN_INFO "Create state  entry  failed!!!\n"));
		remove_proc_entry("softq_stat", NULL);
		return -1;
	}

	state_entry->read_proc = softq_state_proc_read;
	state_entry->write_proc = softq_state_proc_write;


	times_entry = create_proc_entry("times", 0, softq_proc);

	if (!times_entry) 
	{
		DBGPRINT(RT_DEBUG_ERROR, (KERN_INFO "Create times_entry  failed!!!\n"));
		remove_proc_entry("stat", softq_proc);		
		remove_proc_entry("softq_stat", NULL);
		return -1;
	}

	times_entry->read_proc = softq_times_proc_read;
	times_entry->write_proc = softq_times_proc_write;

	staId_entry = create_proc_entry("staId", 0, softq_proc);

	if (!staId_entry) 
	{
		DBGPRINT(RT_DEBUG_ERROR, (KERN_INFO "Create staId_entry  failed!!!\n"));
		remove_proc_entry("stat", softq_proc);		
		remove_proc_entry("times", softq_proc);
		remove_proc_entry("softq_stat", NULL);
		return -1;
	}

	staId_entry->read_proc = softq_staId_proc_read;
	staId_entry->write_proc = softq_staId_proc_write;

	file_entry = create_proc_entry("file", 0, softq_proc);

	if (!staId_entry) 
	{
		DBGPRINT(RT_DEBUG_ERROR, (KERN_INFO "Create staId_entry  failed!!!\n"));
		remove_proc_entry("stat", softq_proc);		
		remove_proc_entry("times", softq_proc);
		remove_proc_entry("softq_stat", NULL);
		return -1;
	}
	
	file_entry->read_proc = softq_file_proc_read;
	file_entry->write_proc = softq_file_proc_write;

	if (!file_entry) 
	{
		DBGPRINT(RT_DEBUG_ERROR, (KERN_INFO "Create file_entry  failed!!!\n"));
		remove_proc_entry("stat", softq_proc);		
		remove_proc_entry("times", softq_proc);		
		remove_proc_entry("staId", softq_proc);
		remove_proc_entry("softq_stat", NULL);
		return -1;
	}

	DBGPRINT(RT_DEBUG_OFF, (KERN_INFO "Create softq_stat ok!!!\n"));

	return 0;
}

int softq_stat_proc_exit(void)
{
	remove_proc_entry("stat", softq_proc);		
	remove_proc_entry("times", softq_proc);	
	remove_proc_entry("staId", softq_proc);	
	remove_proc_entry("file", softq_proc);
	remove_proc_entry("softq_stat", NULL);
	return 0;
}


