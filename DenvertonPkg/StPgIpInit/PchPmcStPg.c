/** @file
  API to be used by PEI modules to enable/disable/query the Static
  Power Gating (ST PG) capabilities in SoC

@copyright
 Copyright (c) 2015 - 2017 Intel Corporation. All rights reserved
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
#include "PchPmcStPg.h"
#include <Include/PchPmcStPgHob.h>
#include <Guid/PchPmcStPgHobGuid.h>
#include <Library/Hoblib.h>
#include <Library/BaseMemoryLib.h>

/**
  Function to get the module's HOB

  param[in out] PchPmcStPgHob  A pointer to PCH PMC ST PG HOB structure

  @retval EFI_SUCCESS          HOB was successfully found
  @retval EFI_NOT_FOUND        HOB was not found
**/
EFI_STATUS
EFIAPI
PmcStPgGetHob (
  IN OUT EFI_PCH_PMC_ST_PG_HOB **PchPmcStPgHob
  )
{
  EFI_HOB_GUID_TYPE     *GuidHob;

  GuidHob = GetFirstGuidHob (&gPchPmcStPgHobGuid);
  if (GuidHob == NULL) {
    ASSERT_EFI_ERROR (EFI_NOT_FOUND);
    return EFI_NOT_FOUND;
  }

  *PchPmcStPgHob = (EFI_PCH_PMC_ST_PG_HOB *)GET_GUID_HOB_DATA(GuidHob);

  return EFI_SUCCESS;
}

/**
  Function to create & store the module's HOB

  param[in] PchPmcStPgHob      A pointer to PCH PMC ST PG HOB structure
  param[out]                   None

  @retval EFI_SUCCESS          HOB was successfully installed
  @retval EFI_OUT_OF_RESOURCES HOB was not installed
**/
EFI_STATUS
EFIAPI
PmcStPgSetHob(
  IN EFI_PCH_PMC_ST_PG_HOB *PchPmcStPgHob
  )
{
  VOID                  *Hob;
  UINT32                Size;

  Size = sizeof (EFI_PCH_PMC_ST_PG_HOB);
  Hob = BuildGuidDataHob (
    &gPchPmcStPgHobGuid,
    PchPmcStPgHob,
    Size
    );

  if (Hob == NULL) {
    ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

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
  )
{
  UINT32                PchPwrmBase;
  UINT32                StPgFdisPmc1;
  PMC_ST_PG_FUNCTION    Function;
  UINT32                FunctionMask;
  EFI_STATUS            Status;
  BOOLEAN               StatusBit;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  Status = PmcStPgResetRequired (&StatusBit);
  if (Status == EFI_SUCCESS) {
    if (PchPwrmBaseGet (&PchPwrmBase) != EFI_SUCCESS) {
      return EFI_NOT_FOUND;
    }
    if (StatusBit) {
      StPgFdisPmc1 = (UINT32)MmioRead32 ((UINTN) (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1));
      DEBUG ((DEBUG_INFO, "\tST_PG_FDIS_PMC_1 = 0x%08x\n", StPgFdisPmc1));
      if (StPgFdisPmc1 & B_PCH_PWRM_ST_FDIS_LK) {
        return EFI_WRITE_PROTECTED;
      }
      for (Function = PMC_ST_PG_FUNC_MIN; Function < PMC_ST_PG_FUNC_MAX; Function++) {
        Status = PmcStPgQueryFunction (Function, &StatusBit);
        if (Status == EFI_SUCCESS) {
          FunctionMask = 1;
          FunctionMask <<= Function;
          if (StatusBit) {
            StPgFdisPmc1 |= FunctionMask;
          } else {
            StPgFdisPmc1 &= ~FunctionMask;
          }
        } else {
          return Status;
        }
      }
      MmioWrite32 ((UINTN) (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1), StPgFdisPmc1);
      DEBUG ((EFI_D_INFO, "\tA Global Reset is Required\n"));
    }
    StPgFdisPmc1 = (UINT32)MmioRead32 ((UINTN) (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1));
    DEBUG ((DEBUG_INFO, "\tST_PG_FDIS_PMC_1 = 0x%08x\n", StPgFdisPmc1));
    DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
    return Status;
  }
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return Status;
}

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
  )
{
  EFI_PCH_PMC_ST_PG_HOB PchPmcStPgHob;
  UINT32                PchPwrmBase;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  if (PchPwrmBaseGet (&PchPwrmBase) != EFI_SUCCESS) {
    return EFI_NOT_FOUND;
  }
  PchPmcStPgHob.StPgFdisPmc1.Data = (UINT32)MmioRead32 ((UINTN) (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1));
  DEBUG ((DEBUG_INFO, "\tST_PG_FDIS_PMC_1 = 0x%08x\n", PchPmcStPgHob.StPgFdisPmc1.Data));

  // Since no modification was made on ST_PG_FDIS_PMC_1, a Reset is not required
  PchPmcStPgHob.ResetRequired = FALSE;

  // Create HOB
  if (PmcStPgSetHob (&PchPmcStPgHob) != EFI_SUCCESS) {
    return EFI_OUT_OF_RESOURCES;
  }
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

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
  )
{
  EFI_PCH_PMC_ST_PG_HOB *PchPmcStPgHob;
  UINT32                EnableMask;

  PchPmcStPgHob = NULL;
  if (PMC_ST_PG_FUNC_MAX > Function && Function >= PMC_ST_PG_FUNC_MIN) {
    if (PmcStPgGetHob (&PchPmcStPgHob) != EFI_SUCCESS) {
      return EFI_NOT_FOUND;
    }
    EnableMask = 1;
    EnableMask <<= Function;
    PchPmcStPgHob->StPgFdisPmc1.Data &= ~EnableMask;
    return EFI_SUCCESS;
  }

  return EFI_INVALID_PARAMETER;
}

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
  )
{
  EFI_PCH_PMC_ST_PG_HOB *PchPmcStPgHob;
  UINT32                DisableMask;

  PchPmcStPgHob = NULL;
  if (PMC_ST_PG_FUNC_MAX > Function && Function >= PMC_ST_PG_FUNC_MIN) {
    if (PmcStPgGetHob (&PchPmcStPgHob) != EFI_SUCCESS) {
      return EFI_NOT_FOUND;
    }
    DisableMask = 1;
    DisableMask <<= Function;
    PchPmcStPgHob->StPgFdisPmc1.Data |= DisableMask;
    return EFI_SUCCESS;
  }

  return EFI_INVALID_PARAMETER;
}

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
  )
{
  EFI_PCH_PMC_ST_PG_HOB *PchPmcStPgHob;
  UINT32                FunctionMask;

  PchPmcStPgHob = NULL;
  if (PMC_ST_PG_FUNC_MAX > Function && Function >= PMC_ST_PG_FUNC_MIN) {
    if (PmcStPgGetHob (&PchPmcStPgHob) != EFI_SUCCESS) {
      return EFI_NOT_FOUND;
    }
    FunctionMask = 1;
    FunctionMask <<= Function;
    if ( PchPmcStPgHob->StPgFdisPmc1.Data & FunctionMask ) {
      *pStatus = TRUE;
    } else {
      *pStatus = FALSE;
    }
    return EFI_SUCCESS;
  }

  return EFI_INVALID_PARAMETER;
}

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
  )
{
  EFI_PCH_PMC_ST_PG_HOB *PchPmcStPgHob;
  UINT32                PchPwrmBase;
  UINT32                StPgFdisPmc1;

  PchPmcStPgHob = NULL;
  if (PchPwrmBaseGet (&PchPwrmBase) == EFI_SUCCESS) {
    StPgFdisPmc1 = (UINT32)MmioRead32 ((UINTN) (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1));
    if (PmcStPgGetHob (&PchPmcStPgHob) != EFI_SUCCESS) {
        return EFI_NOT_FOUND;
      }
    if (PchPmcStPgHob->StPgFdisPmc1.Data != StPgFdisPmc1){
      PchPmcStPgHob->ResetRequired = TRUE;
    }
    *pStatus = (BOOLEAN)PchPmcStPgHob->ResetRequired;
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

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
  )
{
  EFI_PCH_PMC_ST_PG_HOB *PchPmcStPgHob;
  UINT32                PchPwrmBase;
  UINT32                StPgFdisPmc1;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  PchPmcStPgHob = NULL;
  if (PchPwrmBaseGet (&PchPwrmBase) == EFI_SUCCESS) {
    MmioOr32 (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1, B_PCH_PWRM_ST_FDIS_LK);
    StPgFdisPmc1 = (UINT32)MmioRead32 ((UINTN) (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1));
    DEBUG ((DEBUG_INFO, "\tST_PG_FDIS_PMC_1 = 0x%08x\n", StPgFdisPmc1));
    if (PmcStPgGetHob (&PchPmcStPgHob) != EFI_SUCCESS) {
      return EFI_NOT_FOUND;
    }
    PchPmcStPgHob->StPgFdisPmc1.Ip.StFdisLk = TRUE;
    DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}
