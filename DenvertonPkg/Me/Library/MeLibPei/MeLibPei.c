/** @file
  This is a library for ME functionality.

@copyright
  Copyright (c) 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#include <PiPei.h>

#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/MeLibPei.h>
#include <MeChipset.h>
#include <MeSetup.h>
#include "Library/MeTypeLib.h"
#ifdef AMT_SUPPORT
#include <Library/AmtLibPei.h>
#endif // AMT_SUPPORT



/**

  @brief
  Enable/Disable All ME devices based on Policy

  @param[in] WhichDevice            Select of Me device
  @param[in] PeiMePlatformPolicyPpi Pointer to ME Policy PPI


**/
EFI_STATUS
MeDeviceConfigure(
  PEI_ME_PLATFORM_POLICY_PPI    *PeiMePlatformPolicyPpi
  )
{

  if (PeiMePlatformPolicyPpi == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG((EFI_D_INFO, "[ME] Disabling ME functions:"));
  if (PeiMePlatformPolicyPpi->HeciCommunication1 == SETUP_HECI_DEVICE_DISABLED) DEBUG((EFI_D_INFO, " 0 (HECI-1)"));
  if (PeiMePlatformPolicyPpi->HeciCommunication2 == SETUP_HECI_DEVICE_DISABLED) DEBUG((EFI_D_INFO, " 1 (HECI-2)"));
  if (PeiMePlatformPolicyPpi->HeciCommunication3 == SETUP_HECI_DEVICE_DISABLED) DEBUG((EFI_D_INFO, " 4 (HECI-3)"));
  if (PeiMePlatformPolicyPpi->IderEnabled == SETUP_HECI_DEVICE_DISABLED)        DEBUG((EFI_D_INFO, " 2 (IDE-R)"));
  if (PeiMePlatformPolicyPpi->SolEnabled == SETUP_HECI_DEVICE_DISABLED)         DEBUG((EFI_D_INFO, " 3 (KT)"));
  DEBUG((EFI_D_INFO, "\n"));

  MeDeviceControl(HECI1, PeiMePlatformPolicyPpi->HeciCommunication1);
  MeDeviceControl(HECI2, PeiMePlatformPolicyPpi->HeciCommunication2);
  MeDeviceControl(HECI3, PeiMePlatformPolicyPpi->HeciCommunication3);
  MeDeviceControl(IDER, PeiMePlatformPolicyPpi->IderEnabled);
  MeDeviceControl(SOL, PeiMePlatformPolicyPpi->SolEnabled);

  return EFI_SUCCESS;
} // MeDeviceConfigure

/**
  This function performs basic initialization for ME
  in PEI phase after memory is initialized.
**/
VOID
EFIAPI
MePostMemInit (
  VOID
  )
{
  PEI_ME_PLATFORM_POLICY_PPI      *PeiMePlatformPolicyPpi = NULL;
  EFI_STATUS                      Status;

#ifdef AMT_SUPPORT
  if (MeTypeIsAmt()) {
    MePostMemInitAmt();
  }
#endif // AMT_SUPPORT

  Status = PeiServicesLocatePpi (
                            &gPeiMePlatformPolicyPpiGuid,
                            0,
                            NULL,
                            &PeiMePlatformPolicyPpi
                            );
  if (!EFI_ERROR(Status)) {
    MeDeviceConfigure(PeiMePlatformPolicyPpi);
  }
}

#ifndef AMT_SUPPORT
/**
  ME End of PEI callback function. Configure HECI devices on End Of Pei
**/
VOID
MeOnEndOfPei(VOID)
{
}
#endif // ~AMT_SUPPORT
