/** @file
  Source code file for Silicon Init Pre Memory module.

@copyright
  Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
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
#include "SiInitPreMem.h"
#include <Library/PerformanceLib.h>


/**
  Check some silicon policies are valid for debugging unexpected problem if these
  values are not initialized or assigned incorrect resource.

  @param[in] SiPolicy The Silicon Policy PPI instance

**/
VOID
SiValidatePolicy (
  IN  SI_POLICY_PPI *SiPolicy
  )
{
  ASSERT (SiPolicy->TempMemBaseAddr != 0);
  ASSERT (SiPolicy->TempMemSize >= MIN_MMIO_SPACE_SIZE);
  ASSERT (SiPolicy->TempIoBaseAddr != 0);
  ASSERT (SiPolicy->TempIoSize >= MIN_IO_SPACE_SIZE);
  ASSERT (SiPolicy->Revision == SI_POLICY_REVISION);
}

/**
  Slicon Initializes after Policy PPI produced, All required polices must be installed before the callback

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The memory discovered PPI.  Not used.

  @retval EFI_SUCCESS             Succeeds.
**/
EFI_STATUS
EFIAPI
SiInitPreMemOnPolicy (
  IN  EFI_PEI_SERVICES             **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN  VOID                         *Ppi
  );

static EFI_PEI_NOTIFY_DESCRIPTOR  mSiInitNotifyList[] = {
  {
#ifndef FSP_FLAG
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
#else
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
#endif
    &gSiPolicyPpiGuid,
    SiInitPreMemOnPolicy
  }
};

/**
  Slicon Initializes after Policy PPI produced, All required polices must be installed before the callback

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The memory discovered PPI.  Not used.

  @retval EFI_SUCCESS             Succeeds.
**/
EFI_STATUS
EFIAPI
SiInitPreMemOnPolicy (
  IN  EFI_PEI_SERVICES             **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN  VOID                         *Ppi
  )
{
  EFI_STATUS         Status;
  SI_POLICY_PPI      *SiPolicyPpi;
  PCH_POLICY_PPI     *PchPolicyPpi;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));


  //
  //Init PMC Static Power Gating (PMC ST PG) for IPs
  //
  Status = PmcStPgInit ();
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesLocatePpi (
             &gSiPolicyPpiGuid,
             0,
             NULL,
             (VOID **)&SiPolicyPpi
             );
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesLocatePpi (
             &gPchPolicyPpiGuid,
             0,
             NULL,
             (VOID **)&PchPolicyPpi
             );
  ASSERT_EFI_ERROR (Status);


#ifdef EFI_DEBUG

  // Denverton AptioV override Start - EIP#278393 
  // To correct the -ve values in AMIPRD report, by passing same 'identifier' value for START and END macro
  //
  // Validate Silicon policies
  //
  
  // PERF_START_EX (&gPerfSiValidateGuid, NULL, NULL, AsmReadTsc(), 0x5000);
  PERF_START_EX (NULL, "SiVal", NULL, AsmReadTsc(), 0x5000);
  SiValidatePolicy (SiPolicyPpi);
  // PERF_END_EX (&gPerfSiValidateGuid, NULL, NULL, AsmReadTsc(), 0x5001);
  PERF_END_EX (NULL, "SiVal", NULL, AsmReadTsc(), 0x5000);
  
  //
  // Validate PCH policies
  //
  
  // PERF_START_EX (&gPerfPchValidateGuid, NULL, NULL, AsmReadTsc(), 0x5010);
  PERF_START_EX (NULL, "PchVal", NULL, AsmReadTsc(), 0x5010);
  PchValidatePolicy (PchPolicyPpi);
  // PERF_END_EX (&gPerfPchValidateGuid, NULL, NULL, AsmReadTsc(), 0x5011);
  PERF_END_EX (NULL, "PchVal", NULL, AsmReadTsc(), 0x5010);
  
  // Denverton AptioV override End - EIP#278393 
  
#endif // EFI_DEBUG

  //
  // Initialize PCH after Policy PPI produced
  //
  
  // Denverton AptioV override Start - EIP#278393
  // To correct the -ve values in AMIPRD report, by passing same 'identifier' value for START and END macro
  
  // PERF_START_EX (&gPerfPchPreMemGuid, NULL, NULL, AsmReadTsc(), 0x5070);
  PERF_START_EX (NULL, "PchOn", NULL, AsmReadTsc(), 0x5070);
  PchOnPolicyInstalled (SiPolicyPpi, PchPolicyPpi);
  // PERF_END_EX (&gPerfPchPreMemGuid, NULL, NULL, AsmReadTsc(), 0x5071);
  PERF_END_EX (NULL, "PchOn", NULL, AsmReadTsc(), 0x5070);
  
  // Denverton AptioV override End - EIP#278393

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
  return EFI_SUCCESS;
}

/**
  Silicon Initializes before Policy PPI produced

  @param[in] FileHandle           The file handle of the file, Not used.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             The function completes successfully
**/
EFI_STATUS
EFIAPI
SiInitPrePolicy (
  IN  EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS                            Status;
  HSUART_IP_GLOBAL_POLICY_PPI           *HsuartIpGlobalPolicy;


  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  //
  // Initialize and Install HsuartIpGlobalPolicy
  //
  Status = HsuartIpPolicyInit (&HsuartIpGlobalPolicy);
  ASSERT_EFI_ERROR (Status);
  Status = HsuartIpInstallPolicyPpi (HsuartIpGlobalPolicy);
  ASSERT_EFI_ERROR (Status);

  //
  // Initializes PCH before Policy initialized
  //
  
  // Denverton AptioV override Start - EIP#278393
  // To correct the -ve values in AMIPRD report, by passing same 'identifier' value for START and END macro
  
  // PERF_START_EX (&gPerfPchPrePolicyGuid, NULL, NULL, AsmReadTsc(), 0x50E0);
  PERF_START_EX (NULL, "PchPre", NULL, AsmReadTsc(), 0x50E0);
  PchInitPrePolicy ();
  // PERF_END_EX (&gPerfPchPrePolicyGuid, NULL, NULL, AsmReadTsc(), 0x50E1);
  PERF_END_EX (NULL, "PchPre", NULL, AsmReadTsc(), 0x50E0);
  
  // Denverton AptioV override End - EIP#278393

  //
  // Register Silicon init call back after PlatformPolicy PPI produced
  //
  Status = PeiServicesNotifyPpi (mSiInitNotifyList);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
  return EFI_SUCCESS;
}
