/** @file
  PlatformInfo GUID

  These GUIDs point the ACPI tables as defined in the ACPI specifications.
  ACPI 2.0 specification defines the ACPI 2.0 GUID. UEFI 2.0 defines the
  ACPI 2.0 Table GUID and ACPI Table GUID.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  @par Revision Reference:
  GUIDs defined in UEFI 2.0 spec.

**/

#ifndef _PLATFORM_INFO_GUID_H_
#include <Library/MeFiaMuxLib.h>
#define _PLATFORM_INFO_GUID_H_

extern EFI_GUID gPlatformInfoHobGuid;
extern CHAR16   EfiPlatformInfoVariable[];

#define PLATFORM_INFO_HOB_REVISION      0x1
#define SOC_STEPPING_A0 0

#define BOARD_NAME_SIZE 32

#pragma pack(1)

typedef struct {
  //
  // These are global resource mapping of the system. Per IOH resource mapping is available in UDS.
  //
  UINT16  PciResourceIoBase;
  UINT16  PciResourceIoLimit;
  UINT32  PciResourceMem32Base;
  UINT32  PciResourceMem32Limit;
  UINT64  PciResourceMem64Base;
  UINT64  PciResourceMem64Limit;
  UINT64  PciExpressBase;
  UINT32  PciExpressSize;
  UINT8   PciHostAddressWidth;
} EFI_PLATFORM_PCI_DATA;

typedef struct {
  UINT8   CpuAddressWidth;
  UINT8   CpuStepping;
  UINT8   CpuSKU;
} EFI_PLATFORM_CPU_DATA;

typedef struct {
  UINT8   SysIoApicEnable;
  UINT8   SysSioExist;
} EFI_PLATFORM_SYS_DATA;

typedef struct {
  UINT32  MemTolm;
  UINT32  MemMaxTolm;
  UINT32  MemTsegSize;
  UINT32  MemIedSize;
  UINT64  MemMir0;
  UINT64  MemMir1;
  UINT32  PunitAddr;
  UINT32  PuintSize;
#ifdef LT_FLAG
  UINT32  MemLtMemSize; //Below TSEG, contains (high to low: (Heap, SinitMem, reserved)3M, LtDevMemNonDMA 2M)
  UINT32  MemDprMemSize;//6M DPR contained in LtMem
#endif
  UINT32  MemConfigSize;
  UINT16  MmioSize;
} EFI_PLATFORM_MEM_DATA;

typedef struct {
  UINT8   PchLanSupported;   // Indicates if PCH LAN on board is supported or not
} EFI_PLATFORM_IIO_DATA;
//
// This HOB definition must be consistent with what is created in the
// PlatformInfo protocol definition.  This way the information in the
// HOB can be directly copied upon the protocol and only the strings
// will need to be updated.
//
#define PLATFORM_NAME_MAX_SIZE    0x40
typedef struct __PLATFORM_INFO_HOB {
  UINT8                       SystemUuid[16];     // 16 bytes
  UINT32                      Signature;          // "$PIT" 0x54495024
  UINT32                      Size;               // Size of the table
  UINT16                      Revision;           // Revision of the table
  UINT16                      Type;               // Platform Type
  UINT8                       CpuType;            // Cpu Type
  UINT8                       CpuStepping;        // Cpu Stepping
  UINT32                      TypeRevisionId;     // Board Revision ID
  BOOLEAN                     ExtendedInfoValid;  // If TRUE then below fields are Valid
  UINT8                       Checksum;           // Checksum minus SystemUuid is valid in DXE only.
  CHAR16                      PlatformName[PLATFORM_NAME_MAX_SIZE];
  EFI_PLATFORM_PCI_DATA       PciData;
  EFI_PLATFORM_CPU_DATA       CpuData;
  EFI_PLATFORM_MEM_DATA       MemData;
  EFI_PLATFORM_SYS_DATA       SysData;
  EFI_PLATFORM_IIO_DATA       IioData;
  UINT8                       BoardId;
  UINT8                       PlatformFlavor;
} EFI_PLATFORM_INFO;

typedef enum {
  BoardTypeCrb        = 0, // Reference Validation Platform
  BoardTypeSv,             // Boards used for Silicon Validation
  BoardTypeMax
} BOARD_TYPES;

typedef enum {
  BOARD_ID_Simics,
  BOARD_ID_OstrichBayS = 0x4F,
  BOARD_ID_OstrichBay,
  BOARD_ID_CormorantLake,
  BOARD_ID_Harcuvar,
  BOARD_ID_AspenCove,
  BOARD_ID_Minnehaha,
  BOARD_ID_PineLake,
  BOARD_ID_CarCreekModule,
  BOARD_ID_BigBugCreek,
  BOARD_ID_PineLakeB,
  BOARD_ID_CarCreekModule2,
  BOARD_ID_Nano,
  BOARD_ID_BigBugCreek2,
  BOARD_ID_Delphi  
} BOARD_ID_LIST;
//
//
//
#define GPIO_SCORE_OFFSET       0x0
#define GPIO_SSUS_OFFSET        0x1000
//
// GPIO Initialization Data Structure for ValleyView
// SC = SCORE, SS= SSUS
//

//
//  IO space
//


#define PORT_CTRL_OWNED     1
#define PORT_CTRL_NOT_OWNED 0

#define PORT_OWNED_PCIE 0
#define PORT_OWNED_SATA 1
#define PORT_OWNED_USB  2


#define MAX_PCIE_CTRL     2
#define MAX_PCIE_PORTS    8

//
//Bifurcation Control 0 (BIFCTL0):
// 111-101: Reserved
// 100: x8
// 011: x4x4
// 010: x4x2x2
// 001: x2x2x4
// 000: x2x2x2x2
//
#define PCI_BIF_CTRL_x8   4
#define PCI_BIF_CTRL_x4   3
#define PCI_BIF_CTRL_x4x2 2
#define PCI_BIF_CTRL_x2x4 1
#define PCI_BIF_CTRL_x2   0

#define MAX_SATA_CTRL   2
#define MAX_SATA_PORTS  8
#define MAX_USB_PORTS   4


#define TRIDENT_A1A3    0x80
#define TRIDENT_A2A8    0x81
#define TRIDENT_A4      0x82
#define TRIDENT_A5      0x83
#define TRIDENT_A6A9    0x84
#define TRIDENT_A7      0x85
#define TRIDENT_A10     0x86
#define TRIDENT_B1      0x87
#define TRIDENT_B2      0x88
#define TRIDENT_B3      0x89
#define TRIDENT_B4      0x8A
#define TRIDENT_B5B7    0x8B
#define TRIDENT_B6      0x8C
#define TRIDENT_B8      0x8D
#define TRIDENT_B9      0x8E
#define TRIDENT_B10     0x8F

#define TRIDENT_SIMICS_A 0x50
#define TRIDENT_SIMICS_B 0x51

#define SKU_HSIO_20     20
#define SKU_HSIO_12     12
#define SKU_HSIO_10     10
#define SKU_HSIO_08      8
#define SKU_HSIO_06      6

typedef struct {
    UINT16          			  NumLanesSupported;
	UINT8           			  PcieBifCtr[MAX_PCIE_CTRL];
	ME_FIA_CONFIG                 FiaConfig;
} HSIO_INFORMATION;


typedef struct {
    UINT8 TridentIdA;
    UINT8 TridentIdB;
    HSIO_INFORMATION HsioInfo;
} TRIDENT_INFO;

typedef struct{
 UINT8 BD_FAB0:1;
 UINT8 BD_FAB1:1;
 UINT8 RESERVED0:1;
 UINT8 RESERVED1:1;
 UINT8 MANFACTURING_MODE:1;
 UINT8 RTC_CMOS_CLEAR:1;
 UINT8 ADV_DBG_DFX:1;
 UINT8 MOHANPKBIFID:1;

 UINT8 TRIDENT_ID_0:1;
 UINT8 TRIDENT_ID_1:1;
 UINT8 TRIDENT_ID_2:1;
 UINT8 PCI_PRSNT_N_0:1;
 UINT8 PCI_PRSNT_N_1:1;
 UINT8 PCI_PRSNT_N_2:1;
 UINT8 PCI_PRSNT_N_3:1;
 UINT8 RESERVED3:1;
}GPIO_EXPANDER44;

typedef struct _PLATFORM_INFO_HOB {
  UINT8                       RevisonId;           // Structure Revision ID
  UINT16                      IohSku;
  UINT8                       IohRevision;
  UINT16                      IchSku;
  UINT8                       IchRevision;
  EFI_PLATFORM_PCI_DATA       PciData;
  EFI_PLATFORM_CPU_DATA       CpuData;
  EFI_PLATFORM_MEM_DATA       MemData;
  EFI_PLATFORM_SYS_DATA       SysData;
  UINT8                       BoardRev;            // Board Revision
  GPIO_EXPANDER44             GpioExp44;
  UINT32                      SsidSvid;
  UINT16                      AudioSubsystemDeviceId;
  UINT64                      AcpiOemId;
  UINT64                      AcpiOemTableId;
  UINT8                       PlatformType;
  UINT8                       PlatformFlavor;
  UINT8                       BoardType;
  BOARD_ID_LIST               BoardId;             // Board ID
  UINT8                       TridentIdA;
  UINT8                       TridentIdB;
  HSIO_INFORMATION            FiaHsioInfo;
  UINT8                       PlatformGeneration;
  UINT8                       EcPresent;
  CHAR8                       BoardName[BOARD_NAME_SIZE];
  UINT8						  MfgMode;        //1=Boot with Mfg mode;
  UINT8						  RtcWeak;        //1=RTC power failure
  UINT8                       NvClear;
} PLATFORM_INFO_HOB;


#pragma pack()

#endif
