/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MeTypeSetLib.c

@brief:
  Me Type Set Lib implementation.
  It is for set ME Type in system configuration in DXE

**/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <PiDxe.h>
#include <Library/IoLib.h>
#include <Library/PciExpressLib.h>
#include "Library/PciLib.h"
#include <Library/DebugLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include "MeAccess.h"
#include "PchAccess.h"
#include "HeciRegs.h"
#include "Library/MeTypeSetLib.h"
#include "MeTypeLibInternal.h"
#include "Sps.h"
#ifdef AMT_SUPPORT
#include "Protocol/AmtWrapperProtocol.h"
#include <Library/UefiBootServicesTableLib.h>
#endif // AMT_SUPPORT

EFI_STATUS
MeTypeSet (DXE_ME_POLICY_PROTOCOL  *MePlatformPolicyInstance)
{
  ON_BOARD_ME_TYPE        MeType = GetOnBoardMeType();

  if (MePlatformPolicyInstance == NULL) {
    DEBUG ((EFI_D_ERROR, "[HECI] Invalid parametes (MeTypeSet)\n"));
    return EFI_INVALID_PARAMETER;
  }

  MeTypeShowDebug(L"MeTypeSet Called");
  if ((MeType != ME_TYPE_SPS) && (MeType != ME_TYPE_AMT)) {
    DEBUG ((EFI_D_ERROR, "[HECI] Unknown MeType !!!\n"));
    return EFI_UNSUPPORTED;
  }

  MePlatformPolicyInstance->MeType = MeType;

  return EFI_SUCCESS;
}

#if AMT_SUPPORT

EFI_STATUS
MeRemoteSessionActiveSet (DXE_ME_POLICY_PROTOCOL  *MePlatformPolicyInstance)
{
  EFI_STATUS              Status = EFI_INVALID_PARAMETER;
  UINT8                   RemoteSessionActive;
  AMT_WRAPPER_PROTOCOL    *AmtWrapper = NULL;

  DEBUG ((EFI_D_INFO, "[HECI] MeRemoteSessionActiveSet Called\n"));

  if (MePlatformPolicyInstance == NULL) {
    DEBUG ((EFI_D_ERROR, "[HECI] Invalid parametes (MeRemoteSessionActiveSet)\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (GetOnBoardMeType() == ME_TYPE_AMT) {
    Status = gBS->LocateProtocol(&gEfiAmtWrapperProtocolGuid, NULL, &AmtWrapper);
    if (!EFI_ERROR(Status)) {
      DEBUG ((EFI_D_INFO, "[HECI] Get ActiveManagementEnableStorageRedir\n"));
      RemoteSessionActive = AmtWrapper->ActiveManagementEnableStorageRedir() ||
                            AmtWrapper->ActiveManagementEnableSol() ||
                            AmtWrapper->ActiveManagementEnableKvm();
    } else {
      RemoteSessionActive = 0;
    }
  } else {
    // No AMT, so no remote sessions
    RemoteSessionActive = 0;
  }
  DEBUG ((EFI_D_INFO, "[HECI] RemoteSessionActive present in setup data = 0x%x\n",
          MePlatformPolicyInstance->MeAmtConfig.RemoteSessionActive));
  if (MePlatformPolicyInstance->MeAmtConfig.RemoteSessionActive != (UINT8)RemoteSessionActive) {
    DEBUG ((EFI_D_INFO, "[HECI] RemoteSessionActive present in setup is different to detected. Updating ...\n"));
    MePlatformPolicyInstance->MeAmtConfig.RemoteSessionActive = (UINT8)RemoteSessionActive;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
MeMeStateControlSet (DXE_ME_POLICY_PROTOCOL  *MePlatformPolicyInstance)
{
  ON_BOARD_ME_TYPE        MeType = GetOnBoardMeType();
  UINT8                   MeStateControl = 0;


  DEBUG ((EFI_D_INFO, "[HECI] MeMeStateControlSet Called\n"));
  if (MePlatformPolicyInstance == NULL) {
    DEBUG ((EFI_D_ERROR, "[HECI] Invalid parametes (MeMeStateControlSet)\n"));
    return EFI_INVALID_PARAMETER;
  }

  if ((MeType == ME_TYPE_AMT) || (MeType == ME_TYPE_SPS)) {
    MeStateControl = 1;
  }
  if (MePlatformPolicyInstance->MeAmtConfig.MeStateControl != (UINT8)MeStateControl) {
    DEBUG ((EFI_D_INFO, "[HECI] MeStateControl present in setup is different to detected. Updating ...\n"));
    MePlatformPolicyInstance->MeAmtConfig.MeStateControl = (UINT8)MeStateControl;
  }

  return EFI_SUCCESS;
}
#endif // AMT_SUPPORT
