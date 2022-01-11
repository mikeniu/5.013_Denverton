//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c) 2010 - 2013, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformCmosLib.h

Abstract:

--*/

#ifndef _PLATFORM_CMOS_LIB_H_
#define _PLATFORM_CMOS_LIB_H_

#define CMOS_ATTRIBUTE_EXCLUDE_FROM_CHECKSUM 0x1
#define CMOS_ATTRIBUTE_EXCLUDE_FROM_INIT_DATA 0x2

typedef struct {
  UINT8 CmosAddress;
  UINT8 DefaultValue;
  UINT8 Attribute;
} CMOS_ENTRY;

/**
  Funtion to return platform CMOS entry.

  @param [out]  CmosEntry  Platform CMOS entry.

  @param [out]  CmosEnryCount Number of platform CMOS entry.

  @return Status.
**/
EFI_STATUS
EFIAPI
GetPlatformCmosEntry (
  OUT CMOS_ENTRY  **CmosEntry,
  OUT UINTN       *CmosEnryCount
  );

/**
  Funtion to check if Battery lost or CMOS cleared.

  @reval TRUE  Battery is always present.
  @reval FALSE CMOS is cleared.
**/
BOOLEAN
EFIAPI
CheckCmosBatteryStatus (
  VOID
  );

#endif // _PLATFORM_CMOS_LIB_H_
