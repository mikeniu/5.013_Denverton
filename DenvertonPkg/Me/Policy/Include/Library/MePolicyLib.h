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
  MePolicyLib.h

@brief:
  Header file for Me Policy functionality

**/
#ifndef _ME_POLICY_LIB_H_
#define _ME_POLICY_LIB_H_

#include "Protocol/MePlatformPolicyProtocol.h"

/**

  Check if Me is enabled.

  @param[in] None.

**/
EFI_STATUS
MePolicyLibInit (
  VOID
  )
;

/**

  @brief

  Check if End of Post Message is enabled in setup options.

  @param[in] None.

  @retval FALSE                   EndOfPost is disabled.
  @retval TRUE                    EndOfPost is enabled.

**/
BOOLEAN
MeEndOfPostEnabled (
  VOID
  )
;

/**

  @brief
  Show Me Error message.

  @param[in] MsgId                Me error message ID.


**/
VOID
MeReportError (
  ME_ERROR_MSG_ID MsgId
  )
;

/**

  @brief
  Check if MeFwDowngrade is enabled in setup options.

  @param[in] None.

  @retval FALSE                   MeFwDowngrade is disabled.
  @retval TRUE                    MeFwDowngrade is enabled.

**/
BOOLEAN
MeFwDowngradeSupported (
  VOID
  )
;

/**
  Check if Core Bios Done Message is enabled in setup options.

  @retval FALSE                   Core Bios Done is disabled.
  @retval TRUE                    Core Bios Done is enabled.
**/
BOOLEAN
CoreBiosDoneEnabled (
    VOID
  );

/**

  Dump DXE ME Platform Policy

  @param[in] None.

**/
VOID
DxeMePolicyDebugDump (
  VOID
  )
;

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
;

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
;

#endif // _ME_POLICY_LIB_H_
