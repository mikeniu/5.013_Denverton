/** @file
  Prototype of the PeiSaPolicy library.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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
#ifndef _PEI_SA_POLICY_LIBRARY_H_
#define _PEI_SA_POLICY_LIBRARY_H_

#include <Ppi/SaPolicy.h>
#include <Include/Library/ConfigBlockLib.h>
 
/**
  CreateConfigBlocks creates the Config Blocks for SA Policy.
  @param[out] SiSaPolicyPpi      The pointer to get SA Policy PPI instance

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/  
EFI_STATUS
EFIAPI
CreateConfigBlocks (
  IN OUT  SI_SA_POLICY_PPI          **SiSaPolicyPpi
  );

/**
  SaInstallPolicyPpi installs SiSaPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] SiSaPolicyPpi       The pointer to SA Policy PPI instance

  @retval EFI_SUCCESS                   The policy is installed.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
SiSaInstallPolicyPpi (
  IN  SI_SA_POLICY_PPI            *SiSaPolicyPpi
  );

#endif // _PEI_SA_POLICY_LIBRARY_H_
