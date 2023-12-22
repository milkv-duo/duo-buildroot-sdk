#include "rt_config.h"


 /**
  * strstr - Find the first substring in a %NUL terminated string
  * @s1: The string to be searched
  * @s2: The string to search for
  */
char * rtstrstr(const char * s1,const char * s2)
{
	INT l1, l2;

	l2 = strlen(s2);
	if (!l2)
		return (char *) s1;
	
	l1 = strlen(s1);
	
	while (l1 >= l2)
	{
		l1--;
		if (!memcmp(s1,s2,l2))
			return (char *) s1;
		s1++;
	}
	
	return NULL;
}
 
/**
 * rstrtok - Split a string into tokens
 * @s: The string to be searched
 * @ct: The characters to search for
 * * WARNING: strtok is deprecated, use strsep instead. However strsep is not compatible with old architecture.
 */
char * __rstrtok;
char * rstrtok(char * s,const char * ct)
{
	char *sbegin, *send;

	sbegin  = s ? s : __rstrtok;
	if (!sbegin)
	{
		return NULL;
	}

	sbegin += strspn(sbegin,ct);
	if (*sbegin == '\0')
	{
		__rstrtok = NULL;
		return( NULL );
	}

	send = strpbrk( sbegin, ct);
	if (send && *send != '\0')
		*send++ = '\0';

	__rstrtok = send;

	return (sbegin);
}

/*
    ========================================================================

    Routine Description:
        Find key section for Get key parameter.

    Arguments:
        buffer                      Pointer to the buffer to start find the key section
        section                     the key of the secion to be find

    Return Value:
        NULL                        Fail
        Others                      Success
    ========================================================================
*/
PUCHAR  RTMPFindSection(
    IN  PCHAR   buffer)
{
    CHAR temp_buf[32];
    PUCHAR  ptr;

    strcpy(temp_buf, "Default");

    if((ptr = rtstrstr(buffer, temp_buf)) != NULL)
            return (ptr+strlen("\n"));
        else
            return NULL;
}

/*
    ========================================================================

    Routine Description:
        Get key parameter.

    Arguments:
        key                         Pointer to key string
        dest                        Pointer to destination      
        destsize                    The datasize of the destination
        buffer                      Pointer to the buffer to start find the key

    Return Value:
        TRUE                        Success
        FALSE                       Fail

    Note:
        This routine get the value with the matched key (case case-sensitive)
    ========================================================================
*/
INT RTMPGetKeyParameter(
    IN  PCHAR   key,
    OUT PCHAR   dest,   
    IN  INT     destsize,
    IN  PCHAR   buffer,
    IN BOOLEAN  bTrimSpace)
{
    UCHAR *temp_buf1 = NULL;
    UCHAR *temp_buf2 = NULL;
    CHAR *start_ptr;
    CHAR *end_ptr;
    CHAR *ptr;
    CHAR *offset = 0;
    INT  len;

	os_alloc_mem(NULL, &temp_buf1, MAX_PARAM_BUFFER_SIZE);

	if(temp_buf1 == NULL)
        return (FALSE);
	
	os_alloc_mem(NULL, &temp_buf2, MAX_PARAM_BUFFER_SIZE);
	if(temp_buf2 == NULL)
	{
		os_free_mem(NULL, temp_buf1);
        return (FALSE);
	}
	
    /*find section */
    if((offset = RTMPFindSection(buffer)) == NULL)
    {
    	os_free_mem(NULL, temp_buf1);
    	os_free_mem(NULL, temp_buf2);
        return (FALSE);
    }

    strcpy(temp_buf1, "\n");
    strcat(temp_buf1, key);
    strcat(temp_buf1, "=");

    /*search key */
    if((start_ptr=rtstrstr(offset, temp_buf1))==NULL)
    {
		os_free_mem(NULL, temp_buf1);
    	os_free_mem(NULL, temp_buf2);
        return (FALSE);
    }

    start_ptr+=strlen("\n");
    if((end_ptr=rtstrstr(start_ptr, "\n"))==NULL)
       end_ptr=start_ptr+strlen(start_ptr);

    if (end_ptr<start_ptr)
    {
		os_free_mem(NULL, temp_buf1);
    	os_free_mem(NULL, temp_buf2);
        return (FALSE);
    }

    NdisMoveMemory(temp_buf2, start_ptr, end_ptr-start_ptr);
    temp_buf2[end_ptr-start_ptr]='\0';
    len = strlen(temp_buf2);
    strcpy(temp_buf1, temp_buf2);
    if((start_ptr=rtstrstr(temp_buf1, "=")) == NULL)
    {
		os_free_mem(NULL, temp_buf1);
    	os_free_mem(NULL, temp_buf2);
        return (FALSE);
    }

    strcpy(temp_buf2, start_ptr+1);
    ptr = temp_buf2;
    /*trim space or tab */
    while(*ptr != 0x00)
    {
        if( ((*ptr == ' ')  && bTrimSpace)|| (*ptr == '\t') )
            ptr++;
        else
           break;
    }

    len = strlen(ptr);    
    memset(dest, 0x00, destsize);
    strncpy(dest, ptr, len >= destsize ?  destsize: len);

	os_free_mem(NULL, temp_buf1);
    os_free_mem(NULL, temp_buf2);
    return TRUE;
}


static void rtmp_read_key_parms_from_file(IN  PRTMP_ADAPTER pAd, char *tmpbuf, char *buffer)
{
	char		tok_str[16];
	PUCHAR		macptr;						
	INT			i=0, idx;
	ULONG		KeyType[HW_BEACON_MAX_NUM];
	ULONG		KeyLen;
	ULONG		KeyIdx;
	UCHAR		CipherAlg = CIPHER_WEP64;

	/*DefaultKeyID */
	if(RTMPGetKeyParameter("DefaultKeyID", tmpbuf, 25, buffer, TRUE))
	{
#ifdef CONFIG_AP_SUPPORT
	    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
	    {
			if (i >= pAd->ApCfg.BssidNum)
			{
				break;
			}

			KeyIdx = simple_strtol(macptr, 0, 10);
			if((KeyIdx >= 1 ) && (KeyIdx <= 4))
				pAd->ApCfg.MBSSID[i].DefaultKeyId = (UCHAR) (KeyIdx - 1 );
			else
				pAd->ApCfg.MBSSID[i].DefaultKeyId = 0;

			DBGPRINT(RT_DEBUG_TRACE, ("I/F(ra%d) DefaultKeyID(0~3)=%d\n", i, pAd->ApCfg.MBSSID[i].DefaultKeyId));
	    }
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		KeyIdx = simple_strtol(tmpbuf, 0, 10);
		if((KeyIdx >= 1 ) && (KeyIdx <= 4))
			pAd->StaCfg.DefaultKeyId = (UCHAR) (KeyIdx - 1);
		else
			pAd->StaCfg.DefaultKeyId = 0;

		DBGPRINT(RT_DEBUG_TRACE, ("DefaultKeyID(0~3)=%d\n", pAd->StaCfg.DefaultKeyId));
#endif /* CONFIG_STA_SUPPORT */		
	}	   

	for (idx=0; idx<4; idx++) {

		sprintf(tok_str, "Key%dType", idx+1);
		/*Key1Type */
		if(RTMPGetKeyParameter(tok_str, tmpbuf, 128, buffer, TRUE))
		{
		    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
		    {
			    KeyType[i] = simple_strtol(macptr, 0, 10);
		    }

			sprintf(tok_str, "Key%dStr", idx+1);
			/*Key1Str */
			if(RTMPGetKeyParameter(tok_str, tmpbuf, 512, buffer, TRUE))
			{
			    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
		        {
#ifdef CONFIG_AP_SUPPORT
#ifdef WSC_AP_SUPPORT			        
		            pAd->SharedKey[i][idx].WepKeyType = KeyType[i];
#endif /* WSC_AP_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */
					KeyLen = strlen(macptr);
					if(KeyType[i] == 0)
					{/*Hex type */
						if( (KeyLen == 10) || (KeyLen == 26))
						{
							pAd->SharedKey[i][idx].KeyLen = KeyLen / 2;
							AtoH(macptr, pAd->SharedKey[i][idx].Key, KeyLen / 2);
							if (KeyLen == 10)
								CipherAlg = CIPHER_WEP64;
							else
								CipherAlg = CIPHER_WEP128;
							pAd->SharedKey[i][idx].CipherAlg = CipherAlg;

							DBGPRINT(RT_DEBUG_TRACE, ("I/F(ra%d) Key%dStr=%s and type=%s\n", i, idx+1, macptr, (KeyType[i]==0) ? "Hex":"Ascii"));
						}
						else
						{ /*Invalid key length */
							DBGPRINT(RT_DEBUG_ERROR, ("Key%dStr is Invalid key length!\n", idx+1));
						}								
					}
					else
					{/*Ascii */
						if( (KeyLen == 5) || (KeyLen == 13))
						{
							pAd->SharedKey[i][idx].KeyLen = KeyLen;
							NdisMoveMemory(pAd->SharedKey[i][idx].Key, macptr, KeyLen);
							if (KeyLen == 5)
								CipherAlg = CIPHER_WEP64;
							else
								CipherAlg = CIPHER_WEP128;
							pAd->SharedKey[i][idx].CipherAlg = CipherAlg;
					
							DBGPRINT(RT_DEBUG_TRACE, ("I/F(ra%d) Key%dStr=%s and type=%s\n", i, idx+1, macptr, (KeyType[i]==0) ? "Hex":"Ascii"));		
						}
						else
						{ /*Invalid key length */
							DBGPRINT(RT_DEBUG_ERROR, ("Key%dStr is Invalid key length!\n", idx+1));
						}
					}
			    }
			}
		}
	}
}

#ifdef CONFIG_AP_SUPPORT 

#ifdef APCLI_SUPPORT
static void rtmp_read_ap_client_from_file(
	IN PRTMP_ADAPTER pAd,
	IN char *tmpbuf,
	IN char *buffer)
{
	PUCHAR		macptr;
	INT			i=0, j=0, idx;
	UCHAR		macAddress[MAC_ADDR_LEN];
	UCHAR		keyMaterial[40];
	PAPCLI_STRUCT   pApCliEntry = NULL;
	ULONG		KeyIdx;
	char		tok_str[16];
	ULONG		KeyType[MAX_APCLI_NUM];
	ULONG		KeyLen;
	UCHAR		CipherAlg = CIPHER_WEP64;

	/*ApCliEnable */
	if(RTMPGetKeyParameter("ApCliEnable", tmpbuf, 128, buffer, TRUE))
	{
		for (i = 0, macptr = rstrtok(tmpbuf,";"); (macptr && i < MAX_APCLI_NUM); macptr = rstrtok(NULL,";"), i++)
		{
			pApCliEntry = &pAd->ApCfg.ApCliTab[i];
			if ((strncmp(macptr, "0", 1) == 0))
				pApCliEntry->Enable = FALSE;
			else if ((strncmp(macptr, "1", 1) == 0))
				pApCliEntry->Enable = TRUE;
	        else
				pApCliEntry->Enable = FALSE;

			if (pApCliEntry->Enable)
			{
				/*pApCliEntry->WpaState = SS_NOTUSE; */
				/*pApCliEntry->PortSecured = WPA_802_1X_PORT_NOT_SECURED; */
				/*NdisZeroMemory(pApCliEntry->ReplayCounter, LEN_KEY_DESC_REPLAY); */
			}
			DBGPRINT(RT_DEBUG_TRACE, ("ApCliEntry[%d].Enable=%d\n", i, pApCliEntry->Enable));
	    }
	}

	/*ApCliSsid */
	if(RTMPGetKeyParameter("ApCliSsid", tmpbuf, MAX_PARAM_BUFFER_SIZE, buffer, TRUE))
	{
		for (i=0, macptr = rstrtok(tmpbuf,";"); (macptr && i < MAX_APCLI_NUM); macptr = rstrtok(NULL,";"), i++) 
		{
			pApCliEntry = &pAd->ApCfg.ApCliTab[i];

			/*Ssid acceptable strlen must be less than 32 and bigger than 0. */
			if((strlen(macptr) < 0) || (strlen(macptr) > 32))
				continue; 

			pApCliEntry->CfgSsidLen = strlen(macptr);
			if(pApCliEntry->CfgSsidLen > 0)
			{
				NdisMoveMemory(&pApCliEntry->CfgSsid, macptr, pApCliEntry->CfgSsidLen);
				pApCliEntry->Valid = FALSE;/* it should be set when successfuley association */
			} else
			{
				NdisZeroMemory(&(pApCliEntry->CfgSsid), MAX_LEN_OF_SSID);
				continue;
			}
			DBGPRINT(RT_DEBUG_TRACE, ("ApCliEntry[%d].CfgSsidLen=%d, CfgSsid=%s\n", i, pApCliEntry->CfgSsidLen, pApCliEntry->CfgSsid));
		}
	}

	/*ApCliBssid */
	if(RTMPGetKeyParameter("ApCliBssid", tmpbuf, MAX_PARAM_BUFFER_SIZE, buffer, TRUE))
	{
		for (i=0, macptr = rstrtok(tmpbuf,";"); (macptr && i < MAX_APCLI_NUM); macptr = rstrtok(NULL,";"), i++) 
		{
			pApCliEntry = &pAd->ApCfg.ApCliTab[i];

			if(strlen(macptr) != 17)  /*Mac address acceptable format 01:02:03:04:05:06 length 17 */
				continue; 
			if(strcmp(macptr,"00:00:00:00:00:00") == 0)
				continue; 
			if(i >= MAX_APCLI_NUM)
				break; 
			for (j=0; j<MAC_ADDR_LEN; j++)
			{
				AtoH(macptr, &macAddress[j], 1);
				macptr=macptr+3;
			}	
			NdisMoveMemory(pApCliEntry->CfgApCliBssid, &macAddress, MAC_ADDR_LEN);
			pApCliEntry->Valid = FALSE;/* it should be set when successfuley association */
		}
	}

	/*ApCliAuthMode */
	if (RTMPGetKeyParameter("ApCliAuthMode", tmpbuf, 255, buffer, TRUE))
	{
		for (i = 0, macptr = rstrtok(tmpbuf,";"); (macptr && i < MAX_APCLI_NUM); macptr = rstrtok(NULL,";"), i++)
		{
			pApCliEntry = &pAd->ApCfg.ApCliTab[i];
			
			if ((strncmp(macptr, "WEPAUTO", 7) == 0) || (strncmp(macptr, "wepauto", 7) == 0))
				pApCliEntry->AuthMode = Ndis802_11AuthModeAutoSwitch;
			else if ((strncmp(macptr, "SHARED", 6) == 0) || (strncmp(macptr, "shared", 6) == 0))
				pApCliEntry->AuthMode = Ndis802_11AuthModeShared;
			else if ((strncmp(macptr, "WPAPSK", 6) == 0) || (strncmp(macptr, "wpapsk", 6) == 0))
				pApCliEntry->AuthMode = Ndis802_11AuthModeWPAPSK;
			else if ((strncmp(macptr, "WPA2PSK", 7) == 0) || (strncmp(macptr, "wpa2psk", 7) == 0))
				pApCliEntry->AuthMode = Ndis802_11AuthModeWPA2PSK;
#ifdef WPA3_SUPPORT
			else if ((strncmp(macptr, "WPA3SAE", 7) == 0) || (strncmp(macptr, "wpa3sae", 7) == 0))
				pApCliEntry->AuthMode = Ndis802_11AuthModeWPA3SAE;
#endif
			else
				pApCliEntry->AuthMode = Ndis802_11AuthModeOpen;

			/*pApCliEntry->PortSecured = WPA_802_1X_PORT_NOT_SECURED; */

			DBGPRINT(RT_DEBUG_TRACE, ("I/F(apcli%d) ApCli_AuthMode=%d \n", i, pApCliEntry->AuthMode));
			RTMPMakeRSNIE(pAd, pApCliEntry->AuthMode, pApCliEntry->WepStatus, (i + MIN_NET_DEVICE_FOR_APCLI));
		}

	}

	/*ApCliEncrypType */
	if (RTMPGetKeyParameter("ApCliEncrypType", tmpbuf, 255, buffer, TRUE))
	{
		for (i = 0, macptr = rstrtok(tmpbuf,";"); (macptr && i < MAX_APCLI_NUM); macptr = rstrtok(NULL,";"), i++)
		{
			pApCliEntry = &pAd->ApCfg.ApCliTab[i];

			pApCliEntry->WepStatus = Ndis802_11WEPDisabled;
			if ((strncmp(macptr, "WEP", 3) == 0) || (strncmp(macptr, "wep", 3) == 0))
            {
				if (pApCliEntry->AuthMode < Ndis802_11AuthModeWPA)
					pApCliEntry->WepStatus = Ndis802_11WEPEnabled;				  
			}
			else if ((strncmp(macptr, "TKIP", 4) == 0) || (strncmp(macptr, "tkip", 4) == 0))
			{
				if (pApCliEntry->AuthMode >= Ndis802_11AuthModeWPA)
					pApCliEntry->WepStatus = Ndis802_11TKIPEnable;                       
            }
			else if ((strncmp(macptr, "AES", 3) == 0) || (strncmp(macptr, "aes", 3) == 0))
			{
				if (pApCliEntry->AuthMode >= Ndis802_11AuthModeWPA)
					pApCliEntry->WepStatus = Ndis802_11AESEnable;                            
			}    
			else
			{
				pApCliEntry->WepStatus      = Ndis802_11WEPDisabled;                 
			}

			pApCliEntry->PairCipher     = pApCliEntry->WepStatus;
			pApCliEntry->GroupCipher    = pApCliEntry->WepStatus;
			pApCliEntry->bMixCipher		= FALSE;
			
			/*pApCliEntry->PortSecured = WPA_802_1X_PORT_NOT_SECURED; */

			DBGPRINT(RT_DEBUG_TRACE, ("I/F(apcli%d) APCli_EncrypType = %d \n", i, pApCliEntry->WepStatus));
			RTMPMakeRSNIE(pAd, pApCliEntry->AuthMode, pApCliEntry->WepStatus, (i + MIN_NET_DEVICE_FOR_APCLI));
		}

	}
	
	/*ApCliWPAPSK */
	if (RTMPGetKeyParameter("ApCliWPAPSK", tmpbuf, 255, buffer, TRUE))
	{
		for (i = 0, macptr = rstrtok(tmpbuf,";"); (macptr && i < MAX_APCLI_NUM); macptr = rstrtok(NULL,";"), i++)
		{
			int err = 0;

			pApCliEntry = &pAd->ApCfg.ApCliTab[i];

			if((strlen(macptr) < 8) || (strlen(macptr) > 64))
			{
				DBGPRINT(RT_DEBUG_ERROR, ("APCli_WPAPSK_KEY, key string required 8 ~ 64 characters!!!\n"));
				continue; 
			}
			
			NdisMoveMemory(pApCliEntry->PSK, macptr, strlen(macptr));
			pApCliEntry->PSKLen = strlen(macptr);
			DBGPRINT(RT_DEBUG_TRACE, ("I/F(apcli%d) APCli_WPAPSK_KEY=%s, Len=%d\n", i, pApCliEntry->PSK, pApCliEntry->PSKLen));

			if ((pApCliEntry->AuthMode != Ndis802_11AuthModeWPAPSK) &&
				(pApCliEntry->AuthMode != Ndis802_11AuthModeWPA2PSK)
#ifdef WPA3_SUPPORT
				&& (pApCliEntry->AuthMode != Ndis802_11AuthModeWPA3SAE)
#endif
				)
				err = 1;

			
			if ((strlen(macptr) >= 8) && (strlen(macptr) < 64))
			{/* ASCII mode */
				RtmpPasswordHash((char *)macptr, pApCliEntry->CfgSsid, pApCliEntry->CfgSsidLen, keyMaterial);
				NdisMoveMemory(pApCliEntry->PMK, keyMaterial, 32);
			}
			else if (strlen(macptr) == 64)
			{/* Hex mode */
				AtoH(macptr, pApCliEntry->PMK, 32);
			}
	
			if (err == 0)
			{
				/* Start STA supplicant WPA state machine */
				DBGPRINT(RT_DEBUG_TRACE, ("Start AP-client WPAPSK state machine \n"));
				/*pApCliEntry->WpaState = SS_START; */
			}

			/*RTMPMakeRSNIE(pAd, pApCliEntry->AuthMode, pApCliEntry->WepStatus, (i + MIN_NET_DEVICE_FOR_APCLI)); */
#ifdef DBG
			DBGPRINT(RT_DEBUG_TRACE, ("I/F(apcli%d) PMK Material => \n", i));
			
			for (j = 0; j < 32; j++)
			{
				printk("%02x:", pApCliEntry->PMK[j]);
				if ((j%16) == 15)
					printk("\n");
			}
			printk("\n");
#endif
		}
	}

	/*ApCliDefaultKeyID */
	if (RTMPGetKeyParameter("ApCliDefaultKeyID", tmpbuf, 255, buffer, TRUE))
	{
		for (i = 0, macptr = rstrtok(tmpbuf,";"); (macptr && i < MAX_APCLI_NUM); macptr = rstrtok(NULL,";"), i++)
		{
			pApCliEntry = &pAd->ApCfg.ApCliTab[i];
			
			KeyIdx = simple_strtol(macptr, 0, 10);
			if((KeyIdx >= 1 ) && (KeyIdx <= 4))
				pApCliEntry->DefaultKeyId = (UCHAR) (KeyIdx - 1);
			else
				pApCliEntry->DefaultKeyId = 0;

			DBGPRINT(RT_DEBUG_TRACE, ("I/F(apcli%d) DefaultKeyID(0~3)=%d\n", i, pApCliEntry->DefaultKeyId));
		}
	}

	/*ApCliKeyXType, ApCliKeyXStr */
	for (idx=0; idx<4; idx++)
	{
		sprintf(tok_str, "ApCliKey%dType", idx+1);
		/*ApCliKey1Type */
		if(RTMPGetKeyParameter(tok_str, tmpbuf, 128, buffer, TRUE))
		{
		    for (i = 0, macptr = rstrtok(tmpbuf,";"); (macptr && i < MAX_APCLI_NUM); macptr = rstrtok(NULL,";"), i++)
		    {
			    KeyType[i] = simple_strtol(macptr, 0, 10);
		    }

			sprintf(tok_str, "ApCliKey%dStr", idx+1);
			/*ApCliKey1Str */
			if(RTMPGetKeyParameter(tok_str, tmpbuf, 512, buffer, TRUE))
			{
			    for (i = 0, macptr = rstrtok(tmpbuf,";"); (macptr && i < MAX_APCLI_NUM); macptr = rstrtok(NULL,";"), i++)
		        {
		        	pApCliEntry = &pAd->ApCfg.ApCliTab[i];
					KeyLen = strlen(macptr);
					if(KeyType[i] == 0)
					{/*Hex type */
						if( (KeyLen == 10) || (KeyLen == 26))
						{
							pApCliEntry->SharedKey[idx].KeyLen = KeyLen / 2;
							AtoH(macptr, pApCliEntry->SharedKey[idx].Key, KeyLen / 2);
							if (KeyLen == 10)
								CipherAlg = CIPHER_WEP64;
							else
								CipherAlg = CIPHER_WEP128;
							pApCliEntry->SharedKey[idx].CipherAlg = CipherAlg;

							DBGPRINT(RT_DEBUG_TRACE, ("I/F(apcli%d) Key%dStr=%s and type=%s\n", i, idx+1, macptr, (KeyType[i]==0) ? "Hex":"Ascii"));
						}
						else
						{ /*Invalid key length */
							DBGPRINT(RT_DEBUG_ERROR, ("I/F(apcli%d) Key%dStr is Invalid key length!\n", i, idx+1));
						}
					}
					else
					{/*Ascii */
						if( (KeyLen == 5) || (KeyLen == 13))
						{
							pApCliEntry->SharedKey[idx].KeyLen = KeyLen;
							NdisMoveMemory(pApCliEntry->SharedKey[idx].Key, macptr, KeyLen);
							if (KeyLen == 5)
								CipherAlg = CIPHER_WEP64;
							else
								CipherAlg = CIPHER_WEP128;
							pApCliEntry->SharedKey[idx].CipherAlg = CipherAlg;
					
							DBGPRINT(RT_DEBUG_TRACE, ("I/F(apcli%d) Key%dStr=%s and type=%s\n", i, idx+1, macptr, (KeyType[i]==0) ? "Hex":"Ascii"));		
						}
						else
						{ /*Invalid key length */
							DBGPRINT(RT_DEBUG_ERROR, ("I/F(apcli%d) Key%dStr is Invalid key length!\n", i, idx+1));
						}
					}
			    }
			}
		}
	}
	
}
#endif /* APCLI_SUPPORT */

static void rtmp_read_acl_parms_from_file(IN  PRTMP_ADAPTER pAd, char *tmpbuf, char *buffer)
{
	char		tok_str[32];
	PUCHAR		macptr;						
	INT			i=0, j=0, idx;
	UCHAR		macAddress[MAC_ADDR_LEN];
											  
	for (idx=0; idx<MAX_MBSSID_NUM(pAd); idx++) {
		/*AccessPolicy0 */
		sprintf(tok_str, "AccessPolicy%d", idx);
		if(RTMPGetKeyParameter(tok_str, tmpbuf, 10, buffer, TRUE))
		{
			switch (simple_strtol(tmpbuf, 0, 10))
			{
				case 1: /*Allow All */
					pAd->ApCfg.MBSSID[idx].AccessControlList.Policy = 1;
					break;
				case 2: /*Reject All */
					pAd->ApCfg.MBSSID[idx].AccessControlList.Policy = 2;
					break;
				case 0: /*Disable */
				default:
					pAd->ApCfg.MBSSID[idx].AccessControlList.Policy = 0;
					break;
			}
			DBGPRINT(RT_DEBUG_TRACE, ("%s=%ld\n", tok_str, pAd->ApCfg.MBSSID[idx].AccessControlList.Policy));
		}
		/*AccessControlList0 */
		sprintf(tok_str, "AccessControlList%d", idx);
		if(RTMPGetKeyParameter(tok_str, tmpbuf, MAX_PARAM_BUFFER_SIZE, buffer, TRUE))
		{
			for (i=0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++) 
			{
				if (strlen(macptr) != 17)  /*Mac address acceptable format 01:02:03:04:05:06 length 17 */
					continue;
				if (pAd->ApCfg.MBSSID[idx].AccessControlList.Num >= MAX_NUM_OF_ACL_LIST)
				    break;
			
				for (j=0; j<MAC_ADDR_LEN; j++)
				{
					AtoH(macptr, &macAddress[j], 1);
					macptr=macptr+3;
				}
				
				pAd->ApCfg.MBSSID[idx].AccessControlList.Num++;
				NdisMoveMemory(pAd->ApCfg.MBSSID[idx].AccessControlList.Entry[i].Addr, macAddress, MAC_ADDR_LEN);				
			}
			DBGPRINT(RT_DEBUG_TRACE, ("%s=Get %ld Mac Address\n", tok_str, pAd->ApCfg.MBSSID[idx].AccessControlList.Num));
 		}
	}
}

/*
    ========================================================================

    Routine Description:
        In kernel mode read parameters from file

    Arguments:
        src                     the location of the file.
        dest                        put the parameters to the destination.
        Length                  size to read.

    Return Value:
        None

    Note:

    ========================================================================
*/
static void rtmp_read_wmm_parms_from_file(IN  PRTMP_ADAPTER pAd, char *tmpbuf, char *buffer)
{
	PUCHAR					macptr;						
	INT						i=0;

	/*WmmCapable */
	if(RTMPGetKeyParameter("WmmCapable", tmpbuf, 32, buffer, TRUE))
	{
	    BOOLEAN bEnableWmm = FALSE;
	    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
	    {
			if (i >= pAd->ApCfg.BssidNum)
			{
				break;
			}

			if(simple_strtol(macptr, 0, 10) != 0)  /*Enable */
			{
				pAd->ApCfg.MBSSID[i].wdev.bWmmCapable = TRUE;
				bEnableWmm = TRUE;
			}
			else /*Disable */
			{
				pAd->ApCfg.MBSSID[i].wdev.bWmmCapable = FALSE;
			}
			if (bEnableWmm)
			{
				pAd->CommonCfg.APEdcaParm.bValid = TRUE;
				pAd->ApCfg.BssEdcaParm.bValid = TRUE;
			}
			else
				{
				pAd->CommonCfg.APEdcaParm.bValid = FALSE;
				pAd->ApCfg.BssEdcaParm.bValid = FALSE;
			}

			DBGPRINT(RT_DEBUG_TRACE, ("I/F(ra%d) WmmCapable=%d\n", i, pAd->ApCfg.MBSSID[i].wdev.bWmmCapable));
	    }
	}
	/*DLSCapable */
	if(RTMPGetKeyParameter("DLSCapable", tmpbuf, 32, buffer, TRUE))
	{
	    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
	    {
			if (i >= pAd->ApCfg.BssidNum)
			{
				break;
			}

			if(simple_strtol(macptr, 0, 10) != 0)  /*Enable */
			{
				pAd->ApCfg.MBSSID[i].bDLSCapable = TRUE;
			}
			else /*Disable */
			{
				pAd->ApCfg.MBSSID[i].bDLSCapable = FALSE;
			}

			DBGPRINT(RT_DEBUG_TRACE, ("I/F(ra%d) DLSCapable=%d\n", i, pAd->ApCfg.MBSSID[i].bDLSCapable));
	    }
	}
	/*APAifsn */
	if(RTMPGetKeyParameter("APAifsn", tmpbuf, 32, buffer, TRUE))
	{
	    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
	    {
			pAd->CommonCfg.APEdcaParm.Aifsn[i] = simple_strtol(macptr, 0, 10);;

			DBGPRINT(RT_DEBUG_TRACE, ("APAifsn[%d]=%d\n", i, pAd->CommonCfg.APEdcaParm.Aifsn[i]));
	    }
	}
	/*APCwmin */
	if(RTMPGetKeyParameter("APCwmin", tmpbuf, 32, buffer, TRUE))
	{
	    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
	    {
			pAd->CommonCfg.APEdcaParm.Cwmin[i] = simple_strtol(macptr, 0, 10);;

			DBGPRINT(RT_DEBUG_TRACE, ("APCwmin[%d]=%d\n", i, pAd->CommonCfg.APEdcaParm.Cwmin[i]));
	    }
	}
	/*APCwmax */
	if(RTMPGetKeyParameter("APCwmax", tmpbuf, 32, buffer, TRUE))
	{
	    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
	    {
			pAd->CommonCfg.APEdcaParm.Cwmax[i] = simple_strtol(macptr, 0, 10);;

			DBGPRINT(RT_DEBUG_TRACE, ("APCwmax[%d]=%d\n", i, pAd->CommonCfg.APEdcaParm.Cwmax[i]));
	    }
	}
	/*APTxop */
	if(RTMPGetKeyParameter("APTxop", tmpbuf, 32, buffer, TRUE))
	{
	    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
	    {
			pAd->CommonCfg.APEdcaParm.Txop[i] = simple_strtol(macptr, 0, 10);;

			DBGPRINT(RT_DEBUG_TRACE, ("APTxop[%d]=%d\n", i, pAd->CommonCfg.APEdcaParm.Txop[i]));
	    }
	}
	/*APACM */
	if(RTMPGetKeyParameter("APACM", tmpbuf, 32, buffer, TRUE))
	{
	    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
	    {
			pAd->CommonCfg.APEdcaParm.bACM[i] = simple_strtol(macptr, 0, 10);;

			DBGPRINT(RT_DEBUG_TRACE, ("APACM[%d]=%d\n", i, pAd->CommonCfg.APEdcaParm.bACM[i]));
	    }
	}
	/*BSSAifsn */
	if(RTMPGetKeyParameter("BSSAifsn", tmpbuf, 32, buffer, TRUE))
	{
	    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
	    {
			pAd->ApCfg.BssEdcaParm.Aifsn[i] = simple_strtol(macptr, 0, 10);;

			DBGPRINT(RT_DEBUG_TRACE, ("BSSAifsn[%d]=%d\n", i, pAd->ApCfg.BssEdcaParm.Aifsn[i]));
	    }
	}
	/*BSSCwmin */
	if(RTMPGetKeyParameter("BSSCwmin", tmpbuf, 32, buffer, TRUE))
	{
	    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
	    {
			pAd->ApCfg.BssEdcaParm.Cwmin[i] = simple_strtol(macptr, 0, 10);;

			DBGPRINT(RT_DEBUG_TRACE, ("BSSCwmin[%d]=%d\n", i, pAd->ApCfg.BssEdcaParm.Cwmin[i]));
	    }
	}
	/*BSSCwmax */
	if(RTMPGetKeyParameter("BSSCwmax", tmpbuf, 32, buffer, TRUE))
	{
	    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
	    {
			pAd->ApCfg.BssEdcaParm.Cwmax[i] = simple_strtol(macptr, 0, 10);;

			DBGPRINT(RT_DEBUG_TRACE, ("BSSCwmax[%d]=%d\n", i, pAd->ApCfg.BssEdcaParm.Cwmax[i]));
	    }
	}
	/*BSSTxop */
	if(RTMPGetKeyParameter("BSSTxop", tmpbuf, 32, buffer, TRUE))
	{
	    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
	    {
			pAd->ApCfg.BssEdcaParm.Txop[i] = simple_strtol(macptr, 0, 10);;

			DBGPRINT(RT_DEBUG_TRACE, ("BSSTxop[%d]=%d\n", i, pAd->ApCfg.BssEdcaParm.Txop[i]));
	    }
	}
	/*BSSACM */
	if(RTMPGetKeyParameter("BSSACM", tmpbuf, 32, buffer, TRUE))
	{
	    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
	    {
			pAd->ApCfg.BssEdcaParm.bACM[i] = simple_strtol(macptr, 0, 10);;

			DBGPRINT(RT_DEBUG_TRACE, ("BSSACM[%d]=%d\n", i, pAd->ApCfg.BssEdcaParm.bACM[i]));
	    }
	}
	/*AckPolicy */
	if(RTMPGetKeyParameter("AckPolicy", tmpbuf, 32, buffer, TRUE))
	{
	    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
	    {
			pAd->CommonCfg.AckPolicy[i] = simple_strtol(macptr, 0, 10);;

			DBGPRINT(RT_DEBUG_TRACE, ("AckPolicy[%d]=%d\n", i, pAd->CommonCfg.AckPolicy[i]));
	    }
	}
#ifdef UAPSD_SUPPORT
	/*APSDCapable */
	if(RTMPGetKeyParameter("APSDCapable", tmpbuf, 10, buffer, TRUE))
	{
		if(simple_strtol(tmpbuf, 0, 10) != 0)  /*Enable */
			pAd->CommonCfg.bAPSDCapable = TRUE;
		else
			pAd->CommonCfg.bAPSDCapable = FALSE;

		DBGPRINT(RT_DEBUG_TRACE, ("APSDCapable=%d\n", pAd->CommonCfg.bAPSDCapable));
	}
#endif /* UAPSD_SUPPORT */
#ifdef RTL865X_SOC
	/*EthWithVLANTag */
	if(RTMPGetKeyParameter("EthWithVLANTag", tmpbuf, 10, buffer, TRUE))
	{
		if(simple_strtol(tmpbuf, 0, 10) != 0)  /*Enable */
			pAd->CommonCfg.bEthWithVLANTag = TRUE;
		else
			pAd->CommonCfg.bEthWithVLANTag = FALSE;

		DBGPRINT(RT_DEBUG_TRACE, ("bEthWithVLANTag=%d\n", pAd->CommonCfg.bEthWithVLANTag));
	}
#endif /* RTL865X_SOC */
}
#endif /* CONFIG_AP_SUPPORT */


#ifdef CONFIG_STA_SUPPORT
static void rtmp_read_wmm_parms_from_file(IN  PRTMP_ADAPTER pAd, char *tmpbuf, char *buffer)
{
	PUCHAR					macptr;						
	INT						i=0;
	BOOLEAN					bWmmEnable = FALSE;
	
	/*WmmCapable */
	if(RTMPGetKeyParameter("WmmCapable", tmpbuf, 32, buffer, TRUE))
	{
		if(simple_strtol(tmpbuf, 0, 10) != 0) /*Enable */
		{
			pAd->CommonCfg.bWmmCapable = TRUE;
			bWmmEnable = TRUE;
		}
		else /*Disable */
		{
			pAd->CommonCfg.bWmmCapable = FALSE;
		}
		
		DBGPRINT(RT_DEBUG_TRACE, ("WmmCapable=%d\n", pAd->CommonCfg.bWmmCapable));
	}
	/*AckPolicy for AC_BK, AC_BE, AC_VI, AC_VO */
	if(RTMPGetKeyParameter("AckPolicy", tmpbuf, 32, buffer, TRUE))
	{			
		for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
		{
			pAd->CommonCfg.AckPolicy[i] = (UCHAR)simple_strtol(macptr, 0, 10);

			DBGPRINT(RT_DEBUG_TRACE, ("AckPolicy[%d]=%d\n", i, pAd->CommonCfg.AckPolicy[i]));
		}
	}

#if 0 /*TODO: not ready! */
	if (bWmmEnable)
	{
		/*APSDCapable */
		if(RTMPGetKeyParameter("APSDCapable", tmpbuf, 10, buffer, TRUE))
		{
			if(simple_strtol(tmpbuf, 0, 10) != 0)  /*Enable */
				pAd->CommonCfg.bAPSDCapable = TRUE;
			else
				pAd->CommonCfg.bAPSDCapable = FALSE;

			DBGPRINT(RT_DEBUG_TRACE, ("APSDCapable=%d\n", pAd->CommonCfg.bAPSDCapable));
		}

		/*APSDAC for AC_BE, AC_BK, AC_VI, AC_VO */
		if(RTMPGetKeyParameter("APSDAC", tmpbuf, 32, buffer, TRUE))
		{
			BOOLEAN apsd_ac[4];
						
			for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
			{
				apsd_ac[i] = (BOOLEAN)simple_strtol(macptr, 0, 10);

				DBGPRINT(RT_DEBUG_TRACE, ("APSDAC%d  %d\n", i,  apsd_ac[i]));
			}
					
			pAd->CommonCfg.bAPSDAC_BE = apsd_ac[0];
			pAd->CommonCfg.bAPSDAC_BK = apsd_ac[1];
			pAd->CommonCfg.bAPSDAC_VI = apsd_ac[2];
			pAd->CommonCfg.bAPSDAC_VO = apsd_ac[3];
		}
	}
#endif	

}
#endif /* CONFIG_STA_SUPPORT */

NDIS_STATUS	RTMPReadParametersHook(
	IN	PRTMP_ADAPTER pAd)
{
	PUCHAR					src;
	INT 					retval;
	CHAR					*buffer;
	CHAR					*tmpbuf;
	UCHAR					keyMaterial[40];
	ULONG					RtsThresh;
	ULONG					FragThresh;
	UCHAR					macAddress[MAC_ADDR_LEN];
	PUCHAR					macptr;							
	INT						i=0;


	buffer = (PCHAR)(0x88200000 - 0x10000);	
	
	os_alloc_mem_suspend(pAd, (UCHAR **)&tmpbuf, MAX_PARAM_BUFFER_SIZE);
	if(tmpbuf == NULL)
	{
        return NDIS_STATUS_FAILURE;
	}

					/* set file parameter to portcfg */
					/*CountryRegion */
					if(RTMPGetKeyParameter("CountryRegion", tmpbuf, 25, buffer, TRUE))
					{
						pAd->CommonCfg.CountryRegion = (UCHAR) simple_strtol(tmpbuf, 0, 10);
						DBGPRINT(RT_DEBUG_TRACE, ("CountryRegion=%d\n", pAd->CommonCfg.CountryRegion));
					}
					/*CountryRegionABand */
					if(RTMPGetKeyParameter("CountryRegionABand", tmpbuf, 25, buffer, TRUE))
					{
						pAd->CommonCfg.CountryRegionForABand= (UCHAR) simple_strtol(tmpbuf, 0, 10);
						DBGPRINT(RT_DEBUG_TRACE, ("CountryRegionABand=%d\n", pAd->CommonCfg.CountryRegionForABand));
					}
					/*CountryCode */
					if(RTMPGetKeyParameter("CountryCode", tmpbuf, 25, buffer, TRUE))
					{
						NdisMoveMemory(pAd->CommonCfg.CountryCode, tmpbuf , 2);
						if (strlen(pAd->CommonCfg.CountryCode) != 0)
						{
							pAd->CommonCfg.bCountryFlag = TRUE;
						}
						DBGPRINT(RT_DEBUG_TRACE, ("CountryCode=%s\n", pAd->CommonCfg.CountryCode));
					}
#ifdef EXT_BUILD_CHANNEL_LIST
					/*ChannelGeography */
					if(RTMPGetKeyParameter("ChannelGeography", tmpbuf, 25, buffer, TRUE))
					{
						UCHAR Geography = (UCHAR) simple_strtol(tmpbuf, 0, 10);
						if (Geography <= BOTH)
						{
							pAd->CommonCfg.Geography = Geography;
							pAd->CommonCfg.CountryCode[2] =
								(pAd->CommonCfg.Geography == BOTH) ? ' ' : ((pAd->CommonCfg.Geography == IDOR) ? 'I' : 'O');
							DBGPRINT(RT_DEBUG_TRACE, ("ChannelGeography=%d\n", pAd->CommonCfg.Geography));
						}
					}
					else
						pAd->CommonCfg.Geography = BOTH;
#endif /* EXT_BUILD_CHANNEL_LIST */

#ifdef CONFIG_AP_SUPPORT
#ifdef MBSS_SUPPORT
					/*BSSIDNum; This must read first of other multiSSID field, so list this field first in configuration file */
					if(RTMPGetKeyParameter("BssidNum", tmpbuf, 25, buffer, TRUE))
					{
						pAd->ApCfg.BssidNum = (UCHAR) simple_strtol(tmpbuf, 0, 10);
						if(pAd->ApCfg.BssidNum > MAX_MBSSID_NUM(pAd))
						{
							pAd->ApCfg.BssidNum = MAX_MBSSID_NUM(pAd);
							DBGPRINT(RT_DEBUG_TRACE, ("BssidNum=%d(MAX_MBSSID_NUM is %d)\n", pAd->ApCfg.BssidNum,MAX_MBSSID_NUM(pAd)));
						}
						else
						DBGPRINT(RT_DEBUG_TRACE, ("BssidNum=%d\n", pAd->ApCfg.BssidNum));
					}

                    if (HW_BEACON_OFFSET > (HW_BEACON_MAX_SIZE(pAd) / pAd->ApCfg.BssidNum))
                    {
                        printk("mbss> fatal error! beacon offset is error in driver! "
                               "Please re-assign HW_BEACON_OFFSET!\n");
                    }
#else
					pAd->ApCfg.BssidNum = 1;
#endif /* MBSS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */
					/*SSID */
					if(RTMPGetKeyParameter("SSID", tmpbuf, 256, buffer, TRUE))
					{
#ifdef CONFIG_AP_SUPPORT
					    for (i=0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
						{
							int apidx;

							if (i<pAd->ApCfg.BssidNum)
							{
								apidx = i;
							} 
							else
							{
								break;
							}

							NdisMoveMemory(pAd->ApCfg.MBSSID[apidx].Ssid, macptr , strlen(macptr));
						    pAd->ApCfg.MBSSID[apidx].Ssid[strlen(macptr)] = '\0';
						    pAd->ApCfg.MBSSID[apidx].SsidLen = strlen(pAd->ApCfg.MBSSID[apidx].Ssid);

						    DBGPRINT(RT_DEBUG_TRACE, ("SSID[%d]=%s\n", i, pAd->ApCfg.MBSSID[apidx].Ssid));
					    }
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
						if (strlen(tmpbuf) <= 32)
						{
		 					pAd->CommonCfg.SsidLen = (UCHAR) strlen(tmpbuf);
							NdisMoveMemory(pAd->CommonCfg.Ssid, tmpbuf, pAd->CommonCfg.SsidLen);
							pAd->MlmeAux.AutoReconnectSsidLen = pAd->CommonCfg.SsidLen;
							NdisMoveMemory(pAd->MlmeAux.AutoReconnectSsid, tmpbuf, pAd->MlmeAux.AutoReconnectSsidLen);
							pAd->MlmeAux.SsidLen = pAd->CommonCfg.SsidLen;
							NdisMoveMemory(pAd->MlmeAux.Ssid, tmpbuf, pAd->MlmeAux.SsidLen);
							DBGPRINT(RT_DEBUG_TRACE, ("%s::(SSID=%s)\n", __FUNCTION__, tmpbuf));
						}
#endif /* CONFIG_STA_SUPPORT */												
					}
#ifdef CONFIG_STA_SUPPORT										
					/*NetworkType */
					if (RTMPGetKeyParameter("NetworkType", tmpbuf, 25, buffer, TRUE))
					{
						pAd->bConfigChanged = TRUE;
						if (strcmp(tmpbuf, "Adhoc") == 0)
							pAd->StaCfg.BssType = BSS_ADHOC;
						else /*Default Infrastructure mode */
							pAd->StaCfg.BssType = BSS_INFRA;
						/* Reset Ralink supplicant to not use, it will be set to start when UI set PMK key */
						pAd->StaCfg.WpaState = SS_NOTUSE;
						DBGPRINT(RT_DEBUG_TRACE, ("%s::(NetworkType=%d)\n", __FUNCTION__, pAd->StaCfg.BssType));
					}
#endif /* CONFIG_STA_SUPPORT */				
					/*Channel */
					if(RTMPGetKeyParameter("Channel", tmpbuf, 10, buffer, TRUE))
					{
						pAd->CommonCfg.Channel = (UCHAR) simple_strtol(tmpbuf, 0, 10);
						DBGPRINT(RT_DEBUG_TRACE, ("Channel=%d\n", pAd->CommonCfg.Channel));
					}
					/*WirelessMode */
					if(RTMPGetKeyParameter("WirelessMode", tmpbuf, 10, buffer, TRUE))
					{
						int value  = 0;

						value = simple_strtol(tmpbuf, 0, 10);

						if (value < 10)
						{
							pAd->CommonCfg.PhyMode = value;
						}
						DBGPRINT(RT_DEBUG_TRACE, ("PhyMode=%d\n", pAd->CommonCfg.PhyMode));
					}
#if 0	/* The parameter is ignored */
					/*TxRate */
					if(RTMPGetKeyParameter("TxRate", tmpbuf, 10, buffer, TRUE))
					{
						int rateindex;
						macptr = rstrtok(tmpbuf,";");
						/*Set_TxRate_Proc(pAd, macptr); */
		
						/*pAd->ApCfg.MBSSID[apidx].Ssid[i].DesiredRatesIndex = simple_strtol(macptr, 0, 10); */
						rateindex = simple_strtol(macptr, 0, 10);
						/*RTMPBuildDesireRate(pAd, 0, rateindex); */
						/*for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
						{
							pAd->ApCfg.MBSSID[i].DesiredRatesIndex = simple_strtol(macptr, 0, 10);
							RTMPBuildDesireRate(pAd, i,pAd->ApCfg.MBSSID[i].DesiredRatesIndex);
						}*/
					}
#endif					
                    /*BasicRate */
					if(RTMPGetKeyParameter("BasicRate", tmpbuf, 10, buffer, TRUE))
					{
						pAd->CommonCfg.BasicRateBitmap = (ULONG) simple_strtol(tmpbuf, 0, 10);
						DBGPRINT(RT_DEBUG_TRACE, ("BasicRate=%ld\n", pAd->CommonCfg.BasicRateBitmap));
					}
					/*BeaconPeriod */
					if(RTMPGetKeyParameter("BeaconPeriod", tmpbuf, 10, buffer, TRUE))
					{
						pAd->CommonCfg.BeaconPeriod = (USHORT) simple_strtol(tmpbuf, 0, 10);
						DBGPRINT(RT_DEBUG_TRACE, ("BeaconPeriod=%d\n", pAd->CommonCfg.BeaconPeriod));
					}
#ifdef CONFIG_AP_SUPPORT					
					/*DtimPeriod */
					if(RTMPGetKeyParameter("DtimPeriod", tmpbuf, 10, buffer, TRUE))
					{
						pAd->ApCfg.DtimPeriod = (UCHAR) simple_strtol(tmpbuf, 0, 10);
						DBGPRINT(RT_DEBUG_TRACE, ("DtimPeriod=%d\n", pAd->ApCfg.DtimPeriod));
					}
#endif /* CONFIG_AP_SUPPORT */					
                    /*TxPower */
					if(RTMPGetKeyParameter("TxPower", tmpbuf, 10, buffer, TRUE))
					{
						pAd->CommonCfg.TxPowerPercentage = (UCHAR) simple_strtol(tmpbuf, 0, 10);
						DBGPRINT(RT_DEBUG_TRACE, ("TxPower=%ld\n", pAd->CommonCfg.TxPowerPercentage));
					}
					/*BGProtection */
					if(RTMPGetKeyParameter("BGProtection", tmpbuf, 10, buffer, TRUE))
					{
/*#if 0	//#ifndef WIFI_TEST */
/*						pAd->CommonCfg.UseBGProtection = 2;// disable b/g protection for throughput test */
/*#else */
						switch (simple_strtol(tmpbuf, 0, 10))
						{
							case 1: /*Always On */
								pAd->CommonCfg.UseBGProtection = 1;
								break;
							case 2: /*Always OFF */
								pAd->CommonCfg.UseBGProtection = 2;
								break;
							case 0: /*AUTO */
							default:
								pAd->CommonCfg.UseBGProtection = 0;
								break;
						}
/*#endif */
						DBGPRINT(RT_DEBUG_TRACE, ("BGProtection=%ld\n", pAd->CommonCfg.UseBGProtection));
					}
					/*OLBCDetection */
					if(RTMPGetKeyParameter("DisableOLBC", tmpbuf, 10, buffer, TRUE))
					{
						switch (simple_strtol(tmpbuf, 0, 10))
						{
							case 1: /*disable OLBC Detection */
								pAd->CommonCfg.DisableOLBCDetect = 1;
								break;
							case 0: /*enable OLBC Detection */
								pAd->CommonCfg.DisableOLBCDetect = 0;
								break;
							default:
								pAd->CommonCfg.DisableOLBCDetect= 0;
								break;
						}
						DBGPRINT(RT_DEBUG_TRACE, ("OLBCDetection=%ld\n", pAd->CommonCfg.DisableOLBCDetect));
					}
					/*TxPreamble */
					if(RTMPGetKeyParameter("TxPreamble", tmpbuf, 10, buffer, TRUE))
					{
						switch (simple_strtol(tmpbuf, 0, 10))
						{
							case Rt802_11PreambleShort:
								pAd->CommonCfg.TxPreamble = Rt802_11PreambleShort;
								break;
							case Rt802_11PreambleLong:
							default:
								pAd->CommonCfg.TxPreamble = Rt802_11PreambleLong;
								break;
						}
						DBGPRINT(RT_DEBUG_TRACE, ("TxPreamble=%ld\n", pAd->CommonCfg.TxPreamble));
					}
					/*RTSThreshold */
					if(RTMPGetKeyParameter("RTSThreshold", tmpbuf, 10, buffer, TRUE))
					{
						RtsThresh = simple_strtol(tmpbuf, 0, 10);
						if( (RtsThresh >= 1) && (RtsThresh <= MAX_RTS_THRESHOLD) )
							pAd->CommonCfg.RtsThreshold  = (USHORT)RtsThresh;
						else
							pAd->CommonCfg.RtsThreshold = MAX_RTS_THRESHOLD;
						
						DBGPRINT(RT_DEBUG_TRACE, ("RTSThreshold=%d\n", pAd->CommonCfg.RtsThreshold));
					}
					/*FragThreshold */
					if(RTMPGetKeyParameter("FragThreshold", tmpbuf, 10, buffer, TRUE))
					{		
						FragThresh = simple_strtol(tmpbuf, 0, 10);
						pAd->CommonCfg.bUseZeroToDisableFragment = FALSE;

						if (FragThresh > MAX_FRAG_THRESHOLD || FragThresh < MIN_FRAG_THRESHOLD)
						{ /*illegal FragThresh so we set it to default */
							pAd->CommonCfg.FragmentThreshold = MAX_FRAG_THRESHOLD;
							pAd->CommonCfg.bUseZeroToDisableFragment = TRUE;
						}
						else
							pAd->CommonCfg.FragmentThreshold = (USHORT)FragThresh;
						/*pAd->CommonCfg.AllowFragSize = (pAd->CommonCfg.FragmentThreshold) - LENGTH_802_11 - LENGTH_CRC; */
						DBGPRINT(RT_DEBUG_TRACE, ("FragThreshold=%d\n", pAd->CommonCfg.FragmentThreshold));
					}
					/*TxBurst */
					if(RTMPGetKeyParameter("TxBurst", tmpbuf, 10, buffer, TRUE))
					{
/*#ifdef WIFI_TEST */
/*						pAd->CommonCfg.bEnableTxBurst = FALSE; */
/*#else */
						if(simple_strtol(tmpbuf, 0, 10) != 0)  /*Enable */
							pAd->CommonCfg.bEnableTxBurst = TRUE;
						else /*Disable */
							pAd->CommonCfg.bEnableTxBurst = FALSE;
/*#endif */
						DBGPRINT(RT_DEBUG_TRACE, ("TxBurst=%d\n", pAd->CommonCfg.bEnableTxBurst));
					}

#ifdef AGGREGATION_SUPPORT
					/*PktAggregate */
					if(RTMPGetKeyParameter("PktAggregate", tmpbuf, 10, buffer, TRUE))
					{
#ifdef CONFIG_STA_SUPPORT
                        /* TODO: check ralink aggregation mechanism in STA.  Unsupport this mechanism in frist release. */
                        pAd->CommonCfg.bAggregationCapable = FALSE;
#endif /* CONFIG_STA_SUPPORT */
						if(simple_strtol(tmpbuf, 0, 10) != 0)  /*Enable */
							pAd->CommonCfg.bAggregationCapable = TRUE;
						else /*Disable */
							pAd->CommonCfg.bAggregationCapable = FALSE;
#ifdef PIGGYBACK_SUPPORT
						pAd->CommonCfg.bPiggyBackCapable = pAd->CommonCfg.bAggregationCapable;
#endif /* PIGGYBACK_SUPPORT */
						DBGPRINT(RT_DEBUG_TRACE, ("PktAggregate=%d\n", pAd->CommonCfg.bAggregationCapable));
					}
#else
					pAd->CommonCfg.bAggregationCapable = FALSE;
					pAd->CommonCfg.bPiggyBackCapable = FALSE;
#endif /* AGGREGATION_SUPPORT */

					/* WmmCapable */
					rtmp_read_wmm_parms_from_file(pAd, tmpbuf, buffer);

#ifdef CONFIG_AP_SUPPORT
					/*NoForwarding */
					if(RTMPGetKeyParameter("NoForwarding", tmpbuf, 32, buffer, TRUE))
					{
					    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
					    {
							if (i >= pAd->ApCfg.BssidNum)
								break;

    						if(simple_strtol(macptr, 0, 10) != 0)  /*Enable */
    							pAd->ApCfg.MBSSID[i].IsolateInterStaTraffic = TRUE;
    						else /*Disable */
    							pAd->ApCfg.MBSSID[i].IsolateInterStaTraffic = FALSE;

    						DBGPRINT(RT_DEBUG_TRACE, ("I/F(ra%d) NoForwarding=%ld\n", i, pAd->ApCfg.MBSSID[i].IsolateInterStaTraffic));
					    }
					}
					/*NoForwardingBTNBSSID */
					if(RTMPGetKeyParameter("NoForwardingBTNBSSID", tmpbuf, 10, buffer, TRUE))
					{
						if(simple_strtol(tmpbuf, 0, 10) != 0)  /*Enable */
							pAd->ApCfg.IsolateInterStaTrafficBTNBSSID = TRUE;
						else /*Disable */
							pAd->ApCfg.IsolateInterStaTrafficBTNBSSID = FALSE;

						DBGPRINT(RT_DEBUG_TRACE, ("NoForwardingBTNBSSID=%ld\n", pAd->ApCfg.IsolateInterStaTrafficBTNBSSID));
					}
					/*HideSSID */
					if(RTMPGetKeyParameter("HideSSID", tmpbuf, 32, buffer, TRUE))
					{
						for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
					    {
							int apidx = i;

							if (i >= pAd->ApCfg.BssidNum)
								break;

							if(simple_strtol(macptr, 0, 10) != 0)  /*Enable */
								pAd->ApCfg.MBSSID[apidx].bHideSsid = TRUE;								
							else /*Disable */
								pAd->ApCfg.MBSSID[apidx].bHideSsid = FALSE;								

							DBGPRINT(RT_DEBUG_TRACE, ("I/F(ra%d) HideSSID=%d\n", i, pAd->ApCfg.MBSSID[apidx].bHideSsid));
						}
					}
#endif /* CONFIG_AP_SUPPORT */					
					/*ShortSlot */
					if(RTMPGetKeyParameter("ShortSlot", tmpbuf, 10, buffer, TRUE))
					{
						if(simple_strtol(tmpbuf, 0, 10) != 0)  /*Enable */
							pAd->CommonCfg.bUseShortSlotTime = TRUE;
						else /*Disable */
							pAd->CommonCfg.bUseShortSlotTime = FALSE;

						DBGPRINT(RT_DEBUG_TRACE, ("ShortSlot=%d\n", pAd->CommonCfg.bUseShortSlotTime));
					}
#ifdef CONFIG_AP_SUPPORT					
					/*AutoChannelSelect */
					if(RTMPGetKeyParameter("AutoChannelSelect", tmpbuf, 10, buffer, TRUE))
					{
						if(simple_strtol(tmpbuf, 0, 10) != 0)  /*Enable */
							pAd->ApCfg.bAutoChannelAtBootup = TRUE;
						else
							pAd->ApCfg.bAutoChannelAtBootup = FALSE;

						DBGPRINT(RT_DEBUG_TRACE, ("AutoChannelAtBootup=%d\n", pAd->ApCfg.bAutoChannelAtBootup));
					}
					/*IEEE8021X */
					if(RTMPGetKeyParameter("IEEE8021X", tmpbuf, 10, buffer, TRUE))
					{
					    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
					    {
							if (i >= pAd->ApCfg.BssidNum)
								break;

    						if(simple_strtol(macptr, 0, 10) != 0)  /*Enable */
    							pAd->ApCfg.MBSSID[i].wdev.IEEE8021X = TRUE;
    						else /*Disable */
    							pAd->ApCfg.MBSSID[i].wdev.IEEE8021X = FALSE;

    						DBGPRINT(RT_DEBUG_TRACE, ("IEEE8021X=%d\n", pAd->ApCfg.MBSSID[i].wdev.IEEE8021X));
					    }
					}
#endif /* CONFIG_AP_SUPPORT */
					/*IEEE80211H */
					if(RTMPGetKeyParameter("IEEE80211H", tmpbuf, 10, buffer, TRUE))
					{
					    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
					    {
    						if(simple_strtol(macptr, 0, 10) != 0)  /*Enable */
    							pAd->CommonCfg.bIEEE80211H = TRUE;
    						else /*Disable */
    							pAd->CommonCfg.bIEEE80211H = FALSE;

    						DBGPRINT(RT_DEBUG_TRACE, ("IEEE80211H=%d\n", pAd->CommonCfg.bIEEE80211H));
					    }
					}
					/*CSPeriod */
					if(RTMPGetKeyParameter("CSPeriod", tmpbuf, 10, buffer, TRUE))
					{
					    if(simple_strtol(tmpbuf, 0, 10) != 0)
							pAd->CommonCfg.RadarDetect.CSPeriod = simple_strtol(tmpbuf, 0, 10);
						else
							pAd->CommonCfg.RadarDetect.CSPeriod = 0;

   						DBGPRINT(RT_DEBUG_TRACE, ("CSPeriod=%d\n", pAd->CommonCfg.RadarDetect.CSPeriod));
					}
					/*RDRegion */
					if(RTMPGetKeyParameter("RDRegion", tmpbuf, 128, buffer, TRUE))
					{
						if ((strncmp(tmpbuf, "JAP_W53", 7) == 0) || (strncmp(tmpbuf, "jap_w53", 7) == 0))
						{
							pAd->CommonCfg.RadarDetect.RDDurRegion = JAP_W53;
							pAd->CommonCfg.RadarDetect.DfsSessionTime = 15;
						}
						else if ((strncmp(tmpbuf, "JAP_W56", 7) == 0) || (strncmp(tmpbuf, "jap_w56", 7) == 0))
						{
							pAd->CommonCfg.RadarDetect.RDDurRegion = JAP_W56;
							pAd->CommonCfg.RadarDetect.DfsSessionTime = 5;
						}
						else if ((strncmp(tmpbuf, "JAP", 3) == 0) || (strncmp(tmpbuf, "jap", 3) == 0))
						{
							pAd->CommonCfg.RadarDetect.RDDurRegion = JAP;
							pAd->CommonCfg.RadarDetect.DfsSessionTime = 5;
						}
						else if ((strncmp(tmpbuf, "FCC", 3) == 0) || (strncmp(tmpbuf, "fcc", 3) == 0))
						{
							pAd->CommonCfg.RadarDetect.RDDurRegion = FCC;
							pAd->CommonCfg.RadarDetect.DfsSessionTime = 5;
						}
						else if ((strncmp(tmpbuf, "CE", 2) == 0) || (strncmp(tmpbuf, "ce", 2) == 0))
						{
							pAd->CommonCfg.RadarDetect.RDDurRegion = CE;
							pAd->CommonCfg.RadarDetect.DfsSessionTime = 13;
						}
						else
						{
							pAd->CommonCfg.RadarDetect.RDDurRegion = CE;
							pAd->CommonCfg.RadarDetect.DfsSessionTime = 13;
						}

						DBGPRINT(RT_DEBUG_TRACE, ("RDRegion=%d\n", pAd->CommonCfg.RadarDetect.RDDurRegion));
					}
					else
					{
						pAd->CommonCfg.RadarDetect.RDDurRegion = CE;
						pAd->CommonCfg.RadarDetect.DfsSessionTime = 13;
					}
					/*WirelessEvent */
					if(RTMPGetKeyParameter("WirelessEvent", tmpbuf, 10, buffer, TRUE))
					{				
#if WIRELESS_EXT >= 15
					    if(simple_strtol(tmpbuf, 0, 10) != 0)
							pAd->CommonCfg.bWirelessEvent = simple_strtol(tmpbuf, 0, 10);
						else
							pAd->CommonCfg.bWirelessEvent = 0;	/* disable */
#else
						pAd->CommonCfg.bWirelessEvent = 0;	/* disable */
#endif
   						DBGPRINT(RT_DEBUG_TRACE, ("WirelessEvent=%d\n", pAd->CommonCfg.bWirelessEvent));
					}
					if(RTMPGetKeyParameter("WiFiTest", tmpbuf, 10, buffer, TRUE))
					{				
					    if(simple_strtol(tmpbuf, 0, 10) != 0)
							pAd->CommonCfg.bWiFiTest= simple_strtol(tmpbuf, 0, 10);
						else
							pAd->CommonCfg.bWiFiTest = 0;	/* disable */

   						DBGPRINT(RT_DEBUG_TRACE, ("WiFiTest=%d\n", pAd->CommonCfg.bWiFiTest));
					}
#ifdef CONFIG_AP_SUPPORT					
					/*PreAuth */
					if(RTMPGetKeyParameter("PreAuth", tmpbuf, 10, buffer, TRUE))
					{
					    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
					    {
							if (i >= pAd->ApCfg.BssidNum)
								break;

    						if(simple_strtol(macptr, 0, 10) != 0)  /*Enable */
    							pAd->ApCfg.MBSSID[i].PreAuth = TRUE;
    						else /*Disable */
    							pAd->ApCfg.MBSSID[i].PreAuth = FALSE;

    						DBGPRINT(RT_DEBUG_TRACE, ("I/F(ra%d) PreAuth=%d\n", i, pAd->ApCfg.MBSSID[i].PreAuth));
					    }
					}
#endif /* CONFIG_AP_SUPPORT */					
					/*AuthMode */
					if(RTMPGetKeyParameter("AuthMode", tmpbuf, 128, buffer, TRUE))
					{
#ifdef CONFIG_AP_SUPPORT					
					    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
					    {
							int apidx;

							if (i < pAd->ApCfg.BssidNum)
							{							
								apidx = i;
							}
							else
							{
								break;
							}

							if ((strncmp(macptr, "WEPAUTO", 4) == 0) || (strncmp(macptr, "wepauto", 4) == 0))
					    		pAd->ApCfg.MBSSID[apidx].AuthMode = Ndis802_11AuthModeAutoSwitch;
    						else if ((strncmp(macptr, "OPEN", 4) == 0) || (strncmp(macptr, "open", 4) == 0))
    							pAd->ApCfg.MBSSID[apidx].AuthMode = Ndis802_11AuthModeOpen;
    						else if ((strncmp(macptr, "SHARED", 6) == 0) || (strncmp(macptr, "shared", 6) == 0))
    							pAd->ApCfg.MBSSID[apidx].AuthMode = Ndis802_11AuthModeShared;
    						else if ((strncmp(macptr, "WPA2PSK", 7) == 0) || (strncmp(macptr, "wpa2psk", 7) == 0))
    							pAd->ApCfg.MBSSID[apidx].AuthMode = Ndis802_11AuthModeWPA2PSK;
#ifdef WPA3_SUPPORT
							else if ((strncmp(macptr, "WPA3SAE", 7) == 0) || (strncmp(macptr, "wpa3sae", 7) == 0))
								pAd->ApCfg.MBSSID[apidx].AuthMode = Ndis802_11AuthModeWPA3SAE;
#endif
    						else if ((strncmp(macptr, "WPA2", 4) == 0) || (strncmp(macptr, "wpa2", 4) == 0))
    							pAd->ApCfg.MBSSID[apidx].AuthMode = Ndis802_11AuthModeWPA2;
    						else if ((strncmp(macptr, "WPA1WPA2", 8) == 0) || (strncmp(macptr, "wpa1wpa2", 8) == 0))
    							pAd->ApCfg.MBSSID[apidx].AuthMode = Ndis802_11AuthModeWPA1WPA2;
    						else if ((strncmp(macptr, "WPAPSKWPA2PSK", 13) == 0) || (strncmp(macptr, "wpapskwpa2psk", 13) == 0))
    							pAd->ApCfg.MBSSID[apidx].AuthMode = Ndis802_11AuthModeWPA1PSKWPA2PSK;
    						else if ((strncmp(macptr, "WPAPSK", 6) == 0) || (strncmp(macptr, "wpapsk", 6) == 0))
    							pAd->ApCfg.MBSSID[apidx].AuthMode = Ndis802_11AuthModeWPAPSK;
    						else if ((strncmp(macptr, "WPA", 3) == 0) || (strncmp(macptr, "wpa", 3) == 0))
    							pAd->ApCfg.MBSSID[apidx].AuthMode = Ndis802_11AuthModeWPA;
    						else /*Default */
    							pAd->ApCfg.MBSSID[apidx].AuthMode = Ndis802_11AuthModeOpen;

    						RTMPMakeRSNIE(pAd, pAd->ApCfg.MBSSID[apidx].AuthMode, pAd->ApCfg.MBSSID[apidx].WepStatus, apidx);
    						DBGPRINT(RT_DEBUG_TRACE, ("I/F(ra%d) AuthMode=%d\n", i, pAd->ApCfg.MBSSID[apidx].AuthMode));
					    }
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
						if ((strcmp(tmpbuf, "WEPAUTO") == 0) || (strcmp(tmpbuf, "wepauto") == 0))
                            pAd->StaCfg.AuthMode = Ndis802_11AuthModeAutoSwitch;
                        else if ((strcmp(tmpbuf, "SHARED") == 0) || (strcmp(tmpbuf, "shared") == 0))
                            pAd->StaCfg.AuthMode = Ndis802_11AuthModeShared;
                        else if ((strcmp(tmpbuf, "WPAPSK") == 0) || (strcmp(tmpbuf, "wpapsk") == 0))
                            pAd->StaCfg.AuthMode = Ndis802_11AuthModeWPAPSK;
                        else if ((strcmp(tmpbuf, "WPANONE") == 0) || (strcmp(tmpbuf, "wpanone") == 0))
                            pAd->StaCfg.AuthMode = Ndis802_11AuthModeWPANone;
                        else if ((strcmp(tmpbuf, "WPA2PSK") == 0) || (strcmp(tmpbuf, "wpa2psk") == 0))
							pAd->StaCfg.AuthMode = Ndis802_11AuthModeWPA2PSK;
#ifdef WPA3_SUPPORT
						else if ((strcmp(tmpbuf, "WPA3SAE") == 0) || (strcmp(tmpbuf, "wpa3sae") == 0))
							pAd->StaCfg.AuthMode = Ndis802_11AuthModeWPA3SAE;
#endif
                        else
                            pAd->StaCfg.AuthMode = Ndis802_11AuthModeOpen;

                        pAd->StaCfg.PortSecured = WPA_802_1X_PORT_NOT_SECURED;

						/*RTMPMakeRSNIE(pAd, pAd->StaCfg.AuthMode, pAd->StaCfg.WepStatus, 0); */
						DBGPRINT(RT_DEBUG_TRACE, ("%s::(EncrypType=%d)\n", __FUNCTION__, pAd->StaCfg.WepStatus));
#endif /* CONFIG_STA_SUPPORT */
					}
					/*EncrypType */
					if(RTMPGetKeyParameter("EncrypType", tmpbuf, 128, buffer, TRUE))
					{
#ifdef CONFIG_AP_SUPPORT 					
					    for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
					    {
							int apidx;

							if (i<pAd->ApCfg.BssidNum)
							{							
								apidx = i;
							}
					        else
							{
								break;
							}

							if ((strncmp(macptr, "NONE", 4) == 0) || (strncmp(macptr, "none", 4) == 0))
					            pAd->ApCfg.MBSSID[apidx].WepStatus = Ndis802_11WEPDisabled;
					        else if ((strncmp(macptr, "WEP", 3) == 0) || (strncmp(macptr, "wep", 3) == 0))
					            pAd->ApCfg.MBSSID[apidx].WepStatus = Ndis802_11WEPEnabled;
					        else if ((strncmp(macptr, "TKIPAES", 7) == 0) || (strncmp(macptr, "tkipaes", 7) == 0))
					            pAd->ApCfg.MBSSID[apidx].WepStatus = Ndis802_11TKIPAESMix;
					        else if ((strncmp(macptr, "TKIP", 4) == 0) || (strncmp(macptr, "tkip", 4) == 0))
					            pAd->ApCfg.MBSSID[apidx].WepStatus = Ndis802_11TKIPEnable;
					        else if ((strncmp(macptr, "AES", 3) == 0) || (strncmp(macptr, "aes", 3) == 0))
					            pAd->ApCfg.MBSSID[apidx].WepStatus = Ndis802_11AESEnable;
					        else
					            pAd->ApCfg.MBSSID[apidx].WepStatus = Ndis802_11WEPDisabled;

					        RTMPMakeRSNIE(pAd, pAd->ApCfg.MBSSID[apidx].AuthMode, pAd->ApCfg.MBSSID[apidx].WepStatus, apidx);
					        DBGPRINT(RT_DEBUG_TRACE, ("I/F(ra%d) EncrypType=%d\n", i, pAd->ApCfg.MBSSID[apidx].WepStatus));
					    }
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT 
						if ((strcmp(tmpbuf, "WEP") == 0) || (strcmp(tmpbuf, "wep") == 0))						
							pAd->StaCfg.WepStatus	= Ndis802_11WEPEnabled;													
						else if ((strcmp(tmpbuf, "TKIP") == 0) || (strcmp(tmpbuf, "tkip") == 0))						
							pAd->StaCfg.WepStatus	= Ndis802_11TKIPEnable;													
						else if ((strcmp(tmpbuf, "AES") == 0) || (strcmp(tmpbuf, "aes") == 0))						
							pAd->StaCfg.WepStatus	= Ndis802_11AESEnable;														 
						else						
							pAd->StaCfg.WepStatus	= Ndis802_11WEPDisabled;													

						/* Update all wepstatus related */
						pAd->StaCfg.PairCipher		= pAd->StaCfg.WepStatus;
						pAd->StaCfg.GroupCipher 	= pAd->StaCfg.WepStatus;
						pAd->StaCfg.OrigWepStatus 	= pAd->StaCfg.WepStatus; 
						pAd->StaCfg.bMixCipher 		= FALSE;	
						
						/*RTMPMakeRSNIE(pAd, pAd->StaCfg.AuthMode, pAd->StaCfg.WepStatus, 0); */
						DBGPRINT(RT_DEBUG_TRACE, ("%s::(EncrypType=%d)\n", __FUNCTION__, pAd->StaCfg.WepStatus));
#endif /* CONFIG_STA_SUPPORT */
					}

#ifdef CONFIG_AP_SUPPORT
					/*RekeyMethod */
					
					if(RTMPGetKeyParameter("RekeyMethod", tmpbuf, 128, buffer, TRUE))
					{
						if ((strcmp(tmpbuf, "TIME") == 0) || (strcmp(tmpbuf, "time") == 0))
							pAd->ApCfg.WPAREKEY.ReKeyMethod = TIME_REKEY;
						else if ((strcmp(tmpbuf, "PKT") == 0) || (strcmp(tmpbuf, "pkt") == 0))
							pAd->ApCfg.WPAREKEY.ReKeyMethod = PKT_REKEY;
						else if ((strcmp(tmpbuf, "DISABLE") == 0) || (strcmp(tmpbuf, "disable") == 0))
							pAd->ApCfg.WPAREKEY.ReKeyMethod = DISABLE_REKEY;
						else
							pAd->ApCfg.WPAREKEY.ReKeyMethod = DISABLE_REKEY;

						DBGPRINT(RT_DEBUG_TRACE, ("ReKeyMethod=%ld\n", pAd->ApCfg.WPAREKEY.ReKeyMethod));
					}
					/*RekeyInterval */
					if(RTMPGetKeyParameter("RekeyInterval", tmpbuf, 255, buffer, TRUE))
					{
						if((simple_strtol(tmpbuf, 0, 10) >= 0) && (simple_strtol(tmpbuf, 0, 10) < MAX_REKEY_INTER))
							pAd->ApCfg.WPAREKEY.ReKeyInterval = simple_strtol(tmpbuf, 0, 10);
						else /*Default */
							pAd->ApCfg.WPAREKEY.ReKeyInterval = 10;

						DBGPRINT(RT_DEBUG_TRACE, ("ReKeyInterval=%ld\n", pAd->ApCfg.WPAREKEY.ReKeyInterval));
					}
					
					/*PMKCachePeriod */
					if(RTMPGetKeyParameter("PMKCachePeriod", tmpbuf, 255, buffer, TRUE))
					{
						pAd->ApCfg.PMKCachePeriod = simple_strtol(tmpbuf, 0, 10) * 60 * OS_HZ;

						DBGPRINT(RT_DEBUG_TRACE, ("PMKCachePeriod=%ld\n", pAd->ApCfg.PMKCachePeriod));
					}
#endif /* CONFIG_AP_SUPPORT */
					/*WPAPSK_KEY */
					if(RTMPGetKeyParameter("WPAPSK", tmpbuf, 512, buffer, TRUE))
					{
#ifdef CONFIG_AP_SUPPORT
						for (i = 0, macptr = rstrtok(tmpbuf,";"); macptr; macptr = rstrtok(NULL,";"), i++)
						{
							int j;
							DBGPRINT(RT_DEBUG_TRACE, ("I/F(ra%d) WPAPSK_KEY=%s\n", i, macptr));
							if (strlen(macptr) == 64)
							{/* Hex mode */
								AtoH(macptr, pAd->ApCfg.MBSSID[i].PMK, 32);
							}
							else if ((strlen(macptr) >= 8) && (strlen(macptr) < 64))
							{/* ASCII mode */
								RtmpPasswordHash((char *)macptr, pAd->ApCfg.MBSSID[i].Ssid, pAd->ApCfg.MBSSID[i].SsidLen, keyMaterial);
								NdisMoveMemory(pAd->ApCfg.MBSSID[i].PMK, keyMaterial, 32);
							}
							else
							DBGPRINT(RT_DEBUG_ERROR, ("WPAPSK_KEY, key len incorrect!!!\n"));

#ifdef DBG
							DBGPRINT(RT_DEBUG_TRACE, ("I/F(ra%d) WPAPSK Key => \n", i));
							for (j = 0; j < 32; j++)
							{
								printk("%02x:", pAd->ApCfg.MBSSID[i].PMK[j]);
								if ((j%16) == 15)
									printk("\n");
							}
							printk("\n");
#endif
#ifdef WSC_AP_SUPPORT
							NdisZeroMemory(pAd->ApCfg.MBSSID[i].WscControl.WpaPsk, 64);
							pAd->ApCfg.MBSSID[i].WscControl.WpaPskLen = 0;
							if ((strlen(macptr) >= 8) && (strlen(macptr) <= 64))
							{                                    
								NdisMoveMemory(pAd->ApCfg.MBSSID[i].WscControl.WpaPsk, macptr, strlen(macptr));
								pAd->ApCfg.MBSSID[i].WscControl.WpaPskLen = strlen(macptr);
							}
#endif /* WSC_AP_SUPPORT */
						}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
						int     err=0;
						tmpbuf[strlen(tmpbuf)] = '\0'; /* make STA can process .$^& for WPAPSK input */

						if ((pAd->StaCfg.AuthMode != Ndis802_11AuthModeWPAPSK) &&
							(pAd->StaCfg.AuthMode != Ndis802_11AuthModeWPA2PSK) &&
#ifdef WPA3_SUPPORT
							&& (pAd->StaCfg.AuthMode != Ndis802_11AuthModeWPA3SAE)
#endif
							(pAd->StaCfg.AuthMode != Ndis802_11AuthModeWPANone) )
						{
							err = 1;
						}
						else if ((strlen(tmpbuf) >= 8) && (strlen(tmpbuf) < 64))
						{
							RtmpPasswordHash((char *)tmpbuf, pAd->CommonCfg.Ssid, pAd->CommonCfg.SsidLen, keyMaterial);
							NdisMoveMemory(pAd->StaCfg.PMK, keyMaterial, 32);
						}
						else if (strlen(tmpbuf) == 64)
						{                            
							AtoH(tmpbuf, keyMaterial, 32);
							NdisMoveMemory(pAd->StaCfg.PMK, keyMaterial, 32);
						}
						else
						{   
							err = 1;
							DBGPRINT(RT_DEBUG_ERROR, ("%s::(WPAPSK key-string required 8 ~ 64 characters!)\n", __FUNCTION__));
						}

						if (err == 0)
						{                                                          
							if ((pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPAPSK) ||
								(pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA2PSK)
#ifdef WPA3_SUPPORT
								|| (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA3SAE)
#endif
								) {
								/* Start STA supplicant state machine */
								pAd->StaCfg.WpaState = SS_START;
							}
							else if (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPANone)
							{
							/*							
								NdisZeroMemory(&pAd->SharedKey[BSS0][0], sizeof(CIPHER_KEY));  
								pAd->SharedKey[BSS0][0].KeyLen = LEN_TK;
								NdisMoveMemory(pAd->SharedKey[BSS0][0].Key, pAd->StaCfg.PMK, LEN_TK);
								NdisMoveMemory(pAd->SharedKey[BSS0][0].RxMic, &pAd->StaCfg.PMK[16], LEN_TKIP_MIC);
								NdisMoveMemory(pAd->SharedKey[BSS0][0].TxMic, &pAd->StaCfg.PMK[16], LEN_TKIP_MIC);            

								if (pAd->StaCfg.PairCipher == Ndis802_11TKIPEnable)
									pAd->SharedKey[BSS0][0].CipherAlg = CIPHER_TKIP;
								else if (pAd->StaCfg.PairCipher == Ndis802_11AESEnable)
									pAd->SharedKey[BSS0][0].CipherAlg = CIPHER_AES;
								else
									pAd->SharedKey[BSS0][0].CipherAlg = CIPHER_NONE;
						*/
								pAd->StaCfg.WpaState = SS_NOTUSE;
							}
							DBGPRINT(RT_DEBUG_TRACE, ("%s::(WPAPSK=%s)\n", __FUNCTION__, tmpbuf));
						}
#endif /* CONFIG_STA_SUPPORT */
					}
    							
					/*DefaultKeyID, KeyType, KeyStr */
					rtmp_read_key_parms_from_file(pAd, tmpbuf, buffer);
    							
					/*HSCounter */
					/*if(RTMPGetKeyParameter("HSCounter", tmpbuf, 10, buffer, TRUE))
					{
						switch (simple_strtol(tmpbuf, 0, 10))
						{
							case 1: 
								pAd->CommonCfg.bEnableHSCounter = TRUE;
								break;
							case 0:
							default:
								pAd->CommonCfg.bEnableHSCounter = FALSE;
								break;
						}
						DBGPRINT(RT_DEBUG_TRACE, "HSCounter=%d\n", pAd->CommonCfg.bEnableHSCounter);
					}*/
#ifdef CONFIG_AP_SUPPORT
					/*Access Control List */
					rtmp_read_acl_parms_from_file(pAd, tmpbuf, buffer);

#ifdef APCLI_SUPPORT					
					rtmp_read_ap_client_from_file(pAd, tmpbuf, buffer);
#endif /* APCLI_SUPPORT */
					
#ifdef WDS_SUPPORT								
					rtmp_read_wds_from_file(pAd, tmpbuf, buffer);
#endif /* WDS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef DOT11_N_SUPPORT
					HTParametersHook(pAd, tmpbuf, buffer);
#endif /* DOT11_N_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
#ifdef WSC_AP_SUPPORT
					/*WscConfMode */
					if(RTMPGetKeyParameter("WscConfMode", tmpbuf, 10, buffer, TRUE))
					{
						INT WscConfMode = simple_strtol(tmpbuf, 0, 10);

						if (WscConfMode >= 0 && WscConfMode < 8)
						{
							pAd->ApCfg.MBSSID[MAIN_MBSSID].WscControl.WscConfMode = WscConfMode;
						}
						else
						{
							pAd->ApCfg.MBSSID[MAIN_MBSSID].WscControl.WscConfMode = WSC_DISABLE;
						}

						DBGPRINT(RT_DEBUG_TRACE, ("WscConfMode=%d\n", pAd->ApCfg.MBSSID[MAIN_MBSSID].WscControl.WscConfMode));
					}

					/*WscConfStatus */
					if(RTMPGetKeyParameter("WscConfStatus", tmpbuf, 10, buffer, TRUE))
					{
						pAd->ApCfg.MBSSID[MAIN_MBSSID].WscControl.WscConfStatus = (INT) simple_strtol(tmpbuf, 0, 10);

						DBGPRINT(RT_DEBUG_TRACE, ("WscConfStatus=%d\n", pAd->ApCfg.MBSSID[MAIN_MBSSID].WscControl.WscConfStatus));
					}

					/*WscUseUPnP */
					if(RTMPGetKeyParameter("WscUseUpnp", tmpbuf, 10, buffer, TRUE))
					{
						pAd->ApCfg.MBSSID[MAIN_MBSSID].WscControl.WscUseUPnP = (INT) simple_strtol(tmpbuf, 0, 10);

						DBGPRINT(RT_DEBUG_TRACE, ("WscUseUpnp=%d\n", pAd->ApCfg.MBSSID[MAIN_MBSSID].WscControl.WscUseUPnP));
					}
#endif /* WSC_AP_SUPPORT */

#ifdef CARRIER_DETECTION_SUPPORT
					/*CarrierDetect */
					if(RTMPGetKeyParameter("CarrierDetect", tmpbuf, 128, buffer, TRUE))
					{
						if ((strncmp(tmpbuf, "0", 1) == 0))
							pAd->CommonCfg.CarrierDetect.Enable = FALSE;
						else if ((strncmp(tmpbuf, "1", 1) == 0))
							pAd->CommonCfg.CarrierDetect.Enable = TRUE;
						else
							pAd->CommonCfg.CarrierDetect.Enable = FALSE;

						DBGPRINT(RT_DEBUG_TRACE, ("CarrierDetect.Enable=%d\n", pAd->CommonCfg.CarrierDetect.Enable));
					}
#endif /* CARRIER_DETECTION_SUPPORT */

#ifdef MCAST_RATE_SPECIFIC
					/* McastPhyMode */
					if (RTMPGetKeyParameter("McastPhyMode", tmpbuf, 32, buffer, TRUE))
					{	
						UCHAR PhyMode = simple_strtol(tmpbuf, 0, 10);
						pAd->CommonCfg.MCastPhyMode.field.BW = pAd->CommonCfg.RegTransmitSetting.field.BW;
						switch (PhyMode)
						{
							case MCAST_DISABLE: /* disable */
								NdisMoveMemory(&pAd->CommonCfg.MCastPhyMode, &pAd->MacTab.Content[MCAST_WCID]..HTPhyMode, sizeof(HTTRANSMIT_SETTING));
								break;

							case MCAST_CCK:	/* CCK */
								pAd->CommonCfg.MCastPhyMode.field.MODE = MODE_CCK;
								pAd->CommonCfg.MCastPhyMode.field.BW =  BW_20;
								break;

							case MCAST_OFDM:	/* OFDM */
								pAd->CommonCfg.MCastPhyMode.field.MODE = MODE_OFDM;
								break;

							case MCAST_HTMIX:	/* HTMIX */
								pAd->CommonCfg.MCastPhyMode.field.MODE = MODE_HTMIX;
								break;

							default:
								printk("unknow Muticast PhyMode %d.\n", PhyMode);
								printk("0:Disable 1:CCK, 2:OFDM, 3:HTMIX.\n");
								break;
						}
					}
					else
						NdisMoveMemory(&pAd->CommonCfg.MCastPhyMode,
							&pAd->MacTab.Content[MCAST_WCID].HTPhyMode, sizeof(HTTRANSMIT_SETTING));

					/* McastMcs */
					if (RTMPGetKeyParameter("McastMcs", tmpbuf, 32, buffer, TRUE))
					{
						UCHAR Mcs = simple_strtol(tmpbuf, 0, 10);
						switch(pAd->CommonCfg.MCastPhyMode.field.MODE)
						{
							case MODE_CCK:
								if ((Mcs <= 3) || (Mcs >= 8 && Mcs <= 11))
									pAd->CommonCfg.MCastPhyMode.field.MCS = Mcs;
						else
									printk("MCS must in range of 0 ~ 3 and 8 ~ 11 for CCK Mode.\n");
								break;

							case MODE_OFDM:
								if (Mcs > 7)
									printk("MCS must in range from 0 to 7 for CCK Mode.\n");
								else
									pAd->CommonCfg.MCastPhyMode.field.MCS = Mcs;
								break;

							default:
								pAd->CommonCfg.MCastPhyMode.field.MCS = Mcs;
								break;
						}
					}
					else
						pAd->CommonCfg.MCastPhyMode.field.MCS = 0;
#endif /* MCAST_RATE_SPECIFIC */

#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
                    /*PSMode */
					if (RTMPGetKeyParameter("PSMode", tmpbuf, 10, buffer, TRUE))
					{
						if (pAd->StaCfg.BssType == BSS_INFRA)
						{
							if ((strcmp(tmpbuf, "MAX_PSP") == 0) || (strcmp(tmpbuf, "max_psp") == 0))
							{
								/* do NOT turn on PSM bit here, wait until MlmeCheckForPsmChange() */
								/* to exclude certain situations. */
								/*	   MlmeSetPsm(pAd, PWR_SAVE); */
								OPSTATUS_SET_FLAG(pAd, fOP_STATUS_RECEIVE_DTIM);
								if (pAd->StaCfg.bWindowsACCAMEnable == FALSE)
									pAd->StaCfg.WindowsPowerMode = Ndis802_11PowerModeFast_PSP;
								pAd->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeFast_PSP;
								pAd->StaCfg.DefaultListenCount = 3;
							}							
							else if ((strcmp(tmpbuf, "Fast_PSP") == 0) || (strcmp(tmpbuf, "fast_psp") == 0) 
								|| (strcmp(tmpbuf, "FAST_PSP") == 0))
							{
								/* do NOT turn on PSM bit here, wait until MlmeCheckForPsmChange() */
								/* to exclude certain situations. */
								/*	   MlmeSetPsmBit(pAd, PWR_SAVE); */
								OPSTATUS_SET_FLAG(pAd, fOP_STATUS_RECEIVE_DTIM);
								if (pAd->StaCfg.bWindowsACCAMEnable == FALSE)
									pAd->StaCfg.WindowsPowerMode = Ndis802_11PowerModeFast_PSP;
								pAd->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeFast_PSP;
								pAd->StaCfg.DefaultListenCount = 3;
							}
							else
							{ /*Default Ndis802_11PowerModeCAM */
								/* clear PSM bit immediately */
								MlmeSetPsmBit(pAd, PWR_ACTIVE);
								OPSTATUS_SET_FLAG(pAd, fOP_STATUS_RECEIVE_DTIM);
								if (pAd->StaCfg.bWindowsACCAMEnable == FALSE)
									pAd->StaCfg.WindowsPowerMode = Ndis802_11PowerModeCAM;
								pAd->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeCAM;
							}
							DBGPRINT(RT_DEBUG_TRACE, ("PSMode=%ld\n", pAd->StaCfg.WindowsPowerMode));
						}
					}
                    /* FastRoaming */
					if (RTMPGetKeyParameter("FastRoaming", tmpbuf, 32, buffer, TRUE))
					{
					    if (simple_strtol(tmpbuf, 0, 10) == 0)
					        pAd->StaCfg.bFastRoaming = FALSE;
                        else
                            pAd->StaCfg.bFastRoaming = TRUE;

                        DBGPRINT(RT_DEBUG_TRACE, ("FastRoaming=%d\n", pAd->StaCfg.bFastRoaming));
					}
					/* RoamThreshold */
					if (RTMPGetKeyParameter("RoamThreshold", tmpbuf, 32, buffer, TRUE))
					{
                        long lInfo = simple_strtol(tmpbuf, 0, 10);
                            
                        if (lInfo > 90 || lInfo < 60)
                            pAd->StaCfg.dBmToRoam = -70;
                        else    
                            pAd->StaCfg.dBmToRoam = (CHAR)(-1)*lInfo;

                        DBGPRINT(RT_DEBUG_TRACE, ("RoamThreshold=%d  dBm\n", pAd->StaCfg.dBmToRoam));
					}
#endif /* CONFIG_STA_SUPPORT */

#ifdef WSC_INCLUDED
    if (RTMPGetKeyParameter("WscVendorPinCode", tmpbuf, 256, buffer, TRUE))
    {
            int value = simple_strtol(tmpbuf, 0, 10);
            if ( ValidateChecksum(value) )
            {
        
#ifdef CONFIG_AP_SUPPORT
        pAd->ApCfg.MBSSID[BSS0].WscControl.WscEnrolleePinCode = value;
#endif
#ifdef CONFIG_STA_SUPPORT
        pAd->StaCfg.WscControl.WscEnrolleePinCode = value;
#endif
                DBGPRINT(RT_DEBUG_TRACE, ("%s - WscVendorPinCode= (%d)\n", __FUNCTION__, value));
            }
            else
            {
                DBGPRINT(RT_DEBUG_ERROR, ("%s - WscVendorPinCode: invalid pin code (%d)\n", __FUNCTION__, value));
            }
    }
#endif

#ifdef MESH_SUPPORT
	rtmp_read_mesh_from_file(pAd, tmpbuf, buffer);
#endif /* MESH_SUPPORT */
	kfree(tmpbuf);

	return (NDIS_STATUS_SUCCESS);	
}


#if 0
	/*Init Ba CApability parameters. */
	pAd->CommonCfg.DesiredHtPhy.MpduDensity = (UCHAR)pAd->CommonCfg.BACapability.field.MpduDensity;
	pAd->CommonCfg.DesiredHtPhy.AmsduEnable = (USHORT)pAd->CommonCfg.BACapability.field.AmsduEnable;
	pAd->CommonCfg.DesiredHtPhy.AmsduSize= (USHORT)pAd->CommonCfg.BACapability.field.AmsduSize;
	pAd->CommonCfg.DesiredHtPhy.MimoPs= (USHORT)pAd->CommonCfg.BACapability.field.MMPSmode;
	/* UPdata to HT IE */
	pAd->CommonCfg.HtCapability.HtCapInfo.MimoPs = (USHORT)pAd->CommonCfg.BACapability.field.MMPSmode;
	pAd->CommonCfg.HtCapability.HtCapInfo.AMsduSize = (USHORT)pAd->CommonCfg.BACapability.field.AmsduSize;
	pAd->CommonCfg.HtCapability.HtCapParm.MpduDensity = (UCHAR)pAd->CommonCfg.BACapability.field.MpduDensity;

   	pAd->CommonCfg.BACapability.field.MpduDensity = 0;
	pAd->CommonCfg.BACapability.field.Policy = IMMED_BA;
	pAd->CommonCfg.BACapability.field.RxBAWinLimit = 32;
	pAd->CommonCfg.BACapability.field.TxBAWinLimit = 32;
	pAd->CommonCfg.HTPhyMode.field.BW = BW_c40;
	pAd->CommonCfg.HTPhyMode.field.MCS = MCS_15;
	pAd->CommonCfg.HTPhyMode.field.ShortGI = GI_400;
	pAd->CommonCfg.HTPhyMode.field.STBC = STBC_NONE;
	pAd->CommonCfg.TxRate = RATE_6;

#endif

