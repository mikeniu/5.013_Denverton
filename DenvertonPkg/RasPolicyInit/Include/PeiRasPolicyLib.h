/*++
Copyright (c)  2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PeiRasPolicyLib.h

Abstract:

  Driver for RAS Policy Library.

Revision History

--*/

#ifndef _PEI_RAS_POLICY_LIB_H_
#define _PEI_RAS_POLICY_LIB_H_

#include <Ppi/PeiRasPolicyPpi.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>

/**
  PeiRasCreatePolicyDefaults creates the default setting of RAS Policy.
  It allocates and zero out buffer, and fills in with safe and reasonable default settings.

  @param[out] RasGlobalPolicyPpi - The pointer to get PCIE IP Policy PPI instance

  @retval EFI_SUCCESS - The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES - Insufficient resources to create buffer
**/
EFI_STATUS
PeiRasCreatePolicyDefaults (
  OUT  RAS_GLOBAL_POLICY_PPI **RasGlobalPolicyPpi
  );

/**
  PeiRasInstallPolicyPpi installs RasGlobalPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] RasGlobalPolicyPpi - The pointer to RAS Policy PPI instance

  @retval EFI_SUCCESS              - The policy is installed.
  @retval EFI_OUT_OF_RESOURCES     - Insufficient resources to create buffer
**/
EFI_STATUS
PeiRasInstallPolicyPpi (
  IN  RAS_GLOBAL_POLICY_PPI *RasGlobalPolicyPpi
  );


/**
  Build RAS_GLOBAL_POLICY_PPI HOB

  @param RasGlobalPolicyPpi 		RAS global policy structure
**/

VOID
PeiRasPolicySetHob (
  IN RAS_GLOBAL_POLICY_PPI *RasGlobalPolicyPpi
  );

#endif
