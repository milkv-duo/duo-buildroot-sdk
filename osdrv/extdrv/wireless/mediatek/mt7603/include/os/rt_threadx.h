/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002-2009, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

    Module Name:
	rt_threadx.h

    Abstract:

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
*/

#ifndef __RT_THREADX_H__
#define __RT_THREADX_H__

#include <string.h>                /* memcpy(), strcpy() */
#include "tx_api.h"                /* The ThreadX API. */

#ifdef SIGMATEL_SDK
#include "dc_type.h"
#include "dc_return_codes.h"	/* SDK should include these two files.  They contain the basic definitions and types. */
#include "dc_log_api.h"
#include "dc_rammgr_api.h"
#include "dc_net_drv_api.h"
#include "dc_time_api.h"
#include "errno.h"
#include "dc_am_malloc_api.h"		/* for dc_am_global_setup() */
#include "dc_serial_flash_api.h"   	/* Serial Flash Driver */
#include "dc_flash_api.h"       	/* Flash API */
#include "nwk_init/net_sy.h"    	/* Sigmatel wifi system manager API */
#include "dc_nvram_api.h"       	/* NVRAM API */
/*#include "dc_int_api.h" */

#ifdef RTMP_USB_SUPPORT
#include "dc_usb_host_stack_api.h"
#include "dc_usb20.h"
#include "dc_cache_api.h"
#endif /* RTMP_USB_SUPPORT */

#endif /* SIGMATEL_SDK */

#ifdef EBS_RTIP
#include "rtip.h"
#include "rtipapi.h"
#endif /* EBS_RTIP */

#ifdef RTMP_USB_SUPPORT

#ifdef SIGMATEL_SDK
typedef struct DC_USB_DEVICE_STRUCT	*PUSB_DEV;
typedef struct DC_URB_STRUCT *purbb_t;
/*typedef struct usb_ctrlrequest devctrlrequest; */
#endif /* SIGMATEL_SDK */

#endif /* RTMP_USB_SUPPORT */

#include "dc_radef_common.h"
#include "radef.h"
#include "ra_ac.h"
#include "rt_net_task.h"

/***********************************************************************************
 *	Profile related sections
 ***********************************************************************************/
#ifdef CONFIG_AP_SUPPORT
#ifdef RTMP_MAC_PCI
#define AP_PROFILE_PATH			"/etc/Wireless/RT2860AP/RT2860AP.dat"
#define AP_RTMP_FIRMWARE_FILE_NAME "/etc/Wireless/RT2860AP/RT2860AP.bin"
#define AP_DRIVER_VERSION			"2.3.0.0"
#ifdef MULTIPLE_CARD_SUPPORT
#define CARD_INFO_PATH			"/etc/Wireless/RT2860AP/RT2860APCard.dat"
#endif /* MULTIPLE_CARD_SUPPORT */
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
#define AP_PROFILE_PATH			"/etc/Wireless/RT2870AP/RT2870AP.dat"
#define AP_RTMP_FIRMWARE_FILE_NAME "/etc/Wireless/RT2870AP/RT2870AP.bin"
#define AP_DRIVER_VERSION			"2.1.0.0"
#ifdef MULTIPLE_CARD_SUPPORT
#define CARD_INFO_PATH			"/etc/Wireless/RT2870AP/RT2870APCard.dat"
#endif /* MULTIPLE_CARD_SUPPORT */
#endif /* RTMP_MAC_USB */

#ifdef RTMP_RBUS_SUPPORT
/* This used for rbus-based chip, maybe we can integrate it together. */
#define RTMP_FIRMWARE_FILE_NAME		"/etc_ro/Wireless/RT2860AP/RT2860AP.bin"
#define PROFILE_PATH			"/etc/Wireless/RT2860i.dat"
#define AP_PROFILE_PATH_RBUS		"/etc/Wireless/RT2860/RT2860.dat"
#define RT2880_AP_DRIVER_VERSION	"1.0.0.0"
#endif /* RTMP_RBUS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */


#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_MAC_PCI
#define STA_PROFILE_PATH			"/etc/Wireless/RT2860STA/RT2860STA.dat"
#define STA_DRIVER_VERSION			"2.1.0.0"
#ifdef MULTIPLE_CARD_SUPPORT
#define CARD_INFO_PATH			"/etc/Wireless/RT2860STA/RT2860STACard.dat"
#endif /* MULTIPLE_CARD_SUPPORT */
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
#define STA_PROFILE_PATH			"/etc/Wireless/RT2870STA/RT2870STA.dat"
#define STA_DRIVER_VERSION			"2.1.0.0"
#ifdef MULTIPLE_CARD_SUPPORT
#define CARD_INFO_PATH			"/etc/Wireless/RT2870STA/RT2870STACard.dat"
#endif /* MULTIPLE_CARD_SUPPORT */
#endif /* RTMP_MAC_USB */

#ifdef RTMP_RBUS_SUPPORT
#define RTMP_FIRMWARE_FILE_NAME		"/etc_ro/Wireless/RT2860STA/RT2860STA.bin"
#define PROFILE_PATH			"/etc/Wireless/RT2860i.dat"
#define STA_PROFILE_PATH_RBUS	"/etc/Wireless/RT2860/RT2860.dat"
#define RT2880_STA_DRIVER_VERSION		"1.0.0.0"
#endif /* RTMP_RBUS_SUPPORT */

#endif /* CONFIG_STA_SUPPORT */


/***********************************************************************************
 *	Compiler related definitions
 ***********************************************************************************/
#undef __inline
#define __inline		static inline
#define IN
#define OUT
#define INOUT
#define NDIS_STATUS		INT


/***********************************************************************************
 *	OS Specific definitions and data structures
 ***********************************************************************************/
 typedef unsigned int		ra_dma_addr_t;

typedef struct _iface RTP_FAR *PNET_DEV;
typedef void 				* PNDIS_PACKET;
typedef char				NDIS_PACKET;
typedef PNDIS_PACKET	* PPNDIS_PACKET;
typedef unsigned long		NDIS_PHYSICAL_ADDRESS;
typedef	unsigned long		* PNDIS_PHYSICAL_ADDRESS;
typedef void				* NDIS_HANDLE;
typedef char 				* PNDIS_BUFFER;

typedef struct ifreq		NET_IOCTL;
typedef struct ifreq		* PNET_IOCTL;

typedef struct TX_THREAD_STRUCT 	*RTMP_OS_PID;
typedef struct TX_SEMAPHORE_STRUCT	RTMP_OS_SEM;

#define RT_MOD_DEC_USE_COUNT()	do{}while(0)
#define RT_MOD_INC_USE_COUNT()	do{}while(0)

#define RTMP_INC_REF(_A)		0
#define RTMP_DEC_REF(_A)		0
#define RTMP_GET_REF(_A)		0

#define isxdigit(_char)  \
	(('0' <= (_char) && (_char) <= '9') ||\
	 ('a' <= (_char) && (_char) <= 'f') || \
	 ('A' <= (_char) && (_char) <= 'F') \
	)

#define isdigit(ch) \
	((ch >= '0') && (ch <= '9'))


#define tolower(ch) ((ch >= 'A' && (ch) <= 'Z') ? (ch) - ('A' - 'a') : (ch))

#define toupper(ch) ((ch >= 'a' && (ch) <= 'z') ? (ch) + ('A' - 'a') : (ch))


#define IWEVCUSTOM	0x8C02		/* Driver specific ascii string */
#define IW_PRIV_SIZE_MASK       0x07FF  /* Max number of those args */

/* Define Linux ioctl relative structure, keep only necessary things */
struct iw_point
{
	PVOID		pointer;
	USHORT		length;
	USHORT		flags;
};
	
union iwreq_data
{
	struct iw_point data;
};

struct iwreq {
	union   iwreq_data      u;
};

/* Maximum size of returned data, sync with wireless.h in Linux */
#define IW_SCAN_MAX_DATA        4096    /* In bytes */

#define RTMP_OS_MAX_SCAN_DATA_GET()		IW_SCAN_MAX_DATA


/* TODO: for ifreq structure, we need to check that if there are any difference between Linux and VxWorks */
#if 0
struct ifreq {
	union   iwreq_data      u;
};
#endif


#ifdef EBS_RTIP
/*
	For threadX, we don't have a pointer in OS Net Device structure to hook our private device
	control data structure(i.e., the "RTMP_ADAPTER" data structure), so, we delcare it as a gobal
	paramters so that driver can easily access it.
*/
extern UCHAR *RT_WIFI_DevCtrl;
#endif /* EBS_RTIP */


/*
 *	Data unalignment handling related definitions and data structure
 */
extern void bad_unaligned_access_length(void) __attribute__((noreturn));

struct UINT64 { UINT64 x __attribute__((packed)); };
struct UINT32 { UINT32 x __attribute__((packed)); };
struct UINT16 { UINT16 x __attribute__((packed)); };

/*
 * Elemental unaligned loads 
 */
static inline UINT64 __uldq(const UINT64 *addr)
{
	const struct UINT64 *ptr = (const struct UINT64 *) addr;
	return ptr->x;
}

static inline UINT32 __uldl(const UINT32 *addr)
{
	const struct UINT32 *ptr = (const struct UINT32 *) addr;
	return ptr->x;
}



/*
 * Elemental unaligned stores 
 */
static inline void __ustq(UINT64 val, UINT64 *addr)
{
	struct UINT64 *ptr = (struct UINT64 *) addr;
	ptr->x = val;
}

static inline void __ustl(UINT32 val, UINT32 *addr)
{
	struct UINT32 *ptr = (struct UINT32 *) addr;
	ptr->x = val;
}

static inline void __ustw(UINT16 val, UINT16 *addr)
{
	struct UINT16 *ptr = (struct UINT16 *) addr;
	ptr->x = val;
}

static inline USHORT __uldw(const UINT16 *addr)
{
	const struct UINT16 *ptr = (const struct UINT16 *) addr;
	return ptr->x;
}

#define __get_unaligned(ptr, size) ({		\
	const void *__gu_p = ptr;		\
	__typeof__(*(ptr)) val;			\
	switch (size) {				\
	case 1:					\
		val = *(const UINT8 *)__gu_p;	\
		break;				\
	case 2:					\
		val = __uldw(__gu_p);		\
		break;				\
	case 4:					\
		val = __uldl(__gu_p);		\
		break;				\
	case 8:					\
		val = __uldq(__gu_p);		\
		break;				\
	default:				\
		bad_unaligned_access_length();	\
	};					\
	val;					\
})

#define __put_unaligned(val, ptr, size)		\
do {						\
	void *__gu_p = ptr;			\
	switch (size) {				\
	case 1:					\
		*(UINT8 *)__gu_p = val;		\
	        break;				\
	case 2:					\
		__ustw(val, __gu_p);		\
		break;				\
	case 4:					\
		__ustl(val, __gu_p);		\
		break;				\
	case 8:					\
		__ustq(val, __gu_p);		\
		break;				\
	default:				\
	    	bad_unaligned_access_length();	\
	};					\
} while(0)


#define get_unaligned(ptr) \
	__get_unaligned((ptr), sizeof(*(ptr)))
#define put_unaligned(x,ptr) \
	__put_unaligned((UINT64)(x), (ptr), sizeof(*(ptr)))


/********************** MACROS FOR HANDLING ENDIANESS *************************/
#ifdef RT_BIG_ENDIAN
/*******************************************************************************
 * MACRO NAME    : OS_UINT32_CPU_TO_LE
 * DESCRIPTION   : Macro to convert an 32 bit integer from CPU format to the
 *                 little endian format.
 * PARAMETERS    : DATA IN  Data to be converted.
 * RETURN TYPE   : None
 ******************************************************************************/
#define OS_UINT32_CPU_TO_LE(DATA)                                       \
        (((DATA & 0xFF) << 24) |                                        \
                (((DATA >> 8) & 0xFF) << 16) |                          \
                (((DATA >> 16) & 0xFF) << 8) |                          \
                ((DATA >> 24) & 0xFF))

/*******************************************************************************
 * MACRO NAME    : OS_UINT32_LE_TO_CPU
 * DESCRIPTION   : Macro to convert an 32 bit integer from little endian format
 *                 to the CPU format.
 * PARAMETERS    : DATA IN  Data to be converted.
 * RETURN TYPE   : None
 ******************************************************************************/
#define OS_UINT32_LE_TO_CPU(DATA)                                       \
        (((DATA & 0xFF) << 24) |                                        \
                (((DATA >> 8) & 0xFF) << 16) |                          \
                (((DATA >> 16) & 0xFF) << 8) |                          \
                ((DATA >> 24) & 0xFF))

/*******************************************************************************
 * MACRO NAME    : OS_UINT16_LE_TO_CPU
 * DESCRIPTION   : Macro to convert an 16 bit integer from little endian format
 *                 to the CPU format.
 * PARAMETERS    : DATA IN  Data to be converted.
 * RETURN TYPE   : None
 ******************************************************************************/
#define OS_UINT16_LE_TO_CPU(DATA)                                       \
        ((((DATA >> 8) & 0xFF) | ((DATA & 0xFF) << 8)))

/*******************************************************************************
 * MACRO NAME    : OS_UINT16_CPU_TO_LE
 * DESCRIPTION   : Macro to convert an 16 bit integer from CPU format to the
 *                 little endian format.
 * PARAMETERS    : DATA IN  Data to be converted.
 * RETURN TYPE   : None
 ******************************************************************************/
#define OS_UINT16_CPU_TO_LE(DATA)                                       \
        ((((DATA >> 8) & 0xFF) | ((DATA & 0xFF) << 8)))

#define OS_BUFFER_CPU_TO_LE(BUFFER, LENGTH)

#define OS_BUFFER_LE_TO_CPU(BUFFER, LENGTH)

#else

/*
 * Macros for handling the endianess on little endian
 * systems
 */

/*******************************************************************************
 * MACRO NAME    : OS_BUFFER_LE_TO_CPU
 * DESCRIPTION   : Macro to convert a buffer from little endian format to the
 *                 CPU format.
 * PARAMETERS    : BUFFER   IN  Pointer to the buffer location.
 *                 LENGTH   IN  Number of bytes to be converted.
 * RETURN TYPE   : None
 ******************************************************************************/
#define OS_BUFFER_LE_TO_CPU(BUFFER, LENGTH)

/*******************************************************************************
 * MACRO NAME    : OS_BUFFER_CPU_TO_LE
 * DESCRIPTION   : Macro to convert a buffer from CPU format to the little
 *                 endian format .
 * PARAMETERS    : BUFFER   IN  Pointer to the buffer location.
 *                 LENGTH   IN  Number of bytes to be converted.
 * RETURN TYPE   : None
 ******************************************************************************/
#define OS_BUFFER_CPU_TO_LE(BUFFER, LENGTH)

/*******************************************************************************
 * MACRO NAME    : OS_UINT16_LE_TO_CPU
 * DESCRIPTION   : Macro to convert an 16 bit integer from little endian format
 *                 to the CPU format.
 * PARAMETERS    : DATA IN  Data to be converted.
 * RETURN TYPE   : None
 ******************************************************************************/
#define OS_UINT16_LE_TO_CPU(DATA)                                       \
        DATA

/*******************************************************************************
 * MACRO NAME    : OS_UINT16_CPU_TO_LE
 * DESCRIPTION   : Macro to convert an 16 bit integer from CPU format to the
 *                 little endian format.
 * PARAMETERS    : DATA IN  Data to be converted.
 * RETURN TYPE   : None
 ******************************************************************************/
#define OS_UINT16_CPU_TO_LE(DATA)                                       \
        DATA

/*******************************************************************************
 * MACRO NAME    : OS_UINT32_LE_TO_CPU
 * DESCRIPTION   : Macro to convert an 32 bit integer from little endian format
 *                 to the CPU format.
 * PARAMETERS    : DATA IN  Data to be converted.
 * RETURN TYPE   : None
 ******************************************************************************/
#define OS_UINT32_LE_TO_CPU(DATA)                                       \
        DATA

/*******************************************************************************
 * MACRO NAME    : OS_UINT32_CPU_TO_LE
 * DESCRIPTION   : Macro to convert an 32 bit integer from CPU format to the
 *                 little endian format.
 * PARAMETERS    : DATA IN  Data to be converted.
 * RETURN TYPE   : None
 ******************************************************************************/
#define OS_UINT32_CPU_TO_LE(DATA)                                       \
        DATA

#endif /* RT_BIG_ENDIAN */

/***********************************************************************************
 *	Network related constant definitions
 ***********************************************************************************/
#ifndef IFNAMSIZ
/*
	For the definition of IFNAMESIZ, should refer to the device name exactlly the Network subsytem 
	of Target OS declared.
	For EBS_RTIP, the max length of device name is 22, which defined as "MAX_DEVICE_NAME_LEN" 
	and in "PDEVTABLE->device_name[MAX_DEVICE_NAME_LEN]"
*/
#define IFNAMSIZ 16
#endif

#ifndef MAC_ADDR_LEN
#define MAC_ADDR_LEN	6
#endif

#define ETH_P_IPV6	0x86DD
#define ETH_P_IP	0x0800

#define NDIS_STATUS_SUCCESS                     0x00
#define NDIS_STATUS_FAILURE                     0x01
#define NDIS_STATUS_INVALID_DATA				0x02
#define NDIS_STATUS_RESOURCES                   0x03

#define NDIS_SET_PACKET_STATUS(_p, _status)			do{} while(0)
#define NdisWriteErrorLogEntry(_a, _b, _c, _d)		do{} while(0)

/* statistics counter */
#define STATS_INC_RX_PACKETS(_pAd, _dev)
#define STATS_INC_TX_PACKETS(_pAd, _dev)

#define STATS_INC_RX_BYTESS(_pAd, _dev, len)
#define STATS_INC_TX_BYTESS(_pAd, _dev, len)

#define STATS_INC_RX_ERRORS(_pAd, _dev)
#define STATS_INC_TX_ERRORS(_pAd, _dev)

#define STATS_INC_RX_DROPPED(_pAd, _dev)
#define STATS_INC_TX_DROPPED(_pAd, _dev)


/***********************************************************************************
 *	Ralink Specific network related constant definitions
 ***********************************************************************************/
#define RT_DEV_PRIV_FLAGS_GET(__pDev)									\
	((PNET_DEV)pDev)->priv_flags

#define RT_DEV_PRIV_FLAGS_SET(__pDev, __PrivFlags)						\
	((PNET_DEV)pDev)->priv_flags = __PrivFlags

#ifdef CONFIG_STA_SUPPORT
#define NDIS_PACKET_TYPE_DIRECTED		0
#define NDIS_PACKET_TYPE_MULTICAST		1
#define NDIS_PACKET_TYPE_BROADCAST		2
#define NDIS_PACKET_TYPE_ALL_MULTICAST	3
#define NDIS_PACKET_TYPE_PROMISCUOUS	4
#endif /* CONFIG_STA_SUPPORT */


/***********************************************************************************
 *	OS signaling related constant definitions
 ***********************************************************************************/
#ifdef CONFIG_AP_SUPPORT
#ifdef IAPP_SUPPORT
typedef struct _RT_SIGNAL_STRUC {
	unsigned short Sequence;
	unsigned char MacAddr[MAC_ADDR_LEN];
	unsigned char CurrAPAddr[MAC_ADDR_LEN];
	unsigned char Sig;
} RT_SIGNAL_STRUC, *PRT_SIGNAL_STRUC;

/* definition of signal */
#define SIG_NONE					0
#define SIG_ASSOCIATION				1
#define SIG_REASSOCIATION			2
#endif /* IAPP_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */


/***********************************************************************************
 *	OS file operation related data structure definitions
 ***********************************************************************************/
 /*
	NOTE: For ThreadX, currently don't support file system. so the following definitions is useless.
 */
typedef int RTMP_OS_FD;

typedef struct _RTMP_OS_FS_INFO_
{
	int	fsinfo; /* Dummy structure, for cross-platform compatibility */
}RTMP_OS_FS_INFO;

#define IS_FILE_OPEN_ERR(_fd) 	((_fd) < 0)

#define O_RDONLY 0
#define O_WRONLY 0
#define O_CREAT	0
#define O_TRUNC 0

/***********************************************************************************
 *	OS semaphore related data structure and definitions
 ***********************************************************************************/
/* TODO: We need to find a wrapper function for in_interrupt() */
#define in_interrupt() (0)
#define in_atomic()	(0)

struct os_lock  {
	TX_MUTEX	lock;
	UINT		mutex_status;
};

typedef struct os_lock		NDIS_SPIN_LOCK;

/*
	For ThreadX, it use TX_MUTEX for inter-thread mutual exclusion for resource protection.
	
	There is a priority_inherit parameter used for tx_mutex_create to indicate if the mutex 
	support priority inherit or not. For RT_WIFI, all threads are created with the same thread 
	priority, so the priority_inherit is not necessary and disable for all mutex created by 
	RT_WIFI driver.
*/
#define NdisAllocateSpinLock(__pReserve, __lock)      \
	do{                                       \
		 (__lock)->mutex_status = tx_mutex_create(&((__lock)->lock), "", TX_NO_INHERIT);\
		if ((__lock)->mutex_status != TX_SUCCESS)	\
			dc_log_printf("%s(%d):Create mutexlock failed!\n", __FUNCTION__, (__lock)->mutex_status);\
	}while(0)


/*
	In ThreadX, after the mutex is deleted by following function, all threads suspended waiting 
	for the mutex are resumed and given  TX_DLELETD return status.
*/
#define NdisFreeSpinLock(__lock)          \
	do{\
		(__lock)->mutex_status = tx_mutex_delete(&((__lock)->lock));\
		if ((__lock)->mutex_status != TX_SUCCESS)\
			dc_log_printf("%s(%d): delMutex failed!\n", __FUNCTION__, (__lock)->mutex_status);\
	}while(0)

/*
	Some characteristics in ThreadX
		(1). A mutex can multiple get by the same thread if this thread alreay owns this mutex
		
		(2). the wait_option parameter in tx_mutex_get defines how the service behaves if the 
			mutex is already own by another thread.
				TX_NO_WAIT 				->immediate return, only valid option in initialization
				TX_WAIT_FOREVER		->calling thread suspend indefinitely until the mutex is available
				timeout_value(1~0xfffffffe)	->timer-ticks	to stay suspended while waiting the mutex.
		
		(3). The mutex is made available if the ownership count is zero(via function "tx_mutex_put()")
		
		(4). The MUTEX CANNOT be used in ISR!!!

		(5). tx_mutex_get() is preempt-able and can be called in:
						initialization, threads, and timers
			tx_mutex_put() is preempt-able and can be called in:
						initialization and threads.
			
*/
#define RTMP_SEM_LOCK(__lock)					\
do{												\
	(__lock)->mutex_status = tx_mutex_get(&((__lock)->lock), TX_WAIT_FOREVER);		\
	if ((__lock)->mutex_status != TX_SUCCESS)\
		dc_log_printf("%s(%d): semLock failed!\n", __FUNCTION__, (__lock)->mutex_status);\
}while(0)


#define RTMP_SEM_UNLOCK(__lock)					\
do{												\
	(__lock)->mutex_status = tx_mutex_put(&((__lock)->lock));	\
	if ((__lock)->mutex_status != TX_SUCCESS)\
		dc_log_printf("%s(%d): semUnLock failed!\n", __FUNCTION__, (__lock)->mutex_status);\
}while(0)


/* sample, use semaphore lock to replace IRQ lock, 2007/11/15 */
#define RTMP_IRQ_LOCK(__lock, __irqflags)\
do{					\
	__irqflags = 0;			\
	(__lock)->mutex_status = tx_mutex_get(&((__lock)->lock), TX_WAIT_FOREVER);		\
	if ((__lock)->mutex_status != TX_SUCCESS)\
		dc_log_printf("%s(%d): IRQLock failed!\n", __FUNCTION__, (__lock)->mutex_status);\
}while(0)

#define RTMP_IRQ_UNLOCK(__lock, __irqflag)			\
do{	\
	(__lock)->mutex_status = tx_mutex_put(&((__lock)->lock));\
	if ((__lock)->mutex_status != TX_SUCCESS)\
		dc_log_printf("%s(%d): IRQUnLock failed!\n", __FUNCTION__, (__lock)->mutex_status);\
}while(0)


/*
	For ThreadX, the hardware interrupt enable/disable are controled by the following procedures.
	NOTE: 
		(1). Even one thread disable the hardware interrupt, the thread may suspend too. 
		(2). If the thread in suspend stage, the interrupt will enable again. Until the thread resume 
			again, the interrupt will go back to diable mode.
*/
#define RTMP_INT_LOCK(__lock, __irqflags)			\
do{													\
	__irqflags = tx_interrupt_control(TX_INT_DISABLE);	\
}while(0)


#define RTMP_INT_UNLOCK(__lock, __irqflag)			\
do{													\
	tx_interrupt_control((UINT)__irqflag);	\
}while(0)


#define NdisAcquireSpinLock		RTMP_SEM_LOCK
#define NdisReleaseSpinLock		RTMP_SEM_UNLOCK

typedef int atomic_t;

atomic_t atomic_read(atomic_t *v);
void atomic_inc(atomic_t *v);
void atomic_dec(atomic_t *v);
void atomic_set(atomic_t *v, int i);

#define InterlockedIncrement 	 	atomic_inc
#define NdisInterlockedIncrement 	atomic_inc
#define InterlockedDecrement		atomic_dec
#define NdisInterlockedDecrement 	atomic_dec
#define InterlockedExchange			atomic_set

typedef int	wait_queue_head_t;

#define init_waitqueue_head(_wait)		do{}while(0)
#define wait_event_interruptible_timeout(_wq, _condition, _timeout)		\
		RtmpMSleep((_timeout))


/* TODO: Shiang, maybe we can implement this data structure with EVENT notification mechanism of TheadX */
struct completion
{
	unsigned int	done;
	TX_MUTEX	wait;
};


/*
	In ThreadX, we use the TX_SEMAPHORE(counting semaphore) to implement this semaphore/event 
	trigger/wakeup mechanism
	Creation of semaphore:
		tx_semaphore_create(TX_SEMAPHORE *pSema, char *name_ptr, ULONG initial_count)
			=>pSema    : Pointer to a semaphore contorl block
			     name_ptr: Pointer to the name of the semaphore
			     init_cnt   : Specifies the initial count for this semaphore. Range from 0x0~0xffffffff
			Only available call this function in Initialization and Threads
			
		tx_semaphore_delete(TX_SEMAPHORE *pSema)
			=>pSema    : Pointer to a previously created semahore
			Only available call this function in Threads

		tx_semaphore_get(TX_SEMAPHORE *pSema, ULONG wait_opt)
			=>pSema    : Pointer to previously created counting semaphore
			     wait_opt  :Define how the service behaves if there are no instances of the semaphore 
			     			available. As a result, the specified semaphore's count is decreased by one.
			     			TX_NO_WAIT			-> only valid option for 	non-thread services.
			     			TX_WAIT_FOREVER
			     			timeout_value	(0x1~0xfffffffe ticks)
		
*/
#define RTMP_SEM_EVENT_INIT_LOCKED(_pSema, __pSemaList)	\
	do{																\
		UINT __sem_status;													\
		__sem_status = tx_semaphore_create(_pSema, "", 0);	\
		if (__sem_status != TX_SUCCESS)										\
			dc_log_printf("%s(%d):semaCreateLocked failed!\n", __FUNCTION__, __sem_status);\
	}while(0)
 	
#define RTMP_SEM_EVENT_INIT(_pSema)									\
	do{																\
		UINT __sem_status;												\
		__sem_status = tx_semaphore_create((_pSema), "", 1);					\
		if (__sem_status != TX_SUCCESS)										\
			dc_log_printf("%s(%d):semaCreate failed!\n", __FUNCTION__, __sem_status);\
	}while(0)

#define RTMP_SEM_EVENT_DESTORY(_pSema)	\
	do{																\
		UINT __sem_status;												\
		__sem_status = tx_semaphore_delete((_pSema));					\
		if (__sem_status != TX_SUCCESS)										\
			dc_log_printf("%s(%d):semaDel failed!\n", __FUNCTION__, __sem_status);\
	}while(0)

#define RTMP_SEM_EVENT_WAIT(_pSema, _status)	\
	do{\
		((_status)  = tx_semaphore_get(_pSema, TX_WAIT_FOREVER));\
		if ((_status) != TX_SUCCESS)\
			dc_log_printf("%s(%d):semGetFailed!\n", __FUNCTION__, (_status));\
	}while(0)


#define RTMP_SEM_EVENT_UP(_pSema)				\
	do{\
		UINT _status;\
		ULONG current_value = 0, suspend_cnt;\
		char *name, tx_name[100];\
		name = &tx_name[0];\
		TX_THREAD *first_suspended;\
		TX_SEMAPHORE *next_sema;\
		_status = tx_semaphore_info_get((_pSema), &name, &current_value, &first_suspended, &suspend_cnt, &next_sema);\
		if ((_status == TX_SUCCESS) && (current_value < 0xffffffff)){\
			_status = tx_semaphore_put((_pSema));\
			if ((_status) != TX_SUCCESS)\
				dc_log_printf("%s(%d):semaPut failed!\n", __FUNCTION__, (_status));\
		}else{\
			dc_log_printf("%s(%d):semaGetInfo failed(CurVal=%ld)!\n", __FUNCTION__, (_status), current_value);\
		}\
	}while(0)

#define RTMP_OS_TASK_WAKE_UP(__pTask)						\
	RTMP_SEM_EVENT_UP(&(pTask)->taskSema);

#define RTMP_OS_TASK_GET(__pTask)							\
	(__pTask)

#define RTCMDUp					OS_RTCMDUp

/***********************************************************************************
 *	OS Memory Access related data structure and definitions
 ***********************************************************************************/
#ifdef SIGMATEL_SDK
/*
	Memory allocation related functions. 

	Because SigmaTel DC RAMMGR only allow memory allocation when system 
	do initialization. So, we need a alternately way to handle runtime dynamic 
	memory allocation requirments.
*/
typedef enum _RTMP_DC_RAMMGR_BUF_ID_{
	RTMP_DC_RAM_ID_DEVCTRL = 0,  /* pAd*/
	RTMP_DC_RAM_ID_STACK,
	RTMP_DC_RAM_ID_HTTX,
	RTMP_DC_RAM_ID_HTTX_ALIGN,	/* For alternate 512 bytes aligned HTTX buffer */
	RTMP_DC_RAM_ID_TX,
	RTMP_DC_RAM_ID_RX,
	RTMP_DC_RAM_ID_CTRL_PIPE,
	RTMP_DC_RAM_ID_URB,
#ifdef EBS_RTIP
	RTMP_DC_RAM_ID_NETBUF,
	RTMP_DC_RAM_ID_PKT,
#endif /* EBS_RTIP */
 	RTMP_DC_RAM_ID_MAX,
}RTMP_DC_RAMMGER_BUF_ID;


typedef struct _RTMP_DC_RAM_LIST_{
	struct _RTMP_DC_RAM_LIST_ *next;
}RTMP_DC_RAM_LIST;

typedef struct _RTMP_DC_RAM_BLOCK_ELEMENT_
{
	RTMP_DC_RAMMGER_BUF_ID Id;
	char	*name;
	UINT32	size;
	UINT32	baseAddr;
	UINT32	endAddr;
	UINT32	alignment;
	UINT32	blkSize;
	UINT32	blkNum;
	RTMP_DC_RAM_LIST *pFreeList;
	UINT32	freeCnt;
	DC_RAMMGR_BUFFER_ENTRY dcMemBufEntry;
	NDIS_SPIN_LOCK DcRamBlkLock;
	BOOLEAN		bValid;
}RTMP_DC_RAM_BLOCK_ELEMENT;

extern RTMP_DC_RAM_BLOCK_ELEMENT RtmpDCRamMgrPSBBuf[];

#endif /* SIGMATEL_SDK */

#define MEM_ALLOC_FLAG      (0)
#define GFP_ATOMIC          (0)
#define GFP_DMA             (0)
#define GFP_KERNEL          (0)

#define kmalloc(_size, _flags)	dc_malloc((_size))
#define kfree(_ptr)			dc_free((_ptr))

#define vmalloc(_size)		malloc((_size))
#define vfree(_ptr)			free((_ptr))


#define NdisMoveMemory(Destination, Source, Length) memmove(Destination, Source, Length)
#define NdisCopyMemory(Destination, Source, Length) memcpy(Destination, Source, Length)
#define NdisZeroMemory(Destination, Length)         memset(Destination, 0, Length)
#define NdisFillMemory(Destination, Length, Fill)   memset(Destination, Fill, Length)
#define NdisCmpMemory(Destination, Source, Length)  memcmp(Destination, Source, Length)
#define NdisEqualMemory(Source1, Source2, Length)   (!memcmp(Source1, Source2, Length))
#define RTMPEqualMemory(Source1, Source2, Length)	(!memcmp(Source1, Source2, Length))

#define MlmeAllocateMemory(_pAd, _ppVA)		os_alloc_mem(_pAd, _ppVA, MGMT_DMA_BUFFER_SIZE)
#define MlmeFreeMemory(_pAd, _pVA)			os_free_mem(_pAd, _pVA)

#define COPY_MAC_ADDR(Addr1, Addr2)         memcpy((Addr1), (Addr2), MAC_ADDR_LEN)

static inline int copy_to_user(void *pDst, void *pSrc, int len)
{
	memcpy(pDst,pSrc,len);
	return 0;
}

static inline int copy_from_user(void *pDst, void *pSrc, int len)
{
	memcpy(pDst, pSrc, len);
	return 0;
}

#define wmb()				do{}while(0)

#ifdef SIGMATEL_SDK

#define RTMP_DC_RAM_CTRL_PIPE_NUM	10
#define RTMP_DC_RAM_CTRL_PIPE_SIZE	256

#endif /* SIGMATEL_SDK */

/***********************************************************************************
 *	OS task related data structure and definitions
 ***********************************************************************************/
#define RTMP_TX_TASK_NUM 5		/* 4 for mgmtTask, 1 for NetTask, each with stack size 8092bytes */
#define RTMP_TX_TASK_STACK_SIZE 0x8000
#define RTMP_TX_TASK_PRIORITY 16

typedef void (*RTMP_OS_TASK_CALLBACK)(ULONG);

#define tasklet_hi_schedule(_pTask)														\
	do{																					\
		RTMP_ADAPTER *pAd;																\
		DC_RETURN_CODE _retVal;															\
		pAd = (RTMP_ADAPTER *)RT_WIFI_DevCtrl;											\
		POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;									\
		_retVal = RTMPQueueNetTask(&pObj->NetTaskQueue, _pTask);						\
		if (_retVal == NDIS_STATUS_SUCCESS)												\
			(_pTask)->taskStatus = RTMP_NET_TASK_PENDING;								\
		else																			\
			dc_log_printf("NetTask(%s) add failed!\n", (INT32)((_pTask)->taskName));	\
	}while(0)

#define RTMP_OS_TASKLET_SCHE(__pTasklet)							\
		tasklet_hi_schedule(__pTasklet)

#define RTMP_OS_MGMT_TASK_FLAGS	0

/* TODO: These definitions are for RT2870, not verify yet. */
typedef	int	THREAD_PID;
#define	THREAD_PID_INIT_VALUE	(0)
#define	GET_PID(_v)	do{}while(0)
#define	GET_PID_NUMBER(_v)	do{}while(0)
#define CHECK_PID_LEGALITY(_pid) if(_pid)
#define KILL_THREAD_PID(_A, _B, _C) (0)



/***********************************************************************************
 * Timer related definitions and data structures.
 **********************************************************************************/
/* TODO: Need to check about this in ThreadX */
#ifndef HZ
#define HZ 100
#endif

#ifdef SIGMATEL_SDK
/*
	For SigmaTel SDK, the tick initialized by dc_time_init(UINT32 sys_period_usec, UINT32 main_clock_hz)
	There are two way to get the OS_HZ
		1. by sys_period_usec (Which defined in main.c and now SYS_TIMESLICE_USEC = 1000)
			=> OS_HZ = 1 second / sys_period_usec = (1 * 1000 * 1000) / sys_period_usec = x ticks
		2. by dc_timer_ticks_from_usec(UINT32 *system_ticks, UINT32 usec) with usec = 1000000
		3. by dc_utime_get_from_tick(UINT32 *per_tick_usecs) to retrieves the system tick period in microseconds.
			=> OS_HZ = 1sec/per_tick_usecs = (1* 1000 * 1000) / per_tick_usecs = x ticks
	Here we use the method 1 to convert to OS_HZ.
*/
#define SYS_TIMESLICE_USEC 1000   /* NOTE: for this constant definition, need to sync with main.c */
#define OS_HZ	((1 * 1000 * 1000) / SYS_TIMESLICE_USEC)  /*HZ */
#endif /* SIGMATEL_SDK */

typedef struct TX_TIMER_STRUCT RTMP_OS_TIMER;
typedef struct TX_TIMER_STRUCT NDIS_MINIPORT_TIMER;


typedef void (*TIMER_FUNCTION)(unsigned long);


/*
	The OS_WAIT ONLY can be called from a application thread!
*/
#define OS_WAIT(_time) 			\
	do{\
		unsigned long _ticks = (_time * OS_HZ) /1000; \
		if (_ticks == 0)\
			_ticks++;\
		tx_thread_sleep(_ticks);\
	}while(0)

#define typecheck(type,x) \
({      type __dummy; \
        typeof(x) __dummy2; \
        (void)(&__dummy == &__dummy2); \
        1; \
})

#define time_after(a,b)         \
        (typecheck(unsigned long, a) && \
         typecheck(unsigned long, b) && \
         ((long)(b) - (long)(a) < 0))

#define time_before(a,b)        time_after(b,a)

#define RTMP_TIME_BEFORE(a,b)	time_before(b,a)
#define RTMP_TIME_AFTER(a,b) 	time_after(a, b)
#define mdelay(_us)		RtmpusecDelay((_us))

static inline void NdisGetSystemUpTime(ULONG *sys_time)
{
	*sys_time = tx_time_get();
}


/***********************************************************************************
 *	OS specific cookie data structure binding to RTMP_ADAPTER
 ***********************************************************************************/

struct os_cookie {
#ifdef RTMP_MAC_USB
#ifdef SIGMATEL_SDK
       PUSB_DEV 				pUsb_Dev;
#endif /* SIGMATEL_SDK */
#endif /* RTMP_MAC_USB */

	/*
		For ThreadX, all task declaration should defined between "_taskStartAddr" and "_taskEndAddr", 
		because we use "(_taskEndAddr - _taskStartAddr)/ sizeof(RTMP_NET_TASK_STRUCT)" to calculate
		the total number to tasklet will used in ThreadX, and depends on this number to allocate the stack
		memory segments.
	*/
	ULONG _taskStartAddr;
	RTMP_NET_TASK_QUEUE NetTaskQueue;
	RTMP_NET_TASK_STRUCT rx_done_task;
	RTMP_NET_TASK_STRUCT mgmt_dma_done_task;
	RTMP_NET_TASK_STRUCT ac0_dma_done_task;
#ifdef CONFIG_ATE
	RTMP_NET_TASK_STRUCT ate_ac0_dma_done_task;
#endif /* CONFIG_ATE */
	RTMP_NET_TASK_STRUCT ac1_dma_done_task;
	RTMP_NET_TASK_STRUCT ac2_dma_done_task;
	RTMP_NET_TASK_STRUCT ac3_dma_done_task;
	RTMP_NET_TASK_STRUCT hcca_dma_done_task;
	RTMP_NET_TASK_STRUCT tbtt_task;
#ifdef RTMP_MAC_PCI
	RTMP_NET_TASK_STRUCT fifo_statistic_full_task;
#endif /* RTMP_MAC_PCI */

#ifdef CONFIG_AP_SUPPORT
#ifdef UAPSD_SUPPORT
	RTMP_NET_TASK_STRUCT uapsd_eosp_sent_task;
#endif /* UAPSD_SUPPORT */

#ifdef DFS_SUPPORT
#ifdef DFS_SOFTWARE_SUPPORT
	RTMP_NET_TASK_STRUCT pulse_radar_detect_task;
	RTMP_NET_TASK_STRUCT width_radar_detect_task;
#endif /* DFS_SOFTWARE_SUPPORT */
#endif /* DFS_SUPPORT */

#ifdef CARRIER_DETECTION_SUPPORT
	RTMP_NET_TASK_STRUCT carrier_sense_task;
#endif /* CARRIER_DETECTION_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef RTMP_MAC_USB
	RTMP_NET_TASK_STRUCT null_frame_complete_task;
	RTMP_NET_TASK_STRUCT pspoll_frame_complete_task;
#endif /* RTMP_MAC_USB */

	ULONG _taskEndAddr;

	/*
		signal related data elements
	*/
	unsigned long			apd_pid; /*802.1x daemon pid */
#ifdef CONFIG_AP_SUPPORT
#ifdef IAPP_SUPPORT
	RT_SIGNAL_STRUC			RTSignal;
	unsigned long			IappPid; /*IAPP daemon pid */
#endif /* IAPP_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */
#ifdef WAPI_SUPPORT
	unsigned long			wapi_pid; /*wapi daemon pid */
#endif /* WAPI_SUPPORT */

	/* 
		IOCTL related data elements
	*/
	INT						ioctl_if_type;
	INT 					ioctl_if;

#ifdef WSC_AP_SUPPORT
	/*
		WSC related data elements
	*/
	int					(*WscMsgCallBack)();
#endif /* WSC_AP_SUPPORT */
};

typedef struct os_cookie	* POS_COOKIE;


/***********************************************************************************
 *	OS debugging and printing related definitions and data structure
 ***********************************************************************************/
#define printk	dc_log_printf
#define PRINT_MAC(addr)	\
		addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]
	
#ifdef DBG
extern ULONG RTDebugLevel;
extern ULONG RTDebugFunc;

#define DBGPRINT_RAW(Level, Fmt)    \
do{                                   \
    if (Level <= RTDebugLevel)      \
    {                               \
		dc_log_printf Fmt; 	\
    }                               \
}while(0)

#define DBGPRINT(Level, Fmt)    DBGPRINT_RAW(Level, Fmt)


#define DBGPRINT_ERR(Fmt)           \
do{                                   			\
	dc_log_printf("ERROR!!! ");      	\
    	dc_log_printf Fmt;                 	 	\
}while(0)

#define DBGPRINT_S(Fmt)		\
do{									\
	dc_log_printf Fmt;					\
}while(0)
#else
#define DBGPRINT(Level, Fmt)		do{}while(0)
#define DBGPRINT_RAW(Level, Fmt)	do{}while(0)
#define DBGPRINT_S(Fmt)		do{}while(0)
#define DBGPRINT_ERR(Fmt)			do{}while(0)
#define RTMP_ISR_LOG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)	do{}while(0)
#endif

#undef  ASSERT
#define ASSERT(x)                                                               \
do{                                                                             \
    if (!(x))                                                                   \
    {                                                                           \
        dc_log_printf("%s:%d assert " #x "failed\n", __FILE__, __LINE__);    	\
    }                                                                           \
}while(0)

extern void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen);


/***********************************************************************************
 * Device DMA Access related definitions and data structures.
 **********************************************************************************/
/* This defines the direction arg to the DMA mapping routines. */
#define PCI_DMA_BIDIRECTIONAL	0
#define PCI_DMA_TODEVICE	1
#define PCI_DMA_FROMDEVICE	2
#define PCI_DMA_NONE		3

#define RTMP_PCI_DMA_TODEVICE		PCI_DMA_TODEVICE
#define RTMP_PCI_DMA_FROMDEVICE		PCI_DMA_FROMDEVICE

#ifdef RTMP_MAC_PCI
/* signature for performance enhancement */
#ifdef CACHE_DMA_SUPPORT
extern UCHAR CACHE_DMA_SIGNAUTURE[];
#endif /* CACHE_DMA_SUPPORT */

#endif /* RTMP_MAC_PCI */

#ifdef RT2870
#define PCI_MAP_SINGLE(_handle, _ptr, _size, _sd_idx, _dir) (ULONG)0

#define PCI_UNMAP_SINGLE(_handle, _ptr, _size, _dir)
#endif /* RT2870 */


/*
 * ULONG
 * RTMP_GetPhysicalAddressLow(
 *   IN NDIS_PHYSICAL_ADDRESS  PhysicalAddress);
 */
#define RTMP_GetPhysicalAddressLow(PhysicalAddress)		(PhysicalAddress)

/*
 * ULONG
 * RTMP_GetPhysicalAddressHigh(
 *   IN NDIS_PHYSICAL_ADDRESS  PhysicalAddress);
 */
#define RTMP_GetPhysicalAddressHigh(PhysicalAddress)		(0)

/*
 * VOID
 * RTMP_SetPhysicalAddressLow(
 *   IN NDIS_PHYSICAL_ADDRESS  PhysicalAddress,
 *   IN ULONG  Value);
 */
#define RTMP_SetPhysicalAddressLow(PhysicalAddress, Value)	\
			PhysicalAddress = Value;

/*
 * VOID
 * RTMP_SetPhysicalAddressHigh(
 *   IN NDIS_PHYSICAL_ADDRESS  PhysicalAddress,
 *   IN ULONG  Value);
 */
#define RTMP_SetPhysicalAddressHigh(PhysicalAddress, Value)

/* in VxWorks, MMU is disabled so Physical address == virtual address */
#define VIRT_TO_PHYS(address) 	(address)

#define NdisMIndicateStatus(_w, _x, _y, _z)



/***********************************************************************************
 * Device Register I/O Access related definitions and data structures.
 **********************************************************************************/
#define readl(_x) (*((volatile UINT32 *)(_x)))
#define readb(_x) (*((volatile UINT8 *)(_x)))
#define writel(_v,  _x)							\
	do{												\
		*(volatile UINT *)(_x) = (_v);					\
	}while(0)

#define writew(_v, _x)			\
	do{						\
		*(UINT16 *)(_x) = (_v); \
		/*volatile  sysOutWord((_v), (_x))	;*/		\
	}while(0)
#define writeb(_v, _x)			\
	do{						\
		*(UINT8 *)(_x) = (_v); 	\
		/*sysOutByte((_v), (_x));*/		\
	}while(0)

#ifdef RTMP_MAC_PCI
#if defined(INF_TWINPASS) || defined(INF_DANUBE) || defined(IKANOS_VX_1X0)
#ifdef RELEASE_EXCLUDE
	/* Patch for ASIC turst read/write bug, needs to remove after metel fix */
#endif /* RELEASE_EXCLUDE */
#define RTMP_IO_FORCE_READ32(_A, _R, _pV)									\
{																	\
	(*_pV = readl((void *)((_A)->CSRBaseAddress + (_R))));			\
	(*_pV = SWAP32(*((UINT32 *)(_pV))));                           \
}

#define RTMP_IO_READ32(_A, _R, _pV)									\
do{																	\
    if ((_A)->bPCIclkOff == FALSE)                                      \
    {                                                                   \
	(*_pV = readl((void *)((_A)->CSRBaseAddress + (_R))));			\
	(*_pV = SWAP32(*((UINT32 *)(_pV))));                           \
    }                                                                   \
}while(0)

#define RTMP_IO_READ8(_A, _R, _pV)									\
do{																	\
	(*_pV = readb((void *)((_A)->CSRBaseAddress + (_R))));			\
}while(0)

#define RTMP_IO_WRITE32(_A, _R, _V)									\
do{																	\
    if ((_A)->bPCIclkOff == FALSE)                                      \
    {                                                                   \
	UINT32	_Val;													\
	_Val = SWAP32(_V);												\
	writel(_Val, (void *)((_A)->CSRBaseAddress + (_R)));			\
    }                                                                   \
}while(0)

#define RTMP_IO_WRITE8(_A, _R, _V)									\
do{																	\
	writeb((_V), (PUCHAR)((_A)->CSRBaseAddress + (_R)));			\
}while(0)

#define RTMP_IO_WRITE16(_A, _R, _V)									\
do{																	\
	writew(SWAP16((_V)), (PUSHORT)((_A)->CSRBaseAddress + (_R)));	\
}while(0)
#else
#ifdef RELEASE_EXCLUDE
	/* Patch for ASIC turst read/write bug, needs to remove after metel fix */
#endif /* RELEASE_EXCLUDE */
#define RTMP_IO_FORCE_READ32(_A, _R, _pV)								\
{																\
	(*_pV = readl((void *)((_A)->CSRBaseAddress + (_R))));			\
}

#define RTMP_IO_READ32(_A, _R, _pV)								\
do{																\
    if ((_A)->bPCIclkOff == FALSE)                                  \
    {                                                               \
		(*_pV = readl((void *)((_A)->CSRBaseAddress + (_R))));			\
    }                                                               \
    else															\
		*_pV = 0;													\
}while(0)

#define RTMP_IO_READ8(_A, _R, _pV)								\
do{																\
	(*_pV = readb((void *)((_A)->CSRBaseAddress + (_R))));				\
}while(0)

#define RTMP_IO_WRITE32(_A, _R, _V)												\
do{																				\
    if ((_A)->bPCIclkOff == FALSE)                                  \
    {                                                               \
	writel((_V), (void *)((_A)->CSRBaseAddress + (_R)));								\
    }                                                               \
}while(0)

#ifdef RTMP_RBUS_SUPPORT
#ifdef DFS_HARDWARE_SUPPORT
#define RTMP_DFS_IO_READ32(_R, _pV)		\
do{										\
	(*_pV = readl((void *)(_R)));			\
}while(0)
#endif /* DFS_HARDWARE_SUPPORT */

#define RTMP_DFS_IO_WRITE32(_R, _V)		\
do{										\
	writel(_V, (void *)(_R));				\
}while(0)

#endif /* RTMP_RBUS_SUPPORT */


#if defined(BRCM_6358) || defined(RALINK_2880) || defined(RALINK_3052)
#define RTMP_IO_WRITE8(_A, _R, _V)            \
do{                    \
	ULONG Val;                \
	UCHAR _i;                \
	_i = ((_R) & 0x3);             \
	Val = readl((void *)((_A)->CSRBaseAddress + ((_R) - _i)));   \
	Val = Val & (~(0x000000ff << ((_i)*8)));         \
	Val = Val | ((ULONG)(_V) << ((_i)*8));         \
	writel((Val), (void *)((_A)->CSRBaseAddress + ((_R) - _i)));    \
}while(0)
#else
#define RTMP_IO_WRITE8(_A, _R, _V)							\
do{															\
	writeb((_V), (PUCHAR)((_A)->CSRBaseAddress + (_R)));		\
}while(0)
#endif /* #if defined(BRCM_6358) || defined(RALINK_2880) */

#define RTMP_IO_WRITE16(_A, _R, _V)							\
do{															\
	writew((_V), (PUSHORT)((_A)->CSRBaseAddress + (_R)));	\
}while(0)
#endif /* #if defined(INF_TWINPASS) || defined(INF_DANUBE) || defined(IKANOS_VX_1X0) */
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
#ifdef RELEASE_EXCLUDE
	/* Patch for ASIC turst read/write bug, needs to remove after metel fix */
#endif /* RELEASE_EXCLUDE */
#define RTMP_IO_FORCE_READ32(_A, _R, _pV)								\
	RTUSBReadMACRegister(_A, _R, _pV)

#define RTMP_IO_READ32(_A, _R, _pV)								\
	RTUSBReadMACRegister(_A, _R, _pV)

#define RTMP_IO_READ8(_A, _R, _pV)								\
do{																\
}while(0)

#define RTMP_IO_WRITE32(_A, _R, _V)								\
	RTUSBWriteMACRegister(_A, _R, _V)


#define RTMP_IO_WRITE8(_A, _R, _V)								\
do{																\
	USHORT	_Val = _V;											\
	RTUSBSingleWrite(_A, _R, _Val);								\
}while(0)


#define RTMP_IO_WRITE16(_A, _R, _V)								\
do{																\
	RTUSBSingleWrite(_A, _R, _V);								\
}while(0)
#endif


/***********************************************************************************
 *	Network Related data structure and marco definitions
 ***********************************************************************************/
#define PKTSRC_NDIS             0x7f
#define PKTSRC_DRIVER           0x0f

#ifdef EBS_RTIP

#define CB_OFF  	0
#define CB_MAX_OFFSET		32

#define RTMP_NETBUF_TX		0
#define RTMP_NETBUF_RX		1

typedef struct _RTMP_RTIP_NET_BUF_{
	void *next;
	UCHAR	cb[CB_MAX_OFFSET];
	UCHAR	*pFrameBuf; /* Pointer to internal buffer for Rx. */
	DCU		pDcuBuf;
	UCHAR	*pDataHead;
	UCHAR	*pDataPtr;
	int		pktLen;
	PIFACE	pi;
	BOOLEAN	 bDcuFlg;
}RTMP_RTIP_NET_BUF;

extern INT RtmpIfaceId;
extern PIFACE RtmpPI;

void * RtmpTxNetBuf_put(RTMP_RTIP_NET_BUF *pNetBuf, int n);
void *RtmpTxNetBuf_push(RTMP_RTIP_NET_BUF *pNetBuf, int n);

void hex_dump_DCU(PUCHAR callerStr, DCU pDcu, BOOLEAN bDumpPkt);
void hex_dump_PIFACE(PUCHAR callerStr, PIFACE pi);

#define skb_put(_pNetPkt, n)	RtmpTxNetBuf_put(_pNetPkt, n)
#define skb_push(_pNetPkt, n)		RtmpTxNetBuf_push(_pNetPkt, n)
#define skb_reserve(skb,n)

#define RTMP_OS_NETDEV_STATE_RUNNING(_pNetDev)		((_pNetDev)->open_count > 0)

#define RTMP_OS_NETDEV_GET_PRIV(_pNetDev)			(RT_WIFI_DevCtrl)
#define RTMP_OS_NETDEV_SET_PRIV(_pNetDev, _pPriv)		do{}while(0)
#define RTMP_OS_NETDEV_GET_DEVNAME(_pNetDev) 		((_pNetDev)->pdev->device_name)
#define RTMP_OS_NETDEV_GET_PHYADDR(_pNetDev)		((_pNetDev)->addr.my_hw_addr)
#define RTMP_OS_NETDEV_PHYADDR_LEN(_pNetDev)		(ETH_ALEN)

/* Get & Set NETDEV interface hardware type */
#define RTMP_OS_NETDEV_GET_TYPE(_pNetDev)
#define RTMP_OS_NETDEV_SET_TYPE(_pNetDev, _type)

#define QUEUE_ENTRY_TO_PACKET(pEntry) \
	(PNDIS_PACKET)(pEntry)

#define PACKET_TO_QUEUE_ENTRY(pPacket) \
	(PQUEUE_ENTRY)(pPacket)

#define RELEASE_NDIS_PACKET(_pAd, _pPacket, _Status)  \
		RTMPFreeNdisPacket(_pAd, _pPacket)

/*
 * packet helper
 * 	- convert internal rt packet to os packet or
 *             os packet to rt packet
 */
#define RTPKT_TO_OSPKT(_p)				((RTMP_RTIP_NET_BUF *)(_p))
#define OSPKT_TO_RTPKT(_p)				((PNDIS_PACKET)(_p))
#define GET_OS_PKT_DATAPTR(_pkt) 		((RTPKT_TO_OSPKT(_pkt)->pDataPtr))
#define SET_OS_PKT_DATAPTR(_pkt, _dataPtr) \
							((((RTMP_RTIP_NET_BUF *)(_pkt))->pDataPtr) = (_dataPtr))

#define GET_OS_PKT_LEN(_pkt) \
		(RTPKT_TO_OSPKT(_pkt)->pktLen)

#define SET_OS_PKT_LEN(_pkt, _len)	\
		(RTPKT_TO_OSPKT(_pkt)->pktLen) = (_len)


#define GET_OS_PKT_DATATAIL(_pkt) \
		((RTPKT_TO_OSPKT(_pkt)->pDcuBuf->data) + (RTPKT_TO_OSPKT(_pkt)->pDcuBuf->length))

#define SET_OS_PKT_DATATAIL(_pkt, _start, _len)	do {}while(0)

#define GET_OS_PKT_NETDEV(_pkt) \
		(RTPKT_TO_OSPKT(_pkt)->pi)

#define SET_OS_PKT_NETDEV(_pkt, _pNetDev)	\
		(RTPKT_TO_OSPKT(_pkt)->pi) = (_pNetDev)
		
#define GET_OS_PKT_TYPE(_pkt) \
		(RTPKT_TO_OSPKT(_pkt))


#define GET_OS_PKT_HEADROOM(_pkt)	(0)
		
#define GET_OS_PKT_TOTAL_LEN(_pkt) \
		(RTPKT_TO_OSPKT(_pkt)->pktLen)

#define OS_PKT_COPY(_pkt)		skb_copy(RTPKT_TO_OSPKT(_pkt), GFP_ATOMIC)

#define OS_PKT_TAIL_ADJUST(_pkt, _removedTagLen)								\
	SET_OS_PKT_DATATAIL(_pkt, GET_OS_PKT_DATATAIL(_pkt), (-_removedTagLen));	\
	GET_OS_PKT_LEN(_pkt) -= _removedTagLen;

#define OS_PKT_HEAD_BUF_EXTEND(_pkt, _offset)								\
	skb_push(RTPKT_TO_OSPKT(_pkt), _offset)

#define OS_PKT_TAIL_BUF_EXTEND(_pkt, _Len)									\
	skb_put(RTPKT_TO_OSPKT(_pkt), _Len)

#define OS_PKT_RESERVE(_pkt, _Len)											\
	skb_reserve(RTPKT_TO_OSPKT(_pkt), _Len)

#define GET_OS_PKT_CB(_p)		(RTPKT_TO_OSPKT(_p)->cb)
#define PACKET_CB(_p, _offset)	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF + (_offset)]))


/* TODO: Check if the ntohs is workable in ThreadX */
#define OS_NTOHS(_Val) \
		(ntohs(_Val))
#define OS_HTONS(_Val) \
		(htons(_Val))
#define OS_NTOHL(_Val) \
		(ntohl(_Val))
#define OS_HTONL(_Val) \
		(htonl(_Val))
 
#define RTMP_OS_NETDEV_START_QUEUE(_pNetDev)	do{}while(0)
#define RTMP_OS_NETDEV_STOP_QUEUE(_pNetDev)		do{}while(0)
#endif /* EBS_RTIP */

#define MAX_PACKETS_IN_QUEUE				(64)

#ifdef RTMP_USB_SUPPORT

/******************************************************************************

  	USB related definitions

******************************************************************************/
#define RTMP_USB_PKT_COPY(__pNetPkt, __Len, __pData)					\
	memcpy(skb_put(pNetPkt, ThisFrameLen), pData, ThisFrameLen);		\
	GET_OS_PKT_NETDEV(pNetPkt) = get_netdev_from_bssid(pAd, BSS0);

#define RTMP_DC_USB_ALIGN_BYTE	512
#define RTMP_DC_USB_ALIGN_ORDER 9

#define MODULE_DEVICE_TABLE(A,B)

typedef struct usb_device_id{
	UINT16 uVendorIDorClass;
	UINT16 uProductIDorSubClass;
}USB_DEVICE_ID;

#define USB_DEVICE(A,B) (A),(B)

typedef struct _USB_HOST_CHANNEL
{
	DC_USB_DEVICE *pUSBDev;
	UINT8 EndpointAddress;
	DC_USB_PIPE *pUSBPipe;
}USB_HOST_CHANNEL, *PUSB_HOST_CHANNEL;

/* Setup packet structure  */
typedef struct GNU_PACKED usbhst_setup_packet
    {
    UINT8  bmRequestType;        /* Bitmap for request characteristics       */
    UINT8  bRequest;             /* Specific request                         */
    UINT16 wValue;               /* Word size value field for specific req   */
    UINT16 wIndex;               /* Word size index field for specific req   */
    UINT16 wLength;              /* Num of bytes to transfer if data stage   */
    }USBHST_SETUP_PACKET, *pUSBHST_SETUP_PACKET;

/*+++Add for dc_usb_host.c */
struct DC_SGTL_PIPE {
	struct DC_SGTL_PIPE *next;
	struct DC_SGTL_PIPE *prev;

	struct DC_SGTL_URB *urb_list;
    struct DC_SGTL_URB *urb_last; /* point to the last urb in the pipe */

	/* these are initialized at the time in which the pipe is opened */
	volatile UINT8 dataToggle;
	UINT8 bDeviceAddress;
	UINT8 bEndpointAddress;		/* see USB endpoint descriptor */
	UINT8 bmEndpointAttributes;	/* see USB endpoint descriptor */
	UINT16 wMaxPacketSize;			/* see USB endpoint descriptor */
	UINT8 bInterval;				/* see USB endpoint descriptor */
	UINT16 tIntLastInstalled;		/* for INT endpts, timestamp when installed */

	DC_USB_HOST_SPEED speed;
	UINT8 splitRequired;
	UINT8 TThubPort;
	UINT8 TThubAddress;
};


typedef enum {
	/*
	 * "RUNNING" means that the pipe context is installed and running in the hardware.
	 * "ARMED" means that the pipe context has further work to be done, but is not
	 * currently running in the hardware.
	 */
	DC_SGTL_URB_ARMED,	
	DC_SGTL_URB_RUNNING,

	/* These error codes can be generated upon completion of a pipe data transfer
	 * attempt by the host controller hardware.
	 */
	DC_SGTL_URB_IDLE,			/* Waiting for next phase of the transfer */
	DC_SGTL_URB_COMPLETE,		/* successful completion of a data transfer */
	DC_SGTL_URB_STALL,			/* stall received from device */
	DC_SGTL_URB_TIMEOUT,		/* specified timeout exceeded */
	DC_SGTL_URB_RX_ERROR,		/* maxRetry receive errors encountered */
	DC_SGTL_URB_TX_ERROR,		/* maxRetry ack timeouts encountered */
	DC_SGTL_URB_CANCELLED,		/* transfer was cancelled or device detached*/
	DC_SGTL_URB_UNKNOWN_ERROR,	/* unknown hardware error */
	DC_SGTL_URB_INVALID_HANDLE,	/* invalid URB pointer */
} DC_SGTL_URB_STATUS;


typedef enum {
	DC_SGTL_URB_CTRL_SETUP,
	DC_SGTL_URB_CTRL_DATA,
	DC_SGTL_URB_CTRL_STATUS,
	DC_SGTL_URB_CTRL_COMPLETE
} DC_SGTL_URB_CTRL;


struct DC_SGTL_URB {
	DC_SGTL_URB_STATUS status;
	DC_URB_COMPLETION_STATUS completion_status;
	struct DC_SGTL_PIPE *pipe;
	void *handle;

	UINT8 *base;					/* SDRAM pointer to in/out packet data */
	UINT16 requestedLength;			/* requested length to transfer (bytes) */
	UINT16 length;					/* length remaining in data transfer */
	UINT16 timeoutMSec;				/* timeout in MSec (0 for no timeout) */
	UINT8 zeroPacket;
	UINT8 *setupBase;				/* SDRAM pointer for 8 byte setup packet */
	DC_SGTL_URB_CTRL controlState;	/* tracks phases on control transfers */
	ULONG tStart;					/* start timestamp (for use with timeout) */
	void (*completionCallback)		/* called when transaction completes */
		(DC_URB_COMPLETION_STATUS	status,				/* transaction completion status */
		 UINT32						bytes_transfered,	/* amount of data transfered */
		 void						*urb,				/* handle of pipe of completed transfer */
		 void						*arg);				/* echo of completionCallbackArg */
	void *completionCallbackArg;	/* argument echo back on callback */
	UINT8 splitHalfDone;			/* Is a split packet in progress? */

	struct DC_SGTL_URB *next;
	struct DC_SGTL_URB *prev;

	/* transfer slot used by this URB */
	UINT8 transfer_num;
	UINT8 stop_count;				/* The number of times we tried to stop this transfer. */
};


struct DC_SGTL_USB_HC {
	DC_USB_HCD_ID			hc;
	UINT16					port_status;
	UINT16					port_change_status;
	ULONG					resume_time;
	ULONG					connect_time;
	UINT8					connect_time_set;
	UINT8					present;
	UINT32					max_urbs;
	struct DC_SGTL_URB		*current_URB;
	struct DC_SGTL_URB		*active_URBs;
	struct DC_SGTL_URB		*free_URBs;
	struct DC_SGTL_URB		*URBs;
	UINT32					max_pipes;
	struct DC_SGTL_PIPE		*active_pipes;
	struct DC_SGTL_PIPE		*free_pipes;
	struct DC_SGTL_PIPE		*pipes;
    struct DC_SGTL_PIPE		*nextIntPipe; /* point to next pipe to start search for int xfer */
    struct DC_SGTL_PIPE		*nextCtlBulkPipe; /* point to next pipe to start search for ctl and bulk xfer */
	UINT32					max_buffers;
	TX_TIMER				timer;
	void					(*uh_port_power_control)(DC_USB_HCD_ID hc, UINT8 on_off);
    boolean                 high_speed_enable;

    boolean                 manual_bus_control; /* test mode support - indicator */
    boolean                 delay_control_xfer; /* test mode support - flag to permit many sec delayed */
                                                /*   control transfers. Suspends timeout functionality. */
    UINT8                   resume_mode_k_count;
};

struct DC_SGTL_USB_HC;
extern struct DC_SGTL_USB_HC sgtl_host_controllers[3];
/*---Add for dc_usb_host.c */

/*******************************************************************************
 * Macro Name  : USBHST_FILL_SETUP_PACKET
 * Description : Initialize the SETUP_PACKET structure with the requisite
 *               parameter.
 * Parameters  : pSetup         OUT Setup packet to be filled.
 *               uRequestType   IN  Bitmap for request characteristics.
 *               uRequest       IN  Standard request code.
 *               uValue         IN  Word size value field for standard request.
 *               uIndex         IN  Word size index field for standard request.
 *               uSize          IN  Number of bytes to transfer if there is
 *                                  data stage.
 * Return Type : None
 ******************************************************************************/
#define USBHST_FILL_SETUP_PACKET(pSetup,                                     \
                                 uRequestType,                               \
                                 uRequest,                                   \
                                 uValue,                                     \
                                 uIndex,                                     \
                                 uSize)                                      \
do{                                                                            \
    /* Store bitmap for request characteristics in the setup packet */       \
    (pSetup)->bmRequestType   = (UINT8)(uRequestType);                       \
                                                                             \
    /* Store the request in the setup packet */                              \
    (pSetup)->bRequest        = (UINT8)(uRequest);                           \
                                                                             \
    /* Store word size value field for request in the setup packet */        \
    (pSetup)->wValue          = OS_UINT16_CPU_TO_LE((UINT16)(uValue));       \
                                                                             \
    /* Store word size index field for request  in the setup packet */       \
    (pSetup)->wIndex          = OS_UINT16_CPU_TO_LE((UINT16)(uIndex));       \
                                                                             \
    /* Store number of bytes to transfer if there is data stage */           \
    (pSetup)->wLength         = OS_UINT16_CPU_TO_LE((UINT16)(uSize));        \
}while(0)


#define BULKAGGRE_SIZE          												100
#define RT28XX_PUT_DEVICE
#define RTUSB_ALLOC_URB(ISO)			usb_alloc_urb()
#if 1
#define RTUSB_SUBMIT_URB(pURB)          usb_submit_urb(pURB)
#else
#define RTUSB_SUBMIT_URB(_pURB)	RtmpDcUSB_Enqueue(_pURB)
#endif
#define	RTUSB_URB_ALLOC_BUFFER(pUSBDev, BufSize, pDMAAddr)	\
										usb_buffer_alloc(pUSBDev, BufSize, pDMAAddr)
#define RTUSB_URB_FREE_BUFFER(pUSBDev, BufSize, pTransferBuf, DMAAddr)	\
										usb_buffer_free(pTransferBuf)
#define RTUSB_FREE_URB(pURB)			usb_free_urb(pURB)
#define RTUSB_UNLINK_URB(pURB)			usb_unlink_urb(pURB)

typedef DC_URB *purbb_t;
typedef void pregs;
typedef void USBHST_STATUS;
typedef DC_URB_COMPLETION_STATUS URBCompleteStatus;

USBHST_STATUS RTUSBBulkOutDataPacketComplete(URBCompleteStatus Status, purbb_t pURB, pregs *pt_regs);
USBHST_STATUS RTUSBBulkOutMLMEPacketComplete(URBCompleteStatus Status, purbb_t pURB, pregs *pt_regs);
USBHST_STATUS RTUSBBulkOutNullFrameComplete(URBCompleteStatus Status, purbb_t pURB, pregs *pt_regs);
USBHST_STATUS RTUSBBulkOutRTSFrameComplete(URBCompleteStatus Status, purbb_t pURB, pregs *pt_regs);
USBHST_STATUS RTUSBBulkOutPsPollComplete(URBCompleteStatus Status, purbb_t pURB, pregs *pt_regs);
USBHST_STATUS RTUSBBulkRxComplete(URBCompleteStatus Status, purbb_t pURB, pregs *pt_regs);
USBHST_STATUS RTUSBBulkCmdRspEventComplete(URBCompleteStatus Status, purbb_t pURB, pregs *pt_regs);

DC_USB_DEVICE * GetUSBDevFromUSBPipe(DC_USB_PIPE *USBPipe);
DC_USB_PIPE * GetUSBPipeFromEndpoint(UINT8 EndpointAddress);
void SetUSBHostChannel(UINT32 ChannelIndex, DC_USB_DEVICE *pUSBDev, UINT8 EndpointAddress, DC_USB_PIPE *pUSBPipe);
void ReleaseUSBHostChannel(UINT32 AllocatedIndex);

extern void dump_urb(DC_URB *purb);

DC_URB *usb_alloc_urb(void);
void usb_free_urb(DC_URB *pURB);
void *usb_buffer_alloc(DC_USB_DEVICE *pUSBDev, unsigned int Size, ra_dma_addr_t **DMAAddr);
void usb_buffer_free(void *pUSBBufferAddr);
NDIS_STATUS usb_control_msg(DC_USB_DEVICE *pUSBDev, UINT32 endpointaddress, UINT8 bRequest, UINT8 bmRequestType, 
									UINT16 wValue, UINT16 wIndex, void *pBuffer, UINT16 wLength, int timeout);
NDIS_STATUS usb_submit_urb(DC_URB *pURB);

/* Fill Bulk URB Macro */
#define RTUSB_FILL_TX_BULK_URB	RTUSB_FILL_BULK_URB
#define RTUSB_FILL_HTTX_BULK_URB	RTUSB_FILL_HT_BULK_URB
#define RTUSB_FILL_RX_BULK_URB	RTUSB_FILL_BULK_URB

#define RTUSB_FILL_HT_BULK_URB(_pURB, _pUSBDev, _EndpointAddress, \
					_pTransferBuf, _BufSize, _Complete,_pContext) \
	do{		\
		UINT8 *_actTransBuf = (UINT8 *)(_pTransferBuf);\
		HT_TX_CONTEXT *_pHTTxContext = (HT_TX_CONTEXT *)(_pContext);\
		\
		(_pURB)->pipe = GetUSBPipeFromEndpoint(_EndpointAddress);						\
		if (((_pURB)->pipe->bEndpointAddress & 0x80) == DC_USB20_ENDPOINT_OUT) /* Bit 7 of EndpointAddress field */\
		{	\
			if ( ((UINT32)_pTransferBuf & (RTMP_DC_USB_ALIGN_BYTE -1)) != 0) \
			{\
				NdisMoveMemory(_pHTTxContext->data_dma, _pTransferBuf, _BufSize);\
				_actTransBuf = (UINT8 *)(_pHTTxContext->data_dma);\
			}\
		}\
		(_pURB)->base = _actTransBuf;\
		(_pURB)->requestedLength = (_BufSize);	\
		(_pURB)->completionCallback = (_Complete);	\
		(_pURB)->completionCallbackArg = (_pContext);	\
	}while(0)

#define RTUSB_FILL_BULK_URB(pURB,													\
				pUSBDev,															\
				EndpointAddress,													\
				pTransferBuf,														\
				BufSize,															\
				Complete,															\
				pContext)															\
	do{																				\
		pURB->pipe = GetUSBPipeFromEndpoint(EndpointAddress);						\
		pURB->base = (UINT8 *)pTransferBuf;											\
		if ((((UINT32)pURB->base) & (RTMP_DC_USB_ALIGN_BYTE -1)) != 0)				\
			dc_log_printf("WARNING!NotAligned");									\
		pURB->requestedLength = BufSize;											\
		pURB->completionCallback = Complete;										\
		pURB->completionCallbackArg = pContext;										\
	}while(0)

typedef void (*pURB_CALLBACK_t)(DC_URB_COMPLETION_STATUS Status, struct DC_URB_STRUCT *pURB, void *Arg);

#define usb_complete_t	pURB_CALLBACK_t

#define RTUSB_URB_DMA_MAPPING(pURB)

#define rtusb_urb_context  completionCallbackArg
#define rtusb_urb_status   status

#define RTMP_USB_URB_STATUS_GET(__pUrb)		((purbb_t)__pUrb)->status
		
#define RTUSB_CONTROL_MSG(pUSBDev, uEndpointAddress, Request, RequestType, Value, Index, TmpBuf, TransferBufferLength, Timeout, Ret)	\
	do{																																	\
		if (RequestType == DEVICE_VENDOR_REQUEST_OUT)																					\
			Ret = usb_control_msg(pUSBDev, 0, Request, RequestType, Value, Index, TmpBuf, TransferBufferLength, Timeout);				\
		else if (RequestType == DEVICE_VENDOR_REQUEST_IN)																				\
			Ret = usb_control_msg(pUSBDev, 0, Request, RequestType, Value, Index, TmpBuf, TransferBufferLength, Timeout);				\
		else																															\
		{																																\
			DBGPRINT(RT_DEBUG_ERROR, ("vendor request direction is failed\n"));															\
			Ret = -1;																													\
		}																																\
	}while(0)
/* Usually been not done in USB host controller driver in ThreadX for DC SDK, so make a cache invalidate in class driver */
#define RTUSB_CACHE_INVALIDATE(pURB) dc_cache_invalidate(pURB->base, pURB->base+(pURB->requestedLength - pURB->length))
					
#endif /* RTMP_USB_SUPPORT */

#ifdef RTMP_IOCTL_SUPPORT
/******************************************************************************

  	ThreadX SHELL CMD related definitions

******************************************************************************/
typedef struct _RTMP_IWPRIV_CMD_STRUCT_
{
	RTMP_STRING *pDevName;
	RTMP_STRING *pCmd;
	RTMP_STRING *pParam;
}RTMP_IWPRIV_CMD_STRUCT;

#endif /* RTMP_IOCTL_SUPPORT */

/*******************************************************************************
	ThreadX Error Code Mapping
 *******************************************************************************/
 #define EINVAL TX_THREAD_ERROR

#ifdef CONFIG_ATE
/******************************************************************************

  	ATE related definitions

******************************************************************************/

#ifdef RTMP_USB_SUPPORT
USBHST_STATUS ATE_RTUSBBulkOutDataPacketComplete(URBCompleteStatus Status, purbb_t pURB, pregs *pt_regs);
#endif /* RTMP_USB_SUPPORT */

#define KERN_EMERG
#define ate_print printf
#define ATEDBGPRINT DBGPRINT
#ifdef RTMP_MAC_PCI
#ifdef CONFIG_AP_SUPPORT
#define EEPROM_BIN_FILE_NAME  "/tgtsvr/pci/e2p_ap.bin"
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
#define EEPROM_BIN_FILE_NAME  "/tgtsvr/pci/e2p_sta.bin"
#endif /* CONFIG_STA_SUPPORT */
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
#ifdef CONFIG_AP_SUPPORT
#define EEPROM_BIN_FILE_NAME  "/tgtsvr/usb/e2p_ap.bin"
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
#define EEPROM_BIN_FILE_NAME  "/tgtsvr/usb/e2p_sta.bin"
#endif /* CONFIG_STA_SUPPORT */
#endif /* RTMP_MAC_USB */

#endif /* CONFIG_ATE */


/***********************************************************************************
 *	Other function prototypes definitions
 ***********************************************************************************/
void RTMP_GetCurrentSystemTime(LARGE_INTEGER *time);

long simple_strtol(const char *cp,char **endp,unsigned int base);

RTP_BOOL MainVirtualIF_open(PIFACE pi);
void MainVirtualIF_close(PIFACE pi);

int rt28xx_send_packets(struct _iface RTP_FAR *pi, DCU pDcu);


#undef AP_WSC_INCLUDED
#undef STA_WSC_INCLUDED
#undef WSC_INCLUDED

#ifdef CONFIG_AP_SUPPORT
#ifdef WSC_AP_SUPPORT
#define AP_WSC_INCLUDED
#endif /* WSC_AP_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#ifdef WSC_STA_SUPPORT
#define STA_WSC_INCLUDED
#endif /* WSC_STA_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

#if defined(AP_WSC_INCLUDED) || defined(STA_WSC_INCLUDED)
#define WSC_INCLUDED
#endif

#define INT_MAX         ((int)(~0U>>1))

#define ENOMEM 0
#define ENODEV 3

#define RTMP_OS_PKT_INIT(__pRxPacket, __pNetDev, __pData, __DataSize)		\
{																			\
	PNDIS_PACKET __pRxPkt;													\
	__pRxPkt = RTPKT_TO_OSPKT(__pRxPacket);									\
	SET_OS_PKT_NETDEV(__pRxPkt, __pNetDev);									\
	SET_OS_PKT_DATAPTR(__pRxPkt, __pData);									\
	SET_OS_PKT_LEN(__pRxPkt, __DataSize);									\
	SET_OS_PKT_DATATAIL(__pRxPkt, __pData, __DataSize);						\
}

#define RTMP_THREAD_PID_KILL(__PID)		KILL_THREAD_PID(__PID, SIGTERM, 1)

#define RTMP_OS_ATOMIC							atomic_t
#define RTMP_OS_ATMOIC_INIT(__pAtomic, __pAtomicList)

#define RTMP_OS_TASK							OS_TASK

#define RTMP_OS_TASK_DATA_GET(__pTask)						\
	((__pTask)->priv)

#define RTMP_OS_TASK_IS_KILLED(__pTask)						\
	((__pTask)->task_killed)

#define get_unaligned32							get_unaligned
#define get_unalignedlong						get_unaligned

typedef VOID									* PPCI_DEV;
typedef struct net_device_stats				NET_DEV_STATS;

#ifdef RTMP_MAC_PCI
#ifdef MEMORY_OPTIMIZATION
#define TX_RING_SIZE            64
#define MGMT_RING_SIZE          32
#define RX_RING_SIZE            64
#else
#define TX_RING_SIZE            64 /*64 */
#define MGMT_RING_SIZE          128
#define RX_RING_SIZE            128 /*64 */
#endif
#define MAX_TX_PROCESS          TX_RING_SIZE /*8 */
#define MAX_DMA_DONE_PROCESS    TX_RING_SIZE
#define MAX_TX_DONE_PROCESS     TX_RING_SIZE /*8 */
#define LOCAL_TXBUF_SIZE        2
#endif /* RTMP_MAC_PCI */

#define APCLI_IF_UP_CHECK(pAd, ifidx) (RtmpOSNetDevIsUp((pAd)->ApCfg.ApCliTab[(ifidx)].dev) == TRUE)

#define OS_PKT_HEAD_BUF_EXTEND(_pkt, _offset)								\
	skb_push(RTPKT_TO_OSPKT(_pkt), _offset)

#include "os/rt_os.h"


#ifdef RTMP_WLAN_HOOK_SUPPORT
/*define os layer hook function implementation*/
#define RTMP_OS_TXRXHOOK_CALL(hook,packet,queIdx,priv)
#define RTMP_OS_TXRXHOOK_INIT()
#else
#define RTMP_OS_TXRXHOOK_CALL(hook,packet,queIdx,priv)
#define RTMP_OS_TXRXHOOK_INIT()
#endif



#endif /* __RT_THREADX_H__ */
