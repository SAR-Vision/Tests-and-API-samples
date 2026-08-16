//-----------------------------------------------------------------//
// Name        | clhs_register.h             | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | SC2                         |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | WINDOWS                                           //
//-----------------------------------------------------------------//
// Environment | Microsoft Visual Studio                           //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | CLHS Header                                       //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 0.01 rel. 0.00                               //
//-----------------------------------------------------------------//
// Notes       |                                                   //
//             |                                                   //
//             |                                                   // 
//-----------------------------------------------------------------//
// (c) 2014 PCO AG  * Donaupark 11 *                               //
// D-93309      Kelheim / Germany * Phone: +49 (0)9441 / 2005-0 *  //
// Fax: +49 (0)9441 / 2005-20 * Email: info@pco.de                 //
//-----------------------------------------------------------------//


//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  0.01     | 16.10.2014 |  new file                              //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.200x |                                        //
//-----------------------------------------------------------------//
#ifndef CLHS_REGISTER_H
#define CLHS_REGISTER_H

#pragma pack(push)
#pragma pack(1)

//Bootstrap register addresse
typedef struct _CLHS_GENCP_REG {
  DWORD adr;
  DWORD size;
}CLHS_GENCP_REG32;

typedef struct _CLHS_GENCP_REGA {
  unsigned __int64 adr;
  unsigned __int32 size;
}CLHS_GENCP_REG;


#pragma pack(pop)

#define ABRM_GENCP_VERSION(reg)              reg.adr=0x00000,reg.size=4;
#define ABRM_VENDOR(reg)                     reg.adr=0x00004,reg.size=64;
#define ABRM_MODEL(reg)                      reg.adr=0x00044,reg.size=64;
#define ABRM_FAMILY(reg)                     reg.adr=0x00084,reg.size=64;
#define ABRM_DEVICE_VERSION(reg)             reg.adr=0x000C4,reg.size=64;
#define ABRM_MANUFACTOR(reg)                 reg.adr=0x00104,reg.size=64;
#define ABRM_SERIAL(reg)                     reg.adr=0x00144,reg.size=64;
#define ABRM_USER_NAME(reg)                  reg.adr=0x00184,reg.size=64;
#define ABRM_DEVICE_CAP(reg)                 reg.adr=0x001C4,reg.size=8;
#define ABRM_MAX_RESPONSE(reg)               reg.adr=0x001CC,reg.size=4;
#define ABRM_MANIFEST_OFFSET(reg)            reg.adr=0x001D0,reg.size=8;
#define ABRM_SBMR_OFFSET(reg)                reg.adr=0x001D8,reg.size=8;
#define ABRM_DEVICE_CONFIG(reg)              reg.adr=0x001E0,reg.size=8;
#define ABRM_HEARTBEAT_TIMEOUT(reg)          reg.adr=0x001E8,reg.size=4;
#define ABRM_MESSAGE_CHANNEL_ID(reg)         reg.adr=0x001EC,reg.size=4;
#define ABRM_TIMESTAMP(reg)                  reg.adr=0x001F0,reg.size=8;
#define ABRM_TIMESTAMP_LATCH(reg)            reg.adr=0x001F8,reg.size=4;
#define ABRM_TIMESTAMP_INC(reg)              reg.adr=0x001FC,reg.size=8;
#define ABRM_ACCESS_PRIVILEG(reg)            reg.adr=0x00204,reg.size=4;


#define ABRM_TEST_0(reg)                     reg.adr=0x08000,reg.size=4;
#define ABRM_TEST_1(reg)                     reg.adr=0x08004,reg.size=4;
#define ABRM_TEST_2(reg)                     reg.adr=0x08008,reg.size=4;
#define ABRM_TEST_3(reg)                     reg.adr=0x0800C,reg.size=4;
#define ABRM_TEST_4(reg)                     reg.adr=0x08010,reg.size=4;



#define SBRM_LLDEVICE_ID(reg)                reg.adr=0x10000,reg.size=4;
#define SBRM_PORT_ID(reg)                    reg.adr=0x10004,reg.size=4;
#define SBRM_AVAIL_PORT(reg)                 reg.adr=0x10008,reg.size=4;
#define SBRM_DEVICE_CLASS(reg)               reg.adr=0x1000C,reg.size=4;
#define SBRM_ACTUAL_DEVICE_CONFIG(reg)       reg.adr=0x10010,reg.size=4;
#define SBRM_NEXT_DEVICE_CONFIG(reg)         reg.adr=0x10014,reg.size=4;
#define SBRM_NUM_DEVICE_CONFIG(reg)          reg.adr=0x10018,reg.size=4;
#define SBRM_DEVICE_CONFIG_LIST(reg)         reg.adr=0x1001C,reg.size=256;  //8*32
#define SBRM_DEVICE_CONFIG_ENTRY0(reg)       reg.adr=0x1001C,reg.size=8;  //first
#define SBRM_DEVICE_CONFIG_ENTRY1(reg)       reg.adr=0x10024,reg.size=8;  //second
#define SBRM_DEVICE_CONFIG_ENTRY2(reg)       reg.adr=0x1002C,reg.size=8;  //third
#define SBRM_ACTIVE_LINK_GPIO(reg)           reg.adr=0x1011C,reg.size=4;
#define SBRM_GPIO_INPUT_CAP(reg)             reg.adr=0x10120,reg.size=4;
#define SBRM_GPIO_OUTPUT_CAP(reg)            reg.adr=0x10124,reg.size=4;
#define SBRM_ACTUAL_LINK_SPEED(reg)          reg.adr=0x10128,reg.size=4;
#define SBRM_SUPPORTED_LINK_SPEED_LIST(reg)  reg.adr=0x1012C,reg.size=64;  //4*16
#define SBRM_NUM_SUPPORTED_LINK_SPEEDS(reg)  reg.adr=0x1016C,reg.size=4;
#define SBRM_NEXT_LINK_SPEED(reg)            reg.adr=0x10170,reg.size=4;
#define SBRM_ACTIVATE_HOTPLUG(reg)           reg.adr=0x10174,reg.size=4;
#define SBRM_SENSOR_WIDTH(reg)               reg.adr=0x10178,reg.size=4;
#define SBRM_SENSOR_HEIGHT(reg)              reg.adr=0x1017C,reg.size=4;
#define SBRM_BINNING_HORIZONTAL(reg)         reg.adr=0x10180,reg.size=4;
#define SBRM_BINNING_VERTICAL(reg)           reg.adr=0x10184,reg.size=4;
#define SBRM_DECIMATION_HORIZONTAL(reg)      reg.adr=0x10188,reg.size=4;
#define SBRM_DECIMATION_VERTICAL(reg)        reg.adr=0x1018C,reg.size=4;
#define SBRM_WIDTH_MAX(reg)                  reg.adr=0x10190,reg.size=4;
#define SBRM_HEIGHT_MAX(reg)                 reg.adr=0x10194,reg.size=4;
#define SBRM_SINGLE_ROI(reg)                 reg.adr=0x10198,reg.size=8;
#define SBRM_PIXEL_TYPE(reg)                 reg.adr=0x101A0,reg.size=4;
#define SBRM_BIT_DEPTH(reg)                  reg.adr=0x101A4,reg.size=4;
#define SBRM_PORT_ROI(reg)                   reg.adr=0x101A8,reg.size=64;  //8*8
#define SBRM_DECIMATION_FACTOR(reg)          reg.adr=0x101E8,reg.size=4;
#define SBRM_ELECTRIC_CABLE_SWAP(reg)        reg.adr=0x101EC,reg.size=4;
#define SBRM_ROW_OVERLAP(reg)                reg.adr=0x101F0,reg.size=4;
#define SBRM_PULSE_MODE_CAP(reg)             reg.adr=0x101F4,reg.size=4;


#define MANIFEST_ENTRY_COUNT_OFF(reg)             reg.adr=0x00000,reg.size=8;

#define MANIFEST_ENTRY0_FILE_VERS_OFF(reg)        reg.adr=0x00008,reg.size=4;
#define MANIFEST_ENTRY0_FILE_TYPE_OFF(reg)        reg.adr=0x0000C,reg.size=4;
#define MANIFEST_ENTRY0_FILE_ADDRESS_OFF(reg)     reg.adr=0x00010,reg.size=8;
#define MANIFEST_ENTRY0_FILE_SIZE_OFF(reg)        reg.adr=0x00018,reg.size=8;
#define MANIFEST_ENTRY0_SHA1HASH_OFF(reg)         reg.adr=0x00020,reg.size=20;
#define MANIFEST_ENTRY0_RESERVED_OFF(reg)         reg.adr=0x00034,reg.size=20;

#define MANIFEST_ENTRY1_FILE_VERS_OFF(reg)        reg.adr=0x00048,reg.size=4;
#define MANIFEST_ENTRY1_FILE_TYPE_OFF(reg)        reg.adr=0x0004C,reg.size=4;
#define MANIFEST_ENTRY1_FILE_ADDRESS_OFF(reg)     reg.adr=0x00050,reg.size=8;
#define MANIFEST_ENTRY1_FILE_SIZE_OFF(reg)        reg.adr=0x00058,reg.size=8;
#define MANIFEST_ENTRY1_SHA1HASH_OFF(reg)         reg.adr=0x00060,reg.size=20;
#define MANIFEST_ENTRY1_RESERVED_OFF(reg)         reg.adr=0x00074,reg.size=20;

//#define MANIFEST_ENTRY2_FILE_VERS_OFF        reg.adr=0x00088,reg.size=20;

#define PCO_WIDTH(reg)                            reg.adr=0x00030000,reg.size=4;
#define PCO_HEIGHT(reg)                           reg.adr=0x00030004,reg.size=4;
#define PCO_PIXEL_FORMAT(reg)                     reg.adr=0x00030008,reg.size=4;
#define PCO_ACQUISITION_MODE(reg)                 reg.adr=0x00030010,reg.size=4;
#define PCO_ACQUISITION_START(reg)                reg.adr=0x00030014,reg.size=4;
#define PCO_ACQUISITION_STOP(reg)                 reg.adr=0x00030018,reg.size=4;
#define PCO_WIDTH_MAX(reg)                        reg.adr=0x0003001C,reg.size=4;
#define PCO_HEIGHT_MAX(reg)                       reg.adr=0x00030020,reg.size=4;
#define PCO_OFFSET_X(reg)                         reg.adr=0x00030024,reg.size=4;
#define PCO_OFFSET_Y(reg)                         reg.adr=0x00030028,reg.size=4;

#define PCO_XGM_FRAME_START_COUNT(reg)            reg.adr=0x0003002C,reg.size=4;
#define PCO_XGM_FRAME_END_COUNT(reg)              reg.adr=0x00030030,reg.size=4;
#define PCO_XGM_LINE_COUNT(reg)                   reg.adr=0x00030034,reg.size=4;
#define PCO_XGM_SEQROW_ERROR_COUNT(reg)           reg.adr=0x00030038,reg.size=4;
#define PCO_XGM_CONTROL(reg)                      reg.adr=0x0003003C,reg.size=4;

#define PCO_CAM_FRAME_START_COUNT(reg)            reg.adr=0x00030040,reg.size=4;
#define PCO_CAM_FRAME_END_COUNT(reg)              reg.adr=0x00030044,reg.size=4;
#define PCO_CAM_LINE_COUNT(reg)                   reg.adr=0x00030048,reg.size=4;
#define PCO_CAM_SEQROW_ERROR_COUNT(reg)           reg.adr=0x0003004C,reg.size=4;
#define PCO_CAM_CONTROL(reg)                      reg.adr=0x00030050,reg.size=4;

#define PCO_LINE_ID(reg)                          reg.adr=0x00040120,reg.size=4;
#define PCO_TESTIMAGE(reg)                        reg.adr=0x00040124,reg.size=4;
#define PCO_CONTROL_COMMAND(reg)                  reg.adr=0x00040000;

typedef struct _CLHS_CONFIGREG {
  DWORD AvailPorts;
  DWORD NumConfig;
  DWORD ActualConfig;
  DWORD NextConfig;
  unsigned __int64 config[32];

}CLHS_CONFIGREG;


typedef struct _CLHS_PORT_BOOTREG {
  char Vendor[64];
  char Model[64];
  char Serial[64];
  DWORD LLDeviceID;
  DWORD PortID;
  CLHS_CONFIGREG master;
}CLHS_PORT_BOOTREG;



#endif


