//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  2009 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  AcpiSmmPlatform.h

Abstract:

  Header file for SMM S3 Handler Driver.

--*/

#ifndef _ACPI_SMM_DRIVER_H
#define _ACPI_SMM_DRIVER_H
//
// Include files
//
#include <Protocol/SmmVariable.h>
//
// Driver Consumed Protocol Prototypes
//
#include <Protocol\SmmSxDispatch2.h>
#include <Protocol\SmmSwDispatch2.h>
#include <Protocol\FirmwareVolume.h>
#include <Protocol\GlobalNvsArea.h>
#include <Protocol\PciRootBridgeIo.h>
#include <Library\UefiDriverEntryPoint.h>
#include <Library\IoLib.h>
#include <Library\PciLib.h>
#include <Library\DebugLib.h>
#include <Library\BaseMemoryLib.h>
#include <Library\UefiBootServicesTableLib.h>
#include <Library\BaseLib.h>
#include <Library\UefiRuntimeServicesTableLib.h>
#include <Guid\Acpi.h>
#include <Guid\GlobalVariable.h>
#include <Library/SmmServicesTableLib.h>
#include <Include/SetupVariable.h>
#include <Library/ReportStatusCodeLib.h>
#include <Guid\SmramMemoryReserve.h>
#include <Guid\AcpiVariable.h>
#include <Library/HobLib.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/SmmCpuSaveState.h>
#include <PchAccess.h>

#define RTC_ADDRESS_REGISTER_D    13

#define PCI_DEVICE(Bus, Dev, Func)  \
          Bus, Dev, Func

#define PCI_REG_MASK(Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7) \
          Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7

#define PCI_DEVICE_END    0xFF

//
// BugBug -- Need to get these two values from acpi.h, but right now, they are
//           declared in platform-specific variants of this file, so no easy
//           way to pick-up the include file and work across platforms.
//           Need these definitions to go into a file like common\acpi.h.
//
#define ACPI_ENABLE                 0xA0
#define ACPI_DISABLE                0xA1
#define PCAT_RTC_ADDRESS_REGISTER   0x70
#define PCAT_RTC_DATA_REGISTER      0x71
#define RTC_ADDRESS_REGISTER_D      13

//
// Variable name for saving Patrol Scrub Address on S3
//
#define PATROL_SCRUB_ADDR_VAR_NAME  L"PatrolScrubAddr"

//
// Related data structures definition
//
typedef struct _EFI_ACPI_SMM_DEV {

  EFI_PHYSICAL_ADDRESS   RuntimeScriptTableBase;

  //
  // Parent dispatch driver returned sleep handle
  //
  EFI_HANDLE  S3SleepEntryHandle;

  EFI_HANDLE  S4SleepEntryHandle;

  EFI_HANDLE  S1SleepEntryHandle;

  EFI_HANDLE  S5SoftOffEntryHandle;

  EFI_HANDLE  EnableAcpiHandle;

  EFI_HANDLE  DisableAcpiHandle;

  EFI_HANDLE  PpCallbackHandle;

  EFI_HANDLE  MorCallbackHandle;

  //
  // PCH Power Management I/O register base
  //
  UINT32      PchPmBase;

  UINT32      BootScriptSaved;

} EFI_ACPI_SMM_DEV;


//
// Prototypes
//
EFI_STATUS
InitPlatformAcpiSmm (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable,
  IN  OUT VOID            *CommBuffer,
  IN  OUT UINTN           *CommBufferSize
  );

EFI_STATUS
SxSleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  );

EFI_STATUS
DisableAcpiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  );

EFI_STATUS
EnableAcpiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  );

EFI_STATUS
RegisterToDispatchDriver (
  VOID
  );

EFI_STATUS
GetPchPmBase (
  IN EFI_SMM_SYSTEM_TABLE2       *Smst
  );

EFI_STATUS
SaveRuntimeScriptTable (
  IN EFI_SMM_SYSTEM_TABLE2       *Smst
  );

#endif
