/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FlashMap.c
   
Abstract:

  Build GUIDed HOBs for platform specific flash map.

--*/



#include "PlatformStage1.h"
#include "PlatformFlashMap.h"

EFI_GUID  mFvBlockGuid    = EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL_GUID;
EFI_GUID  mFfsGuid        = EFI_FIRMWARE_FILE_SYSTEM_GUID;

static EFI_FLASH_AREA_DATA          mFlashAreaData[]  = { EFI_FLASH_AREA_DATA_DEFINITION };
//
// htao to PASS build here //bugbug
//

#define NUM_FLASH_AREA_DATA (sizeof (mFlashAreaData) / sizeof (mFlashAreaData[0]))

static EFI_HOB_FLASH_MAP_ENTRY_TYPE mFlashMapHobData[] = { EFI_HOB_FLASH_MAP_ENTRY_TYPE_DATA_DEFINITION };

#define NUM_HOB_FLASH_MAP_DATA  (sizeof (mFlashMapHobData) / sizeof (mFlashMapHobData[0]))

EFI_STATUS
PeimInitializeFlashMap (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
/*++

Routine Description:

  Build GUID HOBs for platform specific flash map.
  
Arguments:

  FfsHeader     Pointer this FFS file header.
  PeiServices   General purpose services available to every PEIM.
    
Returns:

  EFI_SUCCESS   Guid HOBs for platform flash map is built.
  Otherwise     Failed to build the Guid HOB data.

--*/
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  EFI_FLASH_AREA_HOB_DATA       FlashHobData;
  EFI_HOB_FLASH_MAP_ENTRY_TYPE  *Hob;

  //
  // Build flash area entries as GUIDed HOBs.
  //
  for (Index = 0; Index < NUM_FLASH_AREA_DATA; Index++) {
    SetMem (&FlashHobData, sizeof (EFI_FLASH_AREA_HOB_DATA), 0);

    FlashHobData.AreaType               = mFlashAreaData[Index].AreaType;
    FlashHobData.NumberOfEntries        = 1;
    FlashHobData.SubAreaData.Attributes = mFlashAreaData[Index].Attributes;
    FlashHobData.SubAreaData.Base       = (EFI_PHYSICAL_ADDRESS) (UINTN) mFlashAreaData[Index].Base;
    FlashHobData.SubAreaData.Length     = (EFI_PHYSICAL_ADDRESS) (UINTN) mFlashAreaData[Index].Length;

    switch (FlashHobData.AreaType) {
    case EFI_FLASH_AREA_RECOVERY_BIOS:
    case EFI_FLASH_AREA_MAIN_BIOS:
      CopyMem (
                        &FlashHobData.AreaTypeGuid,
                        &mFfsGuid,
                        sizeof (EFI_GUID)
                        );
      CopyMem (
                        &FlashHobData.SubAreaData.FileSystem,
                        &mFvBlockGuid,
                        sizeof (EFI_GUID)
                        );
      break;

    case EFI_FLASH_AREA_GUID_DEFINED:
      CopyMem (
                        &FlashHobData.AreaTypeGuid,
                        &gEfiSystemNvDataHobGuid,
                        sizeof (EFI_GUID)
                        );
      CopyMem (
                        &FlashHobData.SubAreaData.FileSystem,
                        &mFvBlockGuid,
                        sizeof (EFI_GUID)
                        );
      break;

    default:
      break;
    }

    Hob = BuildGuidDataHob (
            &gEfiFlashMapHobGuid,
            &FlashHobData,
            sizeof (EFI_FLASH_AREA_HOB_DATA)
            );
    ASSERT (Hob);
  }

  for (Index = 0; Index < NUM_HOB_FLASH_MAP_DATA; Index++) {
    Status = PeiServicesCreateHob (
                              EFI_HOB_TYPE_GUID_EXTENSION,
                              (UINT16) (sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE)),
                              &Hob
                              );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    CopyMem (
                      Hob,
                      &mFlashMapHobData[Index],
                      sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE)
                      );
    if (mFlashMapHobData[Index].AreaType == EFI_FLASH_AREA_EFI_VARIABLES) {
      Hob->Entries[0].Base    = Hob->Entries[0].Base + EFI_RUNTIME_UPDATABLE_FV_HEADER_LENGTH;
      Hob->Entries[0].Length  = Hob->Entries[0].Length - EFI_RUNTIME_UPDATABLE_FV_HEADER_LENGTH;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetAreaInfo (
  IN  CONST EFI_PEI_SERVICES      **PeiServices,
  IN  EFI_FLASH_AREA_TYPE         AreaType,
  IN  EFI_GUID                    *AreaTypeGuid,
  OUT UINT32                      *NumEntries,
  OUT EFI_FLASH_SUBAREA_ENTRY     **Entries
  )
/*++

Routine Description:

  Get data from the platform specific flash area map.
  
Arguments:

  PeiServices   General purpose services available to every PEIM.
  AreaType      Flash map area type.
  AreaTypeGuid  Guid for the flash map area type.
  NumEntries    On return, filled with the number of sub-areas with the same type.
  Entries       On return, filled with entry pointer to the sub-areas.
    
Returns:

  EFI_SUCCESS   The type of area exists in the flash map and data is returned.
  EFI_NOT_FOUND The type of area does not exist in the flash map.

--*/
{
  EFI_STATUS                    Status;
  EFI_PEI_HOB_POINTERS          Hob;
  EFI_HOB_FLASH_MAP_ENTRY_TYPE  *FlashMapEntry;

  Status = (*PeiServices)->GetHobList (PeiServices, &Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if ((Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION) &&
        (CompareGuid (&Hob.Guid->Name, &gEfiFlashMapHobGuid))) {
      FlashMapEntry = (EFI_HOB_FLASH_MAP_ENTRY_TYPE *)Hob.Raw;
      if ((AreaType != FlashMapEntry->AreaType) ||
          ((AreaType == EFI_FLASH_AREA_GUID_DEFINED) && 
          !CompareGuid (AreaTypeGuid, &FlashMapEntry->AreaTypeGuid))) {
        Hob.Raw = GET_NEXT_HOB (Hob);    
        continue;
      }
      *NumEntries = FlashMapEntry->NumEntries;
      *Entries = FlashMapEntry->Entries;
      return EFI_SUCCESS;
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }
 
  return EFI_NOT_FOUND;
}
