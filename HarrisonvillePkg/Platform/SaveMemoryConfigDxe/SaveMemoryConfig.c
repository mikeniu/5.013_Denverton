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

  SaveMemoryConfig.c

Abstract:
  This is the driver that locates the MemoryConfigurationData HOB, if it
  exists, and saves the data to nvRAM.

Revision History

--*/

#include "SaveMemoryConfig.h"

#define DUNIT_PORT       0x10
//#define SSKPD1_DUNIT_COMMON_REG 0x00020049
#define S5_PATH          BIT2
CHAR16    EfiMemoryConfigVariable[] = L"MemoryConfig";

EFI_STATUS
EFIAPI
SaveMemoryConfigEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
)
/*++

  Routine Description:
    This is the standard EFI driver point that detects whether there is a
    MemoryConfigurationData HOB and, if so, saves its data to nvRAM.

  Arguments:
    ImageHandle   - Handle for the image of this driver
    SystemTable   - Pointer to the EFI System Table

  Returns:
    EFI_SUCCESS   - if the data is successfully saved or there was no data
    EFI_NOT_FOUND - if the HOB list could not be located.
    EFI_UNLOAD_IMAGE - It is not success

--*/
{
  EFI_STATUS        Status;
  VOID              *HobList;
  EFI_HOB_GUID_TYPE *GuidHob;
  VOID              *HobData;
  VOID              *VariableData;
  UINTN             DataSize;
  UINTN             BufferSize;
  UINTN             CompareResult;
  MRC_PARAMETERS    *MrcParameters;

  DataSize      = 0;
  CompareResult = 0;
  VariableData  = NULL;
  GuidHob       = NULL;
  HobList       = NULL;
  HobData       = NULL;

  //
  // Get the HOB list.  If it is not present, then ASSERT.
  //
  HobList = GetHobList ();
  ASSERT (HobList != NULL);

  //
  // Search for the Memory Configuration GUID HOB.  If it is not present, then
  // there's nothing we can do. It may not exist on the update path.
  //
  GuidHob = GetNextGuidHob (&gEfiMemoryConfigDataGuid, HobList);
  if (GuidHob != NULL) {
    HobData = GET_GUID_HOB_DATA (GuidHob);
    MrcParameters = (MRC_PARAMETERS *) HobData;
    DEBUG ((EFI_D_ERROR, "BootModeExecutedinMrc: %d\n", MrcParameters->BootMode));
    DEBUG ((EFI_D_ERROR, "MrcParamsValidFlag;%d\n", MrcParameters->SaveRestore.MrcParamsValidFlag));    
    DEBUG ((EFI_D_ERROR, "CurrentFrequency;%d\n", MrcParameters->DdrFrequency));
    DEBUG ((EFI_D_ERROR, "SystemMemorySize;%d\n", MrcParameters->SystemMemorySize));

    DataSize = sizeof(MRC_PARAMETERS);
    //
    // Use the HOB to save Memory Configuration Data
    //
    BufferSize = DataSize;
    DEBUG ((EFI_D_ERROR, "BufferSize: 0x%04x\n", BufferSize));
    VariableData = AllocatePool (BufferSize);
    if (VariableData == NULL) {
      return EFI_SUCCESS;
    }
    Status = gRT->GetVariable (
               EfiMemoryConfigVariable,
               &gEfiMemoryConfigDataGuid,
               NULL,
               &BufferSize,
               VariableData
             );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      (gBS->FreePool) (VariableData);
      VariableData = AllocatePool (BufferSize);
      if (VariableData == NULL) {
        return EFI_SUCCESS;
      }

      Status = gRT->GetVariable (
                 EfiMemoryConfigVariable,
                 &gEfiMemoryConfigDataGuid,
                 NULL,
                 &BufferSize,
                 VariableData
               );
    }
    DEBUG ((EFI_D_ERROR, "BufferSize:DataSize 0x%04x:0x%04x\n", BufferSize, DataSize));
    CompareResult = CompareMem (HobData, VariableData, DataSize);
    DEBUG ((EFI_D_ERROR, "Compare: %d\n", CompareResult));
    if (((EFI_ERROR (Status)) || BufferSize != DataSize || 0 != CompareMem (HobData, VariableData, DataSize)) && (MrcParameters->BootMode == S5)) {
      DEBUG ((EFI_D_ERROR, "SetVariable\n"));
      Status = gRT->SetVariable (
                 EfiMemoryConfigVariable,
                 &gEfiMemoryConfigDataGuid,
                 (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS),
                 DataSize,
                 HobData
               );
      DEBUG ((EFI_D_ERROR, "Status:%d\n", Status));
      ASSERT ((Status == EFI_SUCCESS) || (Status == EFI_OUT_OF_RESOURCES) );
      if (Status == EFI_OUT_OF_RESOURCES) {
        DEBUG ((EFI_D_ERROR, "EFI_OUT_OF_RESOURCES\n"));
        //
        // We have to delete the Var first, since there's no space for second write
        //
        Status = gRT->SetVariable (
                   EfiMemoryConfigVariable,
                   &gEfiMemoryConfigDataGuid,
                   (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS),
                   0,
                   NULL
                 );

        ASSERT_EFI_ERROR (Status);

        Status = gRT->SetVariable (
                   EfiMemoryConfigVariable,
                   &gEfiMemoryConfigDataGuid,
                   (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS),
                   DataSize,
                   HobData
                 );

        ASSERT_EFI_ERROR (Status);

      }


      DEBUG ( (EFI_D_ERROR, "Restored Size is 0x%x\n", DataSize) );
    }
    (gBS->FreePool) (VariableData);
  }
  //
  // This driver does not produce any protocol services, so always unload it.
  //
  return EFI_SUCCESS;
}
