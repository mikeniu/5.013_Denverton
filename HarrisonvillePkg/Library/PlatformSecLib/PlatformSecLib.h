/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  Internal header file for SEC Platform hook library.

  Copyright (c) 2008, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.


**/

#ifndef  _PLATFORM_SEC_LIB_H_
#define  _PLATFORM_SEC_LIB_H_

#include <PiPei.h>

#include <Ppi/SecPlatformInformation.h>
#include <Ppi/TemporaryRamSupport.h>

#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PerformanceLib.h>
#include <Library/LocalApicLib.h>
//#include <Library/CmosAccessLib.h>

#define Flat32Start                  _ModuleEntryPoint
extern UINT32                        *TopOfCar;

#endif







