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

  PeiPcieIpInitLib.h

Abstract:

  Driver for Pcie IP Initialization library.

Revision History

--*/

#ifndef _PEI_PCIE_IP_INIT_LIB_H_
#define _PEI_PCIE_IP_INIT_LIB_H_

#include <Ppi/PeiPcieIpPolicyPpi.h>
#include <PcieIpRegs.h>

#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <SouthCluster/Include/Register/PchRegsPcr.h>
#include <PchAccess.h>
#include <PcieIpPlatformLib.h>
#include <PeiPcieIpPolicyLib.h>

#define PEIPCIE_OPCODE_BEFORE_BIFURCATION		0xC1
#define PEIPCIE_OPCODE_BEFORE_LINK_TRAINNING	0xC4

#define PEIPCIE_COMMAND                         0x07
#define PEIPCIE_EXPPTMBAR                       0xFE600000
#define PEIPCIE_FOMHINTTBL_MAX                  12

// PCIE's phases in the process of initialization
typedef enum {
  Phase0 = 0,
  Phase1,
} PCIE_IP_INIT_PHASE;


/**
  Initialize Pcie IP

  Arguments:
  @param[in] PcieIpGlobalPolicy - This is the global PCIE policy
  @param[in] PcieIpInitPhase - Define the initialize phase

  Returns:
  @retval EFI_SUCCESS 				- The function completed successfully.
  @retval RETURN_INVALID_PARAMETER 	- invalid parameter
**/
EFI_STATUS
PcieIpInit (
  IN PCIE_IP_GLOBAL_POLICY_PPI *PcieIpGlobalPolicy,
  IN PCIE_IP_INIT_PHASE         PcieIpInitPhase
  );


#endif
