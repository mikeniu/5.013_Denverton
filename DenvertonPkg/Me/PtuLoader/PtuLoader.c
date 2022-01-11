/*++

  Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:

  PtuLoader.c

Abstract:

Load PTU Option ROM

--*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PciLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/Spi.h>
#include <Protocol/PciIo.h>
#include <IndustryStandard/Pci22.h>
#include <Include/MeChipset.h>
#include <Sps.h>

EFI_STATUS
  EFIAPI
PtuLoaderSupported (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     Controller,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
    )
{
  SPS_NMFS                                    NmFs;

  NmFs.UInt32 = PciRead32 (PCI_LIB_ADDRESS (ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER, SPS_REG_NMFS));

  if (NmFs.UInt32 == 0xFFFFFFFF || !NmFs.Bits.NmEnabled || !NmFs.Bits.DirectPtuExecution)
  {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
  EFIAPI
PtuLoaderStop (
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN  EFI_HANDLE                            ControllerHandle,
  IN  UINTN                                 NumberOfChildren,
  IN  EFI_HANDLE                            *ChildHandleBuffer OPTIONAL
    )
{
  return EFI_SUCCESS;
}

EFI_STATUS
  EFIAPI
PtuLoaderStart (
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath OPTIONAL
    )
{
  EFI_STATUS                                  Status;
  UINT32                                      ImageSize;
  PCH_SPI_PROTOCOL                            *SpiProtocol;
  EFI_PCI_EXPANSION_ROM_HEADER                PtuRomHeader;
  MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH     EfiOpRomImageNode;
  EFI_DEVICE_PATH_PROTOCOL                    *PciOptionRomImageDevicePath;
  PCI_DATA_STRUCTURE                          Pcir;
  VOID                                        *PtuImage = NULL;
  UINT16                                      ImageOffset;
  UINT32                                      InitializationSize;
  UINT32                                      ImageLength;
  EFI_HANDLE         ImageHandle;

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gPchSpiProtocolGuid,
                  (VOID **) &SpiProtocol,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    //
    // EFI_ALREADY_STARTED is also an error
    //
    return Status;
  }

  DEBUG((EFI_D_INFO, "[PtuLoader] SPI read PCI expansion ROM header\n"));
  Status = SpiProtocol->FlashRead (
      SpiProtocol,
      FlashRegionDer,
      0,
      sizeof(EFI_PCI_EXPANSION_ROM_HEADER),
      (UINT8 *) &PtuRomHeader
      );

  if (EFI_ERROR (Status))
  {
    DEBUG((EFI_D_ERROR, "[PtuLoader] SPI read PCI expansion ROM header filed: %r\n", Status));
    return Status;
  }

  if (PtuRomHeader.Signature != PCI_EXPANSION_ROM_HEADER_SIGNATURE)
  {
    DEBUG((EFI_D_ERROR, "[PtuLoader] Pci expansion ROM signature not found.\n"));
    return EFI_NOT_FOUND;
  }

  DEBUG((EFI_D_INFO, "[PtuLoader] SPI read PCI data structure\n"));
  Status = SpiProtocol->FlashRead (
      SpiProtocol,
      FlashRegionDer,
      PtuRomHeader.PcirOffset,
      sizeof(PCI_DATA_STRUCTURE),
      (UINT8 *) &Pcir
      );

  if (Pcir.Signature != PCI_DATA_STRUCTURE_SIGNATURE)
  {
    DEBUG((EFI_D_ERROR, "[PtuLoader] PCI data signature not found in DER region\n"));
    return EFI_NOT_FOUND;
  }

  ImageSize   = (UINT32) (Pcir.ImageLength * 512);
  EfiOpRomImageNode.Header.Type     = MEDIA_DEVICE_PATH;
  EfiOpRomImageNode.Header.SubType  = MEDIA_RELATIVE_OFFSET_RANGE_DP;
  SetDevicePathNodeLength (&EfiOpRomImageNode.Header, sizeof (EfiOpRomImageNode));

  EfiOpRomImageNode.StartingOffset  = 0x0;
  EfiOpRomImageNode.EndingOffset    = ImageSize - 1;

  PciOptionRomImageDevicePath = AppendDevicePathNode (NULL, NULL);
  PciOptionRomImageDevicePath = AppendDevicePathNode (PciOptionRomImageDevicePath, &EfiOpRomImageNode.Header);

  if (!PciOptionRomImageDevicePath)
  {
    DEBUG((EFI_D_ERROR, "[PtuLoader] Unable to create PCI OpRom image device path\n"));
    Status = EFI_NOT_FOUND;
    goto FreeDevicePath;
  }

  ImageOffset             = PtuRomHeader.EfiImageHeaderOffset;
  InitializationSize      = PtuRomHeader.InitializationSize * 512;
  ImageLength             = InitializationSize - ImageOffset;

  Status = gBS->AllocatePool(
      EfiBootServicesData,
      ImageLength,
      &PtuImage
      );

  if (EFI_ERROR (Status))
  {
    DEBUG((EFI_D_ERROR, "[PtuLoader] AllocatePool for PtuImage failed: %r\n", Status));
    goto FreeDevicePath;
  }

  DEBUG((EFI_D_INFO, "[PtuLoader] SPI read PtuImage\n"));
  Status = SpiProtocol->FlashRead (
      SpiProtocol,
      FlashRegionDer,
      ImageOffset,
      ImageSize,
      (UINT8 *) PtuImage
      );

  if (EFI_ERROR (Status))
  {
    DEBUG((EFI_D_ERROR, "[PtuLoader] SPI read PtuImage filed: %r\n", Status));
    goto FreeImage;
  }

  ImageHandle = NULL;
  Status = gBS->LoadImage (
      FALSE,
      gImageHandle,
      PciOptionRomImageDevicePath,
      PtuImage,
      ImageLength,
      &ImageHandle
      );

  if (EFI_ERROR (Status))
  {
    DEBUG((EFI_D_ERROR, "[PtuLoader] LoadImage failed: %r\n", Status));
    goto FreeImage;
  }
  else
  {
    DEBUG((EFI_D_ERROR, "[PtuLoader] LoadImage success!!!\n"));
  }

  Status = gBS->StartImage (ImageHandle, NULL, NULL);
  if (EFI_ERROR (Status))
  {
    DEBUG((EFI_D_ERROR, "[PtuLoader] StartImage failed: %r\n", Status));
    goto FreeImage;
  }
  else
  {
    DEBUG((EFI_D_ERROR, "[PtuLoader] StartImage success!!!\n"));
  }

  return EFI_SUCCESS;

FreeImage:
  gBS->FreePool(PtuImage);
FreeDevicePath:
  gBS->FreePool(PciOptionRomImageDevicePath);
  return Status;
}

EFI_DRIVER_BINDING_PROTOCOL gPtuLoaderDriverBinding = {
  PtuLoaderSupported,
  PtuLoaderStart,
  PtuLoaderStop,
  0x10,
  NULL,
  NULL
};

  EFI_STATUS
PtuLoaderEntryPoint (
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
    )
{
  EFI_STATUS                                  Status;

  DEBUG((EFI_D_ERROR, "[PtuLoader] PtuLoaderEntryPoint\n"));

  Status = EfiLibInstallDriverBinding (
      ImageHandle,
      SystemTable,
      &gPtuLoaderDriverBinding,
      ImageHandle
      );

  if (EFI_ERROR (Status))
  {
    DEBUG((EFI_D_ERROR, "[PtuLoader] Unable to install gPtuLoaderDriverBinding\n"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  return EFI_SUCCESS;
}
