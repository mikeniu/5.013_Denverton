/** @file
  This file is PeiSiPolicyLib library creates default settings of RC
  Policy and installs RC Policy PPI.

@copyright
  Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
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
#include "PeiSiPolicyLibrary.h"

/**
  SiCreatePolicyDefaults creates the default setting of Silicon Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] SiPolicyPpi         The pointer to get Silicon Policy PPI instance

  @retval EFI_SUCCESS             The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES    Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
SiCreatePolicyDefaults (
  OUT  SI_POLICY_PPI **SiPolicyPpi
  )
{
  SI_POLICY_PPI *SiPolicy;

  SiPolicy = (SI_POLICY_PPI *) AllocateZeroPool (sizeof (SI_POLICY_PPI));
  if (SiPolicy == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // General intialization
  //
  SiPolicy->Revision        = SI_POLICY_REVISION;

  //
  // PlatformData configuration
  //
//  SiPolicy->EcPresent       = 0;
  //
  // Temporary Bus range for silicon initialization.
  //
  SiPolicy->TempPciBusMin   = 2;
  SiPolicy->TempPciBusMax   = 10;
  //
  // Temporary Memory Base Address for PCI devices to be used to initialize MMIO registers.
  // Minimum size is 2MB bytes.
  //
  SiPolicy->TempMemBaseAddr = TEMP_MEM_BASE_ADDRESS;
  SiPolicy->TempMemSize     = TEMP_MEM_SIZE;
  //
  // Temporary IO Base Address for PCI devices to be used to initialize IO registers.
  // And size of temporary IO space.
  //
  SiPolicy->TempIoBaseAddr  = TEMP_IO_BASE_ADDRESS;
  SiPolicy->TempIoSize      = TEMP_IO_SIZE;

  *SiPolicyPpi = SiPolicy;
  return EFI_SUCCESS;
}

/**
  SiInstallPolicyPpi installs SiPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] SiPolicyPpi         The pointer to Silicon Policy PPI instance

  @retval EFI_SUCCESS            The policy is installed.
  @retval EFI_OUT_OF_RESOURCES   Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
SiInstallPolicyPpi (
  IN  SI_POLICY_PPI *SiPolicyPpi
  )
{
  EFI_STATUS             Status;
  EFI_PEI_PPI_DESCRIPTOR *SiPolicyPpiDesc;

  SiPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (SiPolicyPpiDesc == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  SiPolicyPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  SiPolicyPpiDesc->Guid  = &gSiPolicyPpiGuid;
  SiPolicyPpiDesc->Ppi   = SiPolicyPpi;

  //
  // Print whole SI_POLICY_PPI and serial out.
  //
  SiPrintPolicyPpi (SiPolicyPpi);

  //
  // Install Silicon Policy PPI
  //
  Status = PeiServicesInstallPpi (SiPolicyPpiDesc);
  ASSERT_EFI_ERROR (Status);
  return Status;
}
