/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c)  1999 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:
  
  MePlatformPolicyUpdatePei.c
   
Abstract:

  Do Platform Stage ME initialization.

--*/
#include <MePlatformPolicyUpdatePei.h>


EFI_STATUS
UpdatePeiMePlatformPolicy (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN OUT   ME_PLATFORM_POLICY_PPI      *PeiMePlatformPolicyPpi
  )
/*++
 
Routine Description:
 
  Install the Me Platform Policy Library
  
Arguments:

  PeiServices             General purpose services available to every PEIM.
  PeiMePlatformPolicyPpi  Me Platform Policy Ppi
 
Returns:
 
  EFI_SUCCESS           Initialization complete.
  EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  EFI_DEVICE_ERROR      Device error, driver exits abnormally.
 
--*/
{

  return EFI_SUCCESS;
}

