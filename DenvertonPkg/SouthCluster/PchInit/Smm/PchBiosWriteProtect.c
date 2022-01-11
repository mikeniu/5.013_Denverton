/** @file
  PCH BIOS Write Protect Driver.

@copyright
 Copyright (c) 2011 - 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:

**/
#include "PchInitSmm.h"

///
/// Global variables
///
GLOBAL_REMOVE_IF_UNREFERENCED PCH_TCO_SMI_DISPATCH_PROTOCOL     *mPchTcoSmiDispatchProtocol;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                             mSpiRegBase;

/**
  This hardware SMI handler will be run every time the BIOS Write Enable bit is set.

  @param[in] DispatchHandle       Not used

**/
VOID
EFIAPI
PchSpiBiosWpCallback (
  IN  EFI_HANDLE                              DispatchHandle
  )
{
  ///
  /// Disable BIOSWE bit to protect BIOS
  ///
  MmioAnd8 ((UINTN) (mSpiRegBase + R_PCH_SPI_BC), (UINT8) ~B_PCH_SPI_BC_WPD);
}

/**
  Entry point for Pch Bios Write Protect driver.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval EFI_SUCCESS             Initialization complete.
**/
EFI_STATUS
EFIAPI
InstallPchBiosWriteProtect (
  IN EFI_HANDLE            ImageHandle,
  IN EFI_SYSTEM_TABLE      *SystemTable
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              Handle;

  DEBUG ((DEBUG_INFO, "InstallPchBiosWriteProtect()\n"));

  if (mPchPolicyHob->LockDownConfig.BiosLock != TRUE) {
    return EFI_SUCCESS;
  }

  mSpiRegBase = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_SPI,
                      PCI_FUNCTION_NUMBER_PCH_SPI
                    );
  
  DEBUG ((DEBUG_INFO, "Installing BIOS Write Protect SMI handler\n"));

  ///
  /// Get the PCH TCO SMM dispatch protocol
  ///
  mPchTcoSmiDispatchProtocol = NULL;
  Status = gSmst->SmmLocateProtocol (&gPchTcoSmiDispatchProtocolGuid, NULL, (VOID **) &mPchTcoSmiDispatchProtocol);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Register an SpiBiosWp callback function to handle TCO BIOSWR SMI
  ///
  Handle = NULL;
  Status = mPchTcoSmiDispatchProtocol->SpiBiosWpRegister (
                                         mPchTcoSmiDispatchProtocol,
                                         PchSpiBiosWpCallback,
                                         &Handle
                                         );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

