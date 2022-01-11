/** @file
  This file is SampleCode of the library for Intel Silicon PEI 
  Platform Policy initialzation.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#include "PeiSiPolicyUpdate.h"
#include <Library/PeiServicesTablePointerLib.h>

/**
  This function performs Silicon PEI Policy initialzation.

  @param[in] SiPolicy  The Silicon Policy PPI instance

  @retval EFI_SUCCESS  The function completed successfully
**/
EFI_STATUS
EFIAPI
UpdatePeiSiPolicy (
  IN OUT SI_POLICY_PPI *SiPolicy
  )
{
  PLATFORM_INFO_HOB *PlatformInfo = NULL;
  EFI_STATUS        Status;

  Status = GetPlatformInfo (&PlatformInfo);
  ASSERT_EFI_ERROR (Status);

  if(PlatformInfo == NULL) {
  	  DEBUG ((EFI_D_ERROR, "Failed to get PlatformInfo\n"));
  	  return EFI_ABORTED;
  	}

  //
  // Update Silicon Policy Config
  //
  SiPolicy->EcPresent = PlatformInfo->EcPresent;

  return Status;
}

