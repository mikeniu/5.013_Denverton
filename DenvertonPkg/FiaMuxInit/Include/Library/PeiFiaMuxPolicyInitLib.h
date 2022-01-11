/** @file
  Header file for FIA Mux Policy Init PEI library.

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

@par Specification
**/

#ifndef _PEI_FIA_MUX_POLICY_INIT_LIB_H_
#define _PEI_FIA_MUX_POLICY_INIT_LIB_H_

#include <Ppi/FiaMuxPolicyPpi.h>
#include <Guid/FiaMuxHobGuid.h>

/**
Get FIA_MUX_CONFIG_HOB HOB

@return FiaMuxConfigHob 		FIA MUX Config HOB structure
**/
FIA_MUX_CONFIG_HOB *
FiaMuxPolicyGetHob(
VOID
);

/**
Build FIA_MUX_CONFIG_HOB HOB

@param FiaMuxConfigHob 		FIA MUX Config HOB structure
**/
VOID
FiaMuxPolicySetHob(
IN FIA_MUX_CONFIG_HOB *FiaMuxConfigHob
);

/**
  Build FIA_MUX_CONFIG_HOB from FIA_MUX_GLOBAL_POLICY_PPI

  @param[in] FiaMuxGlobalPolicyPpi - The pointer to FIA MUX Policy PPI instance
**/
VOID
FiaMuxCreateHob(
  IN  FIA_MUX_GLOBAL_POLICY_PPI *FiaMuxGlobalPolicyPpi
);

/**
FiaMuxCreatePolicyDefaults creates the default setting of FIA MUX Policy.
It allocates and zero out buffer, and fills in with safe and reasonable default settings.

@param[out] FiaMuxGlobalPolicyPpi - The pointer to get FIA MUX Policy PPI instance

@retval EFI_SUCCESS               - The policy default is initialized.
@retval EFI_OUT_OF_RESOURCES      - Insufficient resources to create buffer
**/

EFI_STATUS
FiaMuxCreatePolicyDefaults(
OUT  FIA_MUX_GLOBAL_POLICY_PPI **FiaMuxGlobalPolicyPpi
);

/**
FiaMuxInstallPolicyPpi installs FiaMuxGlobalPolicyPpi.

@param[in] FiaMuxGlobalPolicyPpi - The pointer to FIA MUX Policy PPI instance

@retval EFI_SUCCESS              - The policy is installed.
@retval EFI_OUT_OF_RESOURCES     - Insufficient resources to create buffer
**/
EFI_STATUS
FiaMuxInstallPolicyPpi(
IN  FIA_MUX_GLOBAL_POLICY_PPI *FiaMuxGlobalPolicyPpi
);

#endif  //_PEI_FIA_MUX_POLICY_INIT_LIB_H_
