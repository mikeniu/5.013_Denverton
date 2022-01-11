/*++
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement 
--*/
/*++

Copyright (c) 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  SaPlatformPolicyUpdatePei.h

Abstract:

  Header file for SA SaPlatformPolicyUpdatePei.c

--*/
#ifndef _SA_PLATFORM_POLICY_UPDATE_PEI_H_
#define _SA_PLATFORM_POLICY_UPDATE_PEI_H_

//
// External include files do NOT need to be explicitly specified in real EDKII
// environment
//
#include <PlatformBoardId.h>
#include <PlatformDefinitions.h>
#include <Guid/SetupVariable.h>
//#include <SaAccess.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/ConfigBlockLib.h>
#include "PeiPchPolicyUpdate.h"
#include <Ppi/SaPolicy.h>
#include <Guid/PlatformInfo.h>

#define MAX_RANKS     4

#endif

