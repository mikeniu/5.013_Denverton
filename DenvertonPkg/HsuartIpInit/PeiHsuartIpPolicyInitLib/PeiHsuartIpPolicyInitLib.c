/** @file
  This file is a library for PCIE Ip Policy specific Initalization.

@copyright
 Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved This
 software and associated documentation (if any) is furnished
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
#include <PeiHsuartIpPolicyLib.h>

/**
  HsuartIpPolicyInit creates the default setting of Hsuart IP 
  Policy. It allocates and zero out buffer, and fills in with 
  safe and reasonable default settings. 

  @param[out] HsuartIpGlobalPolicyPpi - The pointer to get 
        HSUART IP Policy PPI instance

  @retval EFI_SUCCESS               - The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES      - Insufficient resources to create buffer
**/
EFI_STATUS
HsuartIpPolicyInit (
  OUT  HSUART_IP_GLOBAL_POLICY_PPI **HsuartIpGlobalPolicyPpi
  )
{
  HSUART_IP_GLOBAL_POLICY_PPI *HsuartIpGlobalPolicy;

  HsuartIpGlobalPolicy = (HSUART_IP_GLOBAL_POLICY_PPI *) AllocateZeroPool (sizeof (HSUART_IP_GLOBAL_POLICY_PPI));

  if (HsuartIpGlobalPolicy == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  //  Initialize Policy 
  HsuartIpGlobalPolicy->HsuartNum = MAX_HSUART_INSTANCES;
  HsuartIpGlobalPolicy->DeviceNum = V_HSUART_DEV_NUM;
  HsuartIpGlobalPolicy->DefaultClassCode = V_DEFAULT_CLASS_CODE;
  HsuartIpGlobalPolicy->InterruptPin[0] = V_HSUART_INTERRUPT_INTR_PIN_INTA;
  HsuartIpGlobalPolicy->InterruptPin[1] = V_HSUART_INTERRUPT_INTR_PIN_INTB;
  HsuartIpGlobalPolicy->InterruptPin[2] = V_HSUART_INTERRUPT_INTR_PIN_INTC;

  *HsuartIpGlobalPolicyPpi = HsuartIpGlobalPolicy;

  return EFI_SUCCESS;
}

/**
  HsuartIpInstallPolicyPpi installs HsuartIpGlobalPolicyPpi. 
  While installed, RC assumes the Policy is ready and finalized. 
  So please update and override any setting before calling this 
  function. 

  @param[in] HsuartIpGlobalPolicyPpi - The pointer to HSUART IP 
        Policy PPI instance

  @retval EFI_SUCCESS              - The policy is installed.
  @retval EFI_OUT_OF_RESOURCES     - Insufficient resources to create buffer
**/
EFI_STATUS
HsuartIpInstallPolicyPpi (
  IN  HSUART_IP_GLOBAL_POLICY_PPI *HsuartIpGlobalPolicyPpi
  )
{
  EFI_STATUS             Status;
  EFI_PEI_PPI_DESCRIPTOR *HsuartIpGlobalPolicyPpiDesc;

  HsuartIpGlobalPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (HsuartIpGlobalPolicyPpiDesc == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  HsuartIpGlobalPolicyPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  HsuartIpGlobalPolicyPpiDesc->Guid  = &gHsuartIpPolicyPpiGuid;
  HsuartIpGlobalPolicyPpiDesc->Ppi   = HsuartIpGlobalPolicyPpi;

  //
  // Install Silicon Policy PPI
  //
  Status = PeiServicesInstallPpi (HsuartIpGlobalPolicyPpiDesc);
  ASSERT_EFI_ERROR (Status);
  return Status;
}

