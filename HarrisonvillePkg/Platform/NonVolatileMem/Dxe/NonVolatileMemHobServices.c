//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

@copyright
  Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement


Module Name:

 NonVolatileMemHob.c

Abstract:

  common retrieve function for NonVolatile Memory HOB

--*/
#include "NonVolatileMemHobServices.h"
#include <Library/DebugLib.h>
#include <Library/HobLib.h>

VOID GetNonVolatileMemHob(NON_VOLATILE_MEM_HOB **NonVolatileMemHob) {
  EFI_HOB_GUID_TYPE    *GuidHob;

  DEBUG ((DEBUG_ERROR, "%a()\n", __FUNCTION__));
  //
  // Search for the Platform Info PEIM GUID HOB.
  //
  GuidHob       = GetFirstGuidHob (&gNonVolatileMemHobGuid);
  ASSERT (GuidHob);
  *NonVolatileMemHob  = GET_GUID_HOB_DATA (GuidHob);

  DEBUG ((DEBUG_ERROR, "NonVolatileMemHob {NonVolMemMode:%x, ", (*NonVolatileMemHob)->NonVolMemMode));
  DEBUG ((DEBUG_ERROR,
    "C2fLowStart:%x, C2fLowLength:%x, C2fHighStart:%x, C2fHighLength:%x}\n",
    (*NonVolatileMemHob)->C2fLowStart, (*NonVolatileMemHob)->C2fLowLength,
    (*NonVolatileMemHob)->C2fHighStart, (*NonVolatileMemHob)->C2fHighLength
    ));
  DEBUG ((DEBUG_ERROR,
    " NVDIMM_SMBUS_ADDR {[0]:%x, [1]:%x, [2]:%x, [3]:%x}\n",
    (*NonVolatileMemHob)->NVDIMM_SMBUS_ADDR[0], (*NonVolatileMemHob)->NVDIMM_SMBUS_ADDR[1],
    (*NonVolatileMemHob)->NVDIMM_SMBUS_ADDR[2], (*NonVolatileMemHob)->NVDIMM_SMBUS_ADDR[3]
    ));
}


