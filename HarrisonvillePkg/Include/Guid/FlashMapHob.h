/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
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

  FlashMapHob.h
    
Abstract:

  GUID used for Flash Map HOB entries in the HOB list.

--*/

#ifndef _FLASH_MAP_HOB_GUID_H_
#define _FLASH_MAP_HOB_GUID_H_

//
// Definitions for Flash Map
//
#define EFI_FLASH_MAP_HOB_GUID \
  { 0xb091e7d2, 0x5a0, 0x4198, {0x94, 0xf0, 0x74, 0xb7, 0xb8, 0xc5, 0x54, 0x59} }

extern EFI_GUID gEfiFlashMapHobGuid;

#endif // _FLASH_MAP_HOB_GUID_H_
