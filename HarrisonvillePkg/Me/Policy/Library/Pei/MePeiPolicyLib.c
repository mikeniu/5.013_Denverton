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

  AmtPeiPolicyLib.c

@brief:

  Implementation file for AMT Policy functionality for PEIM

**/
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#ifdef AMT_SUPPORT
#include "Ppi/AmtStatusCode.h"
#endif // AMT_SUPPORT
#include "Ppi/MePlatformPolicyPei.h"
#include "Library/MePolicyPeiLib.h"

/**
  @brief
  Get PEI_ME_PLATFORM_POLICY_PPI

  @param[in] MePlatformPolicy The ME Platform Policy protocol instance

  @retval PEI_ME_PLATFORM_POLICY_PPI*  Pointer to ME Policy PPI
          NULL                         is returned on error

-**/

PEI_ME_PLATFORM_POLICY_PPI*
PeiGetMePolicyPpi (VOID)
{
  EFI_STATUS  Status;
  PEI_ME_PLATFORM_POLICY_PPI  *MePlatformPolicy;

  ///
  /// Locate system configuration variable
  ///
  Status = PeiServicesLocatePpi (
            &gPeiMePlatformPolicyPpiGuid,/// GUID
            0,                           /// INSTANCE
            NULL,                        /// EFI_PEI_PPI_DESCRIPTOR
            (VOID **)&MePlatformPolicy   /// PPI
            );
  if (EFI_ERROR(Status)) {
    MePlatformPolicy = NULL;
    ASSERT_EFI_ERROR (Status);
  }

  return MePlatformPolicy;
}

#ifdef AMT_SUPPORT

/**

  @brief
  Check if AMT WatchDog is enabled in setup options.

  @retval FALSE                   AMT WatchDog is disabled.
  @retval TRUE                    AMT WatchDog is enabled.

**/
BOOLEAN
PeiAmtWatchDog (
  VOID
  )
{
  BOOLEAN                     Supported;
  PEI_ME_PLATFORM_POLICY_PPI  *MePlatformPolicy;

  Supported = FALSE;
  if (ManageabilityModeSetting () != 0) {
    MePlatformPolicy = PeiGetMePolicyPpi();
    ASSERT (MePlatformPolicy != NULL);
    ///
    /// First check if AMT WatchDog is enabled in Setup.
    ///
    if (MePlatformPolicy->WatchDog == 1) {
      Supported = TRUE;
    }
  }

  return Supported;
}

/**

  @brief
  Get WatchDog BIOS Timmer.

  @retval UINT16                  WatchDog BIOS Timer

**/
UINT16
PeiAmtWatchTimerBiosGet (
  VOID
  )
{
  PEI_ME_PLATFORM_POLICY_PPI     *MePlatformPolicy;

  MePlatformPolicy = PeiGetMePolicyPpi();
  ASSERT (MePlatformPolicy != NULL);

  return MePlatformPolicy->WatchDogTimerBios;
}

/**

  @brief
  Check if AMT is enabled in setup options.

  @retval FALSE                   ActiveManagement is disabled.
  @retval TRUE                    ActiveManagement is enabled.

**/
BOOLEAN
PeiAmtSupported (
  VOID
  )
{
  BOOLEAN                     Supported;
  PEI_ME_PLATFORM_POLICY_PPI  *MePlatformPolicy;

  MePlatformPolicy = PeiGetMePolicyPpi();
  ASSERT (MePlatformPolicy != NULL);

  ///
  /// First check if AMT support is enabled in Setup.
  ///
  if (MePlatformPolicy->AmtEnabled == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**

  @brief
  Check if ASF is enabled in setup options.

  @retval FALSE                   ASF is disabled.
  @retval TRUE                    ASF is enabled.

**/
BOOLEAN
PeiAsfSupported (
  VOID
  )
{
  BOOLEAN                     Supported;
  PEI_ME_PLATFORM_POLICY_PPI  *MePlatformPolicy;

  MePlatformPolicy = PeiGetMePolicyPpi();
  ASSERT (MePlatformPolicy != NULL);

  if (MePlatformPolicy->Revision < PEI_ME_PLATFORM_POLICY_PPI_REVISION_2) {
    return FALSE;
  }
  ///
  /// First check if AMT support is enabled in Setup.
  ///
  if (MePlatformPolicy->AsfEnabled == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**

  @brief
  Provide Manageability Mode setting from MEBx BIOS Sync Data

  @retval UINT8                   Manageability Mode = MNT_AMT or MNT_ASF

**/
UINT8
ManageabilityModeSetting (
  VOID
  )
{
  PEI_ME_PLATFORM_POLICY_PPI     *MePlatformPolicy;

  MePlatformPolicy = PeiGetMePolicyPpi();
  ASSERT (MePlatformPolicy != NULL);

  if (MePlatformPolicy->Revision < PEI_ME_PLATFORM_POLICY_PPI_REVISION_2) {
    return 0;
  }

  return (UINT8) (MePlatformPolicy->ManageabilityMode);
}

/**

  @brief
  This will return progress event Option.
  True if the option is enabled.

  @retval True                    progress event is enabled.
  @retval False                   progress event is disabled.

**/
BOOLEAN
PeiFwProgressSupport (
  VOID
  )
{
  BOOLEAN                     Supported = FALSE;
  PEI_ME_PLATFORM_POLICY_PPI  *MePlatformPolicy;

  MePlatformPolicy = PeiGetMePolicyPpi();
  if (MePlatformPolicy != NULL) {
    ///
    /// Check if progress event is enabled in Setup.
    ///
    if (MePlatformPolicy->FWProgress == 1) {
      Supported = TRUE;
    }
  }

  return Supported;
}
#endif // AMT_SUPPORT
