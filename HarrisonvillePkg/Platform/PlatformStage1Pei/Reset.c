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
  
  Reset.c
   
Abstract:
  
  Produce Reset Ppi.
  
--*/


#include "PlatformStage1.h"

//
// Reset Generator I/O Port
//
#define RESET_GENERATOR_PORT  0xCF9

EFI_STATUS
EFIAPI
PchReset(
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
/*++

Routine Description:

  Provide hard reset PPI service.
  To generate full hard reset, write 0x0E to PCH RESET_GENERATOR_PORT (0xCF9).  

Arguments:

  PeiServices       General purpose services available to every PEIM.
  
Returns:
  
  Not return        System reset occured.
  EFI_DEVICE_ERROR  Device error, could not reset the system.

--*/
{
  ((**PeiServices).CpuIo->IoWrite8) (
           PeiServices,
           (**PeiServices).CpuIo,
           RESET_GENERATOR_PORT,
           0x0E
           );
  
  while (1) {
   CpuDeadLoop ();
  }

  //
  // System reset occured, should never reach at this line.
  //
  ASSERT_EFI_ERROR ( EFI_DEVICE_ERROR);

  return EFI_DEVICE_ERROR;
}
