/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/** @file

Copyright (c) 2011, Intel Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.        

Module Name:
  
    SystemNvDataHobGuid.h
    
Abstract:

  GUIDs used for System Non Volatile HOB entries in the in the HOB list and FV Guids carrying
  the System specific information.

--*/

#ifndef _SYSTEM_NV_DATA_HOB_GUID_H_
#define _SYSTEM_NV_DATA_HOB_GUID_H_

#define EFI_SYSTEM_NV_DATA_HOB_GUID \
  {0xd6e5092d, 0xc7b2, 0x4872, {0xaf, 0x66, 0xfd, 0xc0, 0xe6, 0xf9, 0x5e, 0x78}}

typedef struct {
  EFI_GUID                  SystemNvDataHobGuid;
  EFI_GUID                  SystemNvDataFvGuid;
  EFI_LBA                   StartLba;    
  UINTN                     StartLbaOffset;
  EFI_LBA                   EndLba;    
  UINTN                     EndLbaOffset;
  UINT32                    DataTypeSignature;
} NV_SYSTEM_DATA_GUID_TYPE;

extern EFI_GUID gEfiSystemNvDataHobGuid;

#endif
