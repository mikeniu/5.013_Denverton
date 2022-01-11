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

  MemoryRange.c

Abstract:

  PEIM to provide the platform support functionality on Sonoma.
  This file implements the Platform Memory Range PPI

--*/

#include "PlatformStage2.h"
#include <Ppi/ReadOnlyVariable2.h>
#include <Guid/MemoryTypeInformation.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>

#pragma warning (push)
#pragma warning (disable : 4090)

//
// Memory Reserved should be between 125% to 150% of the Current required memory
// otherwise BdsMisc.c would do a reset to make it 125% to avoid s4 resume issues.
//
EFI_MEMORY_TYPE_INFORMATION mDefaultMemoryTypeInformation[] = {
  { EfiACPIReclaimMemory,   0x100  },     // ASL
  { EfiACPIMemoryNVS,       0x1800 },     // ACPI NVS (including S3 related)
  { EfiReservedMemoryType,  0x900  },     // BIOS Reserved (including S3 related)
  { EfiRuntimeServicesCode, 0x100  },     // Runtime Service Code
  { EfiRuntimeServicesData, 0x100  },     // Runtime Service Data
  { EfiMaxMemoryType,       0 }
};

EFI_STATUS
EFIAPI
ChoosePlatformMemoryRange (
  IN CONST EFI_PEI_SERVICES               **PeiServices,
  IN PEI_PLATFORM_MEMORY_RANGE_PPI        *This,
  IN OUT PEI_MEMORY_RANGE_OPTION_ROM      *OptionRomMask,
  IN OUT PEI_MEMORY_RANGE_SMRAM           *SmramMask,
  IN OUT PEI_MEMORY_RANGE_GRAPHICS_MEMORY *GraphicsMemoryMask,
  IN OUT PEI_MEMORY_RANGE_PCI_MEMORY      *PciMemoryMask
  )
/*++

Routine Description:

  Fill in bit masks to specify reserved memory ranges.

Arguments:

  PeiServices         - General purpose services available to every PEIM.
  This                - Pointer to the PEI_PLATFORM_MEMORY_RANGE_PPI instance.
  OptionRomMask       - Reserved memory ranges for Legacy Option ROM.
  SmramMask           - Reserved memory ranges for SMRAM.
  GraphicsMemoryMask  - Reserved memory ranges for Graphic Memory.
  PciMemoryMask       - Reserved memory ranges for PCI.

Returns:

  EFI_SUCCESS - Operation success.

--*/
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetPlatformMemorySize (
  IN CONST EFI_PEI_SERVICES                       **PeiServices,
  IN      PEI_PLATFORM_MEMORY_SIZE_PPI            *This,
  IN OUT  UINT64                                  *MemorySize
  )
/*++

Routine Description:

  This function calculates the minimum platform memory size required
  in order to successfully pass control into DXE.

Arguments:

  PeiServices - General purpose services available to every PEIM.
  This        - PEI_PLATFORM_MEMORY_SIZE_PPI instance.
  MemorySize  - Will be updated with the memory size required.

Returns:

  EFI_SUCCESS - Operation success, the size required is returned in "MemorySize".
  Other       - There are errors when tring to calculate the required memory size.

--*/
{
  EFI_STATUS                            Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *Variable;
  UINTN                                 DataSize;
  EFI_MEMORY_TYPE_INFORMATION           MemoryData [EfiMaxMemoryType + 1];
  UINTN                                 Index;
  EFI_BOOT_MODE                         BootMode;

  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  ASSERT_EFI_ERROR (Status);

  DataSize = sizeof (MemoryData);
  Status = (*PeiServices)->LocatePpi (
             PeiServices,
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             &Variable
             );
  ASSERT_EFI_ERROR (Status);
  Status = Variable->GetVariable (
                       Variable,
                       EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                       &gEfiMemoryTypeInformationGuid,
                       NULL,
                       &DataSize,
                       &MemoryData
                       );

  //
  // Accumulate maximum amount of memory needed
  //
  if (EFI_ERROR (Status) || ( BootMode == BOOT_ON_FLASH_UPDATE )) {
    if ( BootMode == BOOT_IN_RECOVERY_MODE ) {
      *MemorySize = PEI_RECOVERY_MIN_MEMORY_SIZE;
      return EFI_SUCCESS;
    }

    //
    // Use default value to avoid memory fragment. OS boot/installation fails
    // if there is not enough continuous memory available
    //
    *MemorySize = PEI_MIN_MEMORY_SIZE;
    DataSize = sizeof (mDefaultMemoryTypeInformation);
    CopyMem (MemoryData, mDefaultMemoryTypeInformation, DataSize);
    Status = EFI_SUCCESS;
  } else {
    //
    // Start with at least PEI_MIN_MEMORY_SIZE of memory for the DXE Core and the DXE Stack
    //
    *MemorySize = PEI_MIN_MEMORY_SIZE;
  }

  for (Index = 0; Index < DataSize / sizeof (EFI_MEMORY_TYPE_INFORMATION); Index++) {
    *MemorySize += MemoryData[Index].NumberOfPages * EFI_PAGE_SIZE;
  }

  //
  // Build the GUID'd HOB for DXE
  //
  BuildGuidDataHob (
    &gEfiMemoryTypeInformationGuid,
    MemoryData,
    DataSize
    );

  return Status;
}

#pragma warning (pop)
