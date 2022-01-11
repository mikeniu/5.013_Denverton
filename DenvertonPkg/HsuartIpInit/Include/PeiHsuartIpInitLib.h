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

  PeiHsuartIpInitLib.h

Abstract:

  Driver for Hsuart IP Initialization library.

Revision History

--*/

#ifndef _PEI_HSUART_IP_INIT_LIB_H_
#define _PEI_HSUART_IP_INIT_LIB_H_

#include <HsuartIpPolicy.h>
#include <HsuartIpRegs.h>



/**
  Initialize IP Hsuar

  @param[in] HsuartIpGlobalPolicy - This is the global HSUART 
        policy

  @retval EFI_SUCCESS - The function completed successfully.
**/
EFI_STATUS
HsuartIpInit (
  IN HSUART_IP_GLOBAL_POLICY_PPI *HsuartIpGlobalPolicy
  );

#endif
