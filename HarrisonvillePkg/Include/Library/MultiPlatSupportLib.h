//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
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

  MultiPlatSupportLib.h

Abstract:

  MultiPlatSupport library definitions.

  This library provides functions to produce the matched default setting. 

--*/

#ifndef _MULTI_PLATFORM_SUPPORT_LIB_H_
#define _MULTI_PLATFORM_SUPPORT_LIB_H_

EFI_STATUS
EFIAPI
CreateDefaultVariableHob (
  IN UINT16  DefaultId,
  IN UINT16  BoardId
  );
/*++
Description:

  This function finds the matched default data and create GUID hob for it. 

Arguments:

  DefaultId - Specifies the type of defaults to retrieve.
  BoardId   - Specifies the platform board of defaults to retrieve.
  
Returns:

  EFI_SUCCESS - The matched default data is found.
  EFI_NOT_FOUND - The matched default data is not found.
  EFI_OUT_OF_RESOURCES - No enough resource to create HOB.

--*/

#endif
