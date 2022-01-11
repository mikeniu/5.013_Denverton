/** @file
  ME Platform Debug Dump Policy definitions.

@copyright
  Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#ifndef _ME_POLICY_DEBUG_DUMP_DXE_H_
#define _ME_POLICY_DEBUG_DUMP_DXE_H_

//
// ME Policy Dump type
//
typedef enum {
  ME_POLICY_DUMP_ALL             = 0,
  ME_POLICY_DUMP_COMMON          = 1,
  ME_POLICY_DUMP_AMT             = 2,
  ME_POLICY_DUMP_SPS             = 4,
} ME_POLICY_DUMP_TYPE;

/**

  Dump DXE ME Platform Policy function called from Policy

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
;

/**

  Dump DXE ME Platform Policy

  @param[in] None

**/
VOID
DxeMePolicyDebugDump (
  VOID
  )
;
#endif // _ME_POLICY_DEBUG_DUMP_DXE_H_
