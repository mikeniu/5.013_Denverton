/** @file
  CPU Platform Lib implementation.

@copyright
 Copyright (c) 2012 - 2016 Intel Corporation. All rights reserved
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
#include "CpuPlatformLibrary.h"
#include <Library/MmPciLib.h>
#include <SaRegs.h>

/**
  This function is to program Trace Hub ACPI base address to processor's MSR TRACE_HUB_STH_ACPIBAR_BASE.

  @param[in]  TraceHubAcpiBase - Base address of Trace Hub ACPI Base address
**/
VOID
EFIAPI
CpuWriteTraceHubAcpiBase (
  IN UINT64  TraceHubAcpiBase
  )
{
  UINT64        Microcode;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  //
  // Check the pass in Trace Hub ACPI base if 256KB alignment.
  //
  if ((TraceHubAcpiBase & (UINT64) V_EFI_MSR_TRACE_HUB_STH_ACPIBAR_BASE_MASK) != 0) {
    ASSERT ((TraceHubAcpiBase & (UINT64) V_EFI_MSR_TRACE_HUB_STH_ACPIBAR_BASE_MASK) == 0);
    return;
  }

  //
  // Check if the microcode supports the Trace Hub ACPI base MSR.
  //
  Microcode = AsmReadMsr64 (EFI_MSR_UCODE_CR_PATCH_SIGN_ID) & B_EFI_MSR_UCODE_CR_PATCH_SIGN_ID_VERSION_MASK;
  // Denverton AptioV override start - EIP#268350
  // Microcode >>= N_EFI_MSR_UCODE_CR_PATCH_SIGN_ID_VERSION;
  //DEBUG ((EFI_D_INFO, "\tUCODE_CR_PATCH_SIGN_ID = 0x%08x\tValue = 0x%08x",EFI_MSR_UCODE_CR_PATCH_SIGN_ID,(UINT32)(AsmReadMsr64 (EFI_MSR_UCODE_CR_PATCH_SIGN_ID)>>32)));
  Microcode = RShiftU64(Microcode, N_EFI_MSR_UCODE_CR_PATCH_SIGN_ID_VERSION);
  DEBUG ((EFI_D_INFO, "\tUCODE_CR_PATCH_SIGN_ID = 0x%08x\tValue = 0x%08x",EFI_MSR_UCODE_CR_PATCH_SIGN_ID,(UINT32)RShiftU64(AsmReadMsr64 (EFI_MSR_UCODE_CR_PATCH_SIGN_ID),32)));
  // Denverton AptioV override end - EIP#268350
  DEBUG ((EFI_D_INFO, "%08x\n",AsmReadMsr32 (EFI_MSR_UCODE_CR_PATCH_SIGN_ID)));
  if (Microcode >= V_EFI_MSR_UCODE_CR_PATCH_SIGN_ID_B5073167) {
    //
    // Set MSR TRACE_HUB_STH_ACPIBAR_BASE[0] LOCK bit for the AET packets to be directed to NPK MMIO.
    //
    TraceHubAcpiBase += V_EFI_MSR_TRACE_HUB_STH_ACPIBAR_BASE_OFFSET;
    AsmWriteMsr64 (EFI_MSR_TRACE_HUB_STH_ACPIBAR_BASE, TraceHubAcpiBase | B_EFI_MSR_TRACE_HUB_STH_ACPIBAR_BASE_LOCK);
    DEBUG ((EFI_D_INFO, "\tTRACE_HUB_STH_ACPIBAR_BASE = 0x%08x\tValue = 0x%08x\n",EFI_MSR_TRACE_HUB_STH_ACPIBAR_BASE,AsmReadMsr64 (EFI_MSR_TRACE_HUB_STH_ACPIBAR_BASE)));
  }
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return;
}
