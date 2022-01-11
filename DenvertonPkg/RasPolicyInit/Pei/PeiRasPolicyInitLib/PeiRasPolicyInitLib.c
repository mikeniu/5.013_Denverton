/** @file
  This file is a library for RAS Policy Initialization.

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
#include <PeiRasPolicyLib.h>

/**
  Build RAS_GLOBAL_POLICY_PPI HOB

  @param RasGlobalPolicyPpi 		RAS global policy structure
**/

VOID
PeiRasPolicySetHob (
  IN RAS_GLOBAL_POLICY_PPI *RasGlobalPolicyPpi
  )
{
  EFI_PEI_HOB_POINTERS             Hob;
  UINTN                            BufferSize;

  BufferSize = sizeof (RAS_GLOBAL_POLICY_PPI);
  Hob.Raw = BuildGuidHob (
              &gRasPolicyHobGuid,
              BufferSize
              );
  ASSERT (Hob.Raw);

  CopyMem ((VOID *)Hob.Raw, RasGlobalPolicyPpi, sizeof(RAS_GLOBAL_POLICY_PPI));
}

EFI_STATUS
PeiRasCreatePolicyDefaults (
  OUT  RAS_GLOBAL_POLICY_PPI **RasGlobalPolicyPpi
  )
/**
  PeiRasCreatePolicyDefaults creates the default setting of RAS Policy.
  It allocates and zero out buffer, and fills in with safe and reasonable default settings.

  @param[out] RasGlobalPolicyPpi - The pointer to get RAS Policy PPI instance

  @retval EFI_SUCCESS               - The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES      - Insufficient resources to create buffer
**/
{
  RAS_GLOBAL_POLICY_PPI *PeiRasGlobalPolicy;

  PeiRasGlobalPolicy = (RAS_GLOBAL_POLICY_PPI *) AllocateZeroPool (sizeof (RAS_GLOBAL_POLICY_PPI));

  if (PeiRasGlobalPolicy == NULL) {
	  ASSERT (FALSE);
	  return EFI_OUT_OF_RESOURCES;
  }

  //
  // Default RAS configuration
  //
  PeiRasGlobalPolicy->SystemErrorEn                 = 1;
  PeiRasGlobalPolicy->ErrorCloakingEnable           = 1;
  PeiRasGlobalPolicy->MemoryErrorLog                = 1;
  PeiRasGlobalPolicy->MemoryCorrErrorLog            = 1;
  PeiRasGlobalPolicy->MemoryUnCorrErrorLog          = 1;
  PeiRasGlobalPolicy->PCIeErrorLog                  = 1;
  PeiRasGlobalPolicy->PCIeCorrErrorLog              = 1;
  PeiRasGlobalPolicy->PCIeNFErrorLog                = 1;
  PeiRasGlobalPolicy->PCIeFatalErrorLog             = 1;
  PeiRasGlobalPolicy->PCIeSystemErrEnable           = 1;
  PeiRasGlobalPolicy->PCIeParityErrEnable           = 1;
  PeiRasGlobalPolicy->RCECErrorLog                  = 1;
  PeiRasGlobalPolicy->BUnitErrorLog                 = 1;
  PeiRasGlobalPolicy->WheaSupport                   = 1;
  PeiRasGlobalPolicy->WheaEinj_05Extn               = 1;
  PeiRasGlobalPolicy->WheaLogging                   = 1;

  *RasGlobalPolicyPpi = PeiRasGlobalPolicy;

  return EFI_SUCCESS;
}


EFI_STATUS
PeiRasInstallPolicyPpi (
  IN  RAS_GLOBAL_POLICY_PPI *RasGlobalPolicyPpi
  )
/**
  PeiRasInstallPolicyPpi installs RasGlobalPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] RasGlobalPolicyPpi - The pointer to RAS Policy PPI instance

  @retval EFI_SUCCESS              - The policy is installed.
  @retval EFI_OUT_OF_RESOURCES     - Insufficient resources to create buffer
**/

{
  EFI_STATUS             Status;
  EFI_PEI_PPI_DESCRIPTOR *RasGlobalPolicyPpiDec;

  RasGlobalPolicyPpiDec = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (RasGlobalPolicyPpiDec == NULL) {
	ASSERT (FALSE);
	return EFI_OUT_OF_RESOURCES;
  }

  RasGlobalPolicyPpiDec->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  RasGlobalPolicyPpiDec->Guid  = &gRasPolicyPpiGuid;
  RasGlobalPolicyPpiDec->Ppi   = RasGlobalPolicyPpi;

  //
  // Install Ras Policy PPI
  //
  Status = PeiServicesInstallPpi (RasGlobalPolicyPpiDec);
  ASSERT_EFI_ERROR (Status);
  return Status;
}

