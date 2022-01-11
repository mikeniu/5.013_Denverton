/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2012 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MePolicyDebugDumpDxe.c

@brief:
  Dump whole DXE_ME_POLICY_PROTOCOL.
**/
#include "Library/DebugLib.h"
#include "Library/MePolicyLib.h"
#include "Library/MeTypeLib.h"
#include "Protocol/MePlatformPolicyProtocol.h"

extern DXE_ME_POLICY_PROTOCOL  *mDxePlatformMePolicy;

VOID
DxeMePolicyDebugDump (
  VOID
  )
{
  if (mDxePlatformMePolicy == NULL) {
    EFI_STATUS Status = MePolicyLibInit ();
    if (EFI_ERROR (Status)) {
      return;
    }
  }
  MePolicyDebugDump(mDxePlatformMePolicy, ME_POLICY_DUMP_ALL, EFI_D_INFO);
}
/**

  Dump DXE ME Platform Policy

  @param[in] this       - Specifies pointer to ME Platform Policy instance.
  @param[in] WhatToDump - Specifies what part of policy has to be dumped.
  @param[in] DebugLevel - Specifies used debug level for dump output.

**/
VOID
MePolicyDebugDump (
  DXE_ME_POLICY_PROTOCOL *this,
  ME_POLICY_DUMP_TYPE    WhatToDump,
  UINTN                  DebugLevel
  )
{
  if (this == NULL) {
    return;
  }

  if (WhatToDump == ME_POLICY_DUMP_ALL) {
    WhatToDump = ME_POLICY_DUMP_COMMON +
                 ME_POLICY_DUMP_AMT +
                 ME_POLICY_DUMP_SPS;
  }

  if (DebugLevel == 0) {
    DebugLevel = EFI_D_INFO;
  }

  DEBUG ((DebugLevel, "\n------------------------ MePlatformPolicy Dump Begin -----------------\n"));
  DEBUG ((DebugLevel, "Revision : 0x%x\n", this->Revision));

  if (WhatToDump & ME_POLICY_DUMP_COMMON) {
    DEBUG ((DebugLevel, "AnythingChanged : 0x%x\n", this->AnythingChanged));
    DEBUG ((DebugLevel, "MeType : 0x%x\n", this->MeType));
    DEBUG ((DebugLevel, "MeConfig ---\n"));
    DEBUG ((DebugLevel, " EndOfPostEnabled : 0x%x\n", this->MeConfig.EndOfPostEnabled));
    DEBUG ((DebugLevel, " EndOfPostDone : 0x%x\n", this->MeConfig.EndOfPostDone));
    DEBUG ((DebugLevel, " SpsMeShutdownEnabled : 0x%x\n", this->MeConfig.SpsMeShutdownEnabled));
    DEBUG ((DebugLevel, " HeciCommunication1 : 0x%x\n", this->MeConfig.HeciCommunication1));
    DEBUG ((DebugLevel, " HeciCommunication2 : 0x%x\n", this->MeConfig.HeciCommunication2));
    DEBUG ((DebugLevel, " HeciCommunication3 : 0x%x\n", this->MeConfig.HeciCommunication3));
    DEBUG ((DebugLevel, " MeIderEnabled : 0x%x\n", this->MeConfig.MeIderEnabled));
    DEBUG ((DebugLevel, " MeKtEnabled   : 0x%x\n", this->MeConfig.MeKtEnabled));
    DEBUG ((DebugLevel, " Heci1HideInMe : 0x%x\n", this->MeConfig.Heci1HideInMe));
    DEBUG ((DebugLevel, " Heci2HideInMe : 0x%x\n", this->MeConfig.Heci2HideInMe));
    DEBUG ((DebugLevel, " Heci3HideInMe : 0x%x\n", this->MeConfig.Heci3HideInMe));
    DEBUG ((DebugLevel, " MeGrPromotionEnabled : 0x%x\n", this->MeConfig.MeGrPromotionEnabled));
    DEBUG ((DebugLevel, " MeFirmwareMode : 0x%x\n", this->MeConfig.MeFirmwareMode));

    DEBUG ((DebugLevel, "MeMiscConfig ---\n"));
    DEBUG ((DebugLevel, " FviSmbiosType : 0x%x\n", this->MeMiscConfig.FviSmbiosType));
  }
#if AMT_SUPPORT
  if ((WhatToDump & ME_POLICY_DUMP_AMT)
#if SPS_SUPPORT
    && (MeTypeIsAmt())
#endif // SPS_SUPPORT
  ) {
    DEBUG ((DebugLevel, "MeAmtConfig ---\n"));
    DEBUG ((DebugLevel, " MeFwDownGrade : 0x%x\n", this->MeAmtConfig.MeFwDownGrade));
    DEBUG ((DebugLevel, " MeLocalFwUpdEnabled : 0x%x\n", this->MeAmtConfig.MeLocalFwUpdEnabled));
    DEBUG ((DebugLevel, " MbpSecurity : 0x%x\n", this->MeAmtConfig.MbpSecurity));
    DEBUG ((DebugLevel, " MdesCapability : 0x%x\n", this->MeAmtConfig.MdesCapability));
    DEBUG ((DebugLevel, " SvtForPchCap : 0x%x\n", this->MeAmtConfig.SvtForPchCap));
    DEBUG ((DebugLevel, " MdesForBiosState : 0x%x\n", this->MeAmtConfig.MdesForBiosState));
    DEBUG ((DebugLevel, " MeStateControl : 0x%x\n", this->MeAmtConfig.MeStateControl));
    DEBUG ((DebugLevel, " RemoteSessionActive : 0x%x\n", this->MeAmtConfig.RemoteSessionActive));

    DEBUG ((DebugLevel, " PreviousMeStateControl : 0x%x\n", this->MeAmtConfig.PreviousMeStateControl));

    DEBUG ((DebugLevel, " PreviousLocalFwUpdEnabled : 0x%x\n", this->MeAmtConfig.PreviousLocalFwUpdEnabled));

    DEBUG ((DebugLevel, " MeMajor : 0x%x\n", this->MeAmtConfig.MeMajor));
    DEBUG ((DebugLevel, " MeMinor : 0x%x\n", this->MeAmtConfig.MeMinor));
    DEBUG ((DebugLevel, " MeHotFix : 0x%x\n", this->MeAmtConfig.MeHotFix));
    DEBUG ((DebugLevel, " MeBuildNo : 0x%x\n", this->MeAmtConfig.MeBuildNo));
    DEBUG ((DebugLevel, " MeFileSystem : 0x%x\n", this->MeAmtConfig.MeFileSystem));
    DEBUG ((DebugLevel, " MeImageType : 0x%x\n", this->MeAmtConfig.MeImageType));

    DEBUG ((DebugLevel, " AsfEnabled : 0x%x\n", this->MeAmtConfig.AsfEnabled));
    DEBUG ((DebugLevel, " AmtEnabled : 0x%x\n", this->MeAmtConfig.AmtEnabled));
    DEBUG ((DebugLevel, " AmtbxPasswordWrite : 0x%x\n", this->MeAmtConfig.AmtbxPasswordWrite));
    DEBUG ((DebugLevel, " WatchDog : 0x%x\n", this->MeAmtConfig.WatchDog));
    DEBUG ((DebugLevel, " WatchDogOs : 0x%x\n", this->MeAmtConfig.WatchDogOs));
    DEBUG ((DebugLevel, " CiraRequest : 0x%x\n", this->MeAmtConfig.CiraRequest));
    DEBUG ((DebugLevel, " ManageabilityMode : 0x%x\n", this->MeAmtConfig.ManageabilityMode));
    DEBUG ((DebugLevel, " UnConfigureMe : 0x%x\n", this->MeAmtConfig.UnConfigureMe));
    DEBUG ((DebugLevel, " MebxDebugMsg : 0x%x\n", this->MeAmtConfig.MebxDebugMsg));

    DEBUG ((DebugLevel, " ForcMebxSyncUp : 0x%x\n", this->MeAmtConfig.ForcMebxSyncUp));
    DEBUG ((DebugLevel, " UsbrEnabled : 0x%x\n", this->MeAmtConfig.UsbrEnabled));
    DEBUG ((DebugLevel, " UsbLockingEnabled : 0x%x\n", this->MeAmtConfig.UsbLockingEnabled));
    DEBUG ((DebugLevel, " HideUnConfigureMeConfirm : 0x%x\n", this->MeAmtConfig.HideUnConfigureMeConfirm));
    DEBUG ((DebugLevel, " USBProvision : 0x%x\n", this->MeAmtConfig.USBProvision));
    DEBUG ((DebugLevel, " FWProgress : 0x%x\n", this->MeAmtConfig.FWProgress));
    DEBUG ((DebugLevel, " AmtbxHotKeyPressed : 0x%x\n", this->MeAmtConfig.AmtbxHotkeyPressed));
    DEBUG ((DebugLevel, " AmtbxSelectionScreen  : 0x%x\n", this->MeAmtConfig.AmtbxSelectionScreen));

    DEBUG ((DebugLevel, " AtEnabled : 0x%x\n", this->MeAmtConfig.AtEnabled));

    DEBUG ((DebugLevel, " WatchDogTimerOs : 0x%x\n", this->MeAmtConfig.WatchDogTimerOs));

    DEBUG ((DebugLevel, " WatchDogTimerBios : 0x%x\n", this->MeAmtConfig.WatchDogTimerBios));

    DEBUG ((DebugLevel, " CiraTimeout : 0x%x\n", this->MeAmtConfig.CiraTimeout));

    DEBUG ((DebugLevel, " CPU Replacement Timeout : 0x%x\n", this->MeAmtConfig.CpuReplacementTimeout));

    DEBUG ((DebugLevel, " MebxNonUiTextMode : 0x%x\n", this->MeAmtConfig.MebxNonUiTextMode));
    DEBUG ((DebugLevel, " MebxUiTextMode : 0x%x\n", this->MeAmtConfig.MebxUiTextMode));
    DEBUG ((DebugLevel, " MebxGraphicsMode : 0x%x\n", this->MeAmtConfig.MebxGraphicsMode));

    DEBUG ((DebugLevel, " OemResolutionSettingsRsvd : 0x%x\n", this->MeAmtConfig.OemResolutionSettingsRsvd));

    DEBUG ((DebugLevel, " PciDeviceFilterOutTable Pointer : 0x%x\n", this->MeAmtConfig.PciDeviceFilterOutTable));
    DEBUG ((DEBUG_INFO, " CoreBiosDoneEnabled : 0x%x\n", this->MeAmtConfig.CoreBiosDoneEnabled));
#ifdef TESTMENU_FLAG
    DEBUG ((DebugLevel, " MebxLaunchTimeout : 0x%x\n", this->MeAmtConfig.MebxLaunchTimeout));
#endif
  }
#endif // AMT_SUPPORT
#ifdef SPS_SUPPORT
  if ((WhatToDump & ME_POLICY_DUMP_SPS)
#if AMT_SUPPORT
      && (MeTypeIsSps())
#endif // AMT_SUPPORT
  ) {
    DEBUG ((DebugLevel, "MeNmConfig ---\n"));
    DEBUG ((DebugLevel, " NmPwrOptBoot : 0x%x\n", this->MeNmConfig.NmPwrOptBoot));
    DEBUG ((DebugLevel, " NmPwrOptBootOverride : 0x%x\n", this->MeNmConfig.NmPwrOptBootOverride));
    DEBUG ((DebugLevel, " NmCores2Disable : 0x%x\n", this->MeNmConfig.NmCores2Disable));
    DEBUG ((DebugLevel, " NmCores2DisableOverride : 0x%x\n", this->MeNmConfig.NmCores2DisableOverride));
    DEBUG ((DebugLevel, " NmPowerMsmtSupport : 0x%x\n", this->MeNmConfig.NmPowerMsmtSupport));
    DEBUG ((DebugLevel, " NmHwChangeStatus : 0x%x\n", this->MeNmConfig.NmHwChangeStatus));
    DEBUG ((DebugLevel, " SpsAltitude : 0x%x\n", this->MeNmConfig.SpsAltitude));
    DEBUG ((DebugLevel, " SpsMctpBusOwner : 0x%x\n", this->MeNmConfig.SpsMctpBusOwner));
    DEBUG ((DebugLevel, " MeHmrfpoEnableEnabled : 0x%x\n", this->MeNmConfig.MeHmrfpoEnableEnabled));
    DEBUG ((DebugLevel, " MeHmrfpoLockEnabled : 0x%x\n", this->MeNmConfig.MeHmrfpoLockEnabled));
    DEBUG ((DebugLevel, " MeGrLockEnabled : 0x%x\n", this->MeNmConfig.MeGrLockEnabled));
#if TESTMENU_FLAG || ME_TESTMENU_FLAG
    DEBUG ((DebugLevel, " NmPtuLoadOverride : 0x%x\n", this->MeNmConfig.NmPtuLoadOverride));
#endif
  }
#endif // SPS_SUPPORT
  DEBUG ((DebugLevel, "\n------------------------ MePlatformPolicy Dump End -------------------\n"));
}
