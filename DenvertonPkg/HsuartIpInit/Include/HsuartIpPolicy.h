/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

/*++

Copyright (c)  2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  HsuartIpPolicy.h

Abstract:

  Driver for Hsuart Policy structure.

Revision History

--*/

#ifndef _HSUART_IP_POLICY_H_
#define _HSUART_IP_POLICY_H_

#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MmPciLib.h>

#define MAX_HSUART_INSTANCES 3

struct HSUART_IP_GLOBAL_POLICY {
  UINT8                   HsuartNum;
  UINT8                   DeviceNum;
  UINT32                  DefaultClassCode;
  UINT8                   InterruptPin[MAX_HSUART_INSTANCES];
};
extern EFI_GUID gHsuartIpPolicyPpiGuid;

typedef struct HSUART_IP_GLOBAL_POLICY HSUART_IP_GLOBAL_POLICY_PPI;
#endif
