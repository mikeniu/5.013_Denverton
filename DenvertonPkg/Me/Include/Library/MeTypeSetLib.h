/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

/**

Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  MeTypeSetLib.h

@brief:

  HECI Library

**/
#ifndef _ME_TYPE_SET_LIB_H_
#define _ME_TYPE_SET_LIB_H_

#include "MeTypeLib.h"

#include "Protocol/MePlatformPolicyProtocol.h"

EFI_STATUS MeTypeSet (DXE_ME_POLICY_PROTOCOL  *MePlatformPolicyInstance);

#if AMT_SUPPORT
EFI_STATUS MeRemoteSessionActiveSet (DXE_ME_POLICY_PROTOCOL  *MePlatformPolicyInstance);
EFI_STATUS MeMeStateControlSet (DXE_ME_POLICY_PROTOCOL  *MePlatformPolicyInstance);
#endif // AMT_SUPPORT

#endif // _ME_TYPE_LIB_H_
