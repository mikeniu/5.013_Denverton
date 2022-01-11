/** @file
  This file contains definitions to Static PG miscellaneous PCH IP.

@copyright
 Copyright (c) 2016 Intel Corporation. All rights reserved
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

#ifndef _PCH_ST_PG_MISC_H_
#define _PCH_ST_PG_MISC_H_
#include "PchInitPei.h"
#include <Library/MmPciLib.h>
#include <Library/IoLib.h>

/**
  Initialize Misc IP in PEI
  EMMC

  Arguments:
  PchPolicy

  Returns:
  @retval EFI_SUCCESS - The function completed successfully.
  @retval Others 	- Error
**/
EFI_STATUS
EFIAPI
PeiMiscIpInit (
  PCH_POLICY_PPI     *PchPolicy
  );
#endif // _PCH_ST_PG_MISC_H_
