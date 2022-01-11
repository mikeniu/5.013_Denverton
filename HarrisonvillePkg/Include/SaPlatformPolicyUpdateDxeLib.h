/*++
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement                 
--*/
/*++

Copyright (c) 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  SaPlatformPolicyUpdateDxeLib.h

Abstract:

  Header file for DXE SaPlatformPolicyUpdate Library. 
  
--*/
#ifndef _SA_PLATFORM_POLICY_UPDATE_DXE_LIB_H_
#define _SA_PLATFORM_POLICY_UPDATE_DXE_LIB_H_

EFI_STATUS
EFIAPI
UpdateDxeSaPlatformPolicy (
  IN OUT  DXE_PLATFORM_SA_POLICY_PROTOCOL    *DxePlatformSaPolicy
  )
/*++
 
Routine Description:
 
  Get data for platform policy from setup options
  
Arguments:
 
  DxePlatformSaPolicy   - Pointer to DXE SA platform policy structure
 
Returns:
 
  EFI_SUCCESS                     Initialization complete.
 
--*/
;
#endif
