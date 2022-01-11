/** @file
  This file is SampleCode for Intel CPU PEI Policy initialzation.

@copyright
 Copyright (c) 2010 - 2014 Intel Corporation. All rights reserved
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
#include "PeiCpuPolicyInit.h"

/**
  This PEIM performs CPU PEI Policy initialzation.

  @param[in]  FirmwareConfiguration  It uses to skip specific policy init that depends
                                     on the 'FirmwareConfiguration' varaible.

  @retval     EFI_SUCCESS           The PPI is installed and initialized.
  @retval     EFI ERRORS            The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES  No enough resoruces (such as out of memory).
**/
EFI_STATUS
EFIAPI
PeiCpuPolicyInit (
  IN UINT8                     FirmwareConfiguration
  )
{

  EFI_STATUS                        Status;
  SI_CPU_POLICY_PPI                 *SiCpuPolicyPpi;

  //
  // Call CpuCreatePolicyDefaults to initialize platform policy structure
  // and get all intel default policy settings.
  //
  Status = CpuCreatePolicyDefaults (&SiCpuPolicyPpi);
  ASSERT_EFI_ERROR (Status);
  
  UpdatePeiSiCpuPolicy (SiCpuPolicyPpi);
  if ((FirmwareConfiguration >= FwConfigDebug) && (FirmwareConfiguration < FwConfigMax)) {
    // Update Debug Policies
  }

  //
  // Install SiCpuPolicyPpi.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = CpuInstallPolicyPpi (SiCpuPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
