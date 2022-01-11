/*++
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement 
--*/
/*++

Copyright (c) 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PcieIpPlatformPolicyUpdatePei.h

Abstract:

  Header file for PEI PcieIpPlatformPolicyUpdate.

--*/
#ifndef _PCIE_IP_PLATFORM_POLICY_UPDATE_PEI_H_
#define _PCIE_IP_PLATFORM_POLICY_UPDATE_PEI_H_

#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/PeiPcieIpPolicyPpi.h>
#include <Include/SetupVariable.h>

EFI_STATUS
EFIAPI
UpdatePeiPcieIpPolicyRestricted (
  IN OUT  PCIE_IP_GLOBAL_POLICY_PPI       *PcieIpPolicyPpi
  );

#endif
