/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c) 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  IePolicyInitPei.c

@brief:

  This file is SampleCode for Intel IE PEI Platform Policy initialization.

**/

#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include "Ppi/IePlatformPolicyPei.h"
#include "Library/IePlatformPolicyUpdatePeiLib.h"

///
/// Function implementations
///
/**

  @brief
  Initialize Intel IE PEI Platform Policy

  @param[in] FfsHeader            Pointer to Firmware File System file header.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures.

**/


EFI_STATUS
IePolicyInitPeiEntryPoint (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS                  Status;
  PEI_IE_PLATFORM_POLICY_PPI  *IePlatformPolicyPpi;
  EFI_PEI_PPI_DESCRIPTOR      *IePlatformPolicyPpiDesc;

  ///
  /// Allocate descriptor and PPI structures
  ///
  IePlatformPolicyPpi = (PEI_IE_PLATFORM_POLICY_PPI *) AllocateZeroPool (sizeof (PEI_IE_PLATFORM_POLICY_PPI));
  ASSERT (IePlatformPolicyPpi != NULL);
  if (IePlatformPolicyPpi == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  IePlatformPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (IePlatformPolicyPpiDesc != NULL);
  if (IePlatformPolicyPpiDesc == NULL) {
	  FreePool (IePlatformPolicyPpi);     // Denverton AptioV Override  - EIP#343554
    return EFI_OUT_OF_RESOURCES;
  }
  ///
  /// Initialize the PPI
  ///
  IePlatformPolicyPpiDesc->Flags  = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  IePlatformPolicyPpiDesc->Guid   = &gPeiIePlatformPolicyPpiGuid;


  ///
  /// Initialize the Platform Configuration
  ///
  UpdatePeiIePlatformPolicy (IePlatformPolicyPpi);

  IePlatformPolicyPpiDesc->Ppi = IePlatformPolicyPpi;

  ///
  /// TBD put initialize codes in here if needs
  ///
  ///
  /// Install the IE PEI Platform Policy PPI
  ///
  Status = (**PeiServices).InstallPpi (PeiServices, IePlatformPolicyPpiDesc);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "[IE Policy] IE PEI Platform Policy PPI Installed\n"));

  return Status;
}
