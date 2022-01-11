/** @file
  This file is a library for IQAT IP Policy Initialization.

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
#include <IqatIpPolicyLib.h>

/* ****************************************** private functions  ****************************************** */



/* ****************************************** public functions  ****************************************** */

/**
  Get IQAT_IP_GLOBAL_POLICY_PPI HOB

  @return IqatIpGlobalPolicyPpi 		IQAT global policy structure
**/
IQAT_IP_GLOBAL_POLICY_PPI *
IqatIpPolicyGetHob (
  VOID
  )
{
  EFI_HOB_GUID_TYPE            *GuidHob;
  IQAT_IP_GLOBAL_POLICY_PPI    *IqatIpGlobalPolicyPpi;

  GuidHob = GetFirstGuidHob (&gIqatIpPolicyHobGuid);
  ASSERT (GuidHob != NULL);

  IqatIpGlobalPolicyPpi = (IQAT_IP_GLOBAL_POLICY_PPI *)GET_GUID_HOB_DATA (GuidHob);

  return IqatIpGlobalPolicyPpi;
}

/**
  Build IQAT_IP_GLOBAL_POLICY_PPI HOB

  @param IqatIpGlobalPolicyPpi 		IQAT global policy structure
**/
VOID
IqatIpPolicySetHob (
  IN IQAT_IP_GLOBAL_POLICY_PPI *IqatIpGlobalPolicyPpi
  )
{
  EFI_PEI_HOB_POINTERS             Hob;
  UINTN                            BufferSize;

  BufferSize = sizeof (IQAT_IP_GLOBAL_POLICY_PPI);
  Hob.Raw = BuildGuidHob (
              &gIqatIpPolicyHobGuid,
              BufferSize
              );
  ASSERT (Hob.Raw);

  CopyMem ((VOID *)Hob.Raw, IqatIpGlobalPolicyPpi, sizeof(IQAT_IP_GLOBAL_POLICY_PPI));
}

EFI_STATUS
IqatIpCreatePolicyDefaults (
  OUT  IQAT_IP_GLOBAL_POLICY_PPI **IqatIpGlobalPolicyPpi
  )
/**
  IqatIpCreatePolicyDefaults creates the default setting of IQAT IP Policy.
  It allocates and zero out buffer, and fills in with safe and reasonable default settings.

  @param[out] IqatIpGlobalPolicyPpi - The pointer to get IQAT IP Policy PPI instance

  @retval EFI_SUCCESS               - The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES      - Insufficient resources to create buffer
**/
{
  IQAT_IP_GLOBAL_POLICY_PPI *IqatIpGlobalPolicy;

  IqatIpGlobalPolicy = (IQAT_IP_GLOBAL_POLICY_PPI *) AllocateZeroPool (sizeof (IQAT_IP_GLOBAL_POLICY_PPI));

  if (IqatIpGlobalPolicy == NULL) {
	ASSERT (FALSE);
	return EFI_OUT_OF_RESOURCES;
  }

  //
  // Default IQAT configuration
  //
  IqatIpGlobalPolicy->IqatIpConfig.Enable = TRUE;
  IqatIpGlobalPolicy->IqatIpConfig.SetFuseCtl   = FALSE;
  IqatIpGlobalPolicy->IqatIpConfig.FuseCtlValue = 0;
  IqatIpGlobalPolicy->IqatIpConfig.Set64BMrrMpl = TRUE;

  *IqatIpGlobalPolicyPpi = IqatIpGlobalPolicy;

  return EFI_SUCCESS;
}

EFI_STATUS
IqatIpInstallPolicyPpi (
  IN  IQAT_IP_GLOBAL_POLICY_PPI *IqatIpGlobalPolicyPpi
  )
/**
  IqatIpInstallPolicyPpi installs IqatIpGlobalPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] IqatIpGlobalPolicyPpi - The pointer to IQAT IP Policy PPI instance

  @retval EFI_SUCCESS              - The policy is installed.
  @retval EFI_OUT_OF_RESOURCES     - Insufficient resources to create buffer
**/
{
  EFI_STATUS             Status;
  EFI_PEI_PPI_DESCRIPTOR *IqatIpGlobalPolicyPpiDec;

  IqatIpGlobalPolicyPpiDec = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (IqatIpGlobalPolicyPpiDec == NULL) {
	ASSERT (FALSE);
	return EFI_OUT_OF_RESOURCES;
  }

  IqatIpGlobalPolicyPpiDec->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  IqatIpGlobalPolicyPpiDec->Guid  = &gIqatIpPolicyPpiGuid;
  IqatIpGlobalPolicyPpiDec->Ppi   = IqatIpGlobalPolicyPpi;

  //
  // Install Silicon Policy PPI
  //
  Status = PeiServicesInstallPpi (IqatIpGlobalPolicyPpiDec);
  ASSERT_EFI_ERROR (Status);
  return Status;
}


