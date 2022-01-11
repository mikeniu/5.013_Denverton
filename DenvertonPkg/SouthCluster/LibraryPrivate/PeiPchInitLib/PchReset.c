/** @file
  PCH RESET PEIM DRIVER.

@copyright
 Copyright (c) 2011 - 2017 Intel Corporation. All rights reserved
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
#include "PchReset.h"
#include <Ppi/EndOfPeiPhase.h>
/**
  Execute Pch Reset from the host controller.

  @param[in] This                 Pointer to the PCH_RESET_PPI instance.
  @param[in] PchResetType         Pch Reset Types which includes ColdReset, WarmReset, ShutdownReset,
                                  PowerCycleReset, GlobalReset, GlobalResetWithEc

  @retval EFI_SUCCESS             Successfully completed.
  @retval EFI_INVALID_PARAMETER   If ResetType is invalid.
**/
/**
  Silicon Init End of PEI callback function. This is the last change before entering DXE and OS when S3 resume.

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_STATUS       - Always return EFI_SUCCESS
**/
EFI_STATUS
PchDelayResetEndOfPei (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDesc,
  IN VOID                               *Ppi
  );
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PEI_NOTIFY_DESCRIPTOR  mPchDelayResetNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiEndOfPeiSignalPpiGuid,
    PchDelayResetEndOfPei
  }
};

EFI_STATUS
EFIAPI
Reset (
  IN PCH_RESET_PPI  *This,
  IN PCH_RESET_TYPE PchResetType
  )
{
  PCH_RESET_INSTANCE  *PchResetInstance;
  EFI_STATUS          Status;

  PchResetInstance = PCH_RESET_INSTANCE_FROM_THIS (This);
  Status = PchReset (PchResetInstance, PchResetType);

  return Status;
}

/**
  Execute Pch Reset from the host controller.

  @param[in] This                 Pointer to the PCH_RESET_PPI instance.
  @param[in] PchResetType         Pch Reset Types which includes ColdReset, WarmReset, ShutdownReset,
                                  PowerCycleReset, GlobalReset, GlobalResetWithEc

  @retval EFI_SUCCESS             Successfully completed.
  @retval EFI_INVALID_PARAMETER   If ResetType is invalid.
**/
EFI_STATUS
EFIAPI
ResetEndOfPei (
  IN PCH_RESET_PPI  *This,
  IN PCH_RESET_TYPE PchResetType
  )
{
    EFI_STATUS          Status;
    EFI_PEI_PPI_DESCRIPTOR       *ResetEndOfPeiTypePpiDec = NULL;
    EFI_PEI_PPI_DESCRIPTOR       *ResetEndOfPeiTypePpiDecLast = NULL;
    PCH_RESET_END_OF_PEI_PPI     *ResetEndOfPeiPpi = NULL;
    PCH_RESET_END_OF_PEI_PPI     *ResetEndOfPeiPpiLast = NULL;

    DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
    ResetEndOfPeiPpi = (PCH_RESET_END_OF_PEI_PPI*) AllocateZeroPool ( sizeof(PCH_RESET_END_OF_PEI_PPI));
    do {
        if (ResetEndOfPeiPpi == NULL) {
          ASSERT (FALSE);
          Status = EFI_OUT_OF_RESOURCES;
         break;
        }
        ResetEndOfPeiTypePpiDec = (EFI_PEI_PPI_DESCRIPTOR *)AllocateZeroPool(sizeof (EFI_PEI_PPI_DESCRIPTOR));
        if (ResetEndOfPeiTypePpiDec == NULL) {
          ASSERT (FALSE);
          Status = EFI_OUT_OF_RESOURCES;
          break;
        }
        ResetEndOfPeiPpi->ResetPpi = This;
        ResetEndOfPeiPpi->ResetType = PchResetType;

        ResetEndOfPeiTypePpiDec->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
        ResetEndOfPeiTypePpiDec->Guid = &gPchResetEndOfPeiTypeGuid;
        ResetEndOfPeiTypePpiDec->Ppi = ResetEndOfPeiPpi;
        //
        //  Locate if this is there is another reset Type installed
        //
        Status = PeiServicesLocatePpi (
                &gPchResetEndOfPeiTypeGuid,
                0,
                NULL,
                NULL
                );
        if (Status == EFI_NOT_FOUND) {
            //First Install service
            DEBUG ((DEBUG_INFO,  "Adding Callback for End Of Pei Reset Type: %d\n", PchResetType));
            //
            // Setup the Callback to be called at the EndOfPeiAnd Reset
            Status = PeiServicesNotifyPpi (mPchDelayResetNotifyList);
            ASSERT_EFI_ERROR (Status);
            Status = PeiServicesInstallPpi (ResetEndOfPeiTypePpiDec);
            ASSERT_EFI_ERROR (Status);
        } else if (Status == EFI_SUCCESS){
            //
            // There was a previous entry so need to check what is the maximum level of reset
            //
            Status = PeiServicesLocatePpi(
                &gPchResetEndOfPeiTypeGuid,
                0,
                &ResetEndOfPeiTypePpiDecLast,
                (VOID **)&ResetEndOfPeiPpiLast
                );
             ASSERT_EFI_ERROR(Status);
             if ( !EFI_ERROR(Status) &&
                 (PchResetType > ResetEndOfPeiPpiLast->ResetType)) {
                 Status = PeiServicesReInstallPpi(ResetEndOfPeiTypePpiDecLast, ResetEndOfPeiTypePpiDec);
                 ASSERT_EFI_ERROR(Status);
                 DEBUG ((DEBUG_INFO,  "Change of Reset Type End of Pei: %d\n", PchResetType));
             }
        }
    } while(FALSE);
    //
    // If there was an error condition and there was allocation of Memory Deallocate
    //
    if (EFI_ERROR(Status)) {
        if (ResetEndOfPeiPpi != NULL) {
            FreePool(ResetEndOfPeiPpi);
        }
        if (ResetEndOfPeiTypePpiDec != NULL) {
            FreePool(ResetEndOfPeiTypePpiDec);
        }
    }
    DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

    return Status;
}
/**
  Installs PCH RESET PPI

  @retval EFI_SUCCESS             PCH RESET PPI is installed successfully
  @retval EFI_OUT_OF_RESOURCES    Can't allocate pool
**/
EFI_STATUS
InstallPchReset (
  VOID
  )
{
  EFI_STATUS              Status;
  PEI_PCH_RESET_INSTANCE  *PeiPchResetInstance;
  PCH_RESET_INSTANCE      *PchResetInstance;

  DEBUG ((DEBUG_INFO, "InstallPchReset() Start\n"));

  PeiPchResetInstance = (PEI_PCH_RESET_INSTANCE *) AllocateZeroPool (sizeof (PEI_PCH_RESET_INSTANCE));
  if (NULL == PeiPchResetInstance) {
    return EFI_OUT_OF_RESOURCES;
  }

  PchResetInstance = &(PeiPchResetInstance->PchResetInstance);
  PchResetConstructor (PchResetInstance);

  ///
  /// Initialize the Reset ppi instance
  ///
  PchResetInstance->PchResetInterface.PchResetPpi.Reset = Reset;
  PchResetInstance->PchResetInterface.PchResetPpi.ResetEndOfPei = ResetEndOfPei;
  PeiPchResetInstance->PpiDescriptor.Flags  = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PeiPchResetInstance->PpiDescriptor.Guid   = &gPchResetPpiGuid;
  PeiPchResetInstance->PpiDescriptor.Ppi    = &(PchResetInstance->PchResetInterface.PchResetPpi);

  ///
  /// Install the PCH RESET PPI
  ///
  Status = PeiServicesInstallPpi (&PeiPchResetInstance->PpiDescriptor);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "InstallPchReset() End\n"));

  return Status;
}

/**
  Execute call back function for Pch Reset.

  @param[in] PchResetType         Pch Reset Types which includes PowerCycle, Globalreset.

  @retval EFI_SUCCESS             The callback function has been done successfully
  @exception EFI_UNSUPPORTED      Do not do any reset from PCH
**/
EFI_STATUS
PchResetCallback (
  IN     PCH_RESET_TYPE           PchResetType
  )
{
  EFI_STATUS              Status;
  UINTN                   Instance;
  PCH_RESET_CALLBACK_PPI  *PchResetCallbackPpi;

  Instance = 0;
  do {
    Status = PeiServicesLocatePpi (
              &gPchResetCallbackPpiGuid,
              Instance,
              NULL,
              (VOID**) &PchResetCallbackPpi
              );

    switch (Status) {
    case EFI_SUCCESS:
      PchResetCallbackPpi->ResetCallback (PchResetType);
      break;
    case EFI_NOT_FOUND:
      break;
    default:
      ASSERT_EFI_ERROR (Status);
      break;
    }
    ++Instance;
  } while (Status == EFI_SUCCESS);

  return EFI_SUCCESS;
}


EFI_STATUS
PchDelayResetEndOfPei (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDesc,
  IN VOID                               *Ppi
  )
{
    PCH_RESET_END_OF_PEI_PPI     *ResetEndOfPeiPpi;
    EFI_STATUS                    Status;
    DEBUG ((DEBUG_INFO,  "Reset CallBack at the End Of Pei \n"));
    //
    // Locate installed FIA MUX Policy PPI
    //
    Status = PeiServicesLocatePpi(
        &gPchResetEndOfPeiTypeGuid,
        0,
        NULL,
        (VOID **)&ResetEndOfPeiPpi
        );
    ASSERT_EFI_ERROR(Status);
    Status = ResetEndOfPeiPpi->ResetPpi->Reset(ResetEndOfPeiPpi->ResetPpi, ResetEndOfPeiPpi->ResetType);
    return Status;
}
