/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  Null instance of Sec Platform Hook Lib.

  Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.


**/

//
// The package level header files this module uses
//
#include "PlatformSecLib.h"

// Denverton AptioV Override Start - EIP#303338
#include <CpuRegs.h>
// Denverton AptioV Override End - EIP#303338

/**
  Perform those platform specific operations that are requried to be executed as early as possibile.
  
  @return TRUE always return true.
**/
EFI_STATUS
EFIAPI
PlatformSecLibConstructor (
  )
{
  BOOLEAN                       DefaultsRestored;

  //
  // Init Apic Timer for Performance collection.
  // Use EXCEPT_IA32_BOUND as interrupte type.
  //
  PERF_CODE (
    InitializeApicTimer (0, (UINT32) -1, TRUE, 5);
  );

  DefaultsRestored = FALSE;
  
  //
  // Perform a checksum computation and verify if the checksum is correct. If the checksum is incorrect
  // initialize all the CMOS location to their default values and recalculate the checksum.
  //
  //InitCmos (FALSE, &DefaultsRestored);

  return EFI_SUCCESS;
}

/**
  A developer supplied function to perform platform specific operations.

  It's a developer supplied function to perform any operations appropriate to a
  given platform. It's invoked just before passing control to PEI core by SEC
  core. Platform developer may modify the SecCoreData and PPI list that is
  passed to PEI Core. 

  @param  SecCoreData           The same parameter as passing to PEI core. It
                                could be overridden by this function.
  @param  PpiList               The default PPI list passed from generic SEC
                                part.

  @return The final PPI list that platform wishes to passed to PEI core.

**/
EFI_PEI_PPI_DESCRIPTOR *
EFIAPI
SecPlatformMain (
  IN OUT   EFI_SEC_PEI_HAND_OFF        *SecCoreData,
  IN       EFI_PEI_PPI_DESCRIPTOR      *PpiList
  )
{
  return NULL;
}


/**
  This interface conveys state information out of the Security (SEC) phase into PEI.

  @param  PeiServices               Pointer to the PEI Services Table.
  @param  StructureSize             Pointer to the variable describing size of the input buffer.
  @param  PlatformInformationRecord Pointer to the EFI_SEC_PLATFORM_INFORMATION_RECORD.

  @retval EFI_SUCCESS           The data was successfully returned.
  @retval EFI_BUFFER_TOO_SMALL  The buffer was too small.

**/
EFI_STATUS
EFIAPI
SecPlatformInformation (
  IN CONST EFI_PEI_SERVICES                      **PeiServices,
  IN OUT   UINT64                                *StructureSize,
  OUT      EFI_SEC_PLATFORM_INFORMATION_RECORD   *PlatformInformationRecord
  )
{
  UINT32  *BIST;
  UINT32  Size;
  UINT32  Count;
  UINT32  *TopOfCar;

  //
  // The entries of BIST information, together with the number of them,
  // reside in the bottom of stack, left untouched by normal stack operation.
  // This routine copies the BIST information to the buffer pointed by
  // PlatformInformationRecord for output.
  //
  TopOfCar = (UINT32 *)(PcdGet32 (PcdTemporaryRamBase) + PcdGet32 (PcdTemporaryRamSize));
  Count = *(TopOfCar - 1);
  
  // ECPoverride
  //Size  = Count * sizeof (EFI_SEC_PLATFORM_INFORMATION_RECORD);
  Size  = Count * sizeof (IA32_HANDOFF_STATUS);

  if ((*StructureSize) < (UINT64) Size) {
    *StructureSize = Size;
    return EFI_BUFFER_TOO_SMALL;
  }

  *StructureSize  = Size;
  BIST            = (UINT32 *)((UINTN)TopOfCar - sizeof (UINT32) - Size);

  CopyMem (PlatformInformationRecord, BIST, Size);

  return EFI_SUCCESS;
}

// Denverton AptioV Override Start - EIP#303338
/**
  This interface disables temporary memory in SEC Phase.
**/
VOID
EFIAPI
SecPlatformDisableTemporaryMemory (  
  VOID
  )
{
  UINT64  MsrValue;
  UINT64  MtrrDefaultType;

  //
  // Force and INVD.
  //
  AsmInvd ();

  //
  // Disable MTRRs.
  //
  MtrrDefaultType = AsmReadMsr64 (EFI_MSR_CACHE_IA32_MTRR_DEF_TYPE);
  MsrValue = MtrrDefaultType & ~((UINT64)BIT11);
  AsmWriteMsr64 (EFI_MSR_CACHE_IA32_MTRR_DEF_TYPE, MsrValue);
  
  //
  // Clear NEM Run and NEM Setup bits individually.
  //
  MsrValue = AsmReadMsr64 (EFI_MSR_NO_EVICT_MODE);
  MsrValue &= ~((UINT64) BIT1);
  AsmWriteMsr64 (EFI_MSR_NO_EVICT_MODE, MsrValue);
  MsrValue &= ~((UINT64) BIT0);
  AsmWriteMsr64 (EFI_MSR_NO_EVICT_MODE, MsrValue);
  
  //
  // Restore MTRR default setting
  //
  AsmWriteMsr64 (EFI_MSR_CACHE_IA32_MTRR_DEF_TYPE, MtrrDefaultType);
}
// Denverton AptioV Override End - EIP#303338
