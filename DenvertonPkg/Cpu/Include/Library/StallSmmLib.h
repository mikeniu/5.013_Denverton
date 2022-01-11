/*++
 This file contains an 'Intel Peripheral Driver' and is        
 licensed for Intel CPUs and chipsets under the terms of your  
 license agreement with Intel or your vendor.  This file may   
 be modified by the user, subject to additional terms of the   
 license agreement                                             
--*/
/*++

Copyright (c)  1999 - 2008 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  SmmStallLib.h

Abstract:

  This library provides SMM functions for Stall.
  These can be used to save size and simplify code.
  All contents must be runtime and SMM safe.

--*/

#ifndef _SMM_STALL_LIB_H_
#define _SMM_STALL_LIB_H_
#include "PiDxe.h"
//#include "Efi.h"
#include "Pi/PiSmmCis.h"
extern EFI_SMM_SYSTEM_TABLE2  *mSmst;

VOID
SmmStall (
  IN  UINTN   Microseconds
  );
/*++

Routine Description:

  Delay for at least the request number of microseconds
    
Arguments:

  Microseconds - Number of microseconds to delay.

Returns:

  None

--*/

#endif
