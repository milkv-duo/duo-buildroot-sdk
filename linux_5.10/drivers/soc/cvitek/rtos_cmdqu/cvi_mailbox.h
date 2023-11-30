#ifndef __CVI_MAILBOX_H__
#define __CVI_MAILBOX_H__

union cpu_mailbox_info_offset{
	char mbox_info;
	int reserved;
};

union cpu_mailbox_int_clr_offset{
	char mbox_int_clr;
	int reserved;
};
union cpu_mailbox_int_mask_offset{
	char mbox_int_mask;
	int reserved;
};
union cpu_mailbox_int_offset{
	char mbox_int;
	int reserved;
};
union cpu_mailbox_int_raw_offset{
	char mbox_int_raw;
	int reserved;
};

union mailbox_set{
	char mbox_set;
	int reserved;
};
union mailbox_status{
	char mbox_status;
	int reserved;
};

union cpu_mailbox_status{
	char mbox_status;
	int reserved;
};

/* register mapping refers to mailbox user guide*/
struct cpu_mbox_int{
	union cpu_mailbox_int_clr_offset  cpu_mbox_int_clr;
	union cpu_mailbox_int_mask_offset cpu_mbox_int_mask;
	union cpu_mailbox_int_offset      cpu_mbox_int_int;
	union cpu_mailbox_int_raw_offset  cpu_mbox_int_raw;
};

struct mailbox_set_register{
	union  cpu_mailbox_info_offset cpu_mbox_en[4];      //0x00, 0x04, 0x08, 0x0c
	struct cpu_mbox_int cpu_mbox_set[4];                //0x10~0x1C, 0x20~0x2C, 0x30~0x3C, 0x40~0x4C
	int    reserved[4];                                 //0x50~0x5C
	union  mailbox_set mbox_set;                        //0x60
	union  mailbox_status mbox_status;                  //0x64
	int    reserved2[2];                                //0x68~0x6C
	union  cpu_mailbox_status cpu_mbox_status[4];       //0x70
};

struct mailbox_done_register{
	union  cpu_mailbox_info_offset cpu_mbox_done_en[4];
	struct cpu_mbox_int cpu_mbox_done[4];
};

volatile struct mailbox_set_register *mbox_reg;
volatile struct mailbox_done_register *mbox_done_reg;
volatile unsigned long *mailbox_context; // mailbox buffer context is 64 Bytess

#define MAILBOX_MAX_NUM         0x0008
#define MAILBOX_DONE_OFFSET     0x0002
#define MAILBOX_CONTEXT_OFFSET  0x0400

// C906B
#define RECEIVE_CPU  1
// C906L
#define SEND_TO_CPU  2

#endif // end of__CVI_MAILBOX_H__

