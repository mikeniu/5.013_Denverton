/**
 This file contains an 'Intel Peripheral Driver' and is
 licensed for Intel CPUs and chipsets under the terms of your
 license agreement with Intel or your vendor.  This file may
 be modified by the user, subject to additional terms of the
 license agreement
**/

/**

Copyright (c) 2009-2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

File Name:

  SpsPei.h

  General SPS PEI definitions

**/
#ifndef _SPSPEI_H_
#define _SPSPEI_H_

UINT64
EFIAPI
ReadTimeStampCounter(VOID);

#ifdef FSP_FLAG
EFI_STATUS
SpsFspInitComplete(
  IN HECI_PPI                  *PpiHeci,
  IN PEI_ME_PLATFORM_POLICY_PPI    *PeiMePlatformPolicyPpi,
  UINT8                            WorkFlow,
  UINT64                           FeatureSet,
  BOOLEAN                          PwrOptBoot,
  UINT8                            Cores2Disable);
#endif
#endif // _SPSPEI_H_
