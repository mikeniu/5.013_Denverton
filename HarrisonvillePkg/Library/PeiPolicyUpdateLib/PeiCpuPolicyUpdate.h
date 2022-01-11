/*++
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement 
--*/
/*++

Copyright (c) 2009 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  CpuPlatformPolicyUpdatePei.h

Abstract:

  Header file for PEI CpuPlatformPolicyUpdate.

--*/
#ifndef _CPU_PLATFORM_POLICY_UPDATE_PEI_H_
#define _CPU_PLATFORM_POLICY_UPDATE_PEI_H_

//
// External include files do NOT need to be explicitly specified in real EDKII
// environment
//
#include <PiPei.h>

#include <Guid/PlatformInfo.h>
#include <Ppi/CpuPlatformPolicy.h>
#include <Ppi/Wdt.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/Stall.h>
#include <Ppi/CpuIo.h>
#include <Library/DebugLib.h>
#include <Library/PeiPlatformLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiKscLib.h>
#include <Library/OverclockingLib.h>
#include <PlatformBoardId.h>
#include <CpuRegs.h>
#include <BaseCryptLib.h>
#include <SetupVariable.h>
#include <FlashMap.h>
#include <PowerMgmtDefinitions.h>
#include <PchAccess.h>
#include "PeiPchPolicyUpdate.h"

#endif