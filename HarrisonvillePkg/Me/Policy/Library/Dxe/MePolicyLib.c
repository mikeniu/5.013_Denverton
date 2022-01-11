/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2006 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MePolicyLib.c

@brief:
  Implementation file for Me Policy functionality

**/

#include "Library/UefiBootServicesTableLib.h"
#include "Library/DebugLib.h"
#include "Library/MePolicyLib.h"
#include "MeAccess.h"
#include "Library/MeTypeLib.h"

///
/// Global variables
///
DXE_ME_POLICY_PROTOCOL  *mDxePlatformMePolicy = NULL;

/**

  Check if Me is enabled.

  @param[in] None.

**/
EFI_STATUS
MePolicyLibInit (
  VOID
  )
{
  EFI_STATUS  Status;

  if (mDxePlatformMePolicy != NULL) {
    return EFI_SUCCESS;
  }
  //
  // Get the desired platform setup policy.
  //
  Status = gBS->LocateProtocol (&gDxePlatformMePolicyProtocolGuid, NULL, &mDxePlatformMePolicy);
  return Status;
}

/**

  Check if End of Post Message is enabled in setup options.

  @param[in] None.

  @retval FALSE                   EndOfPost is disabled.
  @retval TRUE                    EndOfPost is enabled.

**/
BOOLEAN
MeEndOfPostEnabled (
  VOID
  )
{
  BOOLEAN    Supported;
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  if (mDxePlatformMePolicy->MeConfig.EndOfPostEnabled != 1) {
    Supported = FALSE;
  } else {
    Supported = TRUE;
  }

  return Supported;
}

/**

  Checks if EndOfPost event already happened

  @param[in] None
  @retval TRUE                    if end of post happened
  @retval FALSE                   if not yet

**/
BOOLEAN
MeIsAfterEndOfPost (
  VOID
  )
{
  EFI_STATUS              Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }

  if (mDxePlatformMePolicy->MeConfig.EndOfPostDone == TRUE) {
    return TRUE;
  }

  return FALSE;
}

/**

  Sets EndOfPost event value. Introduced for ICC support

  @param[in]
  @retval TRUE                    if end of post happened
  @retval FALSE                   if not yet

**/
BOOLEAN
SetAfterEndOfPost (
  BOOLEAN vEOP
  )
{
  EFI_STATUS              Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }

  mDxePlatformMePolicy->MeConfig.EndOfPostDone = vEOP;

  return MeIsAfterEndOfPost();
}

/**

  Show Me Error message.

  @param[in] MsgId                Me error message ID.


**/
VOID
MeReportError (
  ME_ERROR_MSG_ID MsgId
  )
{
  EFI_STATUS              Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return;
    }
  }
  mDxePlatformMePolicy->MeReportError (MsgId);

  return ;
}

/**

  Check if MeFwDowngrade is enabled in setup options.

  @param[in] None.

  @retval FALSE                   MeFwDowngrade is disabled.
  @retval TRUE                    MeFwDowngrade is enabled.

**/
BOOLEAN
MeFwDowngradeSupported (
  VOID
  )
{
#if AMT_SUPPORT
  EFI_STATUS              Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  if (mDxePlatformMePolicy->MeAmtConfig.MeFwDownGrade == 1) {
    return TRUE;
  }
#endif // AMT_SUPPORT

  return FALSE;
}

#ifdef AMT_SUPPORT
/**

  @brief
  Check if Asf is enabled in setup options.

  @param[in] None.

  @retval FALSE                   Asf is disabled.
  @retval TRUE                    Asf is enabled.

**/
BOOLEAN
AsfSupported (
  VOID
  )
{
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  ///
  /// First check if ASF support is enabled in Setup.
  ///
  if (mDxePlatformMePolicy->MeAmtConfig.AsfEnabled != 1) {
    return FALSE;
  }

  return TRUE;
}

/**

  @brief
  Check if Amt is enabled in setup options.

  @param[in] None.

  @retval FALSE                   Amt is disabled.
  @retval TRUE                    Amt is enabled.

**/
BOOLEAN
AmtSupported (
  VOID
  )
{
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  ///
  /// First check if AMT support is enabled in Setup.
  ///
  if (mDxePlatformMePolicy->MeAmtConfig.AmtEnabled != 1) {
    return FALSE;
  }

  return TRUE;
}

/**

  @brief

  Check if AMT BIOS Extension hotkey was pressed during BIOS boot.

  @param[in] None.

  @retval FALSE                   MEBx hotkey was not pressed.
  @retval TRUE                    MEBx hotkey was pressed.

**/
BOOLEAN
AmtHotkeyPressed (
  VOID
  )
{
  BOOLEAN    Supported;
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  ///
  /// First check if AMT Setup Prompt is enabled in Setup.
  ///
  if (mDxePlatformMePolicy->MeAmtConfig.AmtbxHotkeyPressed == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**

  @brief
  Check if AMT BIOS Extension Selection Screen is enabled in setup options.

  @param[in] None.

  @retval FALSE                   AMT Selection Screen is disabled.
  @retval TRUE                    AMT Selection Screen is enabled.

**/
BOOLEAN
AmtSelectionScreen (
  VOID
  )
{
  BOOLEAN    Supported;
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  ///
  /// First check if AMT Selection Screen is enabled in Setup.
  ///
  if (mDxePlatformMePolicy->MeAmtConfig.AmtbxSelectionScreen == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**

  @brief
  Return BIOS watchdog timer

  @param[in] None.

  @retval UINT16                  BIOS ASF Watchdog Timer

**/
UINT16
AmtWatchDogTimerBiosGet (
  VOID
  )
{
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return 0;
    }
  }
  return mDxePlatformMePolicy->MeAmtConfig.WatchDogTimerBios;
}

/**

  @brief
  Return OS watchdog timer

  @param[in] None.

  @retval UINT16                  OS ASF Watchdog Timer

**/
UINT16
AmtWatchDogTimerOsGet (
  VOID
  )
{
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return 0;
    }
  }
  return mDxePlatformMePolicy->MeAmtConfig.WatchDogTimerOs;
}

/**

  @brief
  Provide CIRA request information from OEM code.

  @param[in] None.

  @retval Check if any CIRA requirement during POST

**/
BOOLEAN
AmtCiraRequestTrigger (
  VOID
  )
{
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return mDxePlatformMePolicy->MeAmtConfig.CiraRequest == 1;
}

/**

  @brief
  Provide CIRA request Timeout from OEM code.

  @param[in] None.

  @retval CIRA require Timeout for MPS connection to be estabilished

**/
UINT8
AmtCiraRequestTimeout (
  VOID
  )
{
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return mDxePlatformMePolicy->MeAmtConfig.CiraTimeout;
}

/**

  @brief
  Provide Manageability Mode setting from MEBx BIOS Sync Data

  @param[in] None

  @retval UINT8                   Manageability Mode = MNT_AMT or MNT_ASF

**/
UINT8
ManageabilityModeSetting (
  VOID
  )
{
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return 0;
    }
  }
  return (UINT8) (mDxePlatformMePolicy->MeAmtConfig.ManageabilityMode);
}

/**

  @brief
  Check if AMT WatchDog is enabled in setup options.

  @param[in] None.

  @retval FALSE                   AMT WatchDog is disabled.
  @retval TRUE                    AMT WatchDog is enabled.

**/
BOOLEAN
AmtWatchDog (
  VOID
  )
{
  BOOLEAN    Supported;
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  Supported = FALSE;
  if (ManageabilityModeSetting () != 0) {
    ///
    ///
    /// First check if AMT WatchDog is enabled in Setup.
    ///
    if (AsfSupported ()) {
      if (mDxePlatformMePolicy->MeAmtConfig.WatchDog == 1) {
        Supported = TRUE;
      }
    }
  }

  return Supported;
}

/**

  @brief
  Check if AMT WatchDogOs is enabled in setup options.

  @param[in] None.

  @retval FALSE                   AMT WatchDogOs is disabled.
  @retval TRUE                    AMT WatchDogOs is enabled.

**/
BOOLEAN
AmtWatchDogOs (
  VOID
  )
{
  BOOLEAN    Supported;
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  Supported = FALSE;
  if (ManageabilityModeSetting () != 0) {
    ///
    ///
    /// First check if AMT WatchDogOs is enabled in Setup.
    ///
    if (AsfSupported ()) {
      if (mDxePlatformMePolicy->MeAmtConfig.WatchDogOs == 1) {
        Supported = TRUE;
      }
    }
  }

  return Supported;
}


/**

  @brief

  Provide UnConfigure ME without password request from OEM code.

  @param[in] None.

  @retval Check if unConfigure ME without password request

**/
BOOLEAN
AmtUnConfigureMe (
  VOID
  )
{
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return mDxePlatformMePolicy->MeAmtConfig.UnConfigureMe == 1;
}

/**

  @brief
  Provide 'Hiding the Unconfigure ME without password confirmation prompt' request from OEM code.

  @param[in] None.

  @retval Check if 'Hide unConfigure ME without password Confirmation prompt' request

**/
BOOLEAN
AmtHideUnConfigureMeConfPrompt (
  VOID
  )
{
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return mDxePlatformMePolicy->MeAmtConfig.HideUnConfigureMeConfirm == 1;
}

/**

  @brief
  Provide show MEBx debug message request from OEM code.

  @param[in] None.

  @retval Check show MEBx debug message request

 **/
BOOLEAN
AmtMebxDebugMsg (
  VOID
  )
{
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return mDxePlatformMePolicy->MeAmtConfig.MebxDebugMsg == 1;
}

/**

  @brief

  Provide on-board device list table and do not need to report them to AMT.  AMT only need to know removable PCI device
  information.

  @param[in] None.

  @retval on-board device list table pointer other than system device.

**/
UINT32
AmtPciDeviceFilterOutTable (
  VOID
  )
{
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return 0;
    }
  }
  return mDxePlatformMePolicy->MeAmtConfig.PciDeviceFilterOutTable;
}

/**

  @brief
  Check if USB provisioning enabled/disabled in platform policy.

  @param[in] None.

  @retval FALSE                   USB provisioning is disabled.
  @retval TRUE                    USB provisioning is enabled.

**/
BOOLEAN
USBProvisionSupport (
  VOID
  )
{
  BOOLEAN    Supported;
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  Supported = FALSE;

  ///
  /// First check if USB Provision is enabled in Setup.
  ///
  if (mDxePlatformMePolicy->MeAmtConfig.USBProvision == 1) {
    Supported = TRUE;
  }

  return Supported;
}

/**

  @brief
  This will return progress event Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    progress event is enabled.
  @retval False                   progress event is disabled.

**/
BOOLEAN
FwProgressSupport (
  VOID
  )
{
  BOOLEAN    Supported;
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  Supported = FALSE;

  ///
  /// First check if FW Progress is enabled in Setup.
  ///
  if (mDxePlatformMePolicy->MeAmtConfig.FWProgress == 1) {
    Supported = TRUE;
  }

  return Supported;
}

/**

  @brief
  Check if ForcMebxSyncUp is enabled in setup options.

  @param[in] None.

  @retval FALSE                   ForcMebxSyncUp is disabled.
  @retval TRUE                    ForcMebxSyncUp is enabled.

**/
BOOLEAN
AmtForcMebxSyncUp (
  VOID
  )
{
  BOOLEAN    Supported = FALSE;
  EFI_STATUS Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  if (mDxePlatformMePolicy->MeAmtConfig.ForcMebxSyncUp == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**
  Check if Core Bios Done Message is enabled in setup options.

  @retval FALSE                   Core Bios Done is disabled.
  @retval TRUE                    Core Bios Done is enabled.
**/
BOOLEAN
CoreBiosDoneEnabled (
  VOID
  )
{
  BOOLEAN                         Supported = FALSE;
  EFI_STATUS                      Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit ();
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }

  if (mDxePlatformMePolicy->MeAmtConfig.CoreBiosDoneEnabled == 1) {
    Supported = TRUE;
  }

  return Supported;
}
#endif // AMT_SUPPORT
