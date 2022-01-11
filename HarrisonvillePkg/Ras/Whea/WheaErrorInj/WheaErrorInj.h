
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2009-2017 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:
    WheaErrorInj.h

Abstract:
    This is an implementation of the Whea Support protocol.

-----------------------------------------------------------------------*/


#ifndef _WHEA_CORE_EINJ_
#define _WHEA_CORE_EINJ_

//
// Includes
//

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/WheaPlatformHooksLib.h>
#include <Library/HobLib.h>
#include <Library\PciLib.h>
#include <Library\PciExpressLib.h>

#include <Protocol\SmmSwDispatch2.h>
#include <Protocol\SmmBase2.h>
#include <Protocol\WheaPlatformSupport.h>
#include <Protocol\MpService.h>

#include <RasPolicy.h>
#include <Include/SetupVariable.h>
#include <Include/WheaPlatformDefs.h>

// WHEA error injection action definitions
#define WHEA_EINJ_BEGIN_INJECTION_OPERATION                     0x00
#define WHEA_EINJ_GET_TRIGGER_ACTION_TABLE                      0x01
#define WHEA_EINJ_SET_ERROR_TYPE                                0x02
#define WHEA_EINJ_GET_ERROR_TYPE                                0x03
#define WHEA_EINJ_END_OPERATION                                 0x04
#define WHEA_EINJ_EXECUTE_OPERATION                             0x05
#define WHEA_EINJ_CHECK_BUSY_STATUS                             0x06
#define WHEA_EINJ_GET_COMMAND_STATUS                            0x07
#define WHEA_EINJ_SET_ERROR_TYPE_WITH_ADDRES                    0x08
#define WHEA_EINJ_TRIGGER_ERROR                                 0xFF
#define WHEA_EINJ_TRIGGER_ACTION_MAX                            4


// Error injection operation definitions
#define EINJ_BEGIN_OPERATION                                    0x55AA55AA
#define EINJ_END_OPERATION                                      0x00000000

// BIOS runtime Address range used for error injection
#define WHEA_EINJ_ADDR_RANGE                                    0x4000
#define CACHE_LINE_SIZE                                         64

// Error injection operation status
#define WHEA_EINJ_OPERATION_BUSY                                0x01
#define WHEA_EINJ_CMD_SUCCESS                                   0x00
#define WHEA_EINJ_CMD_UNKNOWN_FAILURE                           0x01
#define WHEA_EINJ_CMD_INVALID_ACCESS                            0x02

// EINJ - SET_ERROR_TYPE_WITH_ADDRESS Data Structure flags
#define WHEA_SET_ERROR_TYPE_WITH_PROC_APIC_VALID                0x01
#define WHEA_SET_ERROR_TYPE_WITH_MEMORY_ADDRESS_VALID           0x02
#define WHEA_SET_ERROR_TYPE_WITH_SBDF_VALID                     0x04

// Port for triggering software SMI
#define R_APM_CNT                                               0xB2

// Error injection SW SMI defintions
#define EFI_WHEA_EINJ_EXECUTE_SWSMI                             0x9A

//
// Dunit Addr
//
#define DUNIT0_ADDR                                             0x100800
#define DUNIT1_ADDR                                             0x11A540

//
// WHEA Logging definitions
//
#define NONFATAL_ERROR_TYPE       0x01
#define FATAL_ERROR_TYPE          0x02
#define COR_ERROR_TYPE            0x03
#define SERROR_TYPE               0x04
#define PERROR_TYPE               0x05


#pragma pack (1)

//
// PCIe Info for fake log
//
typedef struct {
  UINT8       ErrorSource;        // PCI or CSI or Mem or Processor etc..
  UINT8       ErrorType;          // PCI
  UINT8       Segment;            // Segment
  UINT8       Bus;                // Bus
  UINT8       Device;             // Device
  UINT8       Function;           // Func
  UINT16      Reg;                // Register Number
  UINT64      Value;              // Value
  UINT16      OEMEvData1;         // More error info
} PCI_PCIE_ERROR_LOCAL_STRUCT;

typedef struct {
  UINT32                HeaderSize;
  UINT32                Revision;
  UINT32                TableSize;
  UINT32                EntryCount;
} WHEA_ERROR_TRIGGER_ACTION_HEADER;

// Error Trigger Action table is a series of injection instructoin entries
typedef struct {
  WHEA_ERROR_TRIGGER_ACTION_HEADER              Header;
  EFI_ACPI_WHEA_SERIALIZATION_ACTION            Trigger0;
  EFI_ACPI_WHEA_SERIALIZATION_ACTION            Trigger1;
  EFI_ACPI_WHEA_SERIALIZATION_ACTION            Trigger2;
  EFI_ACPI_WHEA_SERIALIZATION_ACTION            Trigger3;
  //Add more entries if required
} WHEA_EINJ_TRIGGER_ACTION_TABLE;

typedef struct {
  UINT64                Busy:1;
  UINT64                CmdSts:8;
  UINT64                Resv1:55;
} WHEA_EINJ_OP_STATUS;

typedef struct {
  UINT32		ErrType;
  UINT32		VendorErrTypeExtOffset;
  UINT32		Flags;
  UINT32		ApicId;
  UINT64		MemAddress;
  UINT64		MemAddressRange;
  UINT32		PcieSBDF;
} WHEA_EINJ_SET_ERROR_TYPE_WITH_ADDRESS;

typedef struct {
  UINT32                Length;		
  UINT32                SBDF;		// This provides a PCIe Segment, Bus, Device and Function number which can be used to read the Vendor ID, Device ID and Rev ID, 
					                // so that software can identify the system for error injection purposes            
  UINT16                VendorId;
  UINT16                DeviceId;
  UINT8                 RevId;
  UINT8                 Resvd[3];
  UINT64		        OemDefType1;
  UINT64		        OemDefType2;
} WHEA_EINJ_VENDOR_EXT;

typedef struct {
  UINT64                                 OpState;            // opeartion state, begin/end/etc
  UINT64                                 ErrorToInject[5];
  UINT64                                 TriggerActionTable;
  UINT64                                 ErrorInjectCap;     // Qowrd0 -  Eroor injection cap.
  UINT64                                 OpStatus;
  UINT64                                 CmdStatus;
  UINT64                                 EinjAddr;
  UINT64                                 AddrMask;
  WHEA_EINJ_SET_ERROR_TYPE_WITH_ADDRESS  SetAddressTable;
  WHEA_EINJ_VENDOR_EXT                   VendorExt;
} WHEA_EINJ_PARAM_BUFFER;

#pragma pack ()

#define EinjRegisterExecute { \
  EFI_ACPI_3_0_SYSTEM_IO,     \
  16                          \
  0,                          \
  EFI_ACPI_3_0_BYTE,          \
  0xB2}

#define EinjRegisterFiller  { \
  EFI_ACPI_3_0_SYSTEM_MEMORY, \
  64,                         \
  0,                          \
  EFI_ACPI_3_0_QWORD,         \
  0}

#define EinjRegisterActionFiller  { \
  EFI_ACPI_3_0_SYSTEM_IO,           \
  0x10,                             \
  0x00,                             \
  0x01,                             \
  R_APM_CNT}

#define EC_BASE                  0xE0000000
#define MAX_BOXTYPE_INSTANCES    2
#define NA                       0xFF
#define MAX_CHANNELS             2

//
// All boxtypes supported by the MSP. Boxtypes are distinct components of silicon.
//
#define DUNIT_COMMON                                       0x00
#define DUNIT                                              0x01
#define DDRDQ                                              0x02
#define DDRDQ_PHY                                          0x03
#define DDRCC2_PHY                                         0x04
#define DDRCC1_PHY                                         0x05
#define DDRPLL                                             0x06
#define DDRCC0                                             0x07
#define DDRCC1                                             0x08
#define DDRCC2                                             0x09
#define DDRSAI                                             0x0A
#define BUNIT                                              0x0B
#define RTF                                                0x0C
#define CPGC                                               0x0D
#define BUNIT_MCHBAR                                       0x0E
#define AUNIT_MCHBAR                                       0x0F
#define BUNITMEM                                           0x10
#define PUNITSA                                            0x11
#define PMC_PCI_MMR                                        0x12
#define PMC_PCI_CFG                                        0x13
#define CLTT_CCR                                           0x14
#define SUNIT                                              0x15
#define AUNIT_MSG_MAP_1                                    0x16
#define BUNIT_MSG_MAP_0                                    0x17
#define MAX_BOXES                                          0x18
#define MAX_INSTANCE_PORTS                                 58


//
// Register access methods (sideband, PCI, etc)
//
#define eSB                                                0
#define ePCI                                               1
#define eBAR                                               2
#define eACCESS_METHOD_ERROR                               3

typedef struct {
  UINT8  TotalInstances;
  UINT8  Instances;
  UINT8  InstanceStartIndex;
  UINT8  ReadOperation;
  UINT8  WriteOperation;
  UINT8  AccessMethod;
  UINT32  AccessBase;
  UINT8  SimulationSupported;
  UINT32 StringIndex;
} INSTANCE_PORT_MAP;

#define MCH_BASE_ADDRESS  0xFED10000
#define P2SB_BASE_ADDRESS 0xFD000000
#define PMC_BASE_ADDRESS  0xFE000000


typedef struct {
  UINT16 Port;
  UINT16 Offset;
} INSTANCE_PORT_OFFSET;

typedef struct {
  UINT32  Offset;           // Register Offset relative to the base address.
  UINT64  Mask;             // Starting bit within the register.
  UINT8   ShiftBit;         // Number of bits to shift to get to the register field in question
} REGISTER_ACCESS;

typedef enum {
  NoError,
  InvalidInstance,
  InvalidBoxType,
  InvalidMode,
  BoxTypeNotInSimulation,
  InvalidRemapType,
  UnknownError
} ACCESS_ERROR_TYPE;

typedef enum {
  ModeRead,
  ModeWrite,
  ModeOther
} ACCESS_MODE;

typedef union {
  struct {
    UINT32   injen     :1; //  Error Injection Enable: 0 - Error injection is disabled 1 - Error injection is enabled. 
                           // This bit is self clearing when inject once is selected and the single error injection has been performed. 
                           // This bit should be set to 0 when INJTRIGSEL is set to 1X. Upon assertion, the trigger will automatically set the INJEN field.
    UINT32   injonce   :1;
    UINT32   injadren  :1;
    UINT32   injsrcen  :1;
    UINT32   injecc    :1;
    UINT32   injwrite  :1;
    UINT32   injread   :1;
    UINT32   injpat    :1;
    UINT32   injsrc    :4;
    UINT32   injcap    :2;
    UINT32   injtrigsel:2;
    UINT32   injaddr3  :1;
    UINT32   injamsk3  :1;
    UINT32   injaddr4  :1;
    UINT32   injamsk4  :1;
    UINT32   rsvd3     :4;
    UINT32   injeccpar :8;
  } Bits;
  UINT32 Data;
} ERRINJCTL;

typedef union {
  struct {
    UINT32 eccen          : 1;
    UINT32 serren         : 1;
    UINT32 derren         : 1;
    UINT32 rsvd0_deccctrl : 1;
    UINT32 dparen         : 1;
    UINT32 uce_filter     : 3;
    UINT32 eccovr         : 8;
    UINT32 eccovren       : 1;
    UINT32 encbgen        : 1;
    UINT32 rsvd1_deccctrl : 6;
    UINT32 sbecnt0clr     : 1;
    UINT32 sbecnt1clr     : 1;
    UINT32 sbecnt2clr     : 1;
    UINT32 sbecnt3clr     : 1;
    UINT32 rsvd_58        : 4;
  } Bits;
  UINT32 Data;
} ECCCTRL;

#define R_ECCCTRL                            0x180

#define R_ERRINJCTL                          0x134
#define R_ERRINJADDR                         0x138
#define R_ERRINJMSK                          0x13C
#define R_ERRINJDATA0                        0x140
#define R_ERRINJDATA1                        0x144


#define  RPX16_PCIE_BUS                      00
#define  RPX16_PCIE_DEVICE                   9
#define  RPX16_PCIE_FUNC                     00
#define  RPX16_PCIE_MAX_DEVICE               4

#define  RPX4_PCIE_BUS                       00
#define  RPX4_PCIE_DEVICE                    14
#define  RPX4_PCIE_FUNC                      00
#define  RPX4_PCIE_MAX_DEVICE                4

#define R_PCIE_AER_ERRUNCSEV                 0x10C
#define   B_PCIE_ERRUNCSEV_CTES              BIT14

#define  R_PCIE_VID                          0x00
#define  R_PCIE_EINJCTL                      0x188
#define    B_EINJDIS                         BIT0  //Injection disable
#define    B_INJRCVERR                       BIT1  //Inject Receive error                //correctable
#define    B_INJCTOERR                       BIT2  //Inject completion timeout error     //un correctable

VOID *
EFIAPI
AsmFlushCacheLine (
  IN      VOID                      *LinearAddress
  );

VOID *
EFIAPI
AsmCommitLine (
  IN VOID    *LinearAddress,
  IN UINT32  Value
  );


VOID
AsmWbinvd (
  VOID
  );

VOID
AsmNop (
  VOID
  );

#endif  //_WHEA_CORE_EINJ_
