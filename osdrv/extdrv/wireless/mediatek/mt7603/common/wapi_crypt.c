
#ifdef LINUX
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>		/* Needed for the macros */
#endif /* LINUX */

#include "rtmp_type.h"
#include "wpa_cmm.h"
#include "wapi_def.h"
#include "wapi_sms4.h"

void dump_bin(char *name, void *data, int len)
{
	int i = 0;
	unsigned char *p = data;
	
	if(name != NULL)
	{
		printk("%s(%d): \n", name, len);
	}
	printk("\t");

	for(i=0; i<len; i++)
	{
		printk("%02x ", p[i]);
		if(((i+1)%16) ==0)
			printk("\n\t");
	}
	if(((i+1)%16 ) != 0)
		printk("\n");
}


#if 0
/*ofb encrypt*/
int wpi_encrypt(unsigned char * pofbiv_in,unsigned char * pbw_in,unsigned int plbw_in,unsigned char * pkey,unsigned char * pcw_out)
{
	unsigned int ofbtmp[4];
	unsigned int * pint0, * pint1;
	unsigned char * pchar0, * pchar1,* pchar2;
	unsigned int counter,comp,i;
	unsigned int prkey_in[32];


	if(plbw_in<1)	return 1;
	/*if(plbw_in>65536) return 1; */

	SMS4KeyExt(pkey,  prkey_in, 0);

	/* get the multiple of 16 */
	counter = plbw_in >> 4;
	/* get the remainder of 16 */
	comp = plbw_in & 0x0f;	

	/*get the iv */
	SMS4Crypt(pofbiv_in,(unsigned char *)ofbtmp, prkey_in);
	pint0=(unsigned int *)pbw_in;
	pint1=(unsigned int *)pcw_out;
	for(i=0;i<counter;i++) {
		pint1[0]=pint0[0]^ofbtmp[0];
		pint1[1]=pint0[1]^ofbtmp[1];
		pint1[2]=pint0[2]^ofbtmp[2];
		pint1[3]=pint0[3]^ofbtmp[3];
		SMS4Crypt((unsigned char *)ofbtmp,(unsigned char *)ofbtmp, prkey_in);
		pint0+=4;
		pint1+=4;
	}
	pchar0=(unsigned char *)pint0;
	pchar1=(unsigned char *)pint1;
	pchar2=(unsigned char *)ofbtmp;
	for(i=0;i<comp;i++) {
		pchar1[i]=pchar0[i]^pchar2[i];
	}
	
	return 0;	
}


/*ofb decrypt*/
int wpi_decrypt(unsigned char * pofbiv_in,unsigned char * pcw_in,unsigned int plcw_in,unsigned char * prkey_in,unsigned char * pbw_out)
{
	return wpi_encrypt(pofbiv_in,pcw_in,plcw_in,prkey_in,pbw_out);	
}

/*cbc_mac*/
int wpi_pmac(unsigned char * pmaciv_in,unsigned char * pmac_in,unsigned int pmacpc_in,unsigned char * pkey,unsigned char * pmac_out)
{
	unsigned int  mactmp[4];
	unsigned int i;
	unsigned int * pint0;
	unsigned int prmackey_in[32];

	if(pmacpc_in<1) return 1;
	if(pmacpc_in>4096) return 1;

	SMS4KeyExt(pkey,  prmackey_in, 0);
	
	pint0=(unsigned int *)pmac_in;
	SMS4Crypt(pmaciv_in, (unsigned char *)mactmp, prmackey_in);	
	for(i=0;i<pmacpc_in;i++) {
		mactmp[0]^=pint0[0];
		mactmp[1]^=pint0[1];
		mactmp[2]^=pint0[2];
		mactmp[3]^=pint0[3];
		pint0 += 4;
		SMS4Crypt((unsigned char *)mactmp, (unsigned char *)mactmp, prmackey_in);
	}
	pint0 = (unsigned int *)pmac_out;
	pint0[0] = mactmp[0];
	pint0[1] = mactmp[1];
	pint0[2] = mactmp[2];
	pint0[3] = mactmp[3];

	return 0;
}

void *free_buffer(void *buffer, int buffer_len)
{
	if(buffer != NULL)
	{
		memset(buffer, 0, buffer_len);
		os_free_mem(NULL, buffer);
		/*free(buffer); */
		return NULL;
	}
	else
		return NULL;
}

void print_mem(void *p, int len)
{
	int i = 0;
	unsigned char *pp = p;
	DBGPRINT(RT_DEBUG_TRACE, ("\t"));
	for(i=0; i<len; i++)
	{
		/*printf("0x%02x, ", pp[i]); */
		DBGPRINT(RT_DEBUG_TRACE, ("%02x ", pp[i]));
		if((i + 1) %16 == 0)
			DBGPRINT(RT_DEBUG_TRACE,("\n\t"));
	}
	DBGPRINT(RT_DEBUG_TRACE,("\n"));
}

void print_mem_int(void *p, int len)
{
	int i = 0;
	unsigned int *pp = (unsigned int *)p;
	
	printk("\n");
	for(i=0; i<len; i++)
	{
		/*printf("0x%02x, ", pp[i]); */
		printk("0x%08x ", pp[i]);
		if((i + 1) %4 == 0)
			printk("\n");
	}
	printk("\n");
}

void dump_bin_int(char *name, void *data, int len)
{
	int i = 0;
	unsigned int *p = data;
	
	if(name != NULL)
	{
		printk("%s(%d): \n", name, len/4);
	}
	printk("\t");

	for(i=0; i<len/4; i++)
	{
		printk("%04x ", p[i]);
		if(((i+1)%4) ==0)
			printk("\n\t");
	}
	if(((i+1)%4 ) != 0)
		printk("\n");
}

#define M_LEN  (1024)
#define ee (0.000001)
/*#define CLOCKS_PER_SEC OS_HZ */
INT SMS4_TEST(void)
{
	unsigned char *pt = NULL;
	unsigned char *ct = NULL;
	unsigned long start = 0;
	unsigned long stop = 0;
	int i = 0;
	int counts = 1;	
	float  usetimes;
	unsigned char	*crypt_data = NULL;
	unsigned char 	*decrypt_data = NULL;
	unsigned char 	*data = NULL;
	char tmpstr[] ="a";
	int str_len = 0;
	int ret = 0;
	unsigned char key0[16] = {	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
							0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
	unsigned char pt1[16]   = {	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
							0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
	unsigned char iv1[16]    = {	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
							0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
	unsigned char *key = (unsigned char *)key0;
	unsigned char enkey[128];
	unsigned char ofbct[64];
	unsigned char ofbdect[64];
	unsigned char cbcmac[16];	
#if 0
{
	/*ct_msk - hexdump(len=16): */
	unsigned char ct_msk[16]={	0x40, 0xa1, 0x33, 0x70, 0x70, 0xcb, 0xe5, 0x95,  
								0x8a, 0x1d, 0x82, 0xb4, 0xb9, 0xcd, 0xf8, 0xea };

	/*kek - hexdump(len=16): */
	unsigned char  kek[16]={	0xF3, 0x23, 0x63, 0x89, 0xC3, 0xF2, 0xE5, 0x49,
								0xAA, 0x76, 0x51, 0x94, 0x28, 0xFC, 0x62, 0x06};
	

	/*iv - hexdump(len=16): */
	unsigned char  iv_t[16]={	0x5c, 0x36, 0x5c, 0x36, 0x5c, 0x36, 0x5c, 0x36,
								0x5c, 0x36, 0x5c, 0x36, 0x5c, 0x36, 0x5c, 0x36};
								
	printk("-------------------------------------------------\n");
	dump_bin("kek",kek, 16);
	SMS4KeyExt(kek, (unsigned int *)enkey, 0);
	dump_bin("dekey", enkey, 128);
	dump_bin_int("dekey", enkey, 128);
	dump_bin("kek",kek, 16);
	dump_bin("iv_t", iv_t, 16);
	
	memset(ofbct, 0, 64);
	ret = wpi_decrypt(iv_t, ct_msk, 16, kek, ct_msk);
	dump_bin("pt_msk",ct_msk, 16);
}	
#endif
	printk("--------------------KEY EXT-----------------------\n");
	dump_bin("key",key, 16);
	SMS4KeyExt(key, (unsigned int *)enkey, 0);
	dump_bin("enkey", enkey, 128);
	dump_bin_int("enkey", enkey, 128);
	dump_bin("key",key, 16);
	dump_bin("OFBIV1", iv1, 16);
	
	printk("-------------------CBC-------------------------\n");
	memset(cbcmac, 0, 16);
	ret = wpi_pmac(iv1, pt1, 1, key, cbcmac);
	dump_bin("cbcpt", pt1, 16);
	dump_bin("cbcmac", cbcmac, 16);

	printk("-------------------OFB-------------------------\n");				
	memset(ofbct, 0, 64);
	ret = wpi_encrypt(iv1, pt1, 16, key, ofbct);
	dump_bin("pt1",pt1, 16);
	dump_bin("ct1",ofbct, 16);

	ret = wpi_decrypt(iv1, ofbct, 16, key, ofbdect);	
	dump_bin("ct1",ofbct, 16);
	dump_bin("dect1",ofbdect, 16);
	printk("-------------------------------------------------\n");
	
	memset(ofbct, 0, 64);	
	ret = wpi_encrypt(iv1, pt1, 4, key, ofbct);
	dump_bin("pt2", pt1, 4);
	dump_bin("ct2", ofbct, 4);

	memset(ofbdect, 0, 64);
	ret = wpi_decrypt(iv1, ofbct, 4, key, ofbdect);
	dump_bin("ofbct2",ofbct, 4);
	dump_bin("dect2",ofbdect, 4);

	/*return 1; */
	
	str_len = 1;/*strlen(tmpstr); */
	printk("OS_HZ is %d\n", OS_HZ);

	os_alloc_mem(NULL, (PUCHAR *)&data, M_LEN * str_len);	
	if(data == NULL)
	{
		printk("\n1. mem alloc failure\n");
		return -1;
	}
	
	/*crypt_data = (char *)malloc(M_LEN * str_len); */
	os_alloc_mem(NULL, (PUCHAR *)&crypt_data, M_LEN * str_len);
	if(crypt_data == NULL) 
	{
		printk("\n2. mem alloc failure\n");
		data = free_buffer(data, M_LEN*str_len);	
		return -1;
	}

	/*decrypt_data = (char *)malloc(M_LEN * str_len); */
	os_alloc_mem(NULL, (PUCHAR *)&decrypt_data, M_LEN * str_len);
	if(decrypt_data == NULL)
	{
		printk("\n3. mem alloc failure\n");
		data = free_buffer(data, M_LEN*str_len);
		crypt_data = free_buffer(crypt_data, M_LEN*str_len);
		return -1;
	}
	
	memset(data, 0, M_LEN * str_len);
	memset(crypt_data, 0, M_LEN * str_len);
	memset(decrypt_data, 0, M_LEN * str_len);

	/*
	for(i=0; i<M_LEN; i++)
	{
		memcpy(data +i*str_len, tmpstr, str_len);
	}
	*/
	memset(data, 0x0A, M_LEN * str_len);

	pt = data;
	ct = crypt_data;
/*	NdisGetSystemUpTime(&start); */
/*	printk("start is %ld\n", start); */
	for(i=0; i<counts; i++)
	{
		ret = wpi_encrypt(iv1, pt, M_LEN*str_len, key, ct);
	}
#if 0
	NdisGetSystemUpTime(&stop);	 
	printk("stop is %ld, stop - start is %ld\n", stop, stop - start);

	usetimes = (double)((stop - start))/OS_HZ;

	printk("encrypt go times is %f\n ", usetimes);
	if(usetimes > ee)
	{
		printk("rates is %f M/s\n", 
			(double)((M_LEN*str_len*counts*8))/(usetimes*1000*1000));
	}

	NdisGetSystemUpTime(&start);
#endif	
	for(i=0; i<counts; i++)
	{
		ret = wpi_decrypt(iv1, ct, (M_LEN*str_len), key, decrypt_data);
	}
#if 0	
	NdisGetSystemUpTime(&stop);	
	usetimes = (double)((stop - start))/OS_HZ;
	printk("decrypt go times is %f, \n", usetimes); 

	if(usetimes >ee)
	{
		printk(" rates is %f M/s\n", 
			(M_LEN * str_len*counts*8)/(usetimes*1000*1000));
	}

	NdisGetSystemUpTime(&start);
#endif	
	for(i=0; i<counts; i++)
	{
		ret = wpi_pmac(iv1, ct, (M_LEN * str_len)/16, key, cbcmac);
	}
#if 0	
	NdisGetSystemUpTime(&stop);	
	usetimes = ((stop - start))/OS_HZ;
		
	printk("mic go times is %f, \n", usetimes);

	if(usetimes > ee)
	{	printk(" rates is %f M/s\n", 
			(M_LEN * str_len*counts*8)/(usetimes*1000*1000));
	}
#endif	
	if(memcmp(data, decrypt_data, M_LEN * str_len) != 0)
	{
		printk("decrypt error\n");			
	}
	else
	{
		printk("decrypt ok\n");
	}

	data = free_buffer(data, M_LEN*str_len);
	crypt_data = free_buffer(crypt_data, M_LEN*str_len);
	decrypt_data = free_buffer(decrypt_data, M_LEN*str_len);

	return 1;	
	
}
#endif

/*
	========================================================================
	
	Routine Description:
		Compare two memory block

	Arguments:
		pSrc1		Pointer to first memory address
		pSrc2		Pointer to second memory address
		
	Return Value:
		0:			memory is equal
		1:			pSrc1 memory is larger
		2:			pSrc2 memory is larger

	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
ULONG	WapiCompareMemory(
	IN	PVOID	pSrc1,
	IN	PVOID	pSrc2,
	IN	ULONG	Length)
{
	PUCHAR	pMem1;
	PUCHAR	pMem2;
	ULONG	Index = 0;

	pMem1 = (PUCHAR) pSrc1;
	pMem2 = (PUCHAR) pSrc2;

	for (Index = 0; Index < Length; Index++)
	{
		if (pMem1[Index] > pMem2[Index])
			return (1);
		else if (pMem1[Index] < pMem2[Index])
			return (2);
	}

	/* Equal */
	return (0);
}

/************************************/
/* bitwise_xor()                    */
/* A 128 bit, bitwise exclusive or  */
/************************************/

void bitwise_xor_wpi(unsigned char *ina, unsigned char *inb, unsigned char *out)
{
	int i;
	for (i=0; i<16; i++)
	{
		out[i] = ina[i] ^ inb[i];
	}
}

/*cbc_mac*/
int wpi_cbc_mac_engine(
		unsigned char * maciv_in,
		unsigned char * in_data1,
		unsigned int 	in_data1_len,
		unsigned char * in_data2,
		unsigned int 	in_data2_len,
		unsigned char * pkey,
		unsigned char * mac_out)
{
	unsigned char  	mactmp[16];
	unsigned int 	i;
	unsigned int	num_blocks_1, num_blocks_2;
	unsigned int 	prmackey_in[32];

	num_blocks_1 = (in_data1_len) >> 4; 
	num_blocks_2 = (in_data2_len) >> 4; 	

	if((num_blocks_1 + num_blocks_2 ) < 1) return 1;
	if((num_blocks_1 + num_blocks_2 ) > 4096) return 1;

	SMS4KeyExt(pkey,  prmackey_in, 0);
	SMS4Crypt(maciv_in, mactmp, prmackey_in);	
	
	for (i = 0; i < num_blocks_1; i++) 
	{
		bitwise_xor_wpi(mactmp, in_data1, mactmp);
		in_data1 += 16;
		SMS4Crypt(mactmp, mactmp, prmackey_in);
	}
		
	for (i = 0; i < num_blocks_2; i++) 
	{
		bitwise_xor_wpi(mactmp, in_data2, mactmp);
		in_data2 += 16;
		SMS4Crypt(mactmp, mactmp, prmackey_in);
	}
	memcpy(mac_out, mactmp, 16);
	
	return 0;
}

/*ofb encrypt*/
int wpi_sms4_ofb_engine(
	unsigned char * pofbiv_in,
	unsigned char * pbw_in,
	unsigned int 	plbw_in,
	unsigned char * pkey,
	unsigned char * pcw_out)
{
	unsigned char ofbtmp[16];
	/*unsigned int * pint0, * pint1; */
	/*unsigned char * pchar0, * pchar1,* pchar2; */
	unsigned int counter,comp,i;
	unsigned int prkey_in[32];


	if(plbw_in<1)	return 1;
	/*if(plbw_in>65536) return 1; */

	SMS4KeyExt(pkey, prkey_in, 0);

	/* get the multiple of 16 */
	counter = plbw_in >> 4;
	/* get the remainder of 16 */
	comp = plbw_in & 0x0f;	

	/*get the iv */
	SMS4Crypt(pofbiv_in, ofbtmp, prkey_in);
	
	for(i = 0; i < counter; i++) 
	{
		bitwise_xor_wpi(pbw_in, ofbtmp, pcw_out);
		pbw_in += 16;
		pcw_out += 16;
		SMS4Crypt(ofbtmp, ofbtmp, prkey_in);		
	}

	for(i = 0; i < comp; i++) 
	{
		pcw_out[i]=pbw_in[i]^ofbtmp[i];
	}
	
	return 0;	
}


/*
	========================================================================
	
	Routine Description:
		Make the integrity check header for WPI MIC

	Arguments:		
		pAd			-	pointer to our pAdapter context	
  				
	Return Value:		
		
	Note:
		
	========================================================================
*/
UINT32 wpi_construct_mic_hdr(
		IN  PUCHAR			pHdr,
		IN	UINT32			data_len,
		IN	UCHAR			keyidx,		
		OUT	UCHAR 			*mic_hdr)
{
	UINT8 	from_ds, to_ds, a4_exists, qc_exists;
	//UINT8	frame_type;
	UINT8	frame_subtype;
	UINT8 	mhdr_len = 0;
	UCHAR 	ZERO_MAC_ADDR[MAC_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
	//frame_type = (((*pHdr) >> 2) & 0x03);
	frame_subtype = (((*pHdr) >> 4) & 0x0f);	

	/* Check if Addr4 is available */
	from_ds = ((*(pHdr + 1)) & 0x2) >> 1;
	to_ds = ((*(pHdr + 1)) & 0x1);
	a4_exists = (from_ds & to_ds);

	/* Check if Qos is available */
	qc_exists = ((frame_subtype == 0x08) ||    	/* QoS Data */
				  (frame_subtype == 0x09) ||   	/* QoS Data + CF-Ack */
				  (frame_subtype == 0x0a) || 	/* QoS Data + CF-Poll */
				  (frame_subtype == 0x0b)		/* QoS Data + CF-Ack + CF-Poll */
				 );

	/* Frame control */
	/* bit 4,5,6 must be zero */
	/* bit 11,12,13 must be zero */
	/* bit 14 must be 1 */
	mic_hdr[mhdr_len] = (*pHdr) & 0x8f;
	mic_hdr[mhdr_len + 1] = (*(pHdr + 1)) & 0xc7;
	mhdr_len = 2;
	
	/* Addr1 & 2 */
	WapiMoveMemory(&mic_hdr[mhdr_len], pHdr + 4, 2 * MAC_ADDR_LEN);
	mhdr_len += (2 * MAC_ADDR_LEN);
	
	/* In Sequence Control field, mute sequence numer bits (12-bit) */
	mic_hdr[mhdr_len] = (*(pHdr + 22)) & 0x0f;   
	mic_hdr[mhdr_len + 1] = 0x00;
	mhdr_len += 2;
	
	/* Addr3 */
	WapiMoveMemory(&mic_hdr[mhdr_len], pHdr + 16, MAC_ADDR_LEN);
	mhdr_len += MAC_ADDR_LEN;
		
	/* Fill the Addr4 field. */
	/* If the Addr4 is unused, it shall be padded zero. */
	if (a4_exists)		
		WapiMoveMemory(&mic_hdr[mhdr_len], pHdr + 24, MAC_ADDR_LEN);
	else
		WapiMoveMemory(&mic_hdr[mhdr_len], ZERO_MAC_ADDR, MAC_ADDR_LEN);
	mhdr_len += MAC_ADDR_LEN;

	/* Qos field */
	if (qc_exists & a4_exists)
	{
		mic_hdr[mhdr_len] = (*(pHdr + 30)) & 0x0f;   /* Qos_TC */
		mic_hdr[mhdr_len + 1] = 0x00;
		mhdr_len += 2;
	}
	else if (qc_exists & !a4_exists)
	{
		mic_hdr[mhdr_len] = (*(pHdr + 24)) & 0x0f;   /* Qos_TC */
		mic_hdr[mhdr_len + 1] = 0x00;
		mhdr_len += 2;
	}	

	/* fill the KeyIdx and reserve field */
	mic_hdr[mhdr_len] = keyidx;
	mic_hdr[mhdr_len + 1] = 0x00;
	mhdr_len += 2;

	/* Fill in data len field */
	mic_hdr[mhdr_len] = (data_len >> 8) & 0xff;
	mic_hdr[mhdr_len + 1] = (data_len) & 0xff;
	mhdr_len += 2;

	/* the len must be the multiple of 16 */
	/* If the Qos field doesn't exist, the header length shall be 32. */
	/* If the Qos exists, the header is 34. We need to expand it as 48 by padding zero. */
	/*DBGPRINT(RT_DEBUG_TRACE, ("mhdr_len(%d)\n", mhdr_len)); */
	mhdr_len = (mhdr_len > 32) ? 48 : 32;
				
	return mhdr_len;
}

/*
	========================================================================
	
	Routine Description:
		Calculate the MIC of WPI data.
		
	Arguments:		
		pAd			-	pointer to our pAdapter context	
  				
	Return Value:		
		TRUE	- success
		FLASE	- failure
		
	Note:
		
	========================================================================
*/
BOOLEAN RTMPCalculateWpiMic(
		IN	PUCHAR			pHeader,
		IN  PUCHAR			pData,
		IN	UINT32			data_len,
		IN	UINT8			key_idx,
		IN	PUCHAR 			pKey,
		IN	PUCHAR			pIV,
		OUT	PUCHAR			pOutMic)
{
	UCHAR	mic_header[48];
	UINT8	mic_hdr_len = 0;	
	UINT8	pad_len = 0;
	UINT8	remainder = 0;
		
	WapiZeroMemory(mic_header, 48);
		
	/* Construct integrity verify data header */
	mic_hdr_len = wpi_construct_mic_hdr(pHeader, data_len, key_idx, mic_header);

	/* calculate the padded length */
	if ((remainder = data_len & 0x0f) != 0)
		pad_len = (16 - remainder);

	/* Calculate MIC */
	wpi_cbc_mac_engine(pIV, 
			 mic_header, 
			 mic_hdr_len, 
			 pData, 
			 data_len + pad_len, 
			 pKey, 
			 pOutMic);
		
	return TRUE;
	
}

/*
	========================================================================
	
	Routine Description:
		Encrypt WPI-data by software.
		
	Arguments:		
		pAd			-	pointer to our pAdapter context	
  				
	Return Value:		
		TRUE	- success
		FLASE	- failure
		
	Note:
		
	========================================================================
*/
INT RTMPSoftEncryptSMS4(
		IN	PUCHAR			pHeader,
		IN  PUCHAR			pData,
		IN	UINT32			data_len,				
		IN	UCHAR			key_id,
		IN	PUCHAR 			pKey,
		IN	PUCHAR			pIv)
{
		INT		i;
		UCHAR	out_mic[LEN_WPI_MIC];
		UCHAR	iv_inv[LEN_WAPI_TSC];

		/* Inverse IV byte order */
		for (i=0; i < LEN_WAPI_TSC; i++)
		{
			iv_inv[LEN_WAPI_TSC - 1 - i] = (*(pIv + i)) & 0xFF;
		}

		WapiZeroMemory(out_mic, LEN_WPI_MIC);
		/* clear enough space for calculating WPI MIC later */
		WapiZeroMemory(pData + data_len, LEN_WPI_MIC);
		/* Calculate WPI MIC */
		RTMPCalculateWpiMic(pHeader, 
							pData, 
							data_len, 
							key_id, 
							pKey + 16, 
							iv_inv, 
							out_mic);

		/* append MIC to the data tail */
		WapiMoveMemory(pData + data_len, out_mic, LEN_WPI_MIC);		
		
		/* Encrypt WPI data by software */
		wpi_sms4_ofb_engine(iv_inv, pData, data_len + LEN_WPI_MIC, pKey, pData);			

		return TRUE;
							
}

/*
	========================================================================
	
	Routine Description:
		Decrypt WPI-data by software.
		
	Arguments:		
		pAd			-	pointer to our pAdapter context	
  				
	Return Value:		
		TRUE	- success
		FLASE	- failure
		
	Note:
		
	========================================================================
*/
INT	RTMPSoftDecryptSMS4(
		IN		PUCHAR			pHdr,
		IN		BOOLEAN			bSanityIV,
		IN 		PCIPHER_KEY		pKey,
		INOUT 	PUCHAR			pData,
		INOUT 	UINT16			*DataByteCnt)
{	
	UCHAR	i;
	UCHAR	key_idx;
	PUCHAR 	iv_ptr;
	UCHAR	iv_inv[LEN_WAPI_TSC];	
	UCHAR	MIC[LEN_WPI_MIC];
	UCHAR	TrailMIC[LEN_WPI_MIC];
	/*PUCHAR	mic_ptr; */
	/*UINT32	mic_data_len; */
	/*PUCHAR	buf_ptr = NULL; */
	PUCHAR	plaintext_ptr;
	UINT16	plaintext_len;
	PUCHAR	ciphertext_ptr;
	UINT16	ciphertext_len;
		
	/* Check the key is valid */
	if (pKey->KeyLen == 0)
	{
#if 0	
		DBGPRINT(RT_DEBUG_ERROR, ("%s : The key is not available !\n", 
									__FUNCTION__));
#endif
		return STATUS_WAPI_KEY_INVALID;
	}

	/* extract key index from the IV header */
	key_idx = *(pData) & 0xFF;
	
	/* Skip 2-bytes(key_idx and reserve field) and point to IV field */	
	iv_ptr = pData + 2;

	/* IV sanity check */
	if (bSanityIV)
	{
		if (WapiCompareMemory(iv_ptr, pKey->RxTsc, LEN_WAPI_TSC) > 1)
		{
#if 0
			DBGPRINT(RT_DEBUG_ERROR, ("ERROR : the received IV is invalid!!!\n"));	
			hex_dump("Received IV", iv_ptr, LEN_WAPI_TSC);
			hex_dump("Previous IV", pKey->RxTsc, LEN_WAPI_TSC);
#endif			
			return STATUS_WAPI_IV_MISMATCH;
		}
		else
		{
			/* Record the received IV */
			WapiMoveMemory(pKey->RxTsc, iv_ptr, LEN_WAPI_TSC);
		}
	}
 
	/* Inverse IV byte order for SMS4 calculation */
	for (i = 0; i < LEN_WAPI_TSC; i++)
	{
		iv_inv[LEN_WAPI_TSC - 1 - i] = (*(iv_ptr + i)) & 0xFF;
	}

	/* Skip the WPI IV header (18-bytes) */
	ciphertext_ptr = pData + LEN_WPI_IV_HDR;
	ciphertext_len = *DataByteCnt - LEN_WPI_IV_HDR;

	/* skip payload length is zero */
	if ((*DataByteCnt ) <= LEN_WPI_IV_HDR)
		return FALSE;

	/* Decrypt the WPI MPDU. It shall include plaintext and MIC.
	   The result output would overwrite the original WPI IV header position */
	wpi_sms4_ofb_engine(iv_inv, ciphertext_ptr, ciphertext_len, pKey->Key, pData);
			
	/* Point to the plainext data frame and its length shall exclude MIC length */
	plaintext_ptr = pData;
	plaintext_len = ciphertext_len - LEN_WPI_MIC;
	
	/* Extract peer's MIC and zero the MIC field of received frame */
	WapiMoveMemory(TrailMIC, plaintext_ptr + plaintext_len, LEN_WPI_MIC);
	WapiZeroMemory(plaintext_ptr + plaintext_len, LEN_WPI_MIC);
	
	/* Calculate WPI MIC */
	WapiZeroMemory(MIC, LEN_WPI_MIC);
	RTMPCalculateWpiMic(pHdr, 
						plaintext_ptr, 
						plaintext_len, 
						key_idx, 
						pKey->TxMic, 
						iv_inv, 
						MIC);

	/* Compare the MIC field */
	if (!WapiEqualMemory(MIC, TrailMIC, LEN_WPI_MIC))
	{			
#if 0
		DBGPRINT(RT_DEBUG_ERROR, ("WPI MIC Different!!!\n"));	
		hex_dump("Received MIC", TrailMIC, LEN_WPI_MIC);
		hex_dump("Desired  MIC", MIC, LEN_WPI_MIC);
#endif				
		return STATUS_WAPI_MIC_DIFF;
    }

	/* Update the total data length */
	*DataByteCnt = plaintext_len;
	
	return STATUS_SUCCESS;		
}

//#ifndef RTMP_RBUS_SUPPORT
#if 0
EXPORT_SYMBOL(RTMPSoftEncryptSMS4);
EXPORT_SYMBOL(RTMPSoftDecryptSMS4);

static int __init wapi_module_init(void)
{
    printk("wapi_module_init\n");
    return 0;
}

static void __exit wapi_module_exit(void)
{
     printk("wapi_module_exit\n");
}

module_init(wapi_module_init);
module_exit(wapi_module_exit); 
#endif /* RTMP_RBUS_SUPPORT */

