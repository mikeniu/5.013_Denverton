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

  NonVolatileMemPei.h

Abstract:

  Platform Stage2 header file

--*/
#ifndef _NON_VOLATILE_MEM_PEI_H_
#define _NON_VOLATILE_MEM_PEI_H_

#include "Base.h"
#include "Uefi.h"
#include <Uefi/UefiBaseType.h>

EFI_STATUS
EFIAPI
NonVolatileMemPeiInit (
  IN EFI_PEI_FILE_HANDLE    FfsHeader,
  IN CONST EFI_PEI_SERVICES **PeiServices
  );

#endif //_NON_VOLATILE_MEM_PEI_H_
