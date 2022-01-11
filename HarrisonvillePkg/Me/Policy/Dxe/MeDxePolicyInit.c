/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MeDxePolicyInit.c

@brief:
  This file is SampleCode for Intel ME DXE Platform Policy initialization.


**/
#include <Library/MmPciBaseLib.h>
#include "MeDxePolicyInit.h"
#include "Protocol/HeciProtocol.h"
#include "MeState.h"
#include "Library/MeTypeLib.h"
#include "Library/MeTypeSetLib.h"
#include "MeSetup.h"

DXE_ME_POLICY_PROTOCOL  mMeDxePlatformPolicy = { 0 };

///
/// TS DIMM thermal polling Smbus Address.
/// This is platform specific.
///
UINT8                   mTsDimmSmbusAddress[] = { 0x30, 0x34 };

#if AMT_SUPPORT
#include "Guid/MeBiosExtensionSetup.h"
#include "MeBiosPayloadData.h"
#include "MeBiosPayloadHob.h"
#include <Library/HobLib.h>

//   EDK2_TODO
#ifndef EFI_MAX_ADDRESS
#define EFI_MAX_ADDRESS   0xFFFFFFFF        /* Definition in EfiBind.h */
#endif // EFI_MAX_ADDRESS

UINT64                  mPciDeviceFilterOutTable[]     = { EFI_MAX_ADDRESS };
CHAR16                  gMeBiosExtensionSetupName[] = ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME;
#endif // AMT_SUPPORT
///
/// Function implementations
///
/**

  @brief
  Initilize Intel ME DXE Platform Policy

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval EFI_SUCCESS             Initialization complete.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
MeDxePolicyInitEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS              Status;
  UINTN                   PchSpiBase;
#if AMT_SUPPORT
  ME_BIOS_PAYLOAD_HOB     *MbpHob = NULL;
#endif // AMT_SUPPORT


  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "[ME Policy] ME Policy (Dxe) Entry Point\n"));

  //
  // DFX FW doesn't exchange any HECI messages
  //
  if (MeTypeIsDfx()) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "[ME Policy] ME Policy (Dxe) DFX FW detected: EXIT\n"));
    return EFI_UNSUPPORTED;
  }


  ///
  /// ME DXE Policy Init
  ///
  mMeDxePlatformPolicy.Revision = DXE_PLATFORM_ME_POLICY_PROTOCOL_REVISION_1;

  ///
  /// Initialize the Me Configuration
  ///
  mMeDxePlatformPolicy.MeConfig.EndOfPostEnabled             = 1;
#if AMT_SUPPORT
  mMeDxePlatformPolicy.MeAmtConfig.MeFwDownGrade             = 0;
  mMeDxePlatformPolicy.MeAmtConfig.MeLocalFwUpdEnabled       = 0;
  mMeDxePlatformPolicy.MeAmtConfig.MbpSecurity               = 1;
  mMeDxePlatformPolicy.MeAmtConfig.AsfEnabled                = 1;
  mMeDxePlatformPolicy.MeAmtConfig.AmtEnabled                = 1;
  mMeDxePlatformPolicy.MeAmtConfig.WatchDog                  = 0;
  mMeDxePlatformPolicy.MeAmtConfig.WatchDogOs                = 0;
  mMeDxePlatformPolicy.MeAmtConfig.WatchDogTimerOs           = 0;
  mMeDxePlatformPolicy.MeAmtConfig.WatchDogTimerBios         = 0;
  mMeDxePlatformPolicy.MeAmtConfig.CiraRequest               = 0;
  mMeDxePlatformPolicy.MeAmtConfig.CiraTimeout               = 0;
  mMeDxePlatformPolicy.MeAmtConfig.UnConfigureMe             = 0;
  mMeDxePlatformPolicy.MeAmtConfig.HideUnConfigureMeConfirm  = 0;
  mMeDxePlatformPolicy.MeAmtConfig.MebxDebugMsg              = 0;
  mMeDxePlatformPolicy.MeAmtConfig.USBProvision              = 0;
  mMeDxePlatformPolicy.MeAmtConfig.FWProgress                = 1;
  mMeDxePlatformPolicy.MeAmtConfig.AmtbxSelectionScreen      = 0;
  mMeDxePlatformPolicy.MeAmtConfig.AmtbxHotkeyPressed        = 0;
  mMeDxePlatformPolicy.MeAmtConfig.CoreBiosDoneEnabled       = 1;

  ///
  /// Oem Resolution Settings
  ///
  mMeDxePlatformPolicy.MeAmtConfig.MebxNonUiTextMode = 0;
  ///
  /// MEBX_TEXT_AUTO
  ///
  mMeDxePlatformPolicy.MeAmtConfig.MebxUiTextMode = 0;
  ///
  /// MEBX_TEXT_AUTO
  ///
  mMeDxePlatformPolicy.MeAmtConfig.MebxGraphicsMode = 0;
  ///
  /// MEBX_GRAPHICS_AUTO
  ///
  mMeDxePlatformPolicy.MeAmtConfig.PciDeviceFilterOutTable = (UINT32) (UINTN) mPciDeviceFilterOutTable;

  ///
  /// Please don't change the default value of ForcMebxSyncUp and
  /// This let customer to force MEBX execution if they need and
  ///
  mMeDxePlatformPolicy.MeAmtConfig.ForcMebxSyncUp = 0;

  ///
  /// Get BIOS Sync-up data from MEBx to AMT platform policy
  ///
  mMeDxePlatformPolicy.MeAmtConfig.ManageabilityMode = 0;
  
  MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);
  if ((MbpHob != NULL) &&
      (MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType == INTEL_ME_CORPORATE_FW)) {
    UINTN                   VariableSize;
    ME_BIOS_EXTENSION_SETUP MeBiosExtensionSetupData;
    ///
    /// Get BIOS Sync-up data from MEBx to AMT platform policy
    ///
    VariableSize = sizeof (MeBiosExtensionSetupData);
    Status = gRT->GetVariable (
                gMeBiosExtensionSetupName,
                &gMeBiosExtensionSetupGuid,
                NULL,
                &VariableSize,
                &MeBiosExtensionSetupData
                );
    if (!EFI_ERROR (Status)) {
      mMeDxePlatformPolicy.MeAmtConfig.ManageabilityMode = MeBiosExtensionSetupData.PlatformMngSel;
    }
  }
#endif // AMT_SUPPORT

  ///
  /// Please don't change the default value of EndOfPostDone and
  /// the value will be update to 1 when EOP has been sent to ME FW successfully
  ///
  mMeDxePlatformPolicy.MeConfig.EndOfPostDone = 0;

  ///
  /// Thermal reporting policy is based on strap settings
  ///
#define R_PCH_SPI_STRP15              0x3C

  PchSpiBase = MmioRead32 (MmPciBase (
                           DEFAULT_PCI_BUS_NUMBER_PCH,
                           PCI_DEVICE_NUMBER_PCH_SPI,
                           PCI_FUNCTION_NUMBER_PCH_SPI)
                           + R_PCH_SPI_BAR0) & ~(B_PCH_SPI_BAR0_MASK);
  MmioAndThenOr32 (
    PchSpiBase + R_PCH_SPI_FDOC,
    (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)),
    (UINT32) (V_PCH_SPI_FDOC_FDSS_PCHS | R_PCH_SPI_STRP15)
    );

  mMeDxePlatformPolicy.MeReportError = ShowMeReportError;
#ifdef SPS_SUPPORT
  mMeDxePlatformPolicy.MeNmConfig.MeHmrfpoLockEnabled = TRUE;
  mMeDxePlatformPolicy.MeNmConfig.MeGrLockEnabled = TRUE;
#endif // SPS_SUPPORT
  mMeDxePlatformPolicy.MeConfig.EndOfPostEnabled = TRUE;

  mMeDxePlatformPolicy.MeConfig.HeciCommunication1 = SETUP_HECI_DEVICE_AUTO;
  mMeDxePlatformPolicy.MeConfig.HeciCommunication2 = SETUP_HECI_DEVICE_AUTO;
  mMeDxePlatformPolicy.MeConfig.HeciCommunication3 = SETUP_HECI_DEVICE_AUTO;
  mMeDxePlatformPolicy.MeConfig.MeIderEnabled = SETUP_HECI_DEVICE_AUTO;
  mMeDxePlatformPolicy.MeConfig.MeKtEnabled = SETUP_HECI_DEVICE_AUTO;

  mMeDxePlatformPolicy.saveMePolicyToSetupConfiguration = saveMePolicyToSetupConfiguration;
  mMeDxePlatformPolicy.UpdateMePolicyFromSetup = UpdateMePolicyFromSetup;
  mMeDxePlatformPolicy.MePolicyDebugDump = MePolicyDebugDump;

  InstallMEPlatformPolicyLibrary (&mMeDxePlatformPolicy);

  ///
  /// Misc. Config
  ///
  /// FviSmbiosType is the SMBIOS OEM type (0x80 to 0xFF) defined in SMBIOS Type 14 - Group
  /// Associations structure - item type. FVI structure uses it as SMBIOS OEM type to provide
  /// version information. The default value is type 221.
  ///
  mMeDxePlatformPolicy.MeMiscConfig.FviSmbiosType = 0xDD;

  
  ///
  /// Install protocol to to allow access to this Policy.
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gDxePlatformMePolicyProtocolGuid,
                  &mMeDxePlatformPolicy,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Set MeType variable
  ///
  MeTypeSet(&mMeDxePlatformPolicy);

  ///
  /// Save configuration changes to setup
  ///
  saveMePolicyToSetupConfiguration(&mMeDxePlatformPolicy);

  return Status;
}

/**

@brief
  Show Me Error message.

  @param[in] MsgId                Me error message ID.


**/
VOID
ShowMeReportError (
  IN ME_ERROR_MSG_ID            MsgId
  )
{
  gST->ConOut->ClearScreen (gST->ConOut);

  switch (MsgId) {
  case MSG_EOP_ERROR:
    gST->ConOut->OutputString (gST->ConOut, L"Error sending End Of Post message to ME!\n");
    //
    // Additional delay only for EOP error message
    //
    gBS->Stall (4 * HECI_MSG_DELAY);
    break;

  case MSG_ME_FW_UPDATE_FAILED:
    gST->ConOut->OutputString (gST->ConOut, L"ME FW Update Failed, please try again!\n");
    break;

  case MSG_ASF_BOOT_DISK_MISSING:
    gST->ConOut->OutputString (gST->ConOut, L"Boot disk missing, please insert boot disk and press ENTER\r\n");
    break;

  case MSG_KVM_TIMES_UP:
    gST->ConOut->OutputString (gST->ConOut, L"Error!! Times up and the KVM session was cancelled!!");
    break;

  case MSG_KVM_REJECTED:
    gST->ConOut->OutputString (gST->ConOut, L"Error!! The request has rejected and the KVM session was cancelled!!");
    break;

  case MSG_HMRFPO_LOCK_FAILURE:
    gST->ConOut->OutputString (gST->ConOut, L"(A7) Me FW Downgrade - Request MeSpiLock Failed\n");
    break;

  case MSG_HMRFPO_UNLOCK_FAILURE:
    gST->ConOut->OutputString (gST->ConOut, L"(A7) Me FW Downgrade - Request MeSpiEnable Failed\n");
    break;

  case MSG_ME_FW_UPDATE_WAIT:
    gST->ConOut->OutputString (
                  gST->ConOut,
                  L"Intel(R) Firmware Update is in progress. It may take up to 90 seconds. Please wait.\n"
                  );
    break;

  case MSG_ILLEGAL_CPU_PLUGGED_IN:
    gST->ConOut->OutputString (
                  gST->ConOut,
                  L"\n\n\rAn unsupported CPU/PCH configuration has been identified.\n"
                  );
    gST->ConOut->OutputString (
                  gST->ConOut,
                  L"\rPlease refer to the Haswell Platform Validation Matrix\n\rfor supported CPU/PCH combinations."
                  );
    break;

  case MSG_KVM_WAIT:
    gST->ConOut->OutputString (gST->ConOut, L"Waiting Up to 8 Minutes For KVM FW.....");
    break;

  default:
    DEBUG ((EFI_D_ERROR, "[ME Policy] This Message Id hasn't been defined yet, MsgId = %x\n", MsgId));
    break;
  }

  gBS->Stall (HECI_MSG_DELAY);

}
