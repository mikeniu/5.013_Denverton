/**

This file contains a 'Sample Driver' and is licensed as such
under the terms of your license agreement with Intel or your
vendor.  This file may be modified by the user, subject to
the additional terms of the license agreement

**/

/**
Copyright (c)  2008 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file MePlatformPolicySaveDxe.c

--*/

#include "Protocol/FirmwareVolume.h"
#include "FrameworkDxe.h "
#include "Library/DebugLib.h"
#include "Guid/HiiSetupEnter.h"
#include "Framework/DxeCis.h"
#include "CommonIncludes.h "
#include "Library/UefiBootServicesTableLib.h"
#include "Library/UefiRuntimeServicesTableLib.h"
#include "Library/MePlatformPolicyUpdateDxeLib.h"
#include "MeState.h"
#include "Library/MeTypeLib.h"
#include "Protocol/HeciProtocol.h"
#include "Library/HeciMsgLib.h"
#include <Library/MeSetupLib.h>
#include <Library/BaseMemoryLib.h>
#include <MeSetup.h>

/**

  Function that is called when we save Me Setup Data.

  @param MeNmConfig         Pointer to the Me Nm Policy section with data to be saved
  @param MeNmConfig         Pointer to configuration data that information has to be saved to

  @retval TRUE/FALSE        Save setup indicator

**/
#ifdef SPS_SUPPORT
BOOLEAN
saveMeNmSetupConfiguration(ME_NM_CONFIG *MeNmConfig, ME_RC_CONFIGURATION *pMeCfg)
{
  EFI_STATUS            Status;
  BOOLEAN               SaveSetup = FALSE;
  ME_RC_CONFIGURATION   MeRcConfig;

  if (!MeTypeIsSps()) {
    return FALSE;
  }

  if (MeNmConfig) {
    if (pMeCfg == NULL) {
      Status = GetMeSetupConfig (&MeRcConfig);
      if (!EFI_ERROR(Status)) {
        pMeCfg = &MeRcConfig;
      }
    }
    if (pMeCfg != NULL) {
      if (pMeCfg->NmPwrOptBoot != MeNmConfig->NmPwrOptBoot) {
        pMeCfg->NmPwrOptBoot = MeNmConfig->NmPwrOptBoot;
        SaveSetup = TRUE;
      }
      if (pMeCfg->NmCores2Disable != MeNmConfig->NmCores2Disable) {
        pMeCfg->NmCores2Disable = MeNmConfig->NmCores2Disable;
        SaveSetup = TRUE;
      }
      if ((pMeCfg == &MeRcConfig) && (SaveSetup == TRUE)) {
        SetMeSetupConfig (pMeCfg);
      }
    }
  }

  return SaveSetup;
}
#endif // SPS_SUPPORT

/**

  Function stores Me Platform Policy to Setup configuration

  @param MePlatformPolicyInstance    ME Policy instance to save
  @param saveMask                    What to save

  @retval EFI_SUCCESS           Saving MePolicy to Setup succeed.
  @retval EFI_INVALID_PARAMETER Wrong input parameters

**/
EFI_STATUS
saveMePolicyToSetupConfiguration(DXE_ME_POLICY_PROTOCOL  *MePlatformPolicyInstance)
{
  EFI_STATUS            Status;
  ME_RC_CONFIGURATION   MeRcConfiguration;
  BOOLEAN               SaveSetup = FALSE;

  Status = GetMeSetupConfig (&MeRcConfiguration);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[ME Policy] Failed to read ME_RC_CONFIGURATION!  Status = %r\n", Status));
    return EFI_NOT_READY;
  }

  if (MeRcConfiguration.MeType != MePlatformPolicyInstance->MeType) {
    MeRcConfiguration.MeType = MePlatformPolicyInstance->MeType;
    SaveSetup = TRUE;
  }
  if (MeRcConfiguration.MeFirmwareMode != MePlatformPolicyInstance->MeConfig.MeFirmwareMode) {
    MeRcConfiguration.MeFirmwareMode = MePlatformPolicyInstance->MeConfig.MeFirmwareMode;
    SaveSetup = TRUE;
  }
#ifdef AMT_SUPPORT
#if SPS_SUPPORT
  if (MeTypeIsAmt())
#endif // SPS_SUPPORT
  {
    if (MeRcConfiguration.RemoteSessionActive != MePlatformPolicyInstance->MeAmtConfig.RemoteSessionActive) {
      MeRcConfiguration.RemoteSessionActive = MePlatformPolicyInstance->MeAmtConfig.RemoteSessionActive;
      SaveSetup = TRUE;
    }
    if (MeRcConfiguration.MeStateControl != MePlatformPolicyInstance->MeAmtConfig.MeStateControl) {
      MeRcConfiguration.MeStateControl = MePlatformPolicyInstance->MeAmtConfig.MeStateControl;
      SaveSetup = TRUE;
    }
    if (MeRcConfiguration.MeImageType != MePlatformPolicyInstance->MeAmtConfig.MeImageType) {
      MeRcConfiguration.MeImageType = MePlatformPolicyInstance->MeAmtConfig.MeImageType;
      SaveSetup = TRUE;
    }
    if (MeRcConfiguration.CoreBiosDoneEnabled != MePlatformPolicyInstance->MeAmtConfig.CoreBiosDoneEnabled) {
      MeRcConfiguration.CoreBiosDoneEnabled = MePlatformPolicyInstance->MeAmtConfig.CoreBiosDoneEnabled;
      SaveSetup = TRUE;
    }
  }
#endif // AMT_SUPPORT

#ifdef SPS_SUPPORT
  SaveSetup |= saveMeNmSetupConfiguration(&MePlatformPolicyInstance->MeNmConfig, &MeRcConfiguration);
#endif // SPS_SUPORT

  if (SaveSetup == TRUE) {
    Status = SetMeSetupConfig (&MeRcConfiguration);
  }
  DEBUG ((EFI_D_INFO, "[ME Policy] Save to Setup result : %r\n", Status));

  // Modifying default values to have there correct METype value
  if (!EFI_ERROR(Status)) {
    // Me Policy is stored in setup. Clear AnythingChanged
    MePlatformPolicyInstance->AnythingChanged = 0;
  }

  return Status;
}
