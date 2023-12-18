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
	calib_robust_test.c
*/


#include <linux/init.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/notifier.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <os/rt_linux_txrx_hook.h>
#include "calib_robust.h"


/*
 *   Defination 
*/
#define DRIVER_DESC "Calibration Robust Test Module"

/*
 *   Gloab variable
 */
calib_test_t gCalib;
unsigned int glen=0;
char *gBuffer = NULL;
RTMP_ADAPTER *pGAd = NULL;

extern INT32 RtmpOsRfRead(RTMP_ADAPTER *pAd, UINT32 RFIdx, UINT32 Offset, UINT32 *Value);
extern INT32 RtmpOsMemIORead(RTMP_ADAPTER *pAd,UINT32 addr,UINT32 *value);
extern VOID *RtmpOsGetNetDevPriv(VOID *pDev);
extern VOID AsicSwitchChannel(RTMP_ADAPTER *pAd, UCHAR Channel, BOOLEAN bScan);


static struct calibItem calibItemTable[] ={
{ 
	.calibId = CALIB_TEST_RC,
	.mode =  CALIB_REG_TYPE_PHY,
	.name= "CAL_RC",
	.regs = {
	0x834,
	CALIB_REG_END
	}
},
{ 
	.calibId = CALIB_TEST_RXDCOC,		
	.mode =  CALIB_REG_TYPE_PHY,
	.name = "CAL_RXDCOC",
	.regs = { 
	0x428,0x42C,0x430,0x434,0x438,
	0x43c,0x440,0x444,0x448,0x44c,
	0x450,0x454,0x458,0x45c,0x460,
	0x464,0x6cc,0x6d0,0x6d4,0x6d8,
	0x6dc, 0x6e0, 0x6e4, 0x6e8,
	CALIB_REG_END
	}
},
{
	.calibId = CALIB_TEST_RSSIDCOC,		
	.mode =  CALIB_REG_TYPE_PHY,
	.name= "CAL_RSSIDCOC",
	.regs = {
	0x46c,0x470,
	CALIB_REG_END
	}
},
{ 	.calibId=CALIB_TEST_TXLPFG,
	.mode =  CALIB_REG_TYPE_MAC,
	.name="CAL_TX_LPFG",
	.regs = {
	0x10284,0x11284,
	CALIB_REG_END
	}
},
{
	.calibId=CALIB_TEST_TXLOFT,		
	.mode =  CALIB_REG_TYPE_MAC,
	.name="CAL_TX_LOFT",
	.regs = {	
	0x10264,0x10268,0x1026c,0x10270,0x10274,
	0x10278,0x11264, 0x11268,0x1126c,0x11270, 
	0x11274,0x11278, 
	CALIB_REG_END
	}
},
{	.calibId= CALIB_TEST_TXIQ,
	.mode =  CALIB_REG_TYPE_MAC,
	.name="CAL_TX_IQ",
	.regs = {
	0x10240,0x10244,0x10248,0x1024c,0x10250,
	0x10254,0x11240,0x11244,0x11248,0x1124c,
	0x11250,0x11254,
	CALIB_REG_END
	}
},
{ 	
	.calibId=CALIB_TEST_TXDPD,		
	.mode =  CALIB_REG_TYPE_MAC,
	.name="CAL_TX_DPD",
	.regs = {
	0x10920,0x10924,0x10928,0x1092c,0x10930,
	0x10934,0x10938,0x1093c,0x10940,0x10944,
	0x10948,0x1094c,0x10950,0x10954,0x10958,
	0x1095c,0x11920,0x11924,0x11928,0x1192c,
	0x11930,0x11934,0x11938,0x1193c,0x11940,
	0x11944,0x11948,0x1194c,0x11950,0x11954,
	0x11958,0x1195c, 
	CALIB_REG_END
	}
},
{ 
	.calibId=CALIB_TEST_RXIQC_FI,	
	.mode =  CALIB_REG_TYPE_MAC,
	.name="CAL_RX_FIIQ",
	.regs = {	
	0x10288, 0x1028C, 0x10290, 0x10294, 0x11288,
	0x1128C, 0x11290, 0x11294, 
	CALIB_REG_END
	}
},
{ 
	.calibId=CALIB_TEST_RXIQC_FD,		
	.mode =  CALIB_REG_TYPE_MAC,
	.name="CAL_RX_FDIQ",
	.regs = {
	0x10158, 0x1015C, 0x10160, 0x10164, 0x10168,
	0x1016C, 0x11158, 0x1115C, 0x11160, 0x11164,
	0x11168, 0x1116C,
	CALIB_REG_END
	}
},
{
	.calibId = CALIB_TEST_END
}
};

/*
 *  Local function
 */
struct calibItem* calib_table_get(unsigned int calibId)
{
	unsigned tableId = 0;
	if(calibId >=CALIB_TEST_START && calibId < CALIB_TEST_END)
	{
		do{
			if(calibItemTable[tableId].calibId == calibId || calibItemTable[tableId].calibId ==CALIB_TEST_END)
			{
				return &calibItemTable[tableId];
			}
			tableId ++;
		}while(1);
	}
	return NULL;
}

static void calib_result_show(RTMP_ADAPTER *pAd,unsigned int calibId)
{	
	unsigned int tableId=0,regId=0;
	struct calibItem *pCalibInfo = NULL;
	unsigned int value,reg;
	int ret;

	pCalibInfo = calib_table_get(calibId);

	if(pCalibInfo==NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("[Calib] can't find calib information by calibId %d\n", calibId));
		return ;
	}

	/*reg*/		
	reg = pCalibInfo->regs[regId];

	if(pCalibInfo->mode==CALIB_REG_TYPE_PHY)
	{	
		regId =0;		
		reg = pCalibInfo->regs[regId];
		while(reg !=CALIB_REG_END) 
		{				
			RtmpOsRfRead(pAd,0,reg,&value);
			glen+=sprintf(gBuffer+glen,"0x%08x\t",value);
			RtmpOsRfRead(pAd,1,reg,&value);
			glen+=sprintf(gBuffer+glen,"0x%08x\t",value);
			regId++;				
			reg = pCalibInfo->regs[regId];
		};
	}else
	{
		regId =0;		
		reg = pCalibInfo->regs[regId];

		while( reg !=CALIB_REG_END)
		{				
			RtmpOsMemIORead(pAd, reg, &value);
			glen+=sprintf(gBuffer+glen,"0x%08x\t",value);
			regId++;				
			reg = pCalibInfo->regs[regId];
		};
	}		
	glen+=sprintf(gBuffer+glen,"\r\n");

}

static void calib_robust_routine(struct work_struct *ws)
{
	unsigned int bw = pGAd->CommonCfg.BBPCurrentBW ;
	
	spin_lock(&gCalib.lock);

	if(gCalib.curCalibId==CALIB_TEST_RXIQC_FD)
	{
		pGAd->CommonCfg.BBPCurrentBW = (gCalib.bw == 20) ? BW_20 : BW_40;
		AsicSwitchChannel(pGAd,7,FALSE);
	}
	
	calib_result_show(pGAd,gCalib.curCalibId);
	calib_test_fileWrite(gBuffer,glen);
	memset(gBuffer,0,GSIZE);
	glen=0;

	if(gCalib.curCalibId==CALIB_TEST_RXIQC_FD)
	{
		pGAd->CommonCfg.BBPCurrentBW = bw;
		AsicSwitchChannel(pGAd,pGAd->CommonCfg.Channel,FALSE);
	}
	gCalib.curCount++;
	spin_unlock(&gCalib.lock);
}


/*callback and state machine*/
static int calib_robust_callback(unsigned short hook, struct sk_buff *skb, unsigned char status,void *priv)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER*)priv;
	DBGPRINT(RT_DEBUG_OFF, ("[calib] Calibration ID: %d,State: %d\n", gCalib.curCalibId, status));
	if(pAd == pGAd){
		switch(gCalib.stat){
		case CALIB_STATE_NOP:
		case CALIB_STATE_START_TEST_MODE:
		case CALIB_STATE_STOP_TEST_MODE:
		{
			/*do nothing*/
			gCalib.curCount=0;
		}
		break;
		case CALIB_STATE_CALIB_START:
		{		
			if(status!=NDIS_STATUS_SUCCESS)
			{				
				DBGPRINT(RT_DEBUG_ERROR, ("[calib] rcv response in calib cma failed, retry!\n"));
				
			}else
			{			
				DBGPRINT(RT_DEBUG_OFF, ("[calib] rcv response in calib cmd ok, calibId %d!\n",
						gCalib.curCalibId));
				/*do save result*/
				schedule_work(gCalib.resultWork);
				spin_unlock(&gCalib.lock);
			}
		}
		break;
		default:
			/*do nothing*/
		break;
		}
	}
	return 0;
}


static void calib_test_init(void)
{
	memset(&gCalib,0,sizeof(calib_test_t));
	gCalib.stat= CALIB_STATE_NOP;
	gCalib.times = 10;
	gCalib.curCalibId = 0;
	sprintf(gCalib.fname,"/var/calib_roubst_test");
	gCalib.fstate = 0;
	gCalib.curCount = 0;
	gCalib.bw = 20;
	gCalib.lock = SPIN_LOCK_UNLOCKED;
	/*initial workqueue*/
	 gCalib.resultWork = kzalloc(sizeof(struct work_struct), GFP_KERNEL);
	 INIT_WORK(gCalib.resultWork, calib_robust_routine);
}

static void calib_test_exit(void)
{
	if(gCalib.resultWork)
	{
		kfree(gCalib.resultWork);
	}
	memset(&gCalib,0,sizeof(calib_test_t));
	calib_test_fileClose();
}


static void calib_file_init(void)
{
	gBuffer = kmalloc(GSIZE,GFP_KERNEL);
	memset(gBuffer,0,GSIZE);

}

static void calib_file_exit(void)
{
	memset(gBuffer,0,GSIZE);
	if(gBuffer) 
		kfree(gBuffer);
}

static int calib_getAdapater(void)
{
	struct net_device *pRa = dev_get_by_name(&init_net,"ra0");
	if(pRa)
	{
		pGAd = (RTMP_ADAPTER *)RtmpOsGetNetDevPriv(pRa);
	}
	if(pRa == NULL || pGAd == NULL)
	{
		return -1;
	}
	else 
	{
		return 0;
	}
}

static struct mt_wlanTxRxHookOps  calib_ops __read_mostly = {
	.name	= "CalibRobustTest",
	.hooks	= (1<< WLAN_CALIB_TEST_RSP),
	.hookfun	= calib_robust_callback,
	.me		= THIS_MODULE,
	.priority   =  WLAN_HOOK_PRI_SOFTQ_STAT
};

 
static int __init calib_test_module_init(void)
{
	DBGPRINT(RT_DEBUG_OFF, ("[calib]  %s(): module init and register callback for roubst test\n",
			__func__));
	/*initial global struct*/
	calib_test_init();
	/*register proc file*/
	calib_test_proc_init();
	/*initial file*/
	calib_file_init();
	/*register hook function*/
	RtmpOsTxRxHookRegister(&calib_ops);
	/*get pAd*/
	calib_getAdapater();
	return 0;
}

static void __exit calib_test_module_exit(void)
{
	DBGPRINT(RT_DEBUG_OFF, ("[calib]  %s(): module exist\n", __func__));
	RtmpOsTxRxHookUnRegister(&calib_ops);
	calib_test_exit();
	calib_file_exit();
	calib_test_proc_exit();
}

module_init(calib_test_module_init);
module_exit(calib_test_module_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DRIVER_DESC); /* What does this module do */
MODULE_SUPPORTED_DEVICE("mt7603");

