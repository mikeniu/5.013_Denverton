/** @file
  Source code file for the Silicon Init DXE module

@copyright
  Copyright (c) 2015-2016 Intel Corporation. All rights reserved
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

@par Specification
**/

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/EventGroup.h>
#include <DxePcieIpInitLib.h>
#include <DxeIqatIpInitLib.h>

#include <Library/MmPciLib.h>
#include <Library/IoLib.h>

EFI_EVENT                         EndOfDxeEvent;

/**
  This function handles SiInit task at the end of DXE

  @param[in]  Event       A pointer to the Event that triggered the callback.
  @param[in]  *Context    A pointer to private data registered with the callback function.
**/
VOID
SiInitEndOfDxe (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{

  gBS->CloseEvent (Event);

}


/**
  Entry point for the driver.

  @param[in]  ImageHandle  Image Handle.
  @param[in]  SystemTable  EFI System Table.

  @retval     EFI_SUCCESS  Function has completed successfully.
  @retval     Others       All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SiInitDxe (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{

  UINT8  Data8;
  UINT16 Data16;
  UINT32 Data32;
  UINTN  Address;

  EFI_STATUS    Status;

#if 0
  Status = SmbiosMemory ();
  ASSERT_EFI_ERROR (Status);
#endif

  ///
  /// Performing SiInitEndOfDxe after the gEfiEndOfDxeEventGroup is signaled.
  ///
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  SiInitEndOfDxe,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);


  //
  // Clean UR's status and mask the UREM on RCEC before PCIe enumeration
  // to disable error logging for these common found errors during enumeration.
  //
  DEBUG ((DEBUG_INFO, "Disabling RCEC error reporting before PCIe Enum...\n" ));

  Address = MmPciBase (RCEC_BUS, RCEC_DEVICE, RCEC_FUNC)  + R_RCEC_DEVSTS;
  Data8   = (MmioRead8 (Address) & ~(B_RCEC_DEVSTS_CED | B_RCEC_DEVSTS_NFED |
                                    B_RCEC_DEVSTS_FED | B_RCEC_DEVSTS_URD));
  MmioWrite8 (Address, Data8);

  Address = MmPciBase (RCEC_BUS, RCEC_DEVICE, RCEC_FUNC) +  R_RCEC_ERRUNCSTS;
  Data16  = (MmioRead16 (Address) & ~B_RCEC_ERRUNCSTS_URE);
  MmioWrite16(Address, Data16);

  Address = MmPciBase (RCEC_BUS, RCEC_DEVICE, RCEC_FUNC) +  R_RCEC_ERRCORSTS;
  Data16  = (MmioRead16 (Address) & ~B_RCEC_ERRCORSTS_ANFE);
  MmioWrite16 (Address, Data16);

  Address = MmPciBase (RCEC_BUS, RCEC_DEVICE, RCEC_FUNC) +  R_RCEC_ERRUNCMSK;
  Data32  = (MmioRead32 (Address) | B_RCEC_ERRUNCMSK_UREM);
  MmioWrite32 (Address, Data32);

  DEBUG ((DEBUG_INFO, "PCIe DXE begin!!\n" ));
  // Denverton AptioV override Start
  // The below function helps to program PCI Express capabilities. 
  // Since this will be done by AMI PCI driver, following function can be removed
  /*
  Status= DxePcieIpInit();
  ASSERT_EFI_ERROR (Status);
  */
  //Denverton AptioV override End
  Status= DxeIqatIpInit();
  ASSERT_EFI_ERROR (Status);


  return Status;
}
