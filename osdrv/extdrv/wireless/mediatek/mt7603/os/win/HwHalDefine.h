#ifndef _HW_HAL_DEFINE_H_
#define _HW_HAL_DEFINE_H_


//--------------------------------------------------------------
// --> EEPROM value type definition
//--------------------------------------------------------------
// -->  EEPROM data Define

#define	NUM_EEPROM_TX_PARMS			7
//#define	NUM_EEPROM_BBP_TUNING_PARMS	7
#define	EEPROM_MAC_ADDRESS_BASE_OFFSET		0x4
//#define	EEPROM_BBP_BASE_OFFSET		0x20
#define	EEPROM_TSSI_REF_OFFSET		0x4A
#define	EEPROM_TSSI_DELTA_OFFSET	0x1A
//#define	EEPROM_BBP_TUNING_OFFSET	0x60
#define	EEPROM_MAC_STATUS_OFFSET	0x7E

#define	EEPROM_FREQENCY_OFFSET		0x64

#define	EEPROM_RF_PROGRAM_SEQUENCE 0x5F

#define	EEPROM_TX_PWR_OFFSET_11g	0x46
#define	EEPROM_TX_PWR_OFFSET_11a	0x62

__inline    VOID    RTMPusecDelay(
    IN      ULONG   usec);

//USB
//-------------------------------------------------------
// --> Usb Reguest Check Value Define

#define DEVICE_CLASS_REQUEST_OUT       0x20
#define DEVICE_VENDOR_REQUEST_OUT       0x40
#define DEVICE_VENDOR_REQUEST_IN        0xc0
#define INTERFACE_VENDOR_REQUEST_OUT    0x41
#define INTERFACE_VENDOR_REQUEST_IN     0xc1

#endif
