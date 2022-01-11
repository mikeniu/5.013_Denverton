//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

@copyright
  Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
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

  IPMIPlatformID.h

--*/

#ifndef _IPMIPLATFORM_ID_H_
#define _IPMIPLATFORM_ID_H_

#include <PiPei.h>
#include <Uefi.h>

#include <Ppi/IpmiTransportPpi.h>
//#include <Library/IpmiBaseLib.h>	// Denverton AptioV Override
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/TimerLib.h>
#include <Library/PciLib.h>
#include <Guid/PlatformInfo.h>
#include <Library/PeiPlatformHookLib.h>

#define BMC_GENERAL_APPLICATION  0x30
#define SET_Platform_ID 0x20 

#endif  _IPMIPLATFORM_ID_H_
