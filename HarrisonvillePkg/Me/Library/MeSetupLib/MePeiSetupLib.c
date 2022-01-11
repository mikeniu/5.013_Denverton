/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
--*/
/** @file

  Library functions for MeSetupLib.
  This library instance provides methods to access Setup option.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <SetupVariable.h>
#include <Library/MeSetupLib.h>

EFI_STATUS
GetMeSetupConfig (ME_RC_CONFIGURATION * MeRcConfig)
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  SYSTEM_CONFIGURATION            SystemConfiguration;
  UINTN                           VarSize;

  //
  // Locate system configuration variable
  //
  Status = PeiServicesLocatePpi (
                            &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
                            0,                                // INSTANCE
                            NULL,                             // EFI_PEI_PPI_DESCRIPTOR
                            &VariableServices                 // PPI
                            );
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR(Status)) {
      return EFI_NOT_FOUND;
  }

  VarSize = sizeof (SYSTEM_CONFIGURATION);
  Status = VariableServices->GetVariable (
                              VariableServices,
                              PLATFORM_SETUP_VARIABLE_NAME,
                              &gSetupVariableGuid,
                              NULL,
                              &VarSize,
                              &SystemConfiguration
                              );

  if (EFI_ERROR(Status)) {
      return EFI_NOT_FOUND;
  }

  *MeRcConfig = SystemConfiguration;

  return EFI_SUCCESS;
}

EFI_STATUS
SetMeSetupConfig (ME_RC_CONFIGURATION * MeRcConfig)
{
  return EFI_NOT_FOUND;
}
