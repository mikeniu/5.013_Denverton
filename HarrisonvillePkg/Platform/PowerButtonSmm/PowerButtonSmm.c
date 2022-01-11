/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PowerButtonSmm.c

Abstract:

  PowerButton SMM Driver.

**/

#include "PowerButtonSmm.h"
// Denverton Aptiov Override Start - EIP#271764
#include "AmiDxeLib.h"
// Denverton Aptiov Override End - EIP#271764

EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT mRegisterContext = {EfiPowerButtonEntry};
// Denverton Aptiov Override Start
//#include <PchAccess.h>
//#include <Library/PchCycleDecodingLib.h>
//UINT16                                    mAcpiBaseAddr;
// Denverton Aptiov Override End

/**
  If the power button is pressed, then this function is called.

  @param[in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]      DispatchContext Points to an optional handler context which was specified when the
                                  handler was registered.
  @param[in, out] CommBuffer      A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[in, out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS             The interrupt was handled successfully.

**/
EFI_STATUS
PowerButtonActivated (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  )
{

// Denverton Aptiov Override Start - EIP#364288
// Commented out the following code to prevent shutdown of system
// Thus improving mechanism for multiple handler feature.
// This shutdown functionality is implemented, from PchSmmCoreDispatcher after all child of power button is dipatched.

/*
	  UINT32  Buffer;
	  UINT32  Data32;
	  UINT16  Data16;


	  PchAcpiBaseGet (&mAcpiBaseAddr);

	  //
	  // Clear Power Button Status
	  //
	  gSmst->SmmIo.Io.Read (
	                    &gSmst->SmmIo,
	                    SMM_IO_UINT16,
	                    mAcpiBaseAddr + R_PCH_ACPI_PM1_STS,
	                    1,
	                    &Data16
	                    );
	  Data32 = Data16 | B_PCH_SMI_EN_ON_SLP_EN ;
	  gSmst->SmmIo.Io.Write (
	                    &gSmst->SmmIo,
	                     SMM_IO_UINT32,
	                    mAcpiBaseAddr + R_PCH_ACPI_PM1_STS,
	                    1,
	                     &Data32
	                     );


	  //
	  // Shut it off now
	  //
	  gSmst->SmmIo.Io.Read (
	                    &gSmst->SmmIo,
	                    SMM_IO_UINT32,
	                    mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT,
	                    1,
	                    &Buffer
	                    );
	  Buffer &= ~(B_PCH_ACPI_PM1_CNT_SLP_EN | B_PCH_ACPI_PM1_CNT_SLP_TYP);

	  Buffer |= V_PCH_ACPI_PM1_CNT_S5;

	  gSmst->SmmIo.Io.Write (
	                    &gSmst->SmmIo,
	                    SMM_IO_UINT32,
	                     mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT,
	                    1,
	                     &Buffer
	                     );

	  Buffer |= B_PCH_ACPI_PM1_CNT_SLP_EN;

	  gSmst->SmmIo.Io.Write (
	                    &gSmst->SmmIo,
	                    SMM_IO_UINT32,
	                   mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT,
	                    1,
	                   &Buffer
	                   );
    DEBUG((EFI_D_ERROR, "ResetShutdown Failed\n"));
    CpuDeadLoop();
*/
// Denverton Aptiov Override End - EIP#364288
    return EFI_SUCCESS;
}

/**
  Entry Point of PowerButton SMM driver.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval Others          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
PowerButtonEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                 Status;
  EFI_HANDLE                                 PowerButtonHandle;
  EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL    *PowerButton;

  //
  // Locate the SMM Power Button Dispatch2 Protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmPowerButtonDispatch2ProtocolGuid,
                    NULL,
                    &PowerButton
                    );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Register software SMI handler
  //
  Status = PowerButton->Register (
                          PowerButton,
                          PowerButtonActivated,
                          &mRegisterContext,
                          &PowerButtonHandle
                          );
  
  return Status;
}




    
  
