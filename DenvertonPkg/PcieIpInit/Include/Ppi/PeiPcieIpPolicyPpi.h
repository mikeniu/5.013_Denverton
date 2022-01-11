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

  PeiPcieIpPolicyPpi.h

Abstract:

  Driver for Pcie Policy structure.

Revision History

--*/

#ifndef _PEI_PCIE_IP_POLICY_PPI_H_
#define _PEI_PCIE_IP_POLICY_PPI_H_

#include <PcieIpPolicy.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MmPciLib.h>

typedef struct PCIE_IP_GLOBAL_POLICY PCIE_IP_GLOBAL_POLICY_PPI;			//< global policy IP PCIE

extern EFI_GUID gPcieIpPolicyPpiGuid;

extern EFI_GUID gPcieIpPolicyHobGuid;

#endif
