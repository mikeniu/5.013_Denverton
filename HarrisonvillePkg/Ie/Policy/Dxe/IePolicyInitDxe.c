/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c)  2015 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  IePolicyInitDxe.c

@brief:
  This file is SampleCode for Intel ME DXE Platform Policy initialzation.


**/
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <SetupVariable.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include "IeHob.h"

///
/// Function implementations
///

/**

  @brief
  Read from IE Hob information if IE is enabled via soft-straps

  @param[in] NONE

  @retval TRUE             IE is enabled
  @retval FALSE            IE is disabled

**/
BOOLEAN
IsIeEnabled( VOID )
{
  IE_HOB *IeHob;
  BOOLEAN retVal = FALSE;

  IeHob = GetFirstGuidHob (&gIeHobGuid);
  if ((IeHob != NULL) && (IeHob->IeDisabledInSoftStraps == 0)) {
    retVal = TRUE;
  }

  return retVal;
}

/**

  @brief
  Read from IE Hob information and returns DfxIe field

  @param[in] NONE

  @retval TRUE             IE is Dfx Type
  @retval FALSE            IE is not Dfx Type

**/
BOOLEAN
GetDfxIe( VOID )
{
  IE_HOB *IeHob;

  IeHob = GetFirstGuidHob (&gIeHobGuid);
  if (IeHob != NULL) {
    return IeHob->DfxIe;
  }

  return FALSE;
}


/**

  @brief
  Initialize Intel IE DXE Platform Policy
  At this moment Policy driver will disable IE settings in setup
  when IE is disabled in SoftStraps

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval EFI_SUCCESS             Initialization complete.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
IePolicyInitDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS              Status;
  SYSTEM_CONFIGURATION    SystemConfiguration;
  UINTN                   SetupVariableSize;
  UINT32                  SetupVariableAttributes;
  BOOLEAN                 SaveSetup = FALSE;
  BOOLEAN                 DfxIe;

  DEBUG(( EFI_D_INFO, "[IE Policy] IE Policy Init Dxe\n"));

  SetupVariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
                PLATFORM_SETUP_VARIABLE_NAME,
                &gSetupVariableGuid,
                &SetupVariableAttributes,
                &SetupVariableSize,
                &SystemConfiguration
                );

  if (Status == EFI_SUCCESS) {
    if (IsIeEnabled()) {
      DEBUG(( EFI_D_INFO, "[IE Policy] IE is enabled - enable setup entries\n"));
      if (SystemConfiguration.IeDisabledInSoftStraps == 0) {
        SystemConfiguration.IeDisabledInSoftStraps = 1;
        SaveSetup = TRUE;
      }
    } else {
      DEBUG(( EFI_D_INFO, "[IE Policy] IE is disabled - disable setup entries\n"));
      if (SystemConfiguration.IeDisabledInSoftStraps == 1) {
        SystemConfiguration.IeDisabledInSoftStraps = 0;
        SaveSetup = TRUE;
      }
    }
    DfxIe = GetDfxIe();
    if (SystemConfiguration.DfxIe != DfxIe) {
      SystemConfiguration.DfxIe = DfxIe;
      SaveSetup = TRUE;
    }
  }
  if (SaveSetup) {
    Status = gRT->SetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gSetupVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (SYSTEM_CONFIGURATION),
                  &SystemConfiguration
                  );
  }

  // unload driver as no more required
  return EFI_UNSUPPORTED;
}
