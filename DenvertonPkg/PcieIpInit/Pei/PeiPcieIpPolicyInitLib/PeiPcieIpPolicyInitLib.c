/** @file
  This file is a library for PCIE Ip Policy specific Initalization.

@copyright
 Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
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
#include <PeiPcieIpPolicyLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <PcieIpPlatformLib.h>

/* ****************************************** private functions  ****************************************** */



/* ****************************************** public functions  ****************************************** */

/**
  Initialize PEI Hob with PCIE_IP_GLOBAL_POLICY_PPI

  @param PcieIpGlobalPolicyPpi 		PCIE global policy structure
**/
VOID
PcieIpPolicySetHob (
  IN PCIE_IP_GLOBAL_POLICY_PPI *PcieIpGlobalPolicyPpi
  )
{
  EFI_PEI_HOB_POINTERS             Hob;
  UINTN                            BufferSize;

  BufferSize = sizeof (*PcieIpGlobalPolicyPpi);
  Hob.Raw = BuildGuidHob (
              &gPcieIpPolicyHobGuid,
              BufferSize
              );
  ASSERT (Hob.Raw);

  CopyMem ((VOID *)Hob.Raw, PcieIpGlobalPolicyPpi, sizeof(*PcieIpGlobalPolicyPpi));
}

EFI_STATUS
PcieIpCreatePolicyDefaults (
  OUT  PCIE_IP_GLOBAL_POLICY_PPI **PcieIpGlobalPolicyPpi
  )
/**
  PcieIpCreatePolicyDefaults creates the default setting of PCIE IP Policy.
  It allocates and zero out buffer, and fills in with safe and reasonable default settings.

  @param[out] PcieIpGlobalPolicyPpi - The pointer to get PCIE IP Policy PPI instance

  @retval EFI_SUCCESS               - The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES      - Insufficient resources to create buffer
**/
{
  PCIE_IP_GLOBAL_POLICY_PPI *PcieIpGlobalPolicy;

  PcieIpGlobalPolicy = (PCIE_IP_GLOBAL_POLICY_PPI *) AllocateZeroPool (sizeof (PCIE_IP_GLOBAL_POLICY_PPI));

  if (PcieIpGlobalPolicy == NULL) {
	ASSERT (FALSE);
	return EFI_OUT_OF_RESOURCES;
  }

  PcieIpPlatformInitDefault (PcieIpGlobalPolicy);
  *PcieIpGlobalPolicyPpi = PcieIpGlobalPolicy;
  return EFI_SUCCESS;
}

EFI_STATUS
PcieIpInstallPolicyPpi (
  IN  PCIE_IP_GLOBAL_POLICY_PPI *PcieIpGlobalPolicyPpi
  )
/**
  PcieIpInstallPolicyPpi installs PcieIpGlobalPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] PcieIpGlobalPolicyPpi - The pointer to PCIE IP Policy PPI instance

  @retval EFI_SUCCESS              - The policy is installed.
  @retval EFI_OUT_OF_RESOURCES     - Insufficient resources to create buffer
**/
{
  EFI_STATUS             Status;
  EFI_PEI_PPI_DESCRIPTOR *PcieIpGlobalPolicyPpiDec;

  PcieIpGlobalPolicyPpiDec = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (PcieIpGlobalPolicyPpiDec == NULL) {
	ASSERT (FALSE);
	return EFI_OUT_OF_RESOURCES;
  }

  PcieIpGlobalPolicyPpiDec->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PcieIpGlobalPolicyPpiDec->Guid  = &gPcieIpPolicyPpiGuid;
  PcieIpGlobalPolicyPpiDec->Ppi   = PcieIpGlobalPolicyPpi;

  //
  // Install Silicon Policy PPI
  //
  Status = PeiServicesInstallPpi (PcieIpGlobalPolicyPpiDec);
  ASSERT_EFI_ERROR (Status);
  return Status;
}


