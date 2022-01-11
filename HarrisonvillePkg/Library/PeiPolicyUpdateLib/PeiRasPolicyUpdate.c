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

  PeiRasPolicyUpdate.c

Abstract:

  This file is SampleCode of the library for RAS Policy initialzation.

--*/
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/GlobalVariable.h>
#include <Library/DebugLib.h>
#include <Ppi/PeiRasPolicyPpi.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Include/SetupVariable.h>
#include <Library/PeiServicesLib.h>

EFI_STATUS
EFIAPI
PeiUpdateRasPolicy (
  IN OUT      RAS_GLOBAL_POLICY_PPI       *RasGlobalPolicyPpi
  )
/*++

Routine Description:

  This function performs RAS Platform Policy initialzation.

Arguments:

  RasGlobalPolicyPpi            The RAS Platform Policy PPI instance

Returns:

  EFI_SUCCESS             The PPI is installed and initialized.
  EFI ERRORS              The PPI is not successfully installed.
  EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

--*/
{

  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;
  SYSTEM_CONFIGURATION            SetupVariables;
  EFI_STATUS					            Status;

  VariableSize = sizeof (SYSTEM_CONFIGURATION);

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
  // Denverton Aptiov override Start - EIP#217439
  Status = VariableServices->GetVariable (
                              VariableServices,
                              L"IntelSetup",
                              &gSetupVariableGuid,
                              NULL,
                              &VariableSize,
                              &SetupVariables
                              );
  // Denverton Aptiov override End - EIP#217439
  ASSERT_EFI_ERROR (Status);

  RasGlobalPolicyPpi->SystemErrorEn = SetupVariables.SystemErrorEn;
  RasGlobalPolicyPpi->ErrorCloakingEnable = SetupVariables.ErrorCloakingEnable;
  RasGlobalPolicyPpi->MemoryErrorLog = SetupVariables.MemoryErrorLog;
  RasGlobalPolicyPpi->MemoryCorrErrorLog = SetupVariables.MemoryCorrErrorLog;
  RasGlobalPolicyPpi->MemoryUnCorrErrorLog = SetupVariables.MemoryUnCorrErrorLog;
  RasGlobalPolicyPpi->PCIeErrorLog = SetupVariables.PCIeErrorLog;
  RasGlobalPolicyPpi->PCIeCorrErrorLog = SetupVariables.PCIeCorrErrorLog;
  RasGlobalPolicyPpi->PCIeNFErrorLog = SetupVariables.PCIeNFErrorLog;
  RasGlobalPolicyPpi->PCIeFatalErrorLog = SetupVariables.PCIeFatalErrorLog;
  RasGlobalPolicyPpi->PCIeSystemErrEnable = SetupVariables.PCIeSystemErrEnable;
  RasGlobalPolicyPpi->PCIeParityErrEnable = SetupVariables.PCIeParityErrEnable;
  RasGlobalPolicyPpi->RCECErrorLog = 1;
  RasGlobalPolicyPpi->BUnitErrorLog = 1;
  RasGlobalPolicyPpi->WheaSupport = SetupVariables.WheaSupport;
  RasGlobalPolicyPpi->WheaEinj_05Extn = SetupVariables.WheaEinj_05Extn;
  RasGlobalPolicyPpi->WheaLogging = SetupVariables.WheaLogging;

  return Status;
}
