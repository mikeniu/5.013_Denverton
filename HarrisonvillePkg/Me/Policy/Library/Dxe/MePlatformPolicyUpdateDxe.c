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

@file MePlatformPolicyUpdateDxe.c

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
#include "Protocol/HeciProtocol.h"
#include "Library/HeciMsgLib.h"
#include <Library/MeSetupLib.h>
#include <Library/BaseMemoryLib.h>
#if AMT_SUPPORT
#include "MeBiosPayloadData.h"
#include "MeBiosPayloadHob.h"
#include <Library/HobLib.h>
#include <MeSetup.h>


#endif // AMT_SUPPORT




BOOLEAN MeIsAfterEndOfPost (void);

//
// Function implementations
//
UINT8 GetMeMode(void)
{
  EFI_STATUS        Status;
  UINT32            retVal;
  HECI_PROTOCOL     *Heci;

  //
  // Default ME Mode
  //
  retVal = ME_MODE_FAILED;

  Status = gBS->LocateProtocol (&gHeciProtocolGuid, NULL, &Heci);
  if (!EFI_ERROR (Status)) {
    Status = Heci->GetMeMode (&retVal);
    ASSERT_EFI_ERROR(Status);
  }
  return (UINT8)retVal;
}

/**

  Install the ME Platform Policy Library

  @param mDxePlatformMePolicy  ME Platform Policy in DXE phase

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
InstallMEPlatformPolicyLibrary (
  IN OUT  DXE_ME_POLICY_PROTOCOL  *mDxePlatformMePolicy
)
{
  EFI_STATUS              Status;
  EFI_EVENT               SetupEnterEvent;
  VOID                    *pSetupRegistration;

  DEBUG ((EFI_D_ERROR, "[ME Policy] MePlatformPolicyEntryPoint start [InstallMEPlatformPolicyLibrary()]\n"));
  ASSERT(mDxePlatformMePolicy != NULL);

  Status = UpdateMePolicyFromSetup (mDxePlatformMePolicy, NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[ME Policy] UpdateMePolicyFromSetup Failed and the Status is %x\n", Status));
    return Status;
  }
  //
  // Get info for ME setup options
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  UpdateSetupDataWithFwInfo,
                  NULL,
                  &SetupEnterEvent
                  );
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get FW data if we enter UI
  //
  Status = gBS->RegisterProtocolNotify (
                  &gEfiSetupEnterGuid,
                  SetupEnterEvent,
                  &pSetupRegistration
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

#if SPS_SUPPORT
VOID
UpdateMeNmPolicyFromSetup (
  DXE_ME_POLICY_PROTOCOL  *MePlatformPolicyInstance,
  ME_RC_CONFIGURATION     *MeRcData
  )
{
  DEBUG ((EFI_D_ERROR, "[ME Policy] UpdateMeNmPolicyFromSetup\n"));

  if (MePlatformPolicyInstance->MeNmConfig.NmPwrOptBoot != MeRcData->NmPwrOptBoot) {
    MePlatformPolicyInstance->MeNmConfig.NmPwrOptBoot = MeRcData->NmPwrOptBoot;
    MePlatformPolicyInstance->AnythingChanged = TRUE;
  }
  if (MePlatformPolicyInstance->MeNmConfig.NmPwrOptBootOverride != MeRcData->NmPwrOptBootOverride) {
    MePlatformPolicyInstance->MeNmConfig.NmPwrOptBootOverride = MeRcData->NmPwrOptBootOverride;
    MePlatformPolicyInstance->AnythingChanged = TRUE;
  }
  if (MePlatformPolicyInstance->MeNmConfig.NmCores2Disable != MeRcData->NmCores2Disable) {
    MePlatformPolicyInstance->MeNmConfig.NmCores2Disable = MeRcData->NmCores2Disable;
    MePlatformPolicyInstance->AnythingChanged = TRUE;
  }
  if (MePlatformPolicyInstance->MeNmConfig.NmCores2DisableOverride != MeRcData->NmCores2DisableOverride) {
      MePlatformPolicyInstance->MeNmConfig.NmCores2DisableOverride = MeRcData->NmCores2DisableOverride;
      MePlatformPolicyInstance->AnythingChanged = TRUE;
  }
  if (MePlatformPolicyInstance->MeNmConfig.SpsAltitude != MeRcData->SpsAltitude) {
    MePlatformPolicyInstance->MeNmConfig.SpsAltitude = MeRcData->SpsAltitude;
    MePlatformPolicyInstance->AnythingChanged = TRUE;
  }
  if (MePlatformPolicyInstance->MeNmConfig.SpsMctpBusOwner != MeRcData->SpsMctpBusOwner) {
    MePlatformPolicyInstance->MeNmConfig.SpsMctpBusOwner = MeRcData->SpsMctpBusOwner;
    MePlatformPolicyInstance->AnythingChanged = TRUE;
  }
  if (MePlatformPolicyInstance->MeNmConfig.MeHmrfpoLockEnabled != MeRcData->MeHmrfpoLockEnabled) {
    MePlatformPolicyInstance->MeNmConfig.MeHmrfpoLockEnabled = MeRcData->MeHmrfpoLockEnabled;
    MePlatformPolicyInstance->AnythingChanged = TRUE;
  }
  if (MePlatformPolicyInstance->MeNmConfig.MeHmrfpoEnableEnabled != MeRcData->MeHmrfpoEnableEnabled) {
    MePlatformPolicyInstance->MeNmConfig.MeHmrfpoEnableEnabled = MeRcData->MeHmrfpoEnableEnabled;
    MePlatformPolicyInstance->AnythingChanged = TRUE;
  }
  if (MePlatformPolicyInstance->MeNmConfig.MeGrLockEnabled != MeRcData->MeGrLockEnabled) {
    MePlatformPolicyInstance->MeNmConfig.MeGrLockEnabled = MeRcData->MeGrLockEnabled;
    MePlatformPolicyInstance->AnythingChanged = TRUE;
  }
#if TESTMENU_FLAG || ME_TESTMENU_FLAG
  if (MeRcData->NmPowerMsmtOverride &&
      MePlatformPolicyInstance->MeNmConfig.NmPowerMsmtSupport != MeRcData->NmPowerMsmtSupport)
  {
    MePlatformPolicyInstance->MeNmConfig.NmPowerMsmtSupport = MeRcData->NmPowerMsmtSupport;
    MePlatformPolicyInstance->AnythingChanged = TRUE;
  }
  if (MeRcData->NmHwChangeOverride &&
      MePlatformPolicyInstance->MeNmConfig.NmHwChangeStatus != MeRcData->NmHwChangeStatus)
  {
    MePlatformPolicyInstance->MeNmConfig.NmHwChangeStatus = MeRcData->NmHwChangeStatus;
    MePlatformPolicyInstance->AnythingChanged = TRUE;
  }
  if (MePlatformPolicyInstance->MeNmConfig.NmPtuLoadOverride != MeRcData->NmPtuLoadOverride)
  {
    MePlatformPolicyInstance->MeNmConfig.NmPtuLoadOverride = MeRcData->NmPtuLoadOverride;
    MePlatformPolicyInstance->AnythingChanged = TRUE;
  }
#endif
  MePlatformPolicyInstance->MeNmConfig.TStateEnable = MeRcData->TStateEnable;
  MePlatformPolicyInstance->MeNmConfig.TurboMode = MeRcData->TurboMode;
} // UpdateMeNmPolicyFromSetup()
#endif // SPS_SUPPORT


BOOLEAN
UpdateMeAmtPolicyFromSetup (
  DXE_ME_POLICY_PROTOCOL  *MePlatformPolicyInstance,
  ME_RC_CONFIGURATION      *SetupData
  )
{
  BOOLEAN retVal = FALSE; // Indication if setup has to be saved
  DEBUG ((EFI_D_ERROR, "[ME Policy] UpdateMeAmtPolicyFromSetup\n"));

#if AMT_SUPPORT
#if SPS_SUPPORT
    if (MeTypeIsAmt())
#endif // SPS_SUPPORT
    {
      if (MePlatformPolicyInstance->MeAmtConfig.AsfEnabled != SetupData->Asf) {
        MePlatformPolicyInstance->MeAmtConfig.AsfEnabled = SetupData->Asf;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.AmtEnabled != SetupData->Amt) {
        MePlatformPolicyInstance->MeAmtConfig.AmtEnabled = SetupData->Amt;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.WatchDog != SetupData->WatchDog) {
        MePlatformPolicyInstance->MeAmtConfig.WatchDog = SetupData->WatchDog;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.WatchDogOs != SetupData->WatchDogOs) {
        MePlatformPolicyInstance->MeAmtConfig.WatchDogOs = SetupData->WatchDogOs;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.WatchDogTimerOs != SetupData->WatchDogTimerOs) {
        MePlatformPolicyInstance->MeAmtConfig.WatchDogTimerOs = SetupData->WatchDogTimerOs;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.WatchDogTimerBios != SetupData->WatchDogTimerBios) {
        MePlatformPolicyInstance->MeAmtConfig.WatchDogTimerBios = SetupData->WatchDogTimerBios;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.CiraRequest != SetupData->AmtCiraRequest) {
        MePlatformPolicyInstance->MeAmtConfig.CiraRequest = SetupData->AmtCiraRequest;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.CiraTimeout != SetupData->AmtCiraTimeout) {
        MePlatformPolicyInstance->MeAmtConfig.CiraTimeout = SetupData->AmtCiraTimeout;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.UnConfigureMe != SetupData->UnConfigureMe) {
        MePlatformPolicyInstance->MeAmtConfig.UnConfigureMe = SetupData->UnConfigureMe;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.HideUnConfigureMeConfirm != SetupData->HideUnConfigureMeConfirm) {
        MePlatformPolicyInstance->MeAmtConfig.HideUnConfigureMeConfirm = SetupData->HideUnConfigureMeConfirm;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.MebxDebugMsg != SetupData->MebxDebugMsg) {
        MePlatformPolicyInstance->MeAmtConfig.MebxDebugMsg = SetupData->MebxDebugMsg;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.USBProvision != SetupData->USBProvision) {
        MePlatformPolicyInstance->MeAmtConfig.USBProvision = SetupData->USBProvision;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.FWProgress != SetupData->FWProgress) {
        MePlatformPolicyInstance->MeAmtConfig.FWProgress = SetupData->FWProgress;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.AmtbxSelectionScreen != SetupData->AmtbxSelectionScreen) {
        MePlatformPolicyInstance->MeAmtConfig.AmtbxSelectionScreen = SetupData->AmtbxSelectionScreen;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.AmtbxHotkeyPressed != SetupData->AmtbxHotKeyPressed) {
        MePlatformPolicyInstance->MeAmtConfig.AmtbxHotkeyPressed = SetupData->AmtbxHotKeyPressed;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.MebxNonUiTextMode != SetupData->MebxNonUiTextMode) {
        MePlatformPolicyInstance->MeAmtConfig.MebxUiTextMode = SetupData->MebxUiTextMode;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.MebxGraphicsMode != SetupData->MebxGraphicsMode) {
        MePlatformPolicyInstance->MeAmtConfig.MebxGraphicsMode = SetupData->MebxGraphicsMode;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.CoreBiosDoneEnabled != SetupData->CoreBiosDoneEnabled) {
        MePlatformPolicyInstance->MeAmtConfig.CoreBiosDoneEnabled = SetupData->CoreBiosDoneEnabled;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
#ifdef TESTMENU_FLAG
      if (MePlatformPolicyInstance->MeAmtConfig.MebxLaunchTimeout != SetupData->MebxLaunchTimeout) {
        MePlatformPolicyInstance->MeAmtConfig.MebxLaunchTimeout = SetupData->MebxLaunchTimeout;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.CpuReplacementTimeout != SetupData->CpuReplacementTimeout) {
        MePlatformPolicyInstance->MeAmtConfig.CpuReplacementTimeout = SetupData->CpuReplacementTimeout;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
#endif
      if (MePlatformPolicyInstance->MeAmtConfig.MeFwDownGrade != SetupData->MeFwDowngrade) {
        MePlatformPolicyInstance->MeAmtConfig.MeFwDownGrade = SetupData->MeFwDowngrade;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.MeLocalFwUpdEnabled != SetupData->LocalFwUpdEnabled) {
        MePlatformPolicyInstance->MeAmtConfig.MeLocalFwUpdEnabled = SetupData->LocalFwUpdEnabled;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.RemoteSessionActive != SetupData->RemoteSessionActive) {
        MePlatformPolicyInstance->MeAmtConfig.RemoteSessionActive = SetupData->RemoteSessionActive;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }
      if (MePlatformPolicyInstance->MeAmtConfig.MeStateControl != SetupData->MeStateControl) {
        MePlatformPolicyInstance->MeAmtConfig.MeStateControl = SetupData->MeStateControl;
        MePlatformPolicyInstance->AnythingChanged = TRUE;
      }

      //
      // Reset CIRA and UnConfigureMe setting if either one got set
      //
      if (SetupData->UnConfigureMe == 1 || SetupData->AmtCiraRequest == 1) {
        SetupData->UnConfigureMe  = 0;
        SetupData->AmtCiraRequest = 0;
        retVal = TRUE; // indicate to save setup
      }
    }
    //
    // FW Downgrade BIOS Setup Option is only valid for one boot
    //
    if (SetupData->MeFwDowngrade) {
      SetupData->MeFwDowngrade = 0;
      retVal = TRUE; // indicate to save setup
    }
#endif // AMT_SUPPORT

  return retVal;
}

/**

  Update Me platform policy while MePlatformProtocol is installed.

  @param MePlatformPolicyInstance - ME Platform Policy
  @param pSetupData               - pointer to system configuration
  @param resetRequest             - Returns information if function requests reset

  @retval None

**/
EFI_STATUS
UpdateMePolicyFromSetup (
  DXE_ME_POLICY_PROTOCOL  *MePlatformPolicyInstance,
  VOID                    *mSetupData,
  BOOLEAN                 *resetRequest
  )
{
  EFI_STATUS           Status = EFI_INVALID_PARAMETER;
  ME_RC_CONFIGURATION  MeRcConfiguration;
  ME_RC_CONFIGURATION  *pSetupData;
  
  pSetupData = (ME_RC_CONFIGURATION *)(UINTN)mSetupData;

  DEBUG ((EFI_D_ERROR, "[ME Policy] UpdateMePolicyFromSetup: start\n"));
  ASSERT(MePlatformPolicyInstance != NULL);

  Status = GetMeSetupConfig (&MeRcConfiguration);

  MePlatformPolicyUpdateMeSettings (MePlatformPolicyInstance);

  if (pSetupData == NULL) {
    if (!EFI_ERROR (Status)) {
      pSetupData = &MeRcConfiguration;
    }
  }
  if (pSetupData != NULL) {
#if TESTMENU_FLAG || ME_TESTMENU_FLAG
    if (MePlatformPolicyInstance->MeConfig.EndOfPostEnabled != pSetupData->MeEndOfPostEnabled) {
      MePlatformPolicyInstance->MeConfig.EndOfPostEnabled   = pSetupData->MeEndOfPostEnabled;
      MePlatformPolicyInstance->AnythingChanged = TRUE;
    }
    if (MePlatformPolicyInstance->MeConfig.HeciCommunication1 != pSetupData->MeHeci1Enabled) {
      MePlatformPolicyInstance->MeConfig.HeciCommunication1 = pSetupData->MeHeci1Enabled;
      MePlatformPolicyInstance->AnythingChanged = TRUE;
    }
    if (MePlatformPolicyInstance->MeConfig.HeciCommunication2 != pSetupData->MeHeci2Enabled) {
      MePlatformPolicyInstance->MeConfig.HeciCommunication2 = pSetupData->MeHeci2Enabled;
      MePlatformPolicyInstance->AnythingChanged = TRUE;
    }
    if (MePlatformPolicyInstance->MeConfig.HeciCommunication3 != pSetupData->MeHeci3Enabled) {
      MePlatformPolicyInstance->MeConfig.HeciCommunication3 = pSetupData->MeHeci3Enabled;
      MePlatformPolicyInstance->AnythingChanged = TRUE;
    }
    if (MePlatformPolicyInstance->MeConfig.MeIderEnabled != pSetupData->MeIderEnabled) {
      MePlatformPolicyInstance->MeConfig.MeIderEnabled = pSetupData->MeIderEnabled;
      MePlatformPolicyInstance->AnythingChanged = TRUE;
    }
    if (MePlatformPolicyInstance->MeConfig.MeKtEnabled != pSetupData->MeKtEnabled) {
      MePlatformPolicyInstance->MeConfig.MeKtEnabled = pSetupData->MeKtEnabled;
      MePlatformPolicyInstance->AnythingChanged = TRUE;
    }
    if (MePlatformPolicyInstance->MeConfig.Heci1HideInMe != pSetupData->MeHeci1HideInMe) {
      MePlatformPolicyInstance->MeConfig.Heci1HideInMe = pSetupData->MeHeci1HideInMe;
      MePlatformPolicyInstance->AnythingChanged = TRUE;
    }
    if (MePlatformPolicyInstance->MeConfig.Heci2HideInMe != pSetupData->MeHeci2HideInMe) {
      MePlatformPolicyInstance->MeConfig.Heci2HideInMe = pSetupData->MeHeci2HideInMe;
      MePlatformPolicyInstance->AnythingChanged = TRUE;
    }
    if (MePlatformPolicyInstance->MeConfig.Heci3HideInMe != pSetupData->MeHeci3HideInMe) {
      MePlatformPolicyInstance->MeConfig.Heci3HideInMe = pSetupData->MeHeci3HideInMe;
      MePlatformPolicyInstance->AnythingChanged = TRUE;
    }
#endif // ME_TESTMENU_FLAG

#if defined(SPS_SUPPORT) && SPS_SUPPORT
    if (MePlatformPolicyInstance->MeConfig.SpsMeShutdownEnabled != pSetupData->SpsMeShutdown) {
      MePlatformPolicyInstance->MeConfig.SpsMeShutdownEnabled   = pSetupData->SpsMeShutdown;
      MePlatformPolicyInstance->AnythingChanged = TRUE;
    }
#endif // SPS_SUPPORT

    if (MePlatformPolicyInstance->MeConfig.MeGrPromotionEnabled != pSetupData->MeGrPromotionEnabled) {
      MePlatformPolicyInstance->MeConfig.MeGrPromotionEnabled = pSetupData->MeGrPromotionEnabled;
      MePlatformPolicyInstance->AnythingChanged = TRUE;
    }

#if defined(SPS_SUPPORT) && SPS_SUPPORT
    UpdateMeNmPolicyFromSetup(MePlatformPolicyInstance, &MeRcConfiguration);
#endif // SPS_SUPPORT

    if (!EFI_ERROR (Status)) {
      if (GetMeMode() == ME_MODE_NORMAL) {
        Status = updateMngState(MePlatformPolicyInstance, (VOID *)&MeRcConfiguration, resetRequest);
        if (!EFI_ERROR (Status)) {
          Status = updateFwuData(MePlatformPolicyInstance, (VOID *)&MeRcConfiguration, resetRequest);
        }
      }
      updateMeState(MePlatformPolicyInstance, (VOID *)&MeRcConfiguration, resetRequest);
    }

    //
    // Do not send End of Post for DFX ME FW
    //
    if (MeTypeIsDfx()) {
      MePlatformPolicyInstance->MeConfig.EndOfPostEnabled      = 0;
    }
    if (UpdateMeAmtPolicyFromSetup(MePlatformPolicyInstance, &MeRcConfiguration)) {
      //
      // update FW Downgrade in Setup
      //
      Status = SetMeSetupConfig (&MeRcConfiguration);
    }
  }

  return Status;
}

/**

  Functions performs HECI exchange with FW to update MePlatformPolicy settings.

  @param None

  @retval Status.

**/
EFI_STATUS
EFIAPI
MePlatformPolicyUpdateMeSettings (
  DXE_ME_POLICY_PROTOCOL *MePlatformPolicyInstance
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
#if defined(AMT_SUPPORT) && AMT_SUPPORT
  ME_BIOS_PAYLOAD_HOB   *MbpHob = NULL;
#endif // AMT_SUPPORT

  DEBUG ((EFI_D_ERROR, "[ME Policy] MePlatformPolicyUpdateMeSettings start\n"));
  ASSERT(MePlatformPolicyInstance != NULL);

  MePlatformPolicyInstance->MeConfig.MeFirmwareMode = GetMeMode();
#if defined(AMT_SUPPORT) && AMT_SUPPORT
#if SPS_SUPPORT
  if (MeTypeIsAmt())
#endif // SPS_SUPPORT
  {
  //
  MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);
  if (MbpHob == NULL) {
      MePlatformPolicyInstance->MeConfig.MeFirmwareMode  = ME_MODE_FAILED;
      MePlatformPolicyInstance->MeAmtConfig.MeMajor         = 0;
      MePlatformPolicyInstance->MeAmtConfig.MeMinor         = 0;
      MePlatformPolicyInstance->MeAmtConfig.MeHotFix        = 0;
      MePlatformPolicyInstance->MeAmtConfig.MeBuildNo       = 0;
      MePlatformPolicyInstance->MeAmtConfig.MeFileSystem    = 0;
      MePlatformPolicyInstance->MeAmtConfig.MeImageType     = 0;
    } else {
      MePlatformPolicyInstance->MeAmtConfig.MeMajor      = MbpHob->MeBiosPayload.FwVersionName.MajorVersion;
      MePlatformPolicyInstance->MeAmtConfig.MeMinor      = MbpHob->MeBiosPayload.FwVersionName.MinorVersion;
      MePlatformPolicyInstance->MeAmtConfig.MeHotFix     = MbpHob->MeBiosPayload.FwVersionName.HotfixVersion;
      MePlatformPolicyInstance->MeAmtConfig.MeBuildNo    = MbpHob->MeBiosPayload.FwVersionName.BuildVersion;
      MePlatformPolicyInstance->MeAmtConfig.MeFileSystem = MbpHob->MeBiosPayload.MFSIntegrity;
      MePlatformPolicyInstance->MeAmtConfig.MeImageType  = (UINT8) MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType;
    }
  }
#endif // AMT_SUPPORT

  return Status;
}


/**

  Function that is called when we save UI Data

  @param MePlatformPolicy     Pointer to ME Policy
  @param mSetupData           Pointer to Setup Data
  @param resetRequest         Returns information if function requests reset

  @retval EFI_SUCCESS           Initialization complete.


**/
EFI_STATUS
updateFwuData (
  DXE_ME_POLICY_PROTOCOL *MePlatformPolicy,
  VOID                   *mSetupData,
  BOOLEAN                *resetRequest
  )
{
#if AMT_SUPPORT
  EFI_STATUS              Status;
  HECI_PROTOCOL           *Heci;
  UINT32                  MeMode;
  ME_RC_CONFIGURATION     *pSetupData;
  pSetupData = (ME_RC_CONFIGURATION *)(UINTN)mSetupData;

#if SPS_SUPPORT
  if (MeTypeIsAmt())
#endif // SPS_SUPPORT
  {
    if ((MePlatformPolicy == NULL) || (pSetupData == NULL)) {
      DEBUG ((EFI_D_ERROR, "[ME Policy] ERROR: updateFwuData - Wrong input data\n"));
      return EFI_INVALID_PARAMETER;
    }

    //
    // Skip Local Firmware update if not Consumer sku
    //
    if (pSetupData->MeImageType != INTEL_ME_CONSUMER_FW) {
      return EFI_SUCCESS;
    }

    Status = gBS->LocateProtocol (
               &gHeciProtocolGuid,
               NULL,
               &Heci
             );

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "[ME Policy] Error: Can't find Heci protocol, Status = %r\n", Status));
      return Status;
    }

    //
    // Get ME Operation Mode
    //
    Status = Heci->GetMeMode (&MeMode);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "[ME Policy] Error: Can't get ME mode, Status = %r\n", Status));
      return Status;
    }

    //
    // Skip Local Firmware update if not in normal mode
    //
    if (MeMode != ME_MODE_NORMAL) {
      return EFI_SUCCESS;
    }

    Status = gBS->LocateProtocol (&gDxePlatformMePolicyProtocolGuid, NULL, &MePlatformPolicy);
    if (EFI_ERROR(Status)) {
      // no gDxePlatformMePolicyProtocolGuid, no update (reset should be requested)
      return EFI_SUCCESS;
    }

    if (MePlatformPolicy->MeAmtConfig.PreviousLocalFwUpdEnabled != pSetupData->LocalFwUpdEnabled) {
      if (!MeIsAfterEndOfPost()) {
        Status = HeciSetLocalFwUpdate (pSetupData->LocalFwUpdEnabled);
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "[ME Policy] HeciSetLocalFwUpdate Status = %r\n", Status));
          ASSERT_EFI_ERROR (Status);
          return Status;
        }
      }
      MePlatformPolicy->MeAmtConfig.PreviousLocalFwUpdEnabled = pSetupData->LocalFwUpdEnabled;
      MePlatformPolicy->AnythingChanged = TRUE;
    }
  }
#endif // AMT_SUPPORT
  return EFI_SUCCESS;
}

/**

  Gets data from FW and updates initial values of BIOS setup data structures.
  Function is called only if we enter BIOS Setup, because it sends heci
  messages and takes some time to execute.

  @param Event         Pointer to the event that triggered this Callback Function
  @param Context       VOID Pointer required for Callback function

  @retval EFI_SUCCESS           Initialization complete.


**/
EFI_STATUS
UpdateSetupDataWithFwInfo (
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
{
  EFI_STATUS             Status;
  UINT8                  FwUpdateState;
  ME_RC_CONFIGURATION    MeRcConfiguration;
  ME_RC_CONFIGURATION    *MeRcConfigPtr;
  UINT32                 MeMode;
#if AMT_SUPPORT
  MEFWCAPS_SKU           CurrentFeatures;
  DXE_ME_POLICY_PROTOCOL *MePlatformPolicy;
#endif // AMT_SUPPORT

  FwUpdateState = 0;
  MeRcConfigPtr     = &MeRcConfiguration;

  MeMode = GetMeMode();
  if (MeMode == ME_MODE_FAILED) {
    return EFI_DEVICE_ERROR;
   }

  Status = GetMeSetupConfig (&MeRcConfiguration);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[ME Policy] UpdateSetupDataWithFwInfo - Error %r getting Setup Data \n", Status));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

#if defined(AMT_SUPPORT) && AMT_SUPPORT
#if SPS_SUPPORT
  if (MeTypeIsAmt())
#endif // SPS_SUPPORT
  {
    MeRcConfigPtr->MeStateControl = 0;
    if (MeMode == ME_MODE_NORMAL) {
      MeRcConfigPtr->MeStateControl = 1;
      Status = HeciGetFwFeatureStateMsg (&CurrentFeatures);
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "[ME Policy] HeciGetFwFeatureStateMsg Status = %r\n", Status));
        return EFI_SUCCESS; // Event should not return error status, so just exit routine
      }

      if (CurrentFeatures.Fields.Manageability) {
        MeRcConfigPtr->MngState = 1;
      } else {
        MeRcConfigPtr->MngState = 0;
      }
    }
  }
#endif // AMT_SUPPORT

  Status = SetMeSetupConfig (&MeRcConfiguration);

#if defined(AMT_SUPPORT) && AMT_SUPPORT
#if SPS_SUPPORT
  if (MeTypeIsAmt())
#endif // SPS_SUPPORT
  {
    //
    // Skip Local Firmware update if not Consumer sku or not in normal mode
    //
    if ((MeRcConfigPtr->MeImageType != INTEL_ME_CONSUMER_FW) || (MeMode != ME_MODE_NORMAL)) {
      return EFI_SUCCESS;
    }

    Status = HeciGetLocalFwUpdate (&FwUpdateState);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "[ME Policy] HeciGetLocalFwUpdate Status = %r\n", Status));
      ASSERT_EFI_ERROR (Status);
      return Status;
    }

    //
    // MEBx Provides three options - Enabled/Disable/Password Protected
    // BIOS provides two options. If for some reason Password Protected
    // is selected then update FW with BIOS default/Selection
    //
    if (MeRcConfigPtr->LocalFwUpdEnabled != FwUpdateState) {
      if (FwUpdateState < MAX_FW_UPDATE_BIOS_SELECTIONS) {
        MeRcConfigPtr->LocalFwUpdEnabled = FwUpdateState;
      }

      Status = SetMeSetupConfig (&MeRcConfiguration);
    }
    Status = gBS->LocateProtocol (&gDxePlatformMePolicyProtocolGuid, NULL, &MePlatformPolicy);
    if (!EFI_ERROR(Status)) {
      MePlatformPolicy->MeAmtConfig.PreviousMeStateControl = MeRcConfigPtr->MeStateControl;
      MePlatformPolicy->MeAmtConfig.PreviousLocalFwUpdEnabled = MeRcConfigPtr->LocalFwUpdEnabled;
      MePlatformPolicy->AnythingChanged = FALSE;
    }
  }
#endif // AMT_SUPPORT

  return EFI_SUCCESS;
}

/**

  Function that is called when we save UI Data

  @param MePlatformPolicy     Pointer to ME Policy
  @param mSetupData           Pointer to Setup Data
  @param resetRequest         Returns information if function requests reset

  @retval EFI_SUCCESS           Initialization complete.


**/
EFI_STATUS
updateMngState (
  DXE_ME_POLICY_PROTOCOL *MePlatformPolicy,
  VOID                   *mSetupData,
  BOOLEAN                *resetRequest
)
{
  ME_RC_CONFIGURATION *pSetupData;
  pSetupData = (ME_RC_CONFIGURATION *)(UINTN)mSetupData;
  DEBUG ((EFI_D_ERROR, "[ME Policy] updateMngState - called\n"));
  if ((pSetupData == NULL) || (MePlatformPolicy == NULL)) {
    DEBUG ((EFI_D_ERROR, "[ME Policy] Error: Wrong parameters\n"));
    return EFI_INVALID_PARAMETER;
  }

#if AMT_SUPPORT
#if SPS_SUPPORT
  if (MeTypeIsAmt())
#endif // SPS_SUPPORT
  {
    EFI_STATUS             Status;
    UINT32                 EnableBitmap = 0;
    UINT32                 DisableBitmap = 0;
    MEFWCAPS_SKU           CurrentFeatures;
    UINT8                  AmtState = 0;

    if (resetRequest == NULL) {
      // Called not from setup,
      // in case of change reset cannot be applied
      return EFI_SUCCESS;
    }

    Status = HeciGetFwFeatureStateMsg (&CurrentFeatures);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "[ME Policy] HeciGetFwFeatureStateMsg Status = %r\n", Status));
      return Status;
    }

    if (CurrentFeatures.Fields.Manageability) {
      AmtState = 1;
    } else {
      AmtState = 0;
    }

    if (AmtState == pSetupData->MngState) {
      return EFI_SUCCESS;
    }

    if (pSetupData->MngState == 1) {
      //
      // User wants to enable AMT
      //
      EnableBitmap  = AMT_BITMASK;
      DisableBitmap = CLEAR_AMT_BIT;
    } else if (pSetupData->MngState == 0) {
      //
      // User wants to disable AMT
      //
      EnableBitmap  = CLEAR_AMT_BIT;
      DisableBitmap = AMT_BITMASK;
    }

    if (!MeIsAfterEndOfPost()) {
      Status = HeciFwFeatureStateOverride (EnableBitmap, DisableBitmap);
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "[ME Policy] HeciFwFeatureStateOverride Status = %r\n", Status));
        ASSERT_EFI_ERROR (Status);
        return Status;
      }
      if (resetRequest) {
        *resetRequest = TRUE;
      }
    }

    MePlatformPolicy->AnythingChanged = TRUE;
  }
#endif // AMT_SUPPORT

  return EFI_SUCCESS;
}

/**

  Function that is Called if we change Manageability State BIOS Setup Option

  @param MePlatformPolicy     Pointer to ME Policy
  @param mSetupData           Pointer to Setup Data
  @param resetRequest         Returns information if function requests reset

  @retval EFI_SUCCESS           Initialization complete.


**/
EFI_STATUS
updateMeState (
  DXE_ME_POLICY_PROTOCOL *MePlatformPolicy,
  VOID                   *mSetupData,
  BOOLEAN                *resetRequest
)
{
  ME_RC_CONFIGURATION *pSetupData;
  pSetupData = (ME_RC_CONFIGURATION *)(UINTN)mSetupData;
  
  DEBUG ((EFI_D_ERROR, "[ME Policy] updateMeState called\n"));

  if ((pSetupData == NULL) || (MePlatformPolicy == NULL)) {
    DEBUG ((EFI_D_ERROR, "[ME Policy] ERROR: updateMeState - Invalid parameters\n"));
    return EFI_INVALID_PARAMETER;
  }

#if defined(AMT_SUPPORT) && AMT_SUPPORT
  if (!MeIsAfterEndOfPost()
#if defined(SPS_SUPPORT) && SPS_SUPPORT
      && MeTypeIsAmt()
#endif // SPS_SUPPORT
  ) {
    EFI_STATUS              Status;

    if (MePlatformPolicy->MeAmtConfig.PreviousMeStateControl != pSetupData->MeStateControl) {
      HECI_PROTOCOL *Heci;
      UINT32        MeMode;

      Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "[ME Policy] ERROR: updateMeState(): Can't locate Heci protocol, Status = %r\n", Status));
        return EFI_UNSUPPORTED;
      }
      Status = Heci->GetMeMode (&MeMode);
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "[ME Policy] ERROR: updateMeState(): Can't get ME Mode, Status = %r\n", Status));
        return EFI_UNSUPPORTED;
      }
      //
      // ME State Control
      //
      if (pSetupData->MeStateControl == 0) {
        if (MeMode == ME_MODE_NORMAL) {
          Status = HeciSetMeDisableMsg ();
          if (EFI_ERROR (Status)) {
            DEBUG ((EFI_D_ERROR, "[ME Policy] HeciSetMeDisableMsg Status = %r\n", Status));
            return Status;
          }
        } else {
          DEBUG ((EFI_D_INFO, "[ME Policy] HeciSetMeDisableMsg() not called. ME is not in Normal state already\n"));
        }
      }

      if (pSetupData->MeStateControl == 1) {
        if (MeMode != ME_MODE_NORMAL) {
          Status = HeciSetMeEnableMsg ();
          if (EFI_ERROR (Status)) {
            DEBUG ((EFI_D_ERROR, "[ME Policy] HeciSetMeEnableMsg Status = %r\n", Status));
            return Status;
          }
        } else {
          DEBUG ((EFI_D_INFO, "[ME Policy] HeciSetMeEnableMsg() not called. ME is in Normal state already\n"));
        }
      }
      MePlatformPolicy->MeAmtConfig.PreviousMeStateControl = pSetupData->MeStateControl;
      MePlatformPolicy->AnythingChanged = TRUE;
    }
  }
#endif // AMT_SUPPORT

  return EFI_SUCCESS;
}
