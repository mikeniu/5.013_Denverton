//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PeiPlatformLib.h

Abstract:

--*/
#ifndef _PEI_PLATFORM_LIB_H_
#define _PEI_PLATFORM_LIB_H_

EFI_STATUS
PlatformInitStage1 (
  IN EFI_PEI_SERVICES          **PeiServices
  )
/*++
  Description :
    Program ICH registers for platform specific requirement. For example, decode region 
    program, GPIO program, KSC init
    
  Parameters:
    PeiSercies  - Point to Pei Services structure
    
  Returns:

--*/
;

EFI_STATUS
IsRecoveryMode (
  IN EFI_PEI_SERVICES          **PeiServices
  )
/*++

Routine Description:

  Detect Recovery Mode. 

Arguments:

  PeiServices - General purpose services available to every PEIM.

Returns:

  EFI_SUCCESS     - System in Recovery Mode
  EFI_UNSUPPORTED - System doesn't support Recovery Mode
  EFI_NOT_FOUND   - System is not in Recovery Mode

--*/
;

EFI_STATUS
PlatformInitStage2 (
  IN EFI_PEI_SERVICES          **PeiServices
  )
/*++
  Description :
    Do platform specific programming. For example, KSC init, Chipset programming
    
  Parameters:
    PeiSercies  - Point to Pei Services structure
    
  Returns:

--*/
;

#endif