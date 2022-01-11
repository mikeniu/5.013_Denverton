/** @file
  Silicon Policy HOB is used for gathering platform 
  related Intel silicon information and policy setting. 

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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

#ifndef _SI_POLICY_HOB_H_
#define _SI_POLICY_HOB_H_

#include "SiPolicy.h"

extern EFI_GUID gSiPolicyHobGuid;

typedef CONST struct _SI_POLICY SI_POLICY_HOB;

#endif // _SI_POLICY_HOB_H_
