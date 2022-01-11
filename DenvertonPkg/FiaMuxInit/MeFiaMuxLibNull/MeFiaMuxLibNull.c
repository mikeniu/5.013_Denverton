/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MeFiaMuxLib.c

@brief:
  A Null Me FIA MUX configuration interface implementation.

**/

#include <Uefi.h>
#include <Library/PciLib.h>
#include <Library/MmPciLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MeFiaMuxLib.h>
#include <Library/PeiFiaMuxPolicyInitLib.h>

/**
 *  ************************** NOTE ***************************
 *  This library will be included in build with SVME for SV
 *  debug purpose and will NOT be used building with SPS FW.
 */

/**
 * Sends FIA MUX configuration to ME.
 *
 * param[in]  pConfig       Pointer to FIA MUX configuration to set in ME,
 *                          when NULL, Invalidate field is set to 1 (causing
 *                          invalidate of configuration stored in flash -
 *                          configuration from Soft Straps will be used)
 *
 * @retval EFI_NOT_READY Returned if this API is called before ME signals init
 *                       complete, or after END_OF_POST was sent.
 * @retval EFI_DEVICE_ERROR Returned if HECI transport is not available.
 * @retval EFI_PROTOCOL_ERROR Unexpected protocol error returned by ME.
 * @retval EFI_OUT_OF_RESOURCES Requested configuration refused by ME, because
 *                              it does not match the SoC SKU.
 * @retval EFI_ABORTED Requested configuration refused by ME, because it is
 *                     invalid
 */
EFI_STATUS
EFIAPI
MeFiaMuxConfigSet(IN ME_FIA_CONFIG *pConfig)
{
  EFI_STATUS                      Status;

  Status = EFI_SUCCESS;

  return Status;
} // MeFiaMuxConfigSet()

/**
 * @breaf Read FIA MUX configuration from ME.
 *
 * @param[out] pConfig       Pointer to a buffer for ME FIA MUX configuration
 * @param[out] pLanesAllowed Pointer to a buffer for the number of lanes allowed
 *
 * @retval EFI_NOT_READY Returned if this API is called before ME signals init
 *                       complete, or after END_OF_POST was sent.
 * @retval EFI_INVALID_PARAMETER Returned if both pointers passed are NULL
 * @retval EFI_DEVICE_ERROR Returned if HECI transport is not available.
 * @retval EFI_PROTOCOL_ERROR Unexpected protocol error returned by ME.
 */
EFI_STATUS
EFIAPI
MeFiaMuxConfigGet(OUT ME_FIA_CONFIG *pConfig, OUT UINT32 *pLanesAllowed)
{
  EFI_STATUS                      Status;
  FIA_MUX_GLOBAL_POLICY_PPI       *FiaMuxGlobalPolicy;

  if ( (pConfig == NULL) || (pLanesAllowed == NULL) ) return EFI_INVALID_PARAMETER;

  //
  // Locate installed FIA MUX Policy PPI
  //
  Status = PeiServicesLocatePpi(
    &gFiaMuxPolicyPpiGuid,
    0,
    NULL,
    (VOID **)&FiaMuxGlobalPolicy
    );

  if (EFI_ERROR(Status)) {
    return EFI_SUCCESS;
  }

  CopyMem(pConfig, &FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfigRequest, sizeof(ME_FIA_CONFIG));
  *pLanesAllowed = ME_FIA_MUX_LANE_NUM_MAX;

  return EFI_SUCCESS;
} // MeFiaMuxConfigGet()
