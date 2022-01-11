/** @file
  This code provides a initialization of intel VT-d (Virtualization Technology for Directed I/O).

@copyright
  Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification Reference:
**/
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PostCodeLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciLib.h>
#include <SaAccess.h>
#include <IncludePrivate/Library/VtdInitLib.h>
#include <IncludePrivate/Library/PchRcLib.h>
#include <CpuRegs.h>
#include <SaDataHob.h>

/**
  Configure VT-d Base and capabilities.

  @param[in]   VTD_CONFIG                 VTD config block from SA Policy PPI
  @param[in]   SA_MISC_CONFIG             MISC config block from SA Policy PPI

  @retval EFI_SUCCESS     - Vtd initialization complete
  @exception EFI_UNSUPPORTED - Vtd is not enabled by policy
**/
EFI_STATUS
VtdInit (
  IN       VTD_CONFIG                        *Vtd
  )
{
  UINTN        McD0BaseAddress;
  UINT64       MchBar;
  UINT32       Data32Or;
  UINT32       VtdBase;
  UINT32       VtBarReg;
  SA_DATA_HOB  *SaDataHob;
  EFI_STATUS   Status;

  SaDataHob       = NULL;
  SaDataHob = GetFirstGuidHob (&gSaDataHobGuid);
  if (SaDataHob == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to retrieve SaDataHob for VT-d! VT-d cannot be enabled!\n"));
    return EFI_UNSUPPORTED;
  }

  McD0BaseAddress             = MmPciBase (SA_MC_BUS, 0, 0);
  MchBar                      = MmioRead32 (McD0BaseAddress + R_SA_MCHBAR) &~BIT0;
  MchBar                      += (UINTN)LShiftU64(MmioRead32 (McD0BaseAddress + R_SA_MCHBAR_HI), 32);
  VtBarReg                    = R_SA_MCHBAR_DEFVTDBAR_OFFSET;

  if ((!SaDataHob->VtdData.VtdEnable) || (MmioRead32 (McD0BaseAddress + R_SA_MC_CAPID0_A) & BIT23)) {
    DEBUG ((DEBUG_WARN, "VTd disabled or no capability!\n"));
    return EFI_UNSUPPORTED;
  }
  ///
  /// Check SA supports VTD and VTD is enabled in setup menu
  ///
  DEBUG ((DEBUG_INFO, "VTd enabled\n"));

  ///
  /// Enable VTd in PCH
  ///
  Status = PchPsfEnableVtd ();
  ASSERT_EFI_ERROR (Status);

  ///
  /// 14.1 Program Remap Engine Base Address
  /// Configure VTD BAR
  ///
  Data32Or = SaDataHob->VtdData.BaseAddress[0];
  Data32Or |= 0x1;
  MmioWrite32 ((UINTN)(MchBar + R_SA_MCHBAR_DEFVTDBAR_OFFSET), Data32Or);
  VtdBase = MmioRead32 ((UINTN)(MchBar + VtBarReg)) & ~BIT0;
  DEBUG ((DEBUG_INFO, "VtdBase = %x\n", VtdBase));

  return EFI_SUCCESS;
}
