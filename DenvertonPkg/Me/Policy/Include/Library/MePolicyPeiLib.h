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

#include "Ppi/MePlatformPolicyPei.h"
/**
  @brief
  Get PEI_ME_PLATFORM_POLICY_PPI

  @param[in] MePlatformPolicy The ME Platform Policy protocol instance

  @retval PEI_ME_PLATFORM_POLICY_PPI*  Pointer to ME Policy PPI
          NULL                         is returned on error

-**/

PEI_ME_PLATFORM_POLICY_PPI *
PeiGetMePolicyPpi (
  VOID
  )
;

#ifdef AMT_SUPPORT
/**

  Check if AMT WatchDog is enabled in setup options.

  @retval FALSE                   AMT WatchDog is disabled.
  @retval TRUE                    AMT WatchDog is enabled.

**/
BOOLEAN
PeiAmtWatchDog (
  VOID
  )
;

/**

  Get WatchDog BIOS Timmer.

  @retval UINT16                  WatchDog BIOS Timer

**/
UINT16
PeiAmtWatchTimerBiosGet (
  VOID
  )
;

/**

  Check if AMT is enabled in setup options.

  @retval FALSE                   ActiveManagement is disabled.
  @retval TRUE                    ActiveManagement is enabled.

**/
BOOLEAN
PeiAmtSupported (
  VOID
  )
;

/**

  Check if ASF is enabled in setup options.

  @retval FALSE                   ASF is disabled.
  @retval TRUE                    ASF is enabled.

**/
BOOLEAN
PeiAsfSupported (
  VOID
  )
;

/**

  Provide Manageability Mode setting from MEBx BIOS Sync Data

  @retval UINT8                   Manageability Mode = MNT_AMT or MNT_ASF

**/
UINT8
ManageabilityModeSetting (
  VOID
  )
;

/**

  This will return progress event Option.
  True if the option is enabled.

  @retval True                    progress event is enabled.
  @retval False                   progress event is disabled.

**/
BOOLEAN
PeiFwProgressSupport (
  VOID
  )
;

#endif // AMT_SUPPORT

#endif // _ME_POLICY_LIB_H_
