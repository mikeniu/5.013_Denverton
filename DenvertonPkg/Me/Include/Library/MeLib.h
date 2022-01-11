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
  MeLib.h

@brief:
  Header file for Me functionality

**/
#ifndef _ME_LIB_H_
#define _ME_LIB_H_

#include "Library/HeciMsgLib.h"
#include "MeChipset.h"
#include "HeciRegs.h"

#define FIVE_MS_TIMEOUT   5000

extern EFI_GUID gExitPmAuthProtocolGuid;

/**

  @brief

  Check if Me is enabled

  @param[in] None.


**/
EFI_STATUS
MeLibInit (
  VOID
  )
;

/**

  @brief
  Host client gets Firmware update info from ME client

  @param[in] MECapability         Structure of FirmwareUpdateInfo

  @exception EFI_UNSUPPORTED      No MBP Data Protocol available

**/
EFI_STATUS
HeciGetMeFwInfo (
  IN OUT ME_CAP *MECapability
  )
;


/**

  @brief
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to get Ibex Peak platform type.
  One of usages is to utilize this command to determine if the platform runs in
  4M or 8M size firmware.

  @param[in] RuleData             PlatformBrand,
                                  IntelMeFwImageType,
                                  SuperSku,
                                  PlatformTargetMarketType,
                                  PlatformTargetUsageType

  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
HeciGetPlatformType (
  OUT PLATFORM_TYPE_RULE_DATA   *RuleData
  )
;

/**

  @brief
  Dummy return for Me signal event use

  @param[in] Event                The event that triggered this notification function
  @param[in] ParentImageHandle    Pointer to the notification functions context

  @retval EFI_SUCCESS             Always return EFI_SUCCESS

**/
EFI_STATUS
MeEmptyEventFunction (
  IN  EFI_EVENT                   Event,
  IN  void                        *ParentImageHandle
  )
;


/**
  Send Get Platform Type Request to ME

  @param[in] RuleData             PlatformBrand,
                                  IntelMeFwImageType,
                                  SuperSku,
                                  PlatformTargetMarketType,
                                  PlatformTargetUsageType

  @exception EFI_UNSUPPORTED        Current ME mode doesn't support this function
  @retval EFI_SUCCESS               Command succeeded
  @retval EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT               HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL      Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
MbpGetPlatformTypeThroughHeci (
  OUT PLATFORM_TYPE_RULE_DATA     *RuleData
  )
;

/**
  Send Get Firmware SKU Request to ME

  @param[out] FwCapsSku     Return FwCapsSku mask for Get Firmware Capability SKU

  @exception EFI_UNSUPPORTED        Current ME mode doesn't support this function
  @retval EFI_SUCCESS               Command succeeded
  @retval EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT               HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL      Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
MbpGetFwCapsSkuThroughHeci (
  OUT MEFWCAPS_SKU             *FwCapsSku
  )
;
#endif
