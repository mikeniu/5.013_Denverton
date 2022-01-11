//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2009 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformCpu.h
    
Abstract:

  Protocol used for specifying platform related CPU information and policy setting.

--*/

#ifndef _PLATFORM_UNCORE_POLICY_H_
#define _PLATFORM_UNCORE_POLICY_H_

//
// Protocol revision number
// Any backwards compatible changes to this protocol will result in an update in the revision number
// Major changes will require publication of a new protocol
//
// Revision 1: Original version
//

#define DXE_PLATFORM_SB_POLICY_PROTOCOL_REVISION     1

#define SCH_PORT80_LPC                          0
#define SCH_PORT80_PCI                          1

#define SCH_RSVD_DEFAULT                        0

#define MAX_PCI_EXPRESS_ROOT_PORTS              4
#define SCH_MAX_PCI_BRIDGES                     0

#define SCH_S3_PCI_ITEM_BYTE_SAVED_MAX         256
#define SCH_S3_PCI_ITEM_DWORD_SAVED_PER_BLOCK  8 
//
// Below macro can use this instead: (SCH_S3_PCI_ITEM_DWORD_SAVED_PER_BLOCK * sizeof (UINT32))
//
#define SCH_S3_PCI_ITEM_BYTE_SAVED_PER_BLOCK   32
#define SCH_S3_PCI_ITEM_REG_ENABLES_MAX        (SCH_S3_PCI_ITEM_BYTE_SAVED_MAX / SCH_S3_PCI_ITEM_BYTE_SAVED_PER_BLOCK)

#define SCH_S3_PCI_ITEM_MAX   32

#define SCH_S3_MM_ITEM_MAX      256
//
// User can save a memory mapped area (<= 1K) for S3 
//
#define SCH_S3_MM_ITEM_AREA_MAX 0x400

extern EFI_GUID gDxePlatformUncorePolicyGuid;

//
// ICH Configurations -----------------------------------------------------
//
typedef struct {
    UINT32 Smbus      :1;               // 0: Disabled; 1: Enabled*   
    UINT32 Function0  :1;               // 0: Disabled; 1: Enabled*
    UINT32 PciClockRun:1;               // 0: Disabled; 1: Enabled*
    UINT32 Thermal    :1;               // 0: Disabled; 1: Enabled* 

// BGR092807 - Added for Tunnel Creek PCIe root port disables >>>
    UINT32 PcieRP0    :1;               // 0: Disabled; 1: Enabled*
    UINT32 PcieRP1    :1;               // 0: Disabled; 1: Enabled*
    UINT32 PcieRP2    :1;               // 0: Disabled; 1: Enabled*
    UINT32 PcieRP3    :1;               // 0: Disabled; 1: Enabled*
// BGR092807 - Added for Tunnel Creek PCIe root port disables <<<
    UINT32 Rsvd       :24;              // 0
} SCH_DEVICE_ENABLES;

typedef struct {
  UINT8    InterfaceMode;            // 0: PATA; 1: SATA; 2: COMBINED*; 3: Enhanced Non-AHCI; 4: Enhanced AHCI
  UINT8    IdeCableTypeGpio;         // Gpio pin number of platform of IDE cable selecting function
} HDC_CONFIG;

typedef struct {
  UINT8   SmbusAddressNum;
  UINT8   *SmbusAddressTable;
} RSVD_SMBUS_ADDRESS;

typedef struct {
  RSVD_SMBUS_ADDRESS      SmBusAddress;
} SMBUS_CONFIG;

typedef struct {
  UINT8                   Bus;
  UINT8                   Device;
  UINT8                   Function;
  UINT8                   SaveEnables[SCH_S3_PCI_ITEM_REG_ENABLES_MAX];
} PCI_S3_SAVE_ITEM;

typedef struct {
  UINTN                   NumberOfEntries;
  PCI_S3_SAVE_ITEM        PciS3SaveItems[SCH_S3_PCI_ITEM_MAX];
} PCI_S3_SAVE_TABLE;

typedef struct {
  UINT64                   Address;
  UINT8                    Width;
  UINTN                    Length;
} MM_S3_SAVE_ITEM;

typedef struct {
  UINTN                   NumberOfEntries;
  MM_S3_SAVE_ITEM         MmS3SaveItems[SCH_S3_MM_ITEM_MAX];
} MM_S3_SAVE_TABLE;

typedef struct {
  PCI_S3_SAVE_TABLE       *PciS3SaveTable;
  MM_S3_SAVE_TABLE        *MemoryMappedS3SaveTable;
} S3_RUNTIME_SAVE_CONFIG;

//
// This member is to duplicate CpuPlatformPolicy member PPM_CONFIG for the built-in-EDK for ICH8M module
//
typedef struct {
  UINT8   CxPopUp        :1;
  UINT8   CxPopDown      :1;
  UINT8   FastC4Exit     :1;
  UINT8   SlowC4Exit     :1;
  UINT8   Rsvd           :4;
} CPU_PPM_CONFIG;

//
// This member is for Root Bridge Extra Bus Reservation in SETUP
//
typedef struct {
  UINT8   Bus;
  UINT8   Device;
  UINT8   Function;
  UINT8   BusRsvd;
} RESERVED_BUS_CONFIG;
typedef struct {
  UINT8   MemRsvd;
  UINT8   IoRsvd;
} RESERVED_BRIDGE_RES;

#define S3_RUNTIME_SAVE_CONFIG_SIZE (sizeof (S3_RUNTIME_SAVE_CONFIG))

//
// DXE Platform Policiy ====================================================
//
typedef struct _DXE_UNCORE_POLICY_PROTOCOL {
  SCH_DEVICE_ENABLES      DeviceEnables;
  HDC_CONFIG              HdcConfig;  
  SMBUS_CONFIG            SmbusConfig;
  CPU_PPM_CONFIG          PpmConfig;  
  S3_RUNTIME_SAVE_CONFIG  S3RunTimeSaveConfig;
  UINT8                   Revision;
  RESERVED_BUS_CONFIG     RsvdBusConfig[MAX_PCI_EXPRESS_ROOT_PORTS + SCH_MAX_PCI_BRIDGES];
  CHAR16                  *CapsuleVariableName;
  RESERVED_BRIDGE_RES     RsvdBridgeRes[MAX_PCI_EXPRESS_ROOT_PORTS + SCH_MAX_PCI_BRIDGES];
} DXE_UNCORE_POLICY_PROTOCOL;

#endif                 
