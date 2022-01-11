/**
 This file contains an 'Intel Peripheral Driver' and is
 licensed for Intel CPUs and chipsets under the terms of your
 license agreement with Intel or your vendor.  This file may
 be modified by the user, subject to additional terms of the
 license agreement
**/
/**
@copyright
Copyright (c) 1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:
  PlatformInfoPei.h

Abstract:
  Platform Info Driver.
**/

#ifndef _PLATFORM_INFO_PEI_H_
#define _PLATFORM_INFO_PEI_H_
#include <Uefi.h>
#include <Ppi/Stall.h>
#include <PlatformDefinitions.h>
#include <PlatformBoardId.h>
#include <Ppi/CpuIo.h>
#include <Ppi/PciCfg.h>
#include <Guid/PlatformInfo.h>


// BUGBUG: Disable warning messages 4090: 'function' : different 'const' qualifiers.
// The input PeiServices changed to CONST from EDK II.
// We need to transfter our function prototype to fit spec.
// In this time, we disabled warning before we transfer all library to EDK II.

#define DISABLE_WARNING_4090 1

#define SIZE_BOARD_NAME 32

#define ERROR_CODE_INVALID_PLATFORM_TYPE    0xBD10
#define ERROR_CODE_INVALID_PLATFORM_FLAVOR  0xBD20
#define ERROR_CODE_EC_NOT_PRESENT           0xBD30
#define ERROR_CODE_INVALID_SV_BOARD         0xBD40

// DNV_TODO: fix these GPIO assignments.
//
//  DT board detection
//       Bit3    Bit2    Bit1   Bit0    Board SKU
//        0       0       0      1      MohonPeak CRB
//        0       0       0      1      Tiger Cove CRB
// --------------------------------------------------------
//

#define MohonPeakCrb 0x01
#define TigerCoveCrb 0x03
#define OstrichBaySv 0x04

EFI_STATUS
PlatformInfoPeiInit (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  );

EFI_STATUS
GetBoardId (
  IN  CONST EFI_PEI_SERVICES **PeiServices,
  IN  PLATFORM_INFO_HOB      *PlatformInfoHob,
  IN  EFI_PEI_STALL_PPI      *StallPpi,
  OUT UINT16                 *BoardId
  );

EFI_STATUS
UpdateBoardType (
  IN EFI_PEI_SERVICES **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID *Ppi
);

EFI_STATUS
DetectTridentIds (
   IN OUT PLATFORM_INFO_HOB  *PlatformInfoHob
);

EFI_STATUS
DetectDebugMode (
   IN OUT PLATFORM_INFO_HOB *PlatformInfoHob
);

EFI_STATUS
DetectNvClear(
   IN OUT PLATFORM_INFO_HOB *PlatformInfoHob
);

EFI_STATUS
DetectMfgMode (
   IN OUT PLATFORM_INFO_HOB *PlatformInfoHob
);
EFI_STATUS
BuildSiliconCFIO (
    OUT UINT32  *CFIOConfigRegisters
);
EFI_STATUS
ReadSysctrlCFIO(
  IN  UINT16   StartOffset,
  IN  UINT8   Length,
  OUT UINT32  *CFIOConfigRegisters
);
VOID SysCtrlWriteBiosID();
VOID SysControlConfigComunication();
VOID PrintSysCtrlVersion();
#endif
