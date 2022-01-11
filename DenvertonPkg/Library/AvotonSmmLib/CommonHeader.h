/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/**@file
  Common header file shared by all source files.

  This file includes package header files, library classes and protocol, PPI & GUID definitions.

  Copyright (c) 2007, Intel Corporation. All rights reserved.<BR>
   This software and associated documentation (if any) is furnished
   under a license and may only be used or copied in accordance
   with the terms of the license. Except as permitted by such
   license, no part of this software or documentation may be
   reproduced, stored in a retrieval system, or transmitted in any
   form or by any means without the express written consent of
   Intel Corporation.
**/

#ifndef __COMMON_HEADER_H_
#define __COMMON_HEADER_H_


#include <Base.h>

#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>

#define R_PCH_ACPI_SMI_EN       0x30
#define B_PCH_ACPI_APMC_EN      0x00000020
#define B_PCH_ACPI_EOS          0x00000002
#define B_PCH_ACPI_GBL_SMI_EN   0x00000001
#define R_PCH_ACPI_SMI_STS      0x34
#define B_PCH_ACPI_APM_STS      0x00000020

#endif
