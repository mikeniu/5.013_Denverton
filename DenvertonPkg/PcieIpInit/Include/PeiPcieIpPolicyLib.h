/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

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

  PeiPcieIpPolicy.h

Abstract:

  Driver for Pcie Policy structure.

Revision History

--*/

#ifndef _PEI_PCIE_IP_POLICY_H_
#define _PEI_PCIE_IP_POLICY_H_

#include <Ppi/PeiPcieIpPolicyPpi.h>
#include <Library/Hoblib.h>
#include <Library/BaseMemoryLib.h>
#include <PcieIpRegs.h>

/**
  PcieIpCreatePolicyDefaults creates the default setting of PCIE IP Policy.
  It allocates and zero out buffer, and fills in with safe and reasonable default settings.

  @param[out] PcieIpGlobalPolicyPpi - The pointer to get PCIE IP Policy PPI instance

  @retval EFI_SUCCESS - The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES - Insufficient resources to create buffer
**/
EFI_STATUS
PcieIpCreatePolicyDefaults (
  OUT PCIE_IP_GLOBAL_POLICY_PPI **PcieIpGlobalPolicyPpi
  );

/**
  PcieIpInstallPolicyPpi installs PcieIpGlobalPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] PcieIpGlobalPolicyPpi - The pointer to PCIE IP Policy PPI instance

  @retval EFI_SUCCESS - The policy is installed.
  @retval EFI_OUT_OF_RESOURCES - Insufficient resources to create buffer
**/
EFI_STATUS
PcieIpInstallPolicyPpi (
  IN PCIE_IP_GLOBAL_POLICY_PPI *PcieIpGlobalPolicyPpi
  );

VOID
PcieIpPolicySetHob (
  IN PCIE_IP_GLOBAL_POLICY_PPI *PcieIpGlobalPolicyPpi
  );


#endif
