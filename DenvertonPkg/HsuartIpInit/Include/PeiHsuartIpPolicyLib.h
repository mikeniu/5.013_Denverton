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

  PeiHsuartIpPolicy.h

Abstract:

  Driver for Hsuart Policy structure.

Revision History

--*/

#ifndef _PEI_HSUART_IP_POLICY_H_
#define _PEI_HSUART_IP_POLICY_H_

#include <HsuartIpPolicy.h>
#include <HsuartIpRegs.h>

/**
  HsuartIpPolicyInit creates the default setting of HSUART IP 
  Policy. It allocates and zero out buffer, and fills in with 
  safe and reasonable default settings. 

  @param[out] HsuartIpGlobalPolicyPpi - The pointer to get 
        HSUART IP Policy PPI instance

  @retval EFI_SUCCESS - The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES - Insufficient resources to create buffer
**/
EFI_STATUS
HsuartIpPolicyInit (
  OUT HSUART_IP_GLOBAL_POLICY_PPI **HsuartIpGlobalPolicyPpi
  );

/**
  HsuartIpInstallPolicyPpi installs HsuartIpGlobalPolicyPpi. While
  installed, RC assumes the Policy is ready and finalized. So please update and override 
  any setting before calling this function.

  @param[in] HsuartIpGlobalPolicyPpi - The pointer to HSUART IP 
        Policy PPI instance

  @retval EFI_SUCCESS - The policy is installed.
  @retval EFI_OUT_OF_RESOURCES - Insufficient resources to create buffer
**/
EFI_STATUS
HsuartIpInstallPolicyPpi (
  IN HSUART_IP_GLOBAL_POLICY_PPI *HsuartIpGlobalPolicyPpi
  );

#endif
