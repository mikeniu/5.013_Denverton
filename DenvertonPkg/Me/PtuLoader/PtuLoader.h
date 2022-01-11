/*++

  Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
*/

#ifndef _PTU_LOADER_H_
#define _PTU_LOADER_H_

#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
// #include <Protocol/PciIo.h>
#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>
#include <IndustryStandard/Pci22.h>
#include <Uefi.h>
#include <Protocol/DriverBinding.h>
#include <Library/PciLib.h>
#include <Sps.h>
#include <Include/MeChipset.h>
#include <Protocol/Spi.h>

extern EFI_DRIVER_BINDING_PROTOCOL  gPtuLoaderDriverBinding;

EFI_STATUS
EFIAPI
PtuLoaderSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
PtuLoaderStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
PtuLoaderStop (
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN  EFI_HANDLE                            ControllerHandle,
  IN  UINTN                                 NumberOfChildren,
  IN  EFI_HANDLE                            *ChildHandleBuffer OPTIONAL
  );

#endif
