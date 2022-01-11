/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

/*++

Copyright (c)  1999 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PowerManagement.c

Abstract:

  Power management driver.
  This SMM driver configures and supports the power management features
  for the platform.

--*/

//
// Statements that include other files
//
#include "PowerManagement.h"

//
// Global variables
//

//
// FVID Table Information (Must be initialized by caller)
//
extern FVID_TABLE                   *mFvidPointer;

//
// PPM Processor support protocol
//
PPM_PROCESSOR_SUPPORT_PROTOCOL_2    *mPpmProcessorSupportProtocol = NULL;

//
// Desired platform policy information
//
PPM_PLATFORM_POLICY_PROTOCOL        mPpmPlatformPolicyProtocol;

//
// Globals used by the reference code
//
UINT16                              mNumberOfStates      = 0;
EFI_GLOBAL_NVS_AREA                 *mGlobalNvsAreaPtr   = NULL;
EFI_MP_SERVICES_PROTOCOL            *mMpServices         = NULL;

EFI_STATUS
EFIAPI
InitializePowerManagement (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
/*++

Routine Description:

  Initialize the power management support.
  This function will do boot time configuration:
    Detect HW capabilities and SW configuration
    Initialize HW and software state (primarily MSR and ACPI tables)

Arguments:
  
  ImageHandle   - Pointer to the loaded image protocol for this driver
  SystemTable   - Pointer to the EFI System Table

Returns:

  EFI_SUCCESS   The driver installes/initialized correctly.
  Driver will ASSERT in debug builds on error.  PPM functionality is considered critical for mobile systems.

--*/
{
  EFI_STATUS                                Status;
  EFI_HANDLE                                Handle;
  EFI_HANDLE                                SwHandle;

  EFI_GLOBAL_NVS_AREA_PROTOCOL              *GlobalNvsAreaProtocol;
  PPM_PLATFORM_POLICY_PROTOCOL              *PpmPlatformPolicyProtocolPointer;
  //
  // GUID Definitions
  //
  EFI_GUID  mEfiGlobalNvsAreaProtocolGuid = EFI_GLOBAL_NVS_AREA_PROTOCOL_GUID;

  Handle              = NULL;
  SwHandle            = NULL;

  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, (VOID **)&mMpServices);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_ERROR , "LocateProtocol - gEfiMpServiceProtocolGuid \n "));
  //
  // Locate our shared data area
  //
  Status = gBS->LocateProtocol (&mEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mGlobalNvsAreaPtr = GlobalNvsAreaProtocol->Area;
  DEBUG ((EFI_D_ERROR , "LocateProtocol - mEfiGlobalNvsAreaProtocolGuid \n "));

  //
  // Locate platform configuration information
  // Then copy it to a global variable that we can utilize during SMM/Runtime
  //
  Status = gBS->LocateProtocol (&gPpmPlatformPolicyProtocolGuid, NULL, (VOID **) &PpmPlatformPolicyProtocolPointer);
  ASSERT_EFI_ERROR (Status);
  CopyMem (&mPpmPlatformPolicyProtocol, PpmPlatformPolicyProtocolPointer, sizeof (PPM_PLATFORM_POLICY_PROTOCOL));
  DEBUG ((EFI_D_ERROR , "LocateProtocol - gPpmPlatformPolicyProtocolGuid \n "));

  //
  // Initialize the PPM processor support protocol
  // This is not currently a publicly exposed protocol, so the
  // library just returns an updated pointer to the protocol
  // rather then using locate protocol services.
  //

  mPpmProcessorSupportProtocol = NULL;
  InitializePowerManagementLib(&mPpmProcessorSupportProtocol);
  ASSERT (mPpmProcessorSupportProtocol != NULL);
  if (mPpmProcessorSupportProtocol == NULL) {
    DEBUG ((EFI_D_ERROR, "\nERROR!!! Attempting to use a NULL pointer.\n"));
    ASSERT_EFI_ERROR (EFI_NOT_READY);
    return EFI_NOT_READY;
  }

  //
  // Initialize PPM code (determine HW and configured state, configure hardware and software accordingly
  //
  InitializePpm (mPpmProcessorSupportProtocol);

  return EFI_SUCCESS;
}
