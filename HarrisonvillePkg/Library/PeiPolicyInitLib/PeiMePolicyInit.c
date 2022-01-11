/** @file

  This file is SampleCode for Intel ME PEI Policy initialzation.

@copyright
 Copyright (c) 2012 - 2014 Intel Corporation. All rights reserved
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
#include "PeiMePolicyInit.h"

//
// Function implementations
//
/**
  This PEIM performs ME PEI Policy initialzation.

  @param[in]  FirmwareConfiguration  It uses to skip specific policy init that depends
                                     on the 'FirmwareConfiguration' varaible.

  @retval     EFI_SUCCESS            The PPI is installed and initialized.
  @retval     EFI ERRORS             The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES   Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
PeiMePolicyInit (
  IN UINT8                     FirmwareConfiguration
  )
{
  EFI_STATUS                  Status;
  ME_POLICY_PPI               *MePolicyPpi;

  //
  // Call MeCreatePolicyDefaults to initialize platform policy structure
  // and get all intel default policy settings.
  //
  Status = MeCreatePolicyDefaults (&MePolicyPpi);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ///
  /// Put override codes in here if needs
  /// Update and override all platform related and customized settings below.
  ///
  UpdatePeiMePolicy (MePolicyPpi);
  if ((FirmwareConfiguration >= FwConfigDebug) && (FirmwareConfiguration < FwConfigMax)) {
    // Update Debug Policies
  }

  ///
  /// Install the ME PEI Policy PPI
  /// While installed, RC assumes the Policy is ready and finalized. So please
  /// update and override any setting before calling this function. See above comment as well.
  ///
  Status = MeInstallPolicyPpi (MePolicyPpi);

  return Status;
}
