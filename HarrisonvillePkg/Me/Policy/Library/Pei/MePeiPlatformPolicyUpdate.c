/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file MePeiPlatformPolicyUpdate.c

  Do Platform Stage ME initialization.

**/
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Ppi/MePlatformPolicyPei.h>
#include <Ppi/HeciPpi.h>
#include <Library/MeTypeLib.h>
#include <MeSetup.h>
#include <Library/MeSetupLib.h>

#if SPS_SUPPORT
#include <Guid/PlatformInfo.h>
#include <Library/PeiPlatformLib.h>
#endif //SPS_SUPPORT

#if AMT_SUPPORT
#include "AlertStandardFormat.h"
#include <Guid/AmtForcePushPetPolicy.h>
#include <Guid/MeBiosExtensionSetup.h>

CHAR16                      gMeBiosExtensionSetupName[] = ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME;
ASF_FRAMEWORK_MESSAGE_TYPE  mAmtForcePushPetPolicy[] = {
  AsfMessageChassisIntrusion,
  AsfMessageUserAuthenticationInvalid,
  AsfMessageHddAuthenticationInvalid,
};

/**

  Install the Active Management push PET policy

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.

**/
EFI_STATUS
InstallAmtForcePushPetPolicy (
  VOID
  )
{
  //
  // Platform Policy
  //
  EFI_STATUS  Status = EFI_SUCCESS;

#if defined(SPS_SUPPORT) && SPS_SUPPORT
  if (!MeTypeIsAmt()) {
    return EFI_UNSUPPORTED;
  }
#endif // SPS_SUPPORT

  //
  // Build HOB for setup memory information
  //
  BuildGuidDataHob (
            &gAmtForcePushPetPolicyGuid,
            &mAmtForcePushPetPolicy,
            sizeof (mAmtForcePushPetPolicy)
            );


  return Status;
}
#endif // AMT_SUPPORT

/**

  Install the Me Platform Policy Library

  @param PeiMePlatformPolicyPpi  Me Platform Policy Ppi

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
  @retval EFI_NOT_FOUND         Not all required services are loaded. Try later.

**/
EFI_STATUS
UpdatePeiMePlatformPolicy (
  IN OUT  PEI_ME_PLATFORM_POLICY_PPI  *PeiMePlatformPolicyPpi
  )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  ME_RC_CONFIGURATION             MeConfiguration;
#if SPS_SUPPORT
  PLATFORM_INFO_HOB               *pPlatformInfo = NULL;
#endif;

  if(NULL == PeiMePlatformPolicyPpi)
  {
    DEBUG ((EFI_D_ERROR, "[ME Policy] ERROR: Null pointer error (PeiMePlatformPolicyPpi)\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = GetMeSetupConfig (&MeConfiguration);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[ME Policy] ERROR: System configuration variable cannot be located (%r)\n", Status));
    return EFI_NOT_FOUND;
  }

#if SPS_SUPPORT
  Status = GetPlatformInfo (&pPlatformInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[ME Policy] ERROR: Platfrom info variable cannot be located (%r).\n", Status));
    return EFI_NOT_FOUND;
  } else if (pPlatformInfo == NULL) {
    DEBUG ((EFI_D_ERROR, "[ME Policy] ERROR: Platform info pointer error (pPlatformInfo) ME policy override aborted.\n"));
    return EFI_NOT_FOUND;
  }
#endif;

  if (!EFI_ERROR (Status)) {
    PeiMePlatformPolicyPpi->DidEnabled = MeConfiguration.MeDidEnabled & 0x01;
    PeiMePlatformPolicyPpi->DidInitStat = MeConfiguration.MeDidInitStat & 0x0f;
    PeiMePlatformPolicyPpi->MeTimeout = MeConfiguration.MeTimeout;
    PeiMePlatformPolicyPpi->MeEndOfPostEnabled = MeConfiguration.MeEndOfPostEnabled & 0x01;
#if SPS_SUPPORT
    PeiMePlatformPolicyPpi->NmPwrOptBoot = MeConfiguration.NmPwrOptBoot;
    PeiMePlatformPolicyPpi->NmPwrOptBootOverride = MeConfiguration.NmPwrOptBootOverride;
    PeiMePlatformPolicyPpi->NmCores2Disable = MeConfiguration.NmCores2Disable;
    PeiMePlatformPolicyPpi->NmCores2DisableOverride = MeConfiguration.NmCores2DisableOverride;
    PeiMePlatformPolicyPpi->MeShutdownEnabled = MeConfiguration.SpsMeShutdown & 0x01;
    PeiMePlatformPolicyPpi->MeHmrfpoLockEnabled = MeConfiguration.MeHmrfpoLockEnabled & 0x01;
#if TESTMENU_FLAG || ME_TESTMENU_FLAG
    PeiMePlatformPolicyPpi->EnableMePreDidReset = MeConfiguration.EnableMePreDidReset & 0x01;
#else
    PeiMePlatformPolicyPpi->EnableMePreDidReset = 1;
#endif // TESTMENU_FLAG || ME_TESTMENU_FLAG
#endif // SPS_SUPPORT

#if ME_TESTMENU_FLAG
    PeiMePlatformPolicyPpi->HeciCommunication1 = MeConfiguration.MeHeci1Enabled & 0x03;
    PeiMePlatformPolicyPpi->HeciCommunication2 = MeConfiguration.MeHeci2Enabled & 0x03;
    PeiMePlatformPolicyPpi->HeciCommunication3 = MeConfiguration.MeHeci3Enabled & 0x03;
    PeiMePlatformPolicyPpi->SolEnabled = MeConfiguration.MeKtEnabled & 0x03;
    PeiMePlatformPolicyPpi->IderEnabled = MeConfiguration.MeIderEnabled & 0x03;
    PeiMePlatformPolicyPpi->Heci1HideInMe = MeConfiguration.MeHeci1HideInMe & 0x03;
    PeiMePlatformPolicyPpi->Heci2HideInMe = MeConfiguration.MeHeci2HideInMe & 0x03;
    PeiMePlatformPolicyPpi->Heci3HideInMe = MeConfiguration.MeHeci3HideInMe & 0x03;
    PeiMePlatformPolicyPpi->TestDisableD0I3SettingForHeci = MeConfiguration.TestDisableD0I3SettingForHeci & 0x01;
    PeiMePlatformPolicyPpi->WaitResetWarningAck = MeConfiguration.WaitResetWarningAck & 0x01;
    PeiMePlatformPolicyPpi->FiaMuxMessageEnabled = MeConfiguration.FiaMuxMessageEnabled;
    #else
    // set defaults without setup
    PeiMePlatformPolicyPpi->HeciCommunication1 = SETUP_HECI_DEVICE_AUTO;
    PeiMePlatformPolicyPpi->HeciCommunication2 = SETUP_HECI_DEVICE_AUTO;
    PeiMePlatformPolicyPpi->HeciCommunication3 = SETUP_HECI_DEVICE_AUTO;
    PeiMePlatformPolicyPpi->SolEnabled = SETUP_HECI_DEVICE_AUTO;
    PeiMePlatformPolicyPpi->IderEnabled = SETUP_HECI_DEVICE_AUTO;
    PeiMePlatformPolicyPpi->Heci1HideInMe = 0;
    PeiMePlatformPolicyPpi->Heci2HideInMe = 0;
    PeiMePlatformPolicyPpi->Heci3HideInMe = 0;
#endif // TESTMENU_FLAG || ME_TESTMENU_FLAG

    PeiMePlatformPolicyPpi->HsioEnableMessaging = MeConfiguration.HsioEnableMessaging & 0x01;

#if SPS_SUPPORT
    // Manage Auto for ICC Clock SSC Setting
    if (SETUP_SSC_AUTO == MeConfiguration.SpsIccClkSscSetting) {
      DEBUG ((EFI_D_INFO, "[ME Policy] SSC set as auto, checking board id...\n"));
      // SSC value depends off the board, check board id
      if(BOARD_ID_AspenCove == pPlatformInfo->BoardId) {
        PeiMePlatformPolicyPpi->SpsIccClkSscSetting = SETUP_SSC_DISABLED;
      } else {
        PeiMePlatformPolicyPpi->SpsIccClkSscSetting = SETUP_SSC_ENABLED;
      }
    } else {
      PeiMePlatformPolicyPpi->SpsIccClkSscSetting = MeConfiguration.SpsIccClkSscSetting;
    }
    DEBUG ((EFI_D_INFO, "[ME Policy] SSC %s.\n", (PeiMePlatformPolicyPpi->SpsIccClkSscSetting) ? L"enabled" : L"disabled"));

    // Manage Auto Settings for SPS
    if (MeTypeIsSps()) {
      if (MeTypeIsSpsNm()) {
        if (PeiMePlatformPolicyPpi->HeciCommunication2 == SETUP_HECI_DEVICE_AUTO) {
          PeiMePlatformPolicyPpi->HeciCommunication2 = SETUP_HECI_DEVICE_ENABLED;
        }
      } else {
        if (PeiMePlatformPolicyPpi->HeciCommunication2 == SETUP_HECI_DEVICE_AUTO) {
          PeiMePlatformPolicyPpi->HeciCommunication2 = SETUP_HECI_DEVICE_DISABLED;
        }
      }
      if (PeiMePlatformPolicyPpi->HeciCommunication1 == SETUP_HECI_DEVICE_AUTO) {
        PeiMePlatformPolicyPpi->HeciCommunication1 = SETUP_HECI_DEVICE_ENABLED;
      }
      if (PeiMePlatformPolicyPpi->HeciCommunication3 == SETUP_HECI_DEVICE_AUTO) {
        PeiMePlatformPolicyPpi->HeciCommunication3 = SETUP_HECI_DEVICE_DISABLED;
      }
      if (PeiMePlatformPolicyPpi->SolEnabled == SETUP_HECI_DEVICE_AUTO) {
        PeiMePlatformPolicyPpi->SolEnabled = SETUP_HECI_DEVICE_DISABLED;
      }
      if (PeiMePlatformPolicyPpi->IderEnabled == SETUP_HECI_DEVICE_AUTO) {
        PeiMePlatformPolicyPpi->IderEnabled = SETUP_HECI_DEVICE_DISABLED;
      }
    }
#endif // SPS_SUPPORT

#if AMT_SUPPORT
    // Manage Auto Settings for SPS
    if (MeTypeIsAmt()) {
      HECI_PPI *pHeciPpi;

      if (MeTypeIsCorporateAmt()) {
        if (PeiMePlatformPolicyPpi->SolEnabled == SETUP_HECI_DEVICE_AUTO) {
          PeiMePlatformPolicyPpi->SolEnabled = SETUP_HECI_DEVICE_ENABLED;
        }
      } else {
        if (PeiMePlatformPolicyPpi->SolEnabled == SETUP_HECI_DEVICE_AUTO) {
          PeiMePlatformPolicyPpi->SolEnabled = SETUP_HECI_DEVICE_DISABLED;
        }
      }
      if (PeiMePlatformPolicyPpi->HeciCommunication1 == SETUP_HECI_DEVICE_AUTO) {
        PeiMePlatformPolicyPpi->HeciCommunication1 = SETUP_HECI_DEVICE_ENABLED;
      }
      if (PeiMePlatformPolicyPpi->HeciCommunication2 == SETUP_HECI_DEVICE_AUTO) {
        PeiMePlatformPolicyPpi->HeciCommunication2 = SETUP_HECI_DEVICE_DISABLED;
      }
      if (PeiMePlatformPolicyPpi->HeciCommunication3 == SETUP_HECI_DEVICE_AUTO) {
        // Disabled - no integrated touch device is used for server
        PeiMePlatformPolicyPpi->HeciCommunication3 = SETUP_HECI_DEVICE_DISABLED;
      }
      if (PeiMePlatformPolicyPpi->IderEnabled == SETUP_HECI_DEVICE_AUTO) {
        PeiMePlatformPolicyPpi->IderEnabled = SETUP_HECI_DEVICE_DISABLED;
      }

      Status = PeiServicesLocatePpi(&gHeciPpiGuid, 0, NULL, &pHeciPpi);
      if (!EFI_ERROR(Status)) {
        UINT32 MeMode;

        Status = pHeciPpi->GetMeMode (&MeMode);
        if (!EFI_ERROR(Status)) {
          if ((MeMode == ME_MODE_TEMP_DISABLED) || (MeMode == ME_MODE_SECOVER)) {
            PeiMePlatformPolicyPpi->SolEnabled = SETUP_HECI_DEVICE_DISABLED;
            PeiMePlatformPolicyPpi->HeciCommunication1 = SETUP_HECI_DEVICE_DISABLED;
          }
        }
      }
    }
#endif // AMT_SUPPORT

    // Manage Auto Settings for Dfx
    if (MeTypeIsDfx()) {
       if (PeiMePlatformPolicyPpi->HeciCommunication1 == SETUP_HECI_DEVICE_AUTO) {
        PeiMePlatformPolicyPpi->HeciCommunication1 = SETUP_HECI_DEVICE_ENABLED;
      }
      if (PeiMePlatformPolicyPpi->HeciCommunication2 == SETUP_HECI_DEVICE_AUTO) {
        PeiMePlatformPolicyPpi->HeciCommunication2 = SETUP_HECI_DEVICE_ENABLED;
      }
      if (PeiMePlatformPolicyPpi->HeciCommunication3 == SETUP_HECI_DEVICE_AUTO) {
        PeiMePlatformPolicyPpi->HeciCommunication3 = SETUP_HECI_DEVICE_ENABLED;
      }
      if (PeiMePlatformPolicyPpi->SolEnabled == SETUP_HECI_DEVICE_AUTO) {
        PeiMePlatformPolicyPpi->SolEnabled = SETUP_HECI_DEVICE_ENABLED;
      }
      if (PeiMePlatformPolicyPpi->IderEnabled ==SETUP_HECI_DEVICE_AUTO) {
        PeiMePlatformPolicyPpi->IderEnabled = SETUP_HECI_DEVICE_ENABLED;
      }
    }

#ifdef AMT_SUPPORT
#if defined(SPS_SUPPORT) && SPS_SUPPORT
    if (MeTypeIsAmt())
#endif // SPS_SUPPORT
    {
      ME_BIOS_EXTENSION_SETUP         MeBiosExtensionSetupData;
      UINTN                           VarSize;

      PeiMePlatformPolicyPpi->AmtEnabled         = MeConfiguration.Amt;
      PeiMePlatformPolicyPpi->WatchDog           = MeConfiguration.WatchDog;
      PeiMePlatformPolicyPpi->WatchDogOs         = MeConfiguration.WatchDogOs;
      PeiMePlatformPolicyPpi->WatchDogTimerBios  = MeConfiguration.WatchDogTimerBios;
      PeiMePlatformPolicyPpi->WatchDogTimerOs    = MeConfiguration.WatchDogTimerOs;
      PeiMePlatformPolicyPpi->AsfEnabled         = MeConfiguration.Asf;
      PeiMePlatformPolicyPpi->FWProgress         = MeConfiguration.FWProgress;


      //
      // Get BIOS Sync-up data from MEBx to AMT platform policy
      //
      VarSize = sizeof (MeBiosExtensionSetupData);
      Status = VariableServices->GetVariable (
                                VariableServices,
                                gMeBiosExtensionSetupName,
                                &gMeBiosExtensionSetupGuid,
                                NULL,
                                &VarSize,
                                &MeBiosExtensionSetupData
                                );
      if (Status != EFI_SUCCESS) {
        PeiMePlatformPolicyPpi->ManageabilityMode = 0;
      } else {
        PeiMePlatformPolicyPpi->ManageabilityMode = MeBiosExtensionSetupData.PlatformMngSel;
      }

      InstallAmtForcePushPetPolicy ();
    }
#endif // AMT_SUPPORT
  } else {
    // Can't get Setup configuration, configure default values for Policy
    ZeroMem(PeiMePlatformPolicyPpi, sizeof(PEI_ME_PLATFORM_POLICY_PPI));

    PeiMePlatformPolicyPpi->DidEnabled = 1;
    PeiMePlatformPolicyPpi->DidInitStat = 0;
#if TESTMENU_FLAG || ME_TESTMENU_FLAG
    PeiMePlatformPolicyPpi->WaitResetWarningAck = 1;
#endif // TESTMENU_FLAG || ME_TESTMENU_FLAG
    PeiMePlatformPolicyPpi->HeciCommunication1 = 1;
#ifdef AMT_SUPPORT
    ///
    /// Initialize the AMT Platform Configuration
    ///
    PeiMePlatformPolicyPpi->AmtEnabled        = 0;
    PeiMePlatformPolicyPpi->WatchDog          = 0;
    PeiMePlatformPolicyPpi->WatchDogOs        = 0;
    PeiMePlatformPolicyPpi->WatchDogTimerBios = 0;
    PeiMePlatformPolicyPpi->WatchDogTimerOs   = 0;
    PeiMePlatformPolicyPpi->AsfEnabled        = 1;
    PeiMePlatformPolicyPpi->FWProgress        = 1;
    PeiMePlatformPolicyPpi->ManageabilityMode = 0;
#endif // AMT_SUPPORT
  }
#else
  PeiMePlatformPolicyPpi = NULL;
  Status                 = EFI_NOT_FOUND;
#endif // ME_SUPPORT_FLAG

  return Status;
}
