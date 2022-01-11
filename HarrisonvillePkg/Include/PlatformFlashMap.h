//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

/**

Copyright (c) 2015 Intel Corporation. All rights reserved This
software and associated documentation (if any) is furnished 
under a license and may only be used or copied in accordance 
with the terms of the license. Except as permitted by such 
license, no part of this software or documentation may be 
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file PlatformFlashMap.h

  Platform specific flash map.

**/

#ifndef _PLATFORM_FLASH_MAP_H_
#define _PLATFORM_FLASH_MAP_H_

#include "FlashMap.h"

#undef FLASH_REGION_MICROCODE_FV_BASE
#undef FLASH_REGION_MICROCODE_FV_SIZE

#define FLASH_REGION_MICROCODE_FV_BASE FixedPcdGet32(PcdFlashMicrocodeFvBase)
#define FLASH_REGION_MICROCODE_FV_SIZE FixedPcdGet32(PcdFlashMicrocodeFvSize)

#define EFI_FLASH_AREA_DATA_DEFINITION \
  /* FVMAIN region */\
  {\
    FLASH_REGION_FVMAIN_BASE,\
    FLASH_REGION_FVMAIN_SIZE,\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_MAIN_BIOS,\
    0, 0, 0,\
    { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },\
  },\
  /* OEM_FV region */\
  {\
    FLASH_REGION_OEM_FV_BASE,\
    FLASH_REGION_OEM_FV_SIZE,\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_MAIN_BIOS,\
    0, 0, 0,\
    { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },\
  },\
  /* MICROCODE_FV region */\
  {\
    FLASH_REGION_MICROCODE_FV_BASE,\
    FLASH_REGION_MICROCODE_FV_SIZE,\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_GUID_DEFINED,\
    0, 0, 0,\
    EFI_SYSTEM_NV_DATA_HOB_GUID, \
  },\
  /* RUNTIME_UPDATABLE region */\
  {\
    FLASH_REGION_RUNTIME_UPDATABLE_BASE,\
    FLASH_REGION_RUNTIME_UPDATABLE_SIZE,\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_GUID_DEFINED,\
    0, 0, 0,\
    EFI_SYSTEM_NV_DATA_HOB_GUID, \
  },\
  /* NV_FTW_SPARE region */\
  {\
    FLASH_REGION_NV_FTW_SPARE_BASE,\
    FLASH_REGION_NV_FTW_SPARE_SIZE,\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_FTW_BACKUP,\
    0, 0, 0,\
    { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },\
  },\
/*#ifdef SPLIT_RECOVERY_FV*/ \
  /* FV_MRC{Normal,Recovery} region: need to be updated according to Normal or Recovery mode, now we make a place holder */\
  {\
    FLASH_REGION_FV_MRC_NORMAL_BASE,\
    FLASH_REGION_FV_MRC_NORMAL_SIZE,\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_RECOVERY_BIOS,\
    0, 0, 0,\
    { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },\
  },\
/*#endif*/ \
  /* FV_SECPEI region */\
  {\
    FLASH_REGION_FV_SECPEI_BASE,\
    FLASH_REGION_FV_SECPEI_SIZE,\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_RECOVERY_BIOS,\
    0, 0, 0,\
    { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },\
  },\


//
// EFI_HOB_FLASH_MAP_ENTRY_TYPE definition
//
#define EFI_HOB_FLASH_MAP_ENTRY_TYPE_DATA_DEFINITION \
  /* RUNTIME_UPDATABLE.NV_VARIABLE_STORE Subregion */\
  {\
    EFI_HOB_TYPE_GUID_EXTENSION,\
    sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE ),\
    0,\
    EFI_FLASH_MAP_HOB_GUID, \
    0, 0, 0,\
    EFI_FLASH_AREA_EFI_VARIABLES,\
    { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },\
    1,\
    {\
      EFI_FLASH_AREA_SUBFV | EFI_FLASH_AREA_MEMMAPPED_FV,\
      0,\
      FLASH_REGION_RUNTIME_UPDATABLE_SUBREGION_NV_VARIABLE_STORE_BASE,\
      FLASH_REGION_RUNTIME_UPDATABLE_SUBREGION_NV_VARIABLE_STORE_SIZE,\
      { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },\
    },\
  }, \
  /* RUNTIME_UPDATABLE.NV_FTW_WORKING Subregion */\
  {\
    EFI_HOB_TYPE_GUID_EXTENSION,\
    sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE ),\
    0,\
    EFI_FLASH_MAP_HOB_GUID, \
    0, 0, 0,\
    EFI_FLASH_AREA_FTW_STATE,\
    { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },\
    1,\
    {\
      EFI_FLASH_AREA_SUBFV | EFI_FLASH_AREA_MEMMAPPED_FV,\
      0,\
      FLASH_REGION_RUNTIME_UPDATABLE_SUBREGION_NV_FTW_WORKING_BASE,\
      FLASH_REGION_RUNTIME_UPDATABLE_SUBREGION_NV_FTW_WORKING_SIZE,\
      { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },\
    },\
  }, \
  /* NVSTORAGE.NV_FTW_SPARE Subregion */\
  {\
    EFI_HOB_TYPE_GUID_EXTENSION,\
    sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE ),\
    0,\
    EFI_FLASH_MAP_HOB_GUID, \
    0, 0, 0,\
    EFI_FLASH_AREA_FTW_BACKUP,\
    { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },\
    1,\
    {\
      EFI_FLASH_AREA_SUBFV | EFI_FLASH_AREA_MEMMAPPED_FV,\
      0,\
      FLASH_REGION_NV_FTW_SPARE_BASE,\
      FLASH_REGION_NV_FTW_SPARE_SIZE,\
      { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },\
    },\
  }
#endif // #ifndef _PLATFORM_FLASH_MAP_H_

