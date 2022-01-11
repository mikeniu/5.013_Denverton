/** @file
  This file is a library for IQAT IP DXE specific Initalization.

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
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

#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiSpec.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Guid/EventGroup.h>
#include <Protocol/PciEnumerationComplete.h>
#include <DxeIqatIpInitLib.h>
#include <IqatIpInitLib.h>

/**
  This function handles DXE Iqat Init task at PCI ENUMERATION COMPLETE.

  @param[in]  Event       A pointer to the Event that triggered the callback.
  @param[in]  *Context    A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
DxeIqatInitOnPciEnumComplete (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS          Status;
  VOID                *ProtocolPointer;

  ///
  /// Check if this is first time called by EfiCreateProtocolNotifyEvent() or not,
  /// if it is, we will skip it until real event is triggered
  ///
  Status = gBS->LocateProtocol (&gEfiPciEnumerationCompleteProtocolGuid, NULL, (VOID **) &ProtocolPointer);
  if (EFI_SUCCESS != Status) {
     return;
  }

  gBS->CloseEvent (Event);

  //
  // IQAT Initialization
  //
  IqatIpInit();
}

/**
  Initialize IQAT IP in DXE

  Arguments:
  NONE

  Returns:
  @retval EFI_SUCCESS - The function completed successfully.
  @retval Others 	- Error
**/
EFI_STATUS
DxeIqatIpInit (
  VOID
)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  VOID        *Registration;

  DEBUG ((DEBUG_INFO, "DxeIqatIpInit Start\n"));

  ///
  /// Performing DxeIqatInitOnPciEnumComplete after the gEfiPciEnumerationCompleteProtocolGuid is signaled.
  ///
  EfiCreateProtocolNotifyEvent (
    &gEfiPciEnumerationCompleteProtocolGuid,
    TPL_CALLBACK,
    DxeIqatInitOnPciEnumComplete,
    NULL,
    &Registration
    );

  DEBUG ((DEBUG_INFO, "DxeIqatIpInit End\n"));

  return Status;
}
