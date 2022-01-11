/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformSaInit.c

Abstract:

  Do Platform Stage System Agent initialization.

--*/

#include "PlatformStage2.h"
//#include "NbSetup.h"
#include "PlatformDefinitions.h"
#include "PlatformBoardId.h"
#include "Guid\PlatformInfo.h"
#include <Library/DebugPrintErrorLevelLib.h>
#include <Ppi/SaPlatformPolicy.h>


EFI_STATUS
GetPlatformInfo(
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  OUT PLATFORM_INFO_HOB                 **PlatformInfoHob
);

EFI_STATUS
PlatformSaInit (
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
/*++

Routine Description:

  Initialize the GPIO IO selection, GPIO USE selection, and GPIO signal inversion registers

Arguments:

  PeiServices   General purpose services available to every PEIM.

Returns:

  EFI_SUCCESS   Operation success.

--*/
{
  return EFI_SUCCESS;
}
