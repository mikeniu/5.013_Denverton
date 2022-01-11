/** @file
  This file is SampleCode of the library for Intel PCH PEI Debug Policy initialization.

@copyright
  Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor. This file may be modified by the user, subject to
  the additional terms of the license agreement.

@par Specification Reference:
**/
#include <Library/PeiPolicyUpdateLib/PeiPchPolicyUpdate.h>
#include <PiPei.h>
#include <SetupVariable.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiServicesLib.h>
#include <PchAccess.h>
#include <Ppi/PchPolicy.h>
#include <Library/PchInfoLib.h>

/**
  This function performs PCH PEI Debug Policy initialization.

  @param[in] PchPolicy       The PCH Policy PPI instance

  @retval EFI_SUCCESS        The PPI is installed and initialized.
  @retval EFI ERRORS         The PPI is not successfully installed.
**/
EFI_STATUS
EFIAPI
UpdatePeiPchPolicyDebug (
  IN OUT  PCH_POLICY_PPI  *PchPolicy
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;
  UINT8                           Index;
  PCH_SETUP                       PchSetup;

  //
  // Retrieve Setup variable
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
             0,                                // INSTANCE
             NULL,                             // EFI_PEI_PPI_DESCRIPTOR
             &VariableServices                 // PPI
             );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (PCH_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"PchSetup",
                               &gPchSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &PchSetup
                               );
  ASSERT_EFI_ERROR (Status);


  //
  // DCI
  //
  PchPolicy->DciConfig.DciEn         = PchSetup.PchDciEn;
  PchPolicy->DciConfig.DciAutoDetect = PchSetup.PchDciAutoDetect;


  //
  // SATA Config
  //
  for (Index = 0; Index < PCH_MAX_SATA_CONTROLLERS; Index++)
  {
    PchPolicy->SataConfig[Index].TestMode = PchSetup.SataTestMode[Index];
  }

  //
  // LockDown
  //
  PchPolicy->LockDownConfig.RtcLock       = PchSetup.PchRtcLock;




  return Status;
}
