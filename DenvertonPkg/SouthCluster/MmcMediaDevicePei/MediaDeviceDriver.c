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

--*/

/*++
Module Name:

  MediaDeviceDriver.c

Abstract:

  UEFI Driver Entry and Binding support.

--*/

#include <MediaDeviceDriverPei.h>

/*++

Routine Description:

  Entry point for EFI drivers.

Arguments:

  ImageHandle - EFI_HANDLE
  SystemTable - EFI_SYSTEM_TABLE

Returns:

  EFI_SUCCESS         Success
  EFI_DEVICE_ERROR    Fail

--*/
EFI_STATUS
EFIAPI
MediaDeviceDriverEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                Status;
  PEI_CARD_DATA             *PeiCardData;
  EFI_PEI_PCI_CFG2_PPI      *PciCfgPpi;
  PEI_SD_CONTROLLER_PPI     *SdControllerPpi;
  UINTN                     Loop;

  Status = PeiServicesLocatePpi (
            &gPeiSdhcPpiGuid,
            0,
            NULL,
            (VOID **) &SdControllerPpi
            );
  
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  PciCfgPpi = (**PeiServices).PciCfg;
  
  Status = SdControllerPpi->DetectCardAndInitHost ( SdControllerPpi);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to DetectCardAndInitHost \n"));
    return Status;
  }

  PeiCardData = (PEI_CARD_DATA*)AllocateZeroPool (sizeof (PEI_CARD_DATA));
  if (PeiCardData == NULL) {
    Status =  EFI_OUT_OF_RESOURCES;
    DEBUG ((EFI_D_ERROR, "Fail to AllocateZeroPool(CARD_DATA) \n"));
    return Status;
  }

  ASSERT (SdControllerPpi->HostCapability.BoundarySize >= 4 * 1024);
  PeiCardData->RawBufferPointer = (UINT8*) AllocatePages (EFI_SIZE_TO_PAGES(SdControllerPpi->HostCapability.BoundarySize * 2));
  if (PeiCardData->RawBufferPointer == NULL) {
    DEBUG ((EFI_D_ERROR, "MediaDeviceDriverBindingStart: Fail to AllocateZeroPool(2*x) \n"));
    FreePool (PeiCardData);
    Status =  EFI_OUT_OF_RESOURCES;
    return Status;
  }
  PeiCardData->AlignedBuffer = PeiCardData->RawBufferPointer - ((UINTN)(PeiCardData->RawBufferPointer) & (SdControllerPpi->HostCapability.BoundarySize - 1)) + SdControllerPpi->HostCapability.BoundarySize;

  PeiCardData->Signature = PEI_CARD_DATA_SIGNATURE;
  PeiCardData->SdControllerPpi  = SdControllerPpi;
  for (Loop = 0; Loop < MAX_NUMBER_OF_PARTITIONS; Loop++) {
    PeiCardData->Partitions[Loop].Signature = PEI_CARD_PARTITION_SIGNATURE;
  }
  Status = MMCSDCardInit ( PeiCardData);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to MMCSDCardInit \n"));
    FreePages ((UINT8*) PeiCardData->RawBufferPointer, EFI_SIZE_TO_PAGES(SdControllerPpi->HostCapability.BoundarySize * 2));
    FreePool (PeiCardData);
    return Status;
  }

  DEBUG ((EFI_D_INFO, "MMC SD card\n"));
  Status = MMCSDBlockIoInit (PeiCardData);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Card BlockIo init failed\n"));
    FreePages ((UINT8*) PeiCardData->RawBufferPointer, EFI_SIZE_TO_PAGES(SdControllerPpi->HostCapability.BoundarySize * 2));
    FreePool (PeiCardData);
    return Status;
  }

  for (Loop = 0; Loop < MAX_NUMBER_OF_PARTITIONS; Loop++) {
    if (PeiCardData->Partitions[Loop].Present) {
      Status = (**PeiServices).InstallPpi (
                                 PeiServices, 
                                 &PeiCardData->Partitions[Loop].BlkIoPpiList
                                 );
    }
  }

  return Status;
}






