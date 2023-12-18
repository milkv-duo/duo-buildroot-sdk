/*
 Module Name:
 	nic.h
 
Abstract:
	NIC related define and macro.

Revision History:
	Who		When		What
	-------	----------	----------------------------------------------
	Kun Wu	02-11-2014	Created

*/



#ifndef	__NIC_H__
#define	__NIC_H__

#define IS_MT7603(_pAdapter)				((_pAdapter->MACVersion & 0x0000FFFF) == 0x7603)
#define IS_MT7636(_pAdapter)				((_pAdapter->MACVersion & 0x0000FFFF) == 0x7636 ||(_pAdapter->MACVersion & 0x0000FFFF) == 0x7606)

#define SEQ_CMD_FW_SCATTERS			0x90
#define SEQ_CMD_FW_STARTREQ			0x91
#define SEQ_CMD_FW_STARTTORUN		0x92
#define SEQ_CMD_FW_RESTART			0x93
#define SEQ_CMD_ROM_PATCH_SEMAPHORE	0xA0
#define SEQ_CMD_ROM_PATCH_STARTREQ	0xA1
#define SEQ_CMD_ROM_PATCH_FINISH		0xA2

//only in MT7636
typedef enum{
	ILM = 0,
	DLM
}RAM_Type;

typedef enum{
	MT7603_RAM = 0,
	MT7636_RAM,
	MT7636_ROM_PATCH
}FW_Type;

#endif __NIC_H__