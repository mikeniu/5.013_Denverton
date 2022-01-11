/*++
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement                 
--*/
/*++

Copyright (c) 2011 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PchPlatformPolicyUpdatePeiLib.h

Abstract:

  Header file for PEI PchPlatformPolicyUpdate Library. 
  
--*/
#ifndef _PCH_PLATFORM_POLICY_UPDATE_PEI_LIB_H_
#define _PCH_PLATFORM_POLICY_UPDATE_PEI_LIB_H_

EFI_STATUS
EFIAPI
UpdatePeiPchPlatformPolicy (
  IN CONST    EFI_PEI_SERVICES            **PeiServices,
  IN OUT  PCH_PLATFORM_POLICY_PPI         *PchPlatformPolicy
  )
/*++

Routine Description:

  This function performs PCH PEI Platform Policy initialzation.

Arguments:

  PeiServices             General purpose services available to every PEIM.
  PchPlatformPolicy       The PCH Platform Policy PPI instance

Returns:

  EFI_SUCCESS             The PPI is installed and initialized.
  EFI ERRORS              The PPI is not successfully installed.
  EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

--*/
;

EFI_STATUS
EFIAPI
UpdatePeiPchInternalPolicy (
  IN CONST    EFI_PEI_SERVICES            **PeiServices,
  IN OUT  PCH_PLATFORM_POLICY_PPI         *PchPlatformPolicy
  )
/*++

Routine Description:

  This function performs PCH PEI Platform Policy initialzation.

Arguments:

  PeiServices             General purpose services available to every PEIM.
  PchPlatformPolicy       The PCH Platform Policy PPI instance

Returns:

  EFI_SUCCESS             The PPI is installed and initialized.
  EFI ERRORS              The PPI is not successfully installed.
  EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

--*/
;
#endif
