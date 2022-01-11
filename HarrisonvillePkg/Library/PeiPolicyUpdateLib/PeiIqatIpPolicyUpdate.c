/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c) 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PchPlatformPolicyUpdatePei.c

Abstract:

  This file is SampleCode of the library for Intel PCH PEI Platform Policy initialzation.

--*/
#include "PeiIqatIpPolicyUpdate.h"

#ifdef ECP_FLAG
EFI_GUID gEfiGlobalVariableGuid = EFI_GLOBAL_VARIABLE_GUID;
#endif

EFI_STATUS
EFIAPI
UpdatePeiIqatIpPolicy (
  IN OUT      IQAT_IP_GLOBAL_POLICY_PPI       *IqatIpGlobalPolicyPpi
  )
/*++

Routine Description:

  This function performs IQAT IP PEI Platform Policy initialization.

Arguments:

  IqatIpPolicy            The IQAT IP Platform Policy PPI instance

Returns:

  EFI_SUCCESS             The PPI is installed and initialized.
  EFI ERRORS              The PPI is not successfully installed.
  EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

--*/
{

  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;
  EFI_GUID                        SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  SYSTEM_CONFIGURATION            SetupVariables;
  EFI_STATUS					  Status = EFI_SUCCESS;

  VariableSize = sizeof (SYSTEM_CONFIGURATION);

//
// Retrieve Setup variable
//
  Status = PeiServicesLocatePpi (
                            &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
                            0,                            // INSTANCE
                            NULL,                         // EFI_PEI_PPI_DESCRIPTOR
                            &VariableServices             // PPI
                            );
  ASSERT_EFI_ERROR (Status);
  // Denverton Aptiov override Start - EIP#217439
  Status = VariableServices->GetVariable (
                              VariableServices,
                              L"IntelSetup",
                              &SystemConfigurationGuid,
                              NULL,
                              &VariableSize,
                              &SetupVariables
                              );
  // Denverton Aptiov override End - EIP#217439
  ASSERT_EFI_ERROR (Status);

  //
  // IQAT config
  //
  IqatIpGlobalPolicyPpi->IqatIpConfig.Enable = SetupVariables.Iqat;
  IqatIpGlobalPolicyPpi->IqatIpConfig.SetFuseCtl   = SetupVariables.Iqat_SetFuseCtl;
  IqatIpGlobalPolicyPpi->IqatIpConfig.FuseCtlValue = SetupVariables.Iqat_FuseCtlValue;
  IqatIpGlobalPolicyPpi->IqatIpConfig.Set64BMrrMpl = SetupVariables.Iqat_Set64BMrrMpl;

  DEBUG ((DEBUG_INFO, "IQAT IP Policy ready!!\n"));

  return Status;
}
