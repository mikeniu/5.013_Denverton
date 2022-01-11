/** @file
  Header file for the API to be used by PEI modules to enable/disable/query
  the Static Power Gating capabilities in SoC.

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
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
#ifndef _PCH_PMC_ST_PG_H_
#define _PCH_PMC_ST_PG_H_

#include <Uefi/UefiBaseType.h>
#include <PchAccess.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PchCycleDecodingLib.h>

/**
  Supported Static Power Gating Functions
**/
typedef enum {
  PMC_ST_PG_FUNC_MIN,
  PMC_ST_PG_GBE0 = PMC_ST_PG_FUNC_MIN,
  PMC_ST_PG_GBE1,
  PMC_ST_PG_SATA0,
  PMC_ST_PG_SATA1,
  PMC_ST_PG_PCIE0,
  PMC_ST_PG_PCIE1,
  PMC_ST_PG_NCPM,
  PMC_ST_PG_EMMC,
  PMC_ST_PG_XHCI,
  PMC_ST_PG_FUNC_MAX
} PMC_ST_PG_FUNCTION;

/**
  Static Power Gating Configuration. It is invoked to configure which
  IP blocks are going to be power gated.

  param[in]                    None
  param[out]                   None

  @retval EFI_SUCCESS          ST PG Configuration was successful
  @retval EFI_NOT_FOUND        One of the following occurred
                                 R_PCH_PWRM_ST_PG_FDIS_PMC_1's base address wasn't found
                                 HOB was not found
  @retval EFI_INVALID_PARAMETER No valid function value was given
  @retval EFI_WRITE_PROTECTED  R_PCH_PWRM_ST_PG_FDIS_PMC_1 has been locked previously
**/
EFI_STATUS
EFIAPI
PmcStPgConfig(
  VOID
  );

/**
  Init function for Static Power Gating API. It should be invoked prior
  to calling other ST PG API functions. It initializes all resources
  required by the API, caches the initial ST PG register, registers
  required callbacks etc.

  param[in]                    None
  param[out]                   None

  @retval EFI_SUCCESS          Successful API initialization
  @retval EFI_NOT_FOUND        R_PCH_PWRM_ST_PG_FDIS_PMC_1's base address wasn't found
  @retval EFI_OUT_OF_RESOURCES Required API resources are not available
**/
EFI_STATUS
EFIAPI
PmcStPgInit(
  VOID
  );

/**
  Static PG enable function. It should be invoked by the caller to
  enable the required IP block, i.e. prevent it from being static power
  gated.

  param[in]                    Function ST PG Function to enable
  param[out]                   None

  @retval EFI_SUCCESS          Function will be enabled
  @retval EFI_NOT_FOUND        HOB was not found
  @retval EFI_INVALID_PARAMETER No valid function value was given
**/
EFI_STATUS
EFIAPI
PmcStPgEnableFunction(
  IN PMC_ST_PG_FUNCTION Function
  );

/**
  Static PG disable API function. It should be invoked by the caller to
  disable the required IP block, i.e. to static power gate it.

  param[in]                    Function ST PG Function to disable
  param[out]                   None

  @retval EFI_SUCCESS          Function will be disabled
  @retval EFI_NOT_FOUND        HOB was not found
  @retval EFI_INVALID_PARAMETER No valid function value was given
**/
EFI_STATUS
EFIAPI
PmcStPgDisableFunction(
  IN PMC_ST_PG_FUNCTION Function
  );

/**
  Query Static PG function. It should be invoked by the
  caller to request the current ST PG state of an IP block.
  A successful call will return the status in the pStatus parameter.
  A pStatus return value of 1, means the function is being static power
  gated. Otherwise 0 will be returned in pStatus.

  param[in]                    Function ST PG Function to query
  param[out]                   pStatus 1: Function is disabled
                                       0: Function is enabled

  @retval EFI_SUCCESS          pStatus contains a valid status
  @retval EFI_NOT_FOUND        HOB was not found
  @retval EFI_INVALID_PARAMETER No valid function value was given
**/
EFI_STATUS
EFIAPI
PmcStPgQueryFunction(
  IN PMC_ST_PG_FUNCTION Function,
  OUT BOOLEAN           *pStatus
  );

/**
  Reset Query function. It should be invoked by the
  caller to check if a reset will be required by the ST PG API.
  A successful call will return the status in the pStatus parameter.
  A pStatus return value of 1, means that a reset will be required.
  Otherwise 0 will be returned in pStatus.

  param[in]                    Function ST PG Function to query
  param[out]                   pStatus 1: A Global Reset is required
                                       0: A Global Reset is not required

  @retval EFI_SUCCESS          pStatus contains a valid status
  @retval EFI_NOT_FOUND        pStatus does not contains a valid status due
                                 R_PCH_PWRM_ST_PG_FDIS_PMC_1's base address wasn't found
                                 HOB was not found
**/
EFI_STATUS
EFIAPI
PmcStPgResetRequired(
  OUT BOOLEAN *pStatus
  );

/**
  Lock function for Static Power Gating Register. It should be invoked
  prior to End of PEI to prevent any further changes.

  param[in]                    None
  param[out]                   None

  @retval EFI_SUCCESS          Lock Bit is set
  @retval EFI_NOT_FOUND        R_PCH_PWRM_ST_PG_FDIS_PMC_1's base address wasn't found
**/
EFI_STATUS
EFIAPI
PmcStPgLock(
  VOID
  );
#endif//_PCH_PMC_ST_PG_H_
