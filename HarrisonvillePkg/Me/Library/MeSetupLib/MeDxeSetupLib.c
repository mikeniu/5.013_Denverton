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

#include <Base.h>
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MeSetupLib.h>

EFI_STATUS
GetMeSetupConfig (ME_RC_CONFIGURATION * MeRcConfig)
{
  EFI_STATUS             Status;
  SYSTEM_CONFIGURATION   SetupDataBuffer;
  UINTN                  SetupVariableSize;

  SetupVariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gSetupVariableGuid,
                  NULL,
                  &SetupVariableSize,
                  &SetupDataBuffer
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[MeSetupLib] GetMeSetupConfig - Error %r getting Setup Data \n", Status));
    //ASSERT_EFI_ERROR (Status);   // Denverton Aptiov override - EIP#337909
    return Status;
  }

  CopyMem((UINT8*)MeRcConfig, (UINT8*)&SetupDataBuffer, sizeof(*MeRcConfig));

  return Status;
}

EFI_STATUS
SetMeSetupConfig (ME_RC_CONFIGURATION * MeRcConfig)
{
  EFI_STATUS             Status;
  UINTN                  SetupVariableSize;
  SYSTEM_CONFIGURATION   SetupDataBuffer;
  UINT32                 SetupVariableAttributes;

  SetupVariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gSetupVariableGuid,
                  &SetupVariableAttributes,
                  &SetupVariableSize,
                  &SetupDataBuffer
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[MeSetupLib] SetMeSetupConfig - Error %r getting Setup Data \n", Status));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  CopyMem((UINT8*)&SetupDataBuffer, (UINT8*)MeRcConfig, sizeof(SetupDataBuffer));

  Status = gRT->SetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gSetupVariableGuid,
                  SetupVariableAttributes,
                  SetupVariableSize,
                  &SetupDataBuffer
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[MeSetupLib] SetMeSetupConfig - Error %r setting Setup Data \n", Status));
    ASSERT_EFI_ERROR (Status);
  }

  return Status;
}
