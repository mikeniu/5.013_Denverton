/** @file
  This file is a library for Static PG miscellaneous PCH IP.

@copyright
 Copyright (c) 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:

**/

#include "PchStPgMisc.h"
#include <Library/DebugLib.h>
#include <PchPmcStPg.h>

/**
  Initialize Misc IP in PEI
  EMMC

  Arguments:
  PchPolicy

  Returns:
  @retval EFI_SUCCESS - The function completed successfully.
  @retval Others 	- Error
**/
EFI_STATUS
EFIAPI
PeiMiscIpInit (
  PCH_POLICY_PPI     *PchPolicy
  )
{
  UINT32      PwrmBase;
  UINT32      FuseDis2State;
  EFI_STATUS  Status = EFI_SUCCESS;

  DEBUG ((DEBUG_INFO, "PeiMiscIpInit Start\n"));

  PchPwrmBaseGet (&PwrmBase);
  FuseDis2State = MmioRead32 ((UINTN) (PwrmBase + R_PCH_PWRM_STPG_FUSE_SS_DIS_RD_2));

  if ((FuseDis2State & B_PCH_PWRM_EMMC_FUSE_SS_DIS) == 0) {
    if (PchPolicy->eMMCEnabled == 0) {
      DEBUG ((DEBUG_INFO, "Disable EMMC device!!\n"));
      Status = PmcStPgDisableFunction(PMC_ST_PG_EMMC);
      ASSERT_EFI_ERROR (Status);
    } else {
      DEBUG ((DEBUG_INFO, "Enabled EMMC device!!\n"));
      Status = PmcStPgEnableFunction(PMC_ST_PG_EMMC);
      ASSERT_EFI_ERROR (Status);
    }
  }
#ifdef FSP_FLAG
  if ((FuseDis2State & B_PCH_PWRM_FUSE_DIS_RD_2_GBE0_FUSE_DIS) == 0) {
    if (PchPolicy->GbE0Enabled == 0) {
      DEBUG ((DEBUG_INFO, "Disable GbE0 device!!\n"));
      Status = PmcStPgDisableFunction(PMC_ST_PG_GBE0);
      ASSERT_EFI_ERROR (Status);
    } else {
      DEBUG ((DEBUG_INFO, "Enabled GbE0 device!!\n"));
      Status = PmcStPgEnableFunction(PMC_ST_PG_GBE0);
      ASSERT_EFI_ERROR (Status);
    }
  }

  if ((FuseDis2State & B_PCH_PWRM_FUSE_DIS_RD_2_GBE1_FUSE_DIS) == 0) {
    if (PchPolicy->GbE1Enabled == 0) {
      DEBUG ((DEBUG_INFO, "Disable GbE1 device!!\n"));
      Status = PmcStPgDisableFunction(PMC_ST_PG_GBE1);
      ASSERT_EFI_ERROR (Status);
    } else {
      DEBUG ((DEBUG_INFO, "Enabled GbE1 device!!\n"));
      Status = PmcStPgEnableFunction(PMC_ST_PG_GBE1);
      ASSERT_EFI_ERROR (Status);
    }
  }
#endif //FSP_FLAG
  DEBUG ((DEBUG_INFO, "PeiMiscIpInit End\n"));

  return Status;
}
