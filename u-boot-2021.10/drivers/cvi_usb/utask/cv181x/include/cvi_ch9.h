#ifndef CVI_CH9_H
#define CVI_CH9_H

#include "cvi_stdtypes.h"
#include "linux/bitops.h"

/** @defgroup ConfigInfo  Configuration and Hardware Operation Information
 *  The following definitions specify the driver operation environment that
 *  is defined by hardware configuration or client code. These defines are
 *  located in the header file of the core driver.
 *  @{
 */

/**********************************************************************
 * Defines
 **********************************************************************
 */

/** Data transfer direction */
#define	CH9_USB_DIR_HOST_TO_DEVICE 0

#define	CH9_USB_DIR_DEVICE_TO_HOST BIT(7)

/** Type of request */
#define	CH9_USB_REQ_TYPE_MASK (3 << 5)

#define	CH9_USB_REQ_TYPE_STANDARD (0 << 5)

#define	CH9_USB_REQ_TYPE_CLASS BIT(5)

#define	CH9_USB_REQ_TYPE_VENDOR (2 << 5)

#define	CH9_USB_REQ_TYPE_OTHER (3 << 5)

/** Recipient of request */
#define	CH9_REQ_RECIPIENT_MASK 0x0f

#define	CH9_USB_REQ_RECIPIENT_DEVICE 0

#define	CH9_USB_REQ_RECIPIENT_INTERFACE 1

#define	CH9_USB_REQ_RECIPIENT_ENDPOINT 2

#define	CH9_USB_REQ_RECIPIENT_OTHER 3

/** Standard  Request Code (chapter 9.4, Table 9-5 of USB Spec.) */
#define	CH9_USB_REQ_GET_STATUS 0

#define	CH9_USB_REQ_CLEAR_FEATURE 1

#define	CH9_USB_REQ_SET_FEATURE 3

#define	CH9_USB_REQ_SET_ADDRESS 5

#define	CH9_USB_REQ_GET_DESCRIPTOR 6

#define	CH9_USB_REQ_SET_DESCRIPTOR 7

#define	CH9_USB_REQ_GET_CONFIGURATION 8

#define	CH9_USB_REQ_SET_CONFIGURATION 9

#define	CH9_USB_REQ_GET_INTERFACE 10

#define	CH9_USB_REQ_SET_INTERFACE 11

#define	CH9_USB_REQ_SYNCH_FRAME 12

#define	CH9_USB_REQ_SET_ENCRYPTION 13

#define	CH9_USB_REQ_GET_ENCRYPTION 14

#define	CH9_USB_REQ_SET_HANDSHAKE 15

#define	CH9_USB_REQ_GET_HANDSHAKE 16

#define	CH9_USB_REQ_SET_CONNECTION 17

#define	CH9_USB_REQ_SET_SCURITY_DATA 18

#define	CH9_USB_REQ_GET_SCURITY_DATA 19

#define	CH9_USB_REQ_SET_WUSB_DATA 20

#define	CH9_USB_REQ_LOOPBACK_DATA_WRITE 21

#define	CH9_USB_REQ_LOOPBACK_DATA_READ 22

#define	CH9_USB_REQ_SET_INTERFACE_DS 23

#define CH9_USB_REQ_CVI_ENTER_DL 66

#define	CH9_USB_REQ_SET_SEL 48

#define	CH9_USB_REQ_ISOCH_DELAY 49

/** Standard Descriptor Types (chapter 9.4 - Table 9-6 of USB Spec.) */
#define	CH9_USB_DT_DEVICE 1

#define	CH9_USB_DT_CONFIGURATION 2

#define	CH9_USB_DT_STRING 3

#define	CH9_USB_DT_INTERFACE 4

#define	CH9_USB_DT_ENDPOINT 5

#define	CH9_USB_DT_DEVICE_QUALIFIER 6

/** USB 2 */
#define	CH9_USB_DT_OTHER_SPEED_CONFIGURATION 7

/** USB 2 */
#define	CH9_USB_DT_INTERFACE_POWER 8

#define	CH9_USB_DT_OTG 9

#define	CH9_USB_DT_DEBUG 10

#define	CH9_USB_DT_INTERFACE_ASSOCIATION 11

#define	CH9_USB_DT_BOS 15

#define	CH9_USB_DT_DEVICE_CAPABILITY 16

#define	CH9_USB_DT_SS_USB_ENDPOINT_COMPANION 48

#define	CH9_USB_DT_SS_PLUS_ISOCHRONOUS_ENDPOINT_COMPANION 49

#define	CH9_USB_DT_OTG 9

/** Descriptor size */
#define	CH9_USB_DS_DEVICE 18

#define	CH9_USB_DS_BOS 5

#define	CH9_USB_DS_DEVICE_ACM 12

/** Capability type: USB 2.0 EXTENSION */
#define	CH9_USB_DS_DEVICE_CAPABILITY_20 7

/** Capability type: SUPERSPEED_USB */
#define	CH9_USB_DS_DEVICE_CAPABILITY_30 10

/** Capability type: CONTAINER_ID */
#define	CH9_USB_DS_DEVICE_CAPABILITY_CONTAINER_ID 21

/** Capability type: Capability type: PRECISION_TIME_MEASUREMENT */
#define	CH9_USB_DS_DEVICE_CAPABILITY_PRECISION_TIME_MEASUREMENT 4

#define	CH9_USB_DS_CONFIGURATION 9

#define	CH9_USB_DS_INTERFACE_ASSOCIATION 8

#define	CH9_USB_DS_SS_USB_ENDPOINT_COMPANION 6

#define	CH9_USB_DS_SS_PLUS_ISOCHRONOUS_ENDPOINT_COMPANION 8

#define	CH9_USB_DS_INTERFACE 9

#define	CH9_USB_DS_ENDPOINT 7

#define	CH9_USB_DS_STRING 3

#define	CH9_USB_DS_OTG 5

/** USB2 */
#define	CH9_USB_DS_DEVICE_QUALIFIER 10

/** USB2 */
#define	CH9_USB_DS_OTHER_SPEED_CONFIGURATION 7

#define	CH9_USB_DS_INTERFACE_POWER 8

/** Standard Feature Selectors (chapter 9.4, Table 9-7 of USB Spec) */
#define	CH9_USB_FS_ENDPOINT_HALT 0

#define	CH9_USB_FS_FUNCTION_SUSPEND 0

#define	CH9_USB_FS_DEVICE_REMOTE_WAKEUP 1

#define	CH9_USB_FS_TEST_MODE 2

#define	CH9_USB_FS_B_HNP_ENABLE 3

#define	CH9_USB_FS_A_HNP_SUPPORT 4

#define	CH9_USB_FS_A_ALT_HNP_SUPPORT 5

#define	CH9_USB_FS_WUSB_DEVICE 6

#define	CH9_USB_FS_U1_ENABLE 48

#define	CH9_USB_FS_U2_ENABLE 49

#define	CH9_USB_FS_LTM_ENABLE 50

#define	CH9_USB_FS_B3_NTF_HOST_REL 51

#define	CH9_USB_FS_B3_RESP_ENABLE 52

#define	CH9_USB_FS_LDM_ENABLE 53

/** Recipient Device (Figure 9-4 of USB Spec) */
#define	CH9_USB_STATUS_DEV_SELF_POWERED BIT(0)

#define	CH9_USB_STATUS_DEV_REMOTE_WAKEUP BIT(1)

#define	CH9_USB_STATUS_DEV_U1_ENABLE BIT(2)

#define	CH9_USB_STATUS_DEV_U2_ENABLE BIT(3)

#define	CH9_USB_STATUS_DEV_LTM_ENABLE BIT(4)

/** Recipient Interface (Figure 9-5 of USB Spec) */
#define	CH9_USB_STATUS_INT_REMOTE_WAKE_CAPABLE BIT(0)

#define	CH9_USB_STATUS_INT_REMOTE_WAKEUP BIT(1)

/** Recipient Endpoint (Figure 9-6 of USB Spec) */
#define	CH9_USB_STATUS_EP_HALT BIT(1)

/** Recipient Endpoint - PTM GetStatus Request(Figure 9-7 of USB Spec) */
#define	CH9_USB_STATUS_EP_PTM_ENABLE BIT(0)

#define	CH9_USB_STATUS_EP_PTM_VALID BIT(1)

#define	CH9_USB_STATUS_EP_PTM_LINK_DELAY_OFFSET (16)

#define	CH9_USB_STATUS_EP_PTM_LINK_DELAY_MASK (0xFFFF << 16)

/**
 * Macros describing information for SetFeauture Request and FUNCTION_SUSPEND selector
 * (chapter 9.4.9, Table 9-9 of USB Spec)
 */
#define	CH9_USB_SF_LOW_POWER_SUSPEND_STATE 0x1

#define	CH9_USB_SF_REMOTE_WAKE_ENABLED 0x2

/**
 * Standard Class Code defined by usb.org
 * (link: http://www.usb.org/developers/defined_class)
 */
#define	CH9_USB_CLASS_INTERFACE 0x0

#define	CH9_USB_CLASS_AUDIO 0x01

#define	CH9_USB_CLASS_CDC 0x02

#define	CH9_USB_CLASS_COMMUNICATION 0x01

#define	CH9_USB_CLASS_HID 0x03

#define	CH9_USB_CLASS_PHYSICAL 0x05

#define	CH9_USB_CLASS_IMAGE 0x06

#define	CH9_USB_CLASS_PRINTER 0x07

#define	CH9_USB_CLASS_MASS_STORAGE 0x08

#define	CH9_USB_CLASS_HUB 0x09

#define	CH9_USB_CLASS_CDC_DATA 0x0A

#define	CH9_USB_CLASS_SMART_CARD 0x0B

#define	CH9_USB_CLASS_CONTENT_SEECURITY 0x0D

#define	CH9_USB_CLASS_VIDEO 0x0E

#define	CH9_USB_CLASS_HEALTHCARE 0x0F

#define	CH9_USB_CLASS_AUDIO_VIDEO 0x10

#define	CH9_USB_CLASS_DIAGNOSTIC 0xDC

#define	CH9_USB_CLASS_WIRELESS 0xE0

#define	CH9_USB_CLASS_MISCELLANEOUS 0xEF

#define	CH9_USB_CLASS_APPLICATION 0xFE

#define	CH9_USB_CLASS_VENDOR 0xFF

/** Device Capability Types Codes (see Table 9-14 of USB Spec 3.1 */
#define	CH9_USB_DCT_WIRELESS_USB 0x01

#define	CH9_USB_DCT_USB20_EXTENSION 0x02

#define	CH9_USB_DCT_SS_USB 0x03

#define	CH9_USB_DCT_CONTAINER_ID 0x04

#define	CH9_USB_DCT_PLATFORM 0x05

#define	CH9_USB_DCT_POWER_DELIVERY_CAPABILITY 0x06

#define	CH9_USB_DCT_BATTERY_INFO_CAPABILITY 0x07

#define	CH9_USB_DCT_PD_CONSUMER_PORT_CAPABILITY 0x08

#define	CH9_USB_DCT_PD_PROVIDER_PORT_CAPABILITY 0x09

#define	CH9_USB_DCT_SS_PLUS 0x0A

#define	CH9_USB_DCT_PRECISION_TIME_MEASUREMENT 0x0B

#define	CH9_USB_DCT_WIRELESS_USB_EXT 0x0C

/** Describe supports LPM defined in bmAttribues field of CUSBD_Usb20ExtensionDescriptor */
#define	CH9_USB_USB20_EXT_LPM_SUPPORT BIT(1)

#define	CH9_USB_USB20_EXT_BESL_AND_ALTERNATE_HIRD BIT(2)

/**
 * Describe supports LTM defined in bmAttribues field
 * of CUSBD_UsbSuperSpeedDeviceCapabilityDescriptor
 */
#define	CH9_USB_SS_CAP_LTM BIT(1)

/**
 * Describe speed supported defined in wSpeedSupported field
 * of CUSBD_UsbSuperSpeedDeviceCapabilityDescriptor
 */
#define	CH9_USB_SS_CAP_SUPPORT_LS BIT(0)

#define	CH9_USB_SS_CAP_SUPPORT_FS BIT(1)

#define	CH9_USB_SS_CAP_SUPPORT_HS BIT(2)

#define	CH9_USB_SS_CAP_SUPPORT_SS BIT(3)

/** Describe encoding of bmSublinkSpeedAttr0 filed from CUSBD_UsbSuperSpeedPlusDescriptor */
#define	CH9_USB_SSP_SID_OFFSET 0

#define	CH9_USB_SSP_SID_MASK 0 0x0000000f

#define	CH9_USB_SSP_LSE_OFFSET 4

#define	CH9_USB_SSP_LSE_MASK (0x00000003 << CUSBD_USB_SSP_LSE_OFFSET)

#define	CH9_USB_SSP_ST_OFFSET 6

#define	CH9_USB_SSP_ST_MASK (0x00000003 << CUSBD_USB_SSP_ST_OFFSET)

#define	CH9_USB_SSP_LP_OFFSET 14

#define	CH9_USB_SSP_LP_MASK (0x00000003 << CUSBD_USB_SSP_LP_OFFSET)

#define	CH9_USB_SSP_LSM_OFFSET 16

#define	CH9_USB_SSP_LSM_MASK (0x0000FFFF << CUSBD_USB_SSP_LSM_OFFSET)

/** Description of bmAttributes field from  Configuration Description */
#define	CH9_USB_CONFIG_RESERVED BIT(7)

/** Self Powered */
#define	CH9_USB_CONFIG_SELF_POWERED BIT(6)

#define CH9_USB_CONFIG_BUS_POWERED BIT(7)

/** Remote Wakeup */
#define	CH9_USB_CONFIG_REMOTE_WAKEUP BIT(5)

/** Definitions for bEndpointAddress field from  Endpoint descriptor */
#define	CH9_USB_EP_DIR_MASK 0x80

#define	CH9_USB_EP_DIR_IN 0x80

#define	CH9_USB_EP_NUMBER_MASK 0x0f

/** Endpoint attributes from Endpoint descriptor - bmAttributes field */
#define	CH9_USB_EP_TRANSFER_MASK 0x03

#define	CH9_USB_EP_CONTROL 0x0

#define	CH9_USB_EP_ISOCHRONOUS 0x01

#define	CH9_USB_EP_BULK 0x02

#define	CH9_USB_EP_INTERRUPT 0x03

/** Synchronization types for ISOCHRONOUS endpoints */
#define	CH9_USB_EP_SYNC_MASK 0xC

#define	CH9_USB_EP_SYNC_NO (0x00 >> 2)

#define	CH9_USB_EP_SYNC_ASYNCHRONOUS (0x1 >> 2)

#define	CH9_USB_EP_SYNC_ADAPTIVE (0x02 >> 2)

#define	CH9_USB_EP_SYNC_SYNCHRONOUS (0x03 >> 2)

#define	CH9_USB_EP_USAGE_MASK (0x3 >> 4)

/** Usage types for ISOCHRONOUS endpoints */
#define	CH9_USB_EP_USAGE_DATA (00 >> 4)

#define	CH9_USB_EP_USAGE_FEEDBACK (0x01 >> 4)

#define	CH9_USB_EP_USAGE_IMPLICIT_FEEDBACK (0x02 >> 4)

/** Usage types for INTERRUPTS endpoints */
#define	CH9_USB_EP_USAGE_PERIODIC (00 >> 4)

#define	CH9_USB_EP_USAGE_NOTIFICATION (0x01 >> 4)

/** Description of fields bmAttributes from OTG descriptor */
#define	CH9_USB_OTG_ADP_MASK 0x4

#define	CH9_USB_OTG_HNP_MASK 0x2

#define	CH9_USB_OTG_SRP_MASK 0x1

/**
 * Test Mode Selectors
 * See USB 2.0 spec Table 9-7
 */
#define	CH9_TEST_J 1

#define	CH9_TEST_K 2

#define	CH9_TEST_SE0_NAK 3

#define	CH9_TEST_PACKET 4

#define	CH9_TEST_FORCE_EN 5

#define	CH9_MAX_PACKET_SIZE_MASK 0x7ff

#define	CH9_PACKET_PER_FRAME_SHIFT 11

/**
 * OTG status selector
 * See USB_OTG_AND_EH_2-0 spec Table 6-4
 */
#define	CH9_OTG_STATUS_SELECTOR 0xF000

/**
 *  @}
 */

/* Conventional codes for class-specific descriptors.  The convention is
 * defined in the USB "Common Class" Spec (3.11).  Individual class specs
 * are authoritative for their usage, not the "common class" writeup.
 */
#define USB_DT_CS_DEVICE		(CH9_USB_REQ_TYPE_CLASS | CH9_USB_DT_DEVICE)
#define USB_DT_CS_CONFIG		(CH9_USB_REQ_TYPE_CLASS | CH9_USB_DT_CONFIG)
#define USB_DT_CS_STRING		(CH9_USB_REQ_TYPE_CLASS | CH9_USB_DT_STRING)
#define USB_DT_CS_INTERFACE		(CH9_USB_REQ_TYPE_CLASS | CH9_USB_DT_INTERFACE)
#define USB_DT_CS_ENDPOINT		(CH9_USB_REQ_TYPE_CLASS | CH9_USB_DT_ENDPOINT)

/** @defgroup DataStructure Dynamic Data Structures
 *  This section defines the data structures used by the driver to provide
 *  hardware information, modification and dynamic operation of the driver.
 *  These data structures are defined in the header file of the core driver
 *  and utilized by the API.
 *  @{
 */

/**********************************************************************
 * Forward declarations
 **********************************************************************/
struct CH9_UsbSetup;
struct CH9_UsbDeviceDescriptor;
struct CH9_UsbBosDescriptor;
struct CH9_UsbCapabilityDescriptor;
struct CH9_Usb20ExtensionDescriptor;
struct CH9_UsbSSDeviceCapabilityDescriptor;
struct CH9_UsbContainerIdDescriptor;
struct CH9_UsbPlatformDescriptor;
struct CH9_UsbSSPlusDescriptor;
struct CH9_UsbPTMCapabilityDescriptor;
struct CH9_UsbConfigurationDescriptor;
struct CH9_UsbInterfaceAssociationDescriptor;
struct CH9_UsbInterfaceDescriptor;
struct CH9_UsbEndpointDescriptor;
struct CH9_UsbSSEndpointCompanionDescriptor;
struct CH9_UsbSSPlusIsocEndpointCompanionDescriptor;
struct CH9_UsbStringDescriptor;
struct CH9_UsbDeviceQualifierDescriptor;
struct CH9_UsbOtherSpeedConfigurationDescriptor;
struct CH9_UsbHeaderDescriptor;
struct CH9_UsbOtgDescriptor;
struct CH9_ConfigParams;

/**********************************************************************
 * Enumerations
 **********************************************************************/
/** USB States defined in USB Specification */
typedef enum {
    /** Device not attached yet */
	CH9_USB_STATE_NONE = 0,
    /** see Figure 9-1 of USB Spec */
	CH9_USB_STATE_ATTACHED = 1,
	CH9_USB_STATE_POWERED = 2,
	CH9_USB_STATE_DEFAULT = 3,
	CH9_USB_STATE_ADDRESS = 4,
	CH9_USB_STATE_CONFIGURED = 5,
	CH9_USB_STATE_SUSPENDED = 6,
	CH9_USB_STATE_ERROR = 7,
} CH9_UsbState;

/** Speeds defined in USB Specification */
typedef enum {
    /** unknown speed - before enumeration */
	CH9_USB_SPEED_UNKNOWN = 0,
    /** (1,5Mb/s) */
	CH9_USB_SPEED_LOW = 1,
    /** usb 1.1 (12Mb/s) */
	CH9_USB_SPEED_FULL = 2,
    /** usb 2.0 (480Mb/s) */
	CH9_USB_SPEED_HIGH = 3,
    /** usb 2.5 wireless */
	CH9_USB_SPEED_WIRELESS = 4,
    /** usb 3.0 GEN 1  (5Gb/s) */
	CH9_USB_SPEED_SUPER = 5,
    /** usb 3.1 GEN2 (10Gb/s) */
	CH9_USB_SPEED_SUPER_PLUS = 6,
} CH9_UsbSpeed;

/**********************************************************************
 * Structures and unions
 *********************************************************************
 */

/** Structure describes USB request (SETUP packet). See USB Specification (chapter 9.3) */
typedef struct CH9_UsbSetup {
    /** Characteristics of request */
	uint8_t bmRequestType;
    /** Specific request */
	uint8_t bRequest;
    /** Field that varies according to request */
	uint16_t wValue;
    /** typically used to pass an index or offset. */
	uint16_t wIndex;
    /** Number of bytes to transfer if there is a data stage */
	uint16_t wLength;
} __packed CH9_UsbSetup;

/** Standard Device Descriptor (see Table 9-11 of USB Spec 3.1) */
typedef struct CH9_UsbDeviceDescriptor {
    /** Size of descriptor */
	uint8_t bLength;
    /** Device descriptor type */
	uint8_t bDescriptorType;
    /** USB Specification Release Number */
	uint16_t bcdUSB;
    /** Class code (assigned by the USB-IF) */
	uint8_t bDeviceClass;
    /** Subclass code (assigned by the USB-IF */
	uint8_t bDeviceSubClass;
    /** Protocol code (assigned by the USB-IF */
	uint8_t bDeviceProtocol;
    /** Maximum packet size for endpoint zero */
	uint8_t bMaxPacketSize0;
    /** Vendor ID (assigned by the USB-IF */
	uint16_t idVendor;
    /** Product ID (assigned by manufacturer) */
	uint16_t idProduct;
    /** Device release number */
	uint16_t bcdDevice;
    /** Index of string descriptor describing manufacturer */
	uint8_t iManufacturer;
    /** Index of string descriptor describing product */
	uint8_t iProduct;
    /** Index of string descriptor for serial number */
	uint8_t iSerialNumber;
    /** Number of possible configurations */
	uint8_t bNumConfigurations;
} __packed CH9_UsbDeviceDescriptor;

/** Binary Device Object Store descriptor (see Table 9-12 of USB Spec 3.1) */
typedef struct CH9_UsbBosDescriptor {
    /** Size of this descriptor */
	uint8_t bLength;
    /** Descriptor type: BOS */
	uint8_t bDescriptorType;
    /** Length of this descriptor and all of its sub descriptors */
	uint16_t wTotalLength;
    /** The number of separate device capability descriptors in the BOS */
	uint8_t bNumDeviceCaps;
} __packed CH9_UsbBosDescriptor;

/** Device Capability Descriptor (see Table 9-12 of USB Spec 3.1) */
typedef struct CH9_UsbCapabilityDescriptor {
    /** Size of this descriptor */
	uint8_t bLength;
    /** Descriptor type: DEVICE CAPABILITY type */
	uint8_t bDescriptorType;
    /** Capability type: USB 2.0 EXTENSION (002h) */
	uint8_t bDevCapabilityType;
    /** Capability specific format */
	uint32_t bmAttributes;
} __packed CH9_UsbCapabilityDescriptor;

/** USB 2.0 Extension Descriptor (see Table 9-15 of USB Spec 3.1) */
typedef struct CH9_Usb20ExtensionDescriptor {
    /** Size of this descriptor */
	uint8_t bLength;
    /** Descriptor type: DEVICE CAPABILITY type */
	uint8_t bDescriptorType;
    /** Capability type: USB 2.0 EXTENSION (002h) */
	uint8_t bDevCapabilityType;
    /** Capability specific format */
	uint32_t bmAttributes;
} __packed CH9_Usb20ExtensionDescriptor;

/** SuperSpeed USB Device Capability Descriptor (see Table 9-16 of USB Spec 3.1) */
typedef struct CH9_UsbSSDeviceCapabilityDescriptor {
    /** Size of this descriptor */
	uint8_t bLength;
    /** DEVICE CAPABILITY Descriptor type */
	uint8_t bDescriptorType;
    /** Capability type: SUPERSPEED_USB */
	uint8_t bDevCapabilityType;
    /** Bitmap encoding of supported device level features */
	uint8_t bmAttributes;
    /** Bitmap encoding of the speed supported by device */
	uint16_t wSpeedSupported;
    /**
     * The lowest speed at which all the functionality
     * supported by the device is available to the user
     */
	uint8_t vFunctionalitySupport;
    /** U1 Device Exit Latency */
	uint8_t bU1DevExitLat;
    /** U2 Device Exit Latency */
	uint16_t bU2DevExitLat;
} __packed CH9_UsbSSDeviceCapabilityDescriptor;

/** Container ID Descriptor (see Table 9-17 of USB Spec 3.1) */
typedef struct CH9_UsbContainerIdDescriptor {
    /** Size of this descriptor */
	uint8_t bLength;
    /** DEVICE CAPABILITY Descriptor type */
	uint8_t bDescriptorType;
    /** Capability type: CONTAINER_ID */
	uint8_t bDevCapabilityType;
    /** Field reserved and shall be set to zero */
	uint8_t bReserved;
    /** unique number to device instance */
	uint8_t ContainerId[16];
} __packed CH9_UsbContainerIdDescriptor;

typedef struct CH9_UsbPlatformDescriptor {
    /** Size of this descriptor */
	uint8_t bLength;
    /** DEVICE CAPABILITY Descriptor type */
	uint8_t bDescriptorType;
    /** Capability type: PLATFORM */
	uint8_t bDevCapabilityType;
    /** Field reserved and shall be set to zero */
	uint8_t bReserved;
    /** unique number to identifies a platform */
	uint8_t PlatformCapabilityUUID[16];
    /** variable length */
	uint8_t CapabilityData[0];
} __packed CH9_UsbPlatformDescriptor;

/** SuperSpeedPlus USB Device Capability  (see Table 9-19 of USB Spec 3.1) */
typedef struct CH9_UsbSSPlusDescriptor {
    /** Size of this descriptor */
	uint8_t bLength;
    /** DEVICE CAPABILITY Descriptor type */
	uint8_t bDescriptorType;
    /** Capability type: SUPERSPEED_PLUS */
	uint8_t bDevCapabilityType;
    /** Field reserved and shall be set to zero */
	uint8_t bReserved;
    /** Bitmap encoding of supported SuperSpeedPlus features */
	uint32_t bmAttributes;
    /** supported functionality */
	uint16_t wFunctionalitySupport;
    /** Reserved. Shall be set to zero */
	uint16_t wReserved;
    /** Sublink Speed Attribute */
	uint32_t bmSublinkSpeedAttr0;
    /** Additional Lane Speed Attributes */
	uint32_t bmSublinkSpeedAttrSSAC;
} __packed CH9_UsbSSPlusDescriptor;

/** SuperSpeedPlus USB Device Capability  (see Table 9-19 of USB Spec 3.1) */
typedef struct CH9_UsbPTMCapabilityDescriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDevCapabilityType;
} __packed CH9_UsbPTMCapabilityDescriptor;

/** Standard Configuration Descriptor (see Table 9-21 of USB Spec 3.1) */
typedef struct CH9_UsbConfigurationDescriptor {
    /** Size of descriptor */
	uint8_t bLength;
    /** Configuration descriptor type */
	uint8_t bDescriptorType;
    /** Total length of configuration */
	uint16_t wTotalLength;
    /** Number of interfaces supported by configuration */
	uint8_t bNumInterfaces;
    /** Value use as an argument to SetConfiguration() request */
	uint8_t bConfigurationValue;
    /** Index of string descriptor describing this configuration */
	uint8_t iConfiguration;
    /** Configuration attributes */
	uint8_t bmAttributes;
    /** Maximum power consumption of the USB device */
	uint8_t bMaxPower;
} __packed CH9_UsbConfigurationDescriptor;

/** Standard Interface Association Descriptor  (see Table 9-22 of USB Spec 3.1) */
typedef struct CH9_UsbInterfaceAssociationDescriptor {
    /** Size of descriptor */
	uint8_t bLength;
    /** Interface Association Descriptor Type */
	uint8_t bDescriptorType;
    /** interface number of this interface that is associated with this function */
	uint8_t bFirstInterface;
    /** Number of contiguous interfaces that are associated with this function */
	uint8_t bInterfaceCount;
    /** Class code assigned by USB-IF */
	uint8_t bFunctionClass;
    /** Subclass code */
	uint8_t bFunctionSubClass;
    /** Protocol code */
	uint8_t bFunctionProtocol;
    /** Index of string descriptor describing this function */
	uint8_t iFunction;
} __packed CH9_UsbInterfaceAssociationDescriptor;

/** Standard Interface Descriptor (see Table 9-23 of USB Spec 3.1) */
typedef struct CH9_UsbInterfaceDescriptor {
    /** Size of descriptor */
	uint8_t bLength;
    /** Interface Descriptor Type */
	uint8_t bDescriptorType;
    /** Number of this interface */
	uint8_t bInterfaceNumber;
    /** Value used to select this alternate setting */
	uint8_t bAlternateSetting;
    /** Class code */
	uint8_t bNumEndpoints;
    /** Subclass code */
	uint8_t bInterfaceClass;
    /** Subclass code */
	uint8_t bInterfaceSubClass;
    /** Protocol code */
	uint8_t bInterfaceProtocol;
    /** Index of string */
	uint8_t iInterface;
} __packed CH9_UsbInterfaceDescriptor;

#define USB_DT_INTERFACE_SIZE		9

#define USB_DIR_OUT			0		/* to device */
#define USB_DIR_IN			0x80		/* to host */
/** Standard Endpoint Descriptor */
typedef struct CH9_UsbEndpointDescriptor {
    /** Size of descriptor */
	uint8_t bLength;
    /** Endpoint Descriptor Type */
	uint8_t bDescriptorType;
    /** The address of the endpoint */
	uint8_t bEndpointAddress;
    /** Endpoint attribute */
	uint8_t bmAttributes;
    /** Maximum packet size for this endpoint */
	uint16_t wMaxPacketSize;
    /** interval for polling endpoint data transfer */
	uint8_t bInterval;
} __packed CH9_UsbEndpointDescriptor;

#define USB_DT_ENDPOINT_SIZE		7
#define USB_DT_ENDPOINT_AUDIO_SIZE	9	/* Audio extension */

/** Standard SuperSpeed Endpoint Companion Descriptor (see Table 9-26 of USB Spec 3.1) */
typedef struct CH9_UsbSSEndpointCompanionDescriptor {
    /** Size of descriptor in bytes */
	uint8_t bLength;
    /** SUPERSPEED_USB_ENDPOINT_COMPANION Descriptor types */
	uint8_t bDescriptorType;
    /** Number of packets that endpoint can transmit as part of burst */
	uint8_t bMaxBurst;
	uint8_t bmAttributes;
    /** The total number of bytes  for every service interval */
	uint16_t wBytesPerInterval;
} __packed CH9_UsbSSEndpointCompanionDescriptor;

/**
 * Standard SuperSpeedPlus Isochronous Endpoint
 * Companion Descriptor (see Table 9-27 of USB Spec 3.1)
 */
typedef struct CH9_UsbSSPlusIsocEndpointCompanionDescriptor {
    /** Size of descriptor in bytes */
	uint8_t bLength;
    /** SUPERSPEEDPLUS_ISOCHRONOUS_ENDPOINT_COMPANION Descriptor types */
	uint8_t bDescriptorType;
    /** Reserved. Shall be set to zero */
	uint16_t wReserved;
    /** The total number of bytes  for every service interval */
	uint32_t dwBytesPerInterval;
} __packed CH9_UsbSSPlusIsocEndpointCompanionDescriptor;

/** Standard String Descriptor */
typedef struct CH9_UsbStringDescriptor {
    /** Size of descriptor */
	uint8_t bLength;
    /** STRING Descriptor Type */
	uint8_t bDescriptorType;
    /** UNICODE encoded string */
	uint8_t *bString;
} __packed CH9_UsbStringDescriptor;

/** Standard Device Qualifier Descriptor (see Table 9-9 of USB Spec 2.0) */
typedef struct CH9_UsbDeviceQualifierDescriptor {
    /** Size of descriptor */
	uint8_t bLength;
    /** Device Qualifier type */
	uint8_t bDescriptorType;
    /** USB Specification version number */
	uint16_t bcdUSB;
    /** Class code */
	uint8_t bDeviceClass;
    /** Subclass code */
	uint8_t bDeviceSubClass;
    /** Protocol code */
	uint8_t bDeviceProtocol;
    /** Maximum packet size for other speed */
	uint8_t bMaxPacketSize0;
    /** Number of other speed configuration */
	uint8_t bNumConfigurations;
    /** Reserved for future use */
	uint8_t bReserved;
} __packed CH9_UsbDeviceQualifierDescriptor;

/** Standard Other_Speed_Configuration descriptor (see Table 9-11 of USB Spec 2.0) */
typedef struct CH9_UsbOtherSpeedConfigurationDescriptor {
    /** Size of descriptor */
	uint8_t bLength;
    /** Configuration descriptor type */
	uint8_t bDescriptorType;
    /** Total length of configuration */
	uint16_t wTotalLength;
    /** Number of interfaces supported by this speed configuration */
	uint8_t bNumInterfaces;
    /** Value to use to select configuration */
	uint8_t bConfigurationValue;
    /** Index of string descriptor describing this configuration */
	uint8_t iConfiguration;
    /** Configuration attributes */
	uint8_t bmAttributes;
    /** Maximum power consumption of the USB device */
	uint8_t bMaxPower;
} __packed CH9_UsbOtherSpeedConfigurationDescriptor;

/**
 * Header descriptor. All descriptor have the same header that
 * consist of bLength and bDescriptorType fields
 */
typedef struct CH9_UsbHeaderDescriptor {
    /** Size of descriptor */
	uint8_t bLength;
    /** descriptor Type */
	uint8_t bDescriptorType;
} __packed CH9_UsbHeaderDescriptor;

/** OTG descriptor (see OTG spec. Table 6.1) */
typedef struct CH9_UsbOtgDescriptor {
    /** Size of descriptor */
	uint8_t bLength;
    /** OTG Descriptor Type */
	uint8_t bDescriptorType;
    /** Attribute field */
	uint8_t bmAttributes;
    /** OTG and EH supplement release number */
	uint16_t bcdOTG;
} __packed CH9_UsbOtgDescriptor;

typedef struct CH9_ConfigParams {
    /** U1 Device exit Latency */
	uint8_t bU1devExitLat;
    /** U2 Device exit Latency */
	uint16_t bU2DevExitLat;
} __packed CH9_ConfigParams;

/**
 *  @}
 */

#endif	/* CVI_CH9_H */
