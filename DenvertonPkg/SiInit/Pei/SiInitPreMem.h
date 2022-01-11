/** @file
  Header file for Silicon Init Pre Memory module.

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

@par Specification
**/
#ifndef _SI_INIT_PRE_MEM_MODULE_H_
#define _SI_INIT_PRE_MEM_MODULE_H_

#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/SiPolicy.h>
#include <IncludePrivate/Library/PchInitLib.h>

#include <HsuartIpInit/Include/PeiHsuartIpPolicyLib.h>
#include <StPgIpInit/Include/PchPmcStPg.h>

//
// IO/MMIO resource limits
//
#define MIN_IO_SPACE_SIZE   0x10
//
// Set minimum MMIO space to 2MB for PCH XDCI requirement
//
#define MIN_MMIO_SPACE_SIZE 0x200000

//
// Pre Mem Performance GUIDs
//
extern EFI_GUID gPerfPchPrePolicyGuid;
extern EFI_GUID gPerfSiValidateGuid;
extern EFI_GUID gPerfPchValidateGuid;
extern EFI_GUID gPerfPchPreMemGuid;

#ifdef EFI_DEBUG
/**
  Check some silicon policies are valid for debugging unexpected problem if these
  values are not initialized or assigned incorrect resource.

  @param[in] SiPolicy The Silicon Policy PPI instance

**/
VOID
SiValidatePolicy (
  IN  SI_POLICY_PPI *SiPolicy
  );
#endif

#endif // _SI_INIT_PRE_MEM_MODULE_H_
