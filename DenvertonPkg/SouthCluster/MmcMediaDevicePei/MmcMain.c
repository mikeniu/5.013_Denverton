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

#include "SdHostDriver.h"
#include <MediaDeviceDriverPei.h>
#include <Ppi/PchPolicy.h>
#include "MmioLib.h"

UINT32 mSdBaseAddress;

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
MmcMainEntryPoint(
                 IN EFI_PEI_FILE_HANDLE FileHandle, 
                 IN CONST EFI_PEI_SERVICES **PeiServices)
{
  EFI_STATUS Status; 
  PCH_POLICY_PPI  *PchPolicy;

  ///
  /// Retrieve all instances of PCH Platform Policy protocol
  ///

  Status = PeiServicesLocatePpi (
	             &gPchPolicyPpiGuid,
	             0,
	             NULL,
	             (VOID **) &PchPolicy
	             );

  ASSERT_EFI_ERROR (Status);

    // check if device preset
  if (MmPci32(0,PCI_BUS_NUMBER_PCH_SCC_EMMC,PCI_DEVICE_NUMBER_PCH_SCC_SDIO_0,PCI_FUNCTION_NUMBER_PCH_SCC_SDIO,R_PCH_SCC_SDIO_DEVVENDID) == 0xFFFFFFFF){
    PchPolicy->eMMCEnabled = FALSE;
  }
  if (PchPolicy->eMMCEnabled == FALSE) {
    DEBUG((EFI_D_ERROR, "eMMC Disabled by Policy \n"));
    return EFI_SUCCESS;
  }
  Status = (*PeiServices)->RegisterForShadow (FileHandle);
  if (Status == EFI_ALREADY_STARTED) {
    mSdBaseAddress = (UINT32)PcdGet32(PcdEmmcBaseAddress0);

    Status = SdHostDriverEntryPoint(FileHandle, PeiServices);
    if (EFI_ERROR(Status)){
      DEBUG((EFI_D_ERROR, "Fail to Init SD Host controller \n"));
      return Status;
    }
    Status = MediaDeviceDriverEntryPoint(FileHandle, PeiServices);
    if (EFI_ERROR(Status)){
      DEBUG((EFI_D_ERROR, "Fail to Init eMMC Card \n"));
      return Status;
    }
  } else if (Status == EFI_NOT_FOUND) {
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}
