/** @file
  Platform Stage1 header file

@copyright
  Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may 
  be modified by the user, subject to additional terms of the license agreement.
**/


#ifndef _EFI_PLATFORM_STAGE1_H_
#define _EFI_PLATFORM_STAGE1_H_

#include <Base.h>
#include <Library/PeiServicesLib.h>

#include <Guid/PlatformInfo.h>
#include <Base.h>
#include <Framework/PeiCis.h>
#include <Ppi/BaseMemoryTest.h>
#include <Ppi/Stall.h>
#include <Ppi/FirmwareVolumeInfo.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <EfiFlashMap.h>
#include <Guid/FlashMapHob.h>
//#include <SaCommonDefinitions.h>
#include <PlatformDefinitions.h>
#include <Library/PeiPolicyInitLib.h>
#include <IndustryStandard/pci22.h>

#include <Ppi/PchPolicy.h>
#include <Library/PcdLib.h>

#include "FirwmareConfigurations.h"

#pragma pack(1)
typedef struct {
  UINT32  RegEax;
  UINT32  RegEbx;
  UINT32  RegEcx;
  UINT32  RegEdx;
} EFI_CPUID_REGISTER;
#pragma pack()
#include <PlatformBoardId.h>

#include <Uefi/UefiInternalFormRepresentation.h>

#include <Guid/FirmwareFileSystem.h>
#include <Guid/SystemNvDataHobGuid.h>
//#include <Guid/FastBootExceptionInfo.h>
//#include <Guid/FastBootFunctionEnabled.h>
#include <Guid/GlobalVariable.h>
#include <Guid/FirmwarePerformance.h>

#include <Ppi/Reset.h>
#include <Ppi/ReadOnlyVariable.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/SecPerformance.h>

#include <Protocol/FirmwareVolumeBlock.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
//#include <Library/PeiDxeSmmPchPlatformLib.h>
#include <Library/PeiPlatformHookLib.h>
#include <Library/PeiPlatformLib.h>
#include <Library/IoLib.h>
#include <Library/MultiPlatSupportLib.h>
#include <Library/PlatformCmosLib.h>
#include <Library/PciLib.h>
#include <Library/PlatformSecLib/PlatformSecLib.h>

#include <CmosMap.h>

#include <PchAccess.h>
#include <Library/MmPciLib.h>

//
// Defines for stall ppi
//
#define PEI_STALL_RESOLUTION    1 

//
// Used in PEI memory test routines
//
#define  MEMORY_TEST_COVER_SPAN         0x40000
#define  MEMORY_TEST_PATTERN            0x5A5A5A5A
#define  FAST_BOOT_OR_MASK              0x01


#define FAST_BOOT_WDT_RESET_SIGNATURE   0xFB

#define ADV_DEBUG_AUTO                  2
#define ADV_DEBUG_ENABLE                1
#define ADV_DEBUG_DISABLE               0

//
// Core Disable
//
#define PCH_PCR_CORE_EXISTS_VECTOR_MCHBAR                   0x7164
#define PCH_PCR_CORE_DISABLE_MASK_MCHBAR                    0x7168
#define PCH_PCR_CORE_DISABLE_MASK_VALUE						(BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define PCH_PCR_CORE_DISABLE_MASK_CORE0                     BIT0
#define PCH_PCR_CORE_DISABLE_MASK_CORE1                     BIT1
#define PCH_PCR_CORE_DISABLE_MASK_CORE2                     BIT2
#define PCH_PCR_CORE_DISABLE_MASK_CORE3                     BIT3
#define PCH_PCR_CORE_DISABLE_MASK_CORE4                     BIT4
#define PCH_PCR_CORE_DISABLE_MASK_CORE5                     BIT5
#define PCH_PCR_CORE_DISABLE_MASK_CORE6                     BIT6
#define PCH_PCR_CORE_DISABLE_MASK_CORE7                     BIT7
#define PCH_PCR_CORE_DISABLE_MASK_CORE8                     BIT8
#define PCH_PCR_CORE_DISABLE_MASK_CORE9                     BIT9
#define PCH_PCR_CORE_DISABLE_MASK_CORE10                    BIT10
#define PCH_PCR_CORE_DISABLE_MASK_CORE11                    BIT11
#define PCH_PCR_CORE_DISABLE_MASK_CORE12                    BIT12
#define PCH_PCR_CORE_DISABLE_MASK_CORE13                    BIT13
#define PCH_PCR_CORE_DISABLE_MASK_CORE14                    BIT14
#define PCH_PCR_CORE_DISABLE_MASK_CORE15                    BIT15

#define CORE_EXISTS_VECTOR_DEFAULT_VALUE                    0x00FF
#define MAX_NUM_MODULES                                     8
#define MAX_NUM_CORES_PMODULE                               2

#define ALL_CORES                                           0
#define ALL_CORES_ODDS                                      0xAAAA
#define ALL_CORES_PAIRS                                     0x5555
#define CORE_1                                              0xFFFE
#define CORE_2                                              0xFFFA
#define CORE_3                                              0xFFEA
#define CORE_4                                              0xFFAA
#define CORE_5                                              0xFEAA
#define CORE_6                                              0xFAAA
#define CORE_7                                              0xEAAA
#define CORE_8                                              0xAAAA
#define CORE_9                                              0xAAA8
#define CORE_10                                             0xAAA0
#define CORE_11                                             0xAA80
#define CORE_12                                             0xAA00
#define CORE_13                                             0xA800
#define CORE_14                                             0xA000
#define CORE_15                                             0x8000
#define CORE_16                                             0x0000

#define IoIn8(Port)         _inp (Port)
#define IoOut8(Port, Data)  _outp (Port, Data)

#define DUMMY_DEFAULT_VARIABLE_HOB_GUID  { 0xD3423BEF, 0xD6AD, 0x4C97, 0xB6, 0x27, 0x03, 0x80, 0xF4, 0x0E, 0x39, 0x05}


//
// Function Prototypes
//
/**
  The function configures the active cores in the system.

  @param[in]     ActiveCores            Indicate how many cores needs to be enable via the setup option
  @param[in]     CoreExitsMask          Indicate how many cores exits in the SoC
  @param[in out] CoreDisableMask        This variable has the actual number of cores enabled in the system

  @retval        EFI_SUCCESS            The required settings programmed successfully
**/
VOID
SetCoreDisableMask (
IN       UINT8   ActiveCores,
IN       UINT32  CoreExitsMask,
IN  OUT  UINT32  *CoreDisableMask
  );

/**
  The function performs Core Disable programming.

  @param[in] PchPolicyPpi       The PCH Policy PPI instance

  @retval    EFI_SUCCESS        The required settings programmed successfully
**/
EFI_STATUS
SetActiveCores (
  IN PCH_POLICY_PPI               *PchPolicyPpi
  );

EFI_STATUS
EFIAPI
PeiInitPlatformStage1 (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

EFI_STATUS
EFIAPI
BaseMemoryTest (
  IN  CONST EFI_PEI_SERVICES    **PeiServices,
  IN  PEI_BASE_MEMORY_TEST_PPI  *This, 
  IN  EFI_PHYSICAL_ADDRESS      BeginAddress,
  IN  UINT64                    MemoryLength,
  IN  PEI_MEMORY_TEST_OP        Operation,
  OUT EFI_PHYSICAL_ADDRESS      *ErrorAddress
  );

EFI_STATUS
EFIAPI
Stall (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN CONST EFI_PEI_STALL_PPI    *This,
  IN UINTN                      Microseconds
  );

EFI_STATUS
PeimInitializeFlashMap (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  );
  
EFI_STATUS
EFIAPI
GetAreaInfo (
  IN  CONST EFI_PEI_SERVICES    **PeiServices,
  IN  EFI_FLASH_AREA_TYPE       AreaType,
  IN  EFI_GUID                  *AreaTypeGuid,
  OUT UINT32                    *NumEntries,
  OUT EFI_FLASH_SUBAREA_ENTRY   **Entries
  );

EFI_STATUS
EFIAPI
PchReset(
  IN CONST EFI_PEI_SERVICES    **PeiServices
  );

BOOLEAN
GetSleepTypeAfterWakeup (
  OUT UINT32            *SleepType
  );

VOID
GetWakeupEventAndSaveToHob (
  VOID
  );

VOID
InitSata (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

VOID
EarlyPlatformPchInit (
  IN CONST EFI_PEI_SERVICES    **PeiServices
  );

VOID
PlatformStage1InitBootMode (
  IN CONST EFI_PEI_SERVICES **PeiServices
  );

EFI_STATUS
EFIAPI
PlatformStage1UpdateBootMode (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Interface
  );

BOOLEAN
EFIAPI
IsFastBootEnabled (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

EFI_STATUS
EFIAPI
FastBootVariableReadyNotify (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Interface
  );

BOOLEAN
FastBootEnabled (
  CONST EFI_PEI_SERVICES        **PeiServices
  );

BOOLEAN
IsOverclockingEnabled (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

#endif
