/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c) 2011 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MePlatformPolicyUpdateDxeLib.h

  Header file for DXE MePlatformPolicyUpdate Library.

**/
#ifndef _ME_PLATFORM_POLICY_UPDATE_DXE_LIB_H_
#define _ME_PLATFORM_POLICY_UPDATE_DXE_LIB_H_

#include <Protocol/MePlatformPolicyProtocol.h>
#include "Library/MeTypeLib.h"

#ifdef AMT_SUPPORT

#include "Guid/MeBiosExtensionSetup.h"

/**

  Check if AMT is enabled in setup options.

  @param[in] None.

  @retval EFI_SUCCESS             AMT platform policy is initialized.
  @retval All other error conditions encountered when no AMT platform policy available.

**/
EFI_STATUS
AmtPolicyLibInit (
  VOID
  )
;

/**

  Check if Asf is enabled in setup options.

  @param[in] None.

  @retval FALSE                   Asf is disabled.
  @retval TRUE                    Asf is enabled.

**/
BOOLEAN
AsfSupported (
  VOID
  )
;

/**

  Check if Amt is enabled in setup options.

  @param[in] None.

  @retval FALSE                   Amt is disabled.
  @retval TRUE                    Amt is enabled.

**/
BOOLEAN
AmtSupported (
  VOID
  )
;

/**

  Check if AMT BIOS Extension hotkey was pressed during BIOS boot.

  @param[in] None.

  @retval FALSE                   MEBx hotkey was not pressed.
  @retval TRUE                    MEBx hotkey was pressed.

**/
BOOLEAN
AmtHotkeyPressed (
  VOID
  )
;

/**

  Check if AMT BIOS Extension Selection Screen is enabled in setup options.

  @param[in] None.

  @retval FALSE                   AMT Selection Screen is disabled.
  @retval TRUE                    AMT Selection Screen is enabled.

**/
BOOLEAN
AmtSelectionScreen (
  VOID
  )
;

/**

  Check if AMT WatchDog is enabled in setup options.

  @param[in] None.

  @retval FALSE                   AMT WatchDog is disabled.
  @retval TRUE                    AMT WatchDog is enabled.

**/
BOOLEAN
AmtWatchDog (
  VOID
  )
;

/**

  Check if AMT WatchDogOs is enabled in setup options.

  @param[in] None.

  @retval FALSE                   AMT WatchDogOs is disabled.
  @retval TRUE                    AMT WatchDogOs is enabled.

**/
BOOLEAN
AmtWatchDogOs (
  VOID
  )
;

/**

  Return BIOS watchdog timer

  @param[in] None.

  @retval UINT16                  BIOS ASF Watchdog Timer

**/
UINT16
AmtWatchDogTimerBiosGet (
  VOID
  )
;

/**

  Return OS watchdog timer

  @param[in] None.

  @retval UINT16                  OS ASF Watchdog Timer

**/
UINT16
AmtWatchDogTimerOsGet (
  VOID
  )
;

/**

  Provide CIRA request information from OEM code.

  @param[in] None.

  @retval Check if any CIRA requirement during POST

**/
BOOLEAN
AmtCiraRequestTrigger (
  VOID
  )
;

/**

  Provide CIRA request Timeout from OEM code.

  @param[in] None.

  @retval CIRA require Timeout for MPS connection to be estabilished

**/
UINT8
AmtCiraRequestTimeout (
  VOID
  )
;

/**

  Provide Manageability Mode setting from MEBx BIOS Sync Data

  @param[in] None

  @retval UINT8                   Manageability Mode = MNT_AMT or MNT_ASF

**/
UINT8
ManageabilityModeSetting (
  VOID
  )
;

/**

  Provide UnConfigure ME without password request from OEM code.

  @param[in] None.

  @retval Check if unConfigure ME without password request

**/
BOOLEAN
AmtUnConfigureMe (
  VOID
  )
;

/**

  Provide 'Hiding the Unconfigure ME without password confirmation prompt' request from OEM code.

  @param[in] None.

  @retval Check if 'Hide unConfigure ME without password Confirmation prompt' request

**/
BOOLEAN
AmtHideUnConfigureMeConfPrompt (
  VOID
  )
;

/**

  Provide show MEBx debug message request from OEM code.

  @param[in] None.

  @retval Check show MEBx debug message request

 **/
BOOLEAN
AmtMebxDebugMsg (
  VOID
  )
;

/**

  Provide on-board device list table and do not need to report them to AMT.  AMT only need to know removable PCI device
  information.

  @param[in] None.

  @retval on-board device list table pointer other than system device.

**/
UINT32
AmtPciDeviceFilterOutTable (
  VOID
  )
;

/**

  Check if USB provisioning enabled/disabled in platform policy.

  @param[in] None.

  @retval FALSE                   USB provisioning is disabled.
  @retval TRUE                    USB provisioning is enabled.

**/
BOOLEAN
USBProvisionSupport (
  VOID
  )
;

/**

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
;

/**

  Check if ForcMebxSyncUp is enabled in setup options.

  @param[in] None.

  @retval FALSE                   ForcMebxSyncUp is disabled.
  @retval TRUE                    ForcMebxSyncUp is enabled.

**/
BOOLEAN
AmtForcMebxSyncUp (
  VOID
  )
;

/**

  Dump DXE Amt Platform Policy

  @param[in] None.

**/
VOID
DxeAmtPolicyDebugDump (
  VOID
  )
;

/**

  Dump ME_BIOS_EXTENSION_SETUP variable

  @param[in] MeBiosExtensionSetup   Pointer to ME_BIOS_EXTENSION_SETUP variable

**/
VOID
DxeMebxSetupVariableDebugDump (
  IN ME_BIOS_EXTENSION_SETUP  *MeBiosExtensionSetup OPTIONAL
  )
;
#endif // AMT_SUPPORT

/**

  Install the ME Platform Policy Library

  @param mDxePlatformMePolicy  ME Platform Policy in DXE phase

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.

**/
EFI_STATUS
InstallMEPlatformPolicyLibrary (
  IN OUT  DXE_ME_POLICY_PROTOCOL  *mDxePlatformMePolicy
  )
;

#define AMT_BITMASK   0x00000004
#define CLEAR_AMT_BIT 0x00000000

#define MAX_FW_UPDATE_BIOS_SELECTIONS 2

#define EFI_NVRAM_UPDATE_GUID \
{ \
  0xd84beff0, 0x159a, 0x4b60, 0x9a, 0xb9, 0xac, 0x5c, 0x47, 0x4b, 0xd3, 0xb1 \
}

/**

  Function stores Me Platform Policy to Setup configuration

  @param MePlatformPolicyInstance    ME Policy instance to save
  @param saveMask                    What to save

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_INVALID_PARAMETER Wrong input parameters

**/
EFI_STATUS
saveMePolicyToSetupConfiguration(
  DXE_ME_POLICY_PROTOCOL  *MePlatformPolicyInstance
)
;

/**

  Function that is called only if we enter BIOS Setup

  @param Event         Pointer to the event that triggered this Callback Function
  @param Context       VOID Pointer required for Callback function

  @retval EFI_SUCCESS           Initialization complete.


**/
EFI_STATUS
UpdateSetupDataWithFwInfo (
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
;

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
;

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
;

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
;

/**

  Update Me platform policy while MePlatformProtocol is installed.

  @param MePlatformPolicyInstance - ME Platform Policy
  @param mSetupData               - pointer to system configuration
  @param resetRequest             - Returns information if function requests reset

  @retval None

**/
EFI_STATUS
UpdateMePolicyFromSetup (
  DXE_ME_POLICY_PROTOCOL *MePlatformPolicyInstance,
  VOID                   *mSetupData,
  BOOLEAN                *resetRequest
  )
;

EFI_STATUS
EFIAPI
MePlatformPolicyUpdateMeSettings (
  DXE_ME_POLICY_PROTOCOL *MePlatformPolicyInstance
)
;

#endif
