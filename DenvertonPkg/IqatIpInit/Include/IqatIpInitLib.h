/** @file
  Header file for IqatIp Lib.
  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

@copyright
 Copyright (c) 2008 - 2015 Intel Corporation. All rights reserved
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
#ifndef _IQAT_IP_INIT_LIB_H_
#define _IQAT_IP_INIT_LIB_H_

#include <PchAccess.h>
#include <IqatIpPolicyHob.h>

/**
  Check whether IQAT controller is present in this chipset SKU.
  
  @retval TRUE                    IQAT is supported in this chipset
  @retval FALSE                   IQAT is not supported
**/
BOOLEAN
EFIAPI
IqatIpIsIqatPresent (
  VOID
  );

/**
  Check whether IQAT controller is enabled (in terms of
  static power gating).

  @retval TRUE    IQAT is enabled
  @retval FALSE   IQAT is statically power gated
**/
BOOLEAN
IqatIpIsIqatEnabled (
  VOID
  );

/**
  Configures IQat Secure RAM

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance

  @retval EFI_INVALID_PARAMETER   The parameter of PchPlatformPolicy is invalid
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureIqatSecureRam (
    VOID
  );

/**
  Configures IQat Registers

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance

**/
VOID
ConfigureIqatRegs (
  IN IQAT_IP_GLOBAL_POLICY_HOB      *IqatIpGlobalPolicy
);

/*++
Routine Description:
  IQAT Init routine.

Arguments:
  N/A

--*/
VOID
IqatIpInit (
  VOID
);

#endif 
