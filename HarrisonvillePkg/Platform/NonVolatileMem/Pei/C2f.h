//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

@copyright
  Copyright (c)  1999 - 2014 Intel Corporation. All rights reserved
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

  C2f.h

Abstract:

  Saves or restores DIMM data from flash

--*/

#ifndef _C2F_DIMM_PROCESSING_
#define _C2F_DIMM_PROCESSING_

#include "Base.h"
#include "Uefi.h"
#include <Uefi/UefiBaseType.h>
#include <Ppi/NonVolatileMemPolicy.h>


EFI_STATUS
EFIAPI
C2FCallback (
	IN EFI_PEI_SERVICES                 **PeiServices,
	IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
	IN VOID                       *Ppi
  );

#endif
