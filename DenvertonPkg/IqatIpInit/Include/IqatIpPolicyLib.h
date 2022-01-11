/** @file
  Header file for IqatIp Policy Init Library.

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

#ifndef _IQAT_IP_POLICY_LIB_H_
#define _IQAT_IP_POLICY_LIB_H_

#include <Ppi/IqatIpPolicyPpi.h>
#include <Library/Hoblib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <IqatIpPolicyHob.h>

/**
  IqatIpCreatePolicyDefaults creates the default setting of IQAT IP Policy.
  It allocates and zero out buffer, and fills in with safe and reasonable default settings.

  @param[out] IqatIpGlobalPolicyPpi - The pointer to get IQAT IP Policy PPI instance

  @retval EFI_SUCCESS - The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES - Insufficient resources to create buffer
**/
EFI_STATUS
IqatIpCreatePolicyDefaults (
  OUT IQAT_IP_GLOBAL_POLICY_PPI **IqatIpGlobalPolicyPpi
  );

/**
  IqatIpInstallPolicyPpi installs IqatIpGlobalPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] IqatIpGlobalPolicyPpi - The pointer to IQAT IP Policy PPI instance

  @retval EFI_SUCCESS - The policy is installed.
  @retval EFI_OUT_OF_RESOURCES - Insufficient resources to create buffer
**/
EFI_STATUS
IqatIpInstallPolicyPpi (
  IN IQAT_IP_GLOBAL_POLICY_PPI *IqatIpGlobalPolicyPpi
  );

/**
  IqatIpPolicySetHob build a HOB for IqatIpGlobalPolicyPpi.

  @param[in] IqatIpGlobalPolicyPpi - The pointer to IQAT IP Policy PPI instance

  @retval EFI_SUCCESS - The policy is installed.
  @retval EFI_OUT_OF_RESOURCES - Insufficient resources to create buffer
**/
VOID
IqatIpPolicySetHob (
  IN IQAT_IP_GLOBAL_POLICY_PPI *IqatIpGlobalPolicyPpi
  );

/**
  IqatIpPolicyGetHob find IqatIpGlobalPolicyPpi from HOB.

  @param[in] IqatIpGlobalPolicyPpi - The pointer to IQAT IP Policy PPI instance

  @retval EFI_SUCCESS - The policy is installed.
  @retval EFI_OUT_OF_RESOURCES - Insufficient resources to create buffer
**/
IQAT_IP_GLOBAL_POLICY_PPI *
IqatIpPolicyGetHob (
  VOID
  );

#endif //_IQAT_IP_POLICY_LIB_H_
