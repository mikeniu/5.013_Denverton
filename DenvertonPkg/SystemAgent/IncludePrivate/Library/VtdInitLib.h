/** @file
  This code provides a initialization of intel VT-d (Virtualization Technology for Directed I/O).

@copyright
 Copyright (c) 1999 - 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _VT_D_H_
#define _VT_D_H_

///
/// Include files
///
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PostCodeLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciLib.h>
#include <SaAccess.h>
#include <Ppi/SaPolicy.h>

#define VTD_ECAP_REG  0x10
#define IR            BIT3

/**
  Configure VT-d Base and capabilities.

  @param[in]   VTD_CONFIG                 VTD config block from SA Policy PPI
  @param[in]   SA_MISC_CONFIG             MISC config block from SA Policy PPI

  @retval EFI_SUCCESS     - Vtd initialization complete
  @exception EFI_UNSUPPORTED - Vtd is not enabled by policy
**/
EFI_STATUS
VtdInit (
  IN       VTD_CONFIG                        *Vtd
  );
#endif
